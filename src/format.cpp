#include <string>
#include <sstream>
#include <iomanip>

#include "format.h"


using std::string;
using std::setfill;
using std::setw;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int h, m, s;

    h = seconds / (60 * 60);
    m = seconds % (60 * 60) / 60;
    s = seconds % 60;

    std::stringstream out;
    out << setfill('0') << setw(2) << h << ':' << setfill('0') << setw(2) << m << ':' << setfill('0') << setw(2) << s;

    return out.str();
 }