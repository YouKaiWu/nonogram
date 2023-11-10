#include "Line.h"
#include <bits/stdc++.h>

using namespace std;

int main()
{

    string s = "Xuuuu1uuuuuuuuuuuuu1uuuuuu";
    vector<int> d = {-1, 1, 2, 1, 1, 1, 3, 3};
    Line line = Line(s, d, true, 0, false);
    cout << line.paint(25, d.size()-1);
    return 0;
}