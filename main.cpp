#define N 25
#include "FileIO.h"

using namespace std;

Line generateLine(vector<int>&, bool, int);
bool complete(vector<Line*> &);
void propagate();
void FP1();

void print(vector<Line*>& rows){
    for(auto r: rows){
        Line line = *r;
        cout << line.s.substr(1) << endl;
    }
};

bool solved = false;
queue<Line*> q;   
vector<Line*> rows(25);
vector<Line*> cols(25);

int main() {
    ifstream inputFile("input.txt");
    string str; 
    if (inputFile.is_open()) {
        while(getline(inputFile, str)){
            
            FileIO::loadCase(inputFile, str, q, rows, cols);
            propagate();
            for(int i = 0; i < 25; i++){
                free(rows[i]);
                free(cols[i]);
            }
            rows.clear();
            cols.clear();
            break;
        }
        inputFile.close();
    } else {
        cerr << "Failed to open the file for reading." << endl;
    }
    return 0;
}




bool complete(vector<Line*>& rows){
    for(auto r: rows){
        Line line = *r;
        for(int i = 1; i <= N; i++){
            if(line.s[i] == 'u')
                return false;
        }
    }
    return true;
};

void propagate(){
    int cnt = 0;
    while(!q.empty()){
        // cout << cnt++ << endl;
        Line* cur = q.front();
        q.pop();
        (*cur).exist = false;
        if(!(*cur).fix(N,(*cur).d.size() - 1)){
            return;
        }
        string origin = (*cur).s;
        (*cur).s = "X" + (*cur).paint(N, (*cur).d.size() - 1); 
        if(origin != ((*cur).s)){
            for(int i = 1; i <= N; i++){
                if(origin[i] != (*cur).s[i]){
                    if((*cur).isRow){
                        (*(cols[i-1])).s[(*cur).id+1] = (*cur).s[i];
                        if(!(*(cols[i-1])).exist) {
                            q.push((cols[i-1]));
                            (*(cols[i-1])).exist = true;
                        }
                            
                    }else{
                        (*(rows[i-1])).s[(*cur).id+1] = (*cur).s[i];
                        if(!(*(rows[i-1])).exist) {
                            q.push((rows[i-1]));
                            (*(rows[i-1])).exist = true;
                        }
                    }
                }
            }
        }
    }   
    if(complete(rows)){
        solved = true;
    }else{
        solved = false;
    }
    print(cols);

};

void FP1(){

};
