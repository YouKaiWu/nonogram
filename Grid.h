#ifndef GRID_H
#define GRID_H
#include "State.h"
#include "Line.h"

class Grid
{
    

public:
    vector<Line> rows;
    vector<Line> cols;
    State status;
};
#endif