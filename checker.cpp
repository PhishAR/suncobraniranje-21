// Usage: [checker] [input] [contestant_output]

#include <iostream>
#include <fstream>
#include <cmath>
#include <cassert>
#include <cstring>
#include <set>

using namespace std;

#define FOR(i, a, b) for (int i = (a); i < int(b); ++i)
#define REP(i, n) FOR(i, 0, n)
const int MAXN = 1e6;
const long double E = 1e-6;

int n;
pair<int, int> points[MAXN];
bool covered[MAXN];
double total = 0.0;

void report_error(const string &exit_message);

double squared_dist(double x1, double y1, double x2, double y2) {
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

string TOO_FEW_POINTS_ERR(double x, double y, double r) {
    return "Suncobran [(" + to_string(x) + ", " + to_string(y) + "), " + to_string(r) +
           "] pokriva manje od dvije osobe.";
}

string EXPECTED_OUTPUT_FORMAT() {
    return "Ocekivani oblik izlaznih podataka je:\n"
           "[int] M - broj postavljenih suncobrana\n"
           "M puta:\n"
           "\t[double, double, double] x y r - centar (x,y) i radijus (r) M-tog suncobrana\n"
           "\t[int] P - broj ljudi koje pokriva M-ti suncobran\n"
           "\tP puta:\n"
           "\t\t[int] idx ∈ [1,N] - indeks sudionika pokrivenog M-tim suncobranom ";
}

string POINT_NOT_IN_CIRCLE(int idx, double c1, double c2, double r) {
    return "Suncobran [(" + to_string(c1) + ", " + to_string(c2) + "), " + to_string(r) +
           "] ne pokriva osobu " + to_string(idx) + ".";
}

string POINT_NOT_COVERED(int idx) {
    return "Osoba " + to_string(idx) + " nije pokrivena.";
}

string MULTIPLE_DETECTED(int idx, double x, double y, double r) {
    return "Osoba " + to_string(idx) + " pojavljuje se vise puta ispod suncobrana [("
           + to_string(x)
           + ", " + to_string(y) + "), " + to_string(r) + "]";
}

/**
 * The main checking function.
 * @param finp official input
 * @param fsol contestant's output (solution)
 */
double checker(ifstream &finp, ifstream &fsol) {
    const string TEST_DATA_ERROR = "Test data error.";
    // Read official input
    memset(covered, false, sizeof covered);

    if (!(finp >> n)) report_error(TEST_DATA_ERROR);

    REP(person_idx, n) {
        if (!(finp >> points[person_idx].first)) report_error(TEST_DATA_ERROR);
        if (!(finp >> points[person_idx].second)) report_error(TEST_DATA_ERROR);
    }

    // Read contestant's output
    int output_len;

    if (!(fsol >> output_len)) report_error(EXPECTED_OUTPUT_FORMAT());
    if (output_len > MAXN)
        report_error("Cini se da si postavio vise od " + to_string(MAXN) + " suncobrana, provjeri svoj izlaz.\n" +
                     EXPECTED_OUTPUT_FORMAT());

    REP(parasol_idx, output_len) {
        double x, y, radius, squared_radius;
        int m;

        if (!(fsol >> x) || !(fsol >> y))
            report_error("Nedostaje centar suncobrana " +
                         to_string(parasol_idx + 1) + ".\n" + EXPECTED_OUTPUT_FORMAT());

        if (!(fsol >> radius))
            report_error("Nedostaje radijus suncobrana " +
                         to_string(parasol_idx + 1) + ".\n" + EXPECTED_OUTPUT_FORMAT());
        squared_radius = radius * radius;

        if (!(fsol >> m))
            report_error("Nedostaje broj ljudi koje pokriva suncobran" +
                         to_string(parasol_idx + 1) + ".\n" + EXPECTED_OUTPUT_FORMAT());

        if (m < 2) report_error(TOO_FEW_POINTS_ERR(x, y, radius));

        set<int> checked;
        REP(p, m) {
            int person_under_parasol_idx;

            if (!(fsol >> person_under_parasol_idx))
                report_error(to_string(p + 1) +
                             ". indeks osobe pod suncobranom " +
                             to_string(parasol_idx + 1) + " nedostaje.\n" + EXPECTED_OUTPUT_FORMAT());

            if (person_under_parasol_idx < 1 || person_under_parasol_idx > n) {
                report_error("Indeks osoba bi trebao biti izmedu 1 i " + to_string(n) + ".");
            }
            --person_under_parasol_idx;

            if (checked.find(person_under_parasol_idx) != checked.end())
                report_error(MULTIPLE_DETECTED(person_under_parasol_idx, x, y, radius));

            checked.insert(person_under_parasol_idx);

            if (squared_dist(x, y, points[person_under_parasol_idx].first, points[person_under_parasol_idx].second) >
                             radius * radius + E
            ){
                report_error(POINT_NOT_IN_CIRCLE(person_under_parasol_idx + 1, x, y, radius));
            }

            covered[person_under_parasol_idx] = true;
        }

        total += squared_radius;
    }

    // if there are more than output_length parasols specified in the output file
    string suffix;
    while (getline(fsol, suffix)) {
        if (suffix.find_first_not_of(" \t\v\r\n") != string::npos) {
            report_error("Predani broj N ne odgovara broju postavljenih suncobrana,"
                         " provjeri svoj izlaz.\n" + EXPECTED_OUTPUT_FORMAT());
        }
    }

    REP(person_idx, n) {
        if (!covered[person_idx]) report_error(POINT_NOT_COVERED(person_idx + 1));
    }

    return M_PI * total;
}

void report_error(const string &exit_message) {
    cout << 0 << " " << exit_message << endl;
    exit(0);
}

int main(int argc, char *argv[]) {
    double total_points;

    cout.precision(9);
    assert(argc == 3);

    ifstream finp(argv[1]);
    ifstream fsol(argv[2]);

    assert(!finp.fail() && !fsol.fail());

    total_points = checker(finp, fsol);
    cout << 1 << " " << total_points << endl;
    return 0;
}
