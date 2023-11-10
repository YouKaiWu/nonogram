#ifndef FILEIO_H
#define FILEIO_H


#include "Line.h"
#include <fstream>
#include <sstream>
#include <queue>

using namespace std;

class FileIO{
public:
    static Line* generateLine(vector<int>& d, bool isRow, int id, bool exist){
        string s =  "X" + string(N, 'u');
        Line* line = new Line(s, d, isRow, id, exist);
        return line;
    };
    static vector<int> make_des(string& str){
        vector<int> res(1, -1);
        istringstream iss(str);
        string word;
        while(getline(iss, word, '\t')){
            res.push_back(stoi(word));
        }
        return res;
    }

    static void loadCase(ifstream& inputFile, string& str, queue<Line*>& q, vector<Line*>& rows, vector<Line*>& cols ){
        for(int i = 0; i < 50; i++){
            getline(inputFile, str);
            vector<int> d = make_des(str);
            if(i < 25){    // generate row
                Line* line = generateLine(d, true, i, true);
                q.push(line);    
                rows[i] = line;
            }
            else{       // generate col
                Line* line = generateLine(d, false, i - 25, true);
                q.push(line);
                cols[i-25] = line;
            }
        }
    };
};

#endif