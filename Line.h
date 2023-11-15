#ifndef LINE_H
#define LINE_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Line{  
public:
    Line(string s, vector<int> d, bool isRow, int id, bool exist){
        this->s = s;
        this->d = d;
        this->isRow = isRow;
        this->id = id;
        this->exist = exist;
    };

    Line() {}
    
    vector<int> d;
    string s;
    bool isRow;
    int id;
    bool exist;

    bool fix(int i, int j){
        if(i == 0){
            if(j == 0)
                return true;
            else
                return false;
        }
        return fix0(i, j) || fix1(i, j);
    };

    bool fix0(int i, int j){
        if(s[i] == '0' || s[i] == 'u')
            return fix(i-1, j);
        return false;
    };

    bool fix1(int i, int j){
        if(j == 1 && i == d[j] && match_sigma(i, j)){
            return fix(i-d[j], j-1);
        }
        if(j >= 1 && i >= d[j] + 1 && match_sigma(i, j)){
            return fix(i-d[j]-1, j-1);
        }
        return false;
    };


    bool match_sigma(int i, int j){
        if(s[i - d[j]] == '1')
            return false;
        for(int i_ = i - d[j] + 1; i_ <= i ;i_++){
            if(s[i_] == '0'){
                return false;
            }
        }
        return true;
    };


    string paint(int i, int j){
        if(i == 0)
            return "X";
        if(fix0(i, j) && !fix1(i, j)){
            return paint0(i, j);
        }else if(!fix0(i, j) && fix1(i, j)){
            return paint1(i, j);
        }else{
            return merge(paint0(i, j) ,paint1(i, j));
        }
    };

    string paint0(int i, int j){ 
        return paint(i-1, j) + "0";    
    };

    string paint1(int i, int j){
        if(j == 1 && i == d[j]){
            return paint(i - d[j], j - 1) + string(d[j], '1');
        }
        return paint(i - d[j] - 1, j - 1) + "0" + string(d[j], '1');
    };

    string merge(const string& paint0, const string& paint1){
        string same = ""; 
        for(int i = 0; i < paint0.size(); i++){
            if(paint0[i] == paint1[i])
                same += paint0[i];
            else
                same += "u";
        }
        return same;
    };
};

#endif