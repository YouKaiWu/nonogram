#define N 25
#include "FileIO.h"

using namespace std;

enum class State{
    SOLVED, INCOMPLETE, CONFLICT, PAINTED 
};

Line generateLine(vector<int>&, bool, int);
bool complete(vector<Line*> &);
void propagate();
void FP1();
void probe();

void print(vector<Line>& rows){
    for(auto r: rows){
        Line line = r;
        cout << line.s.substr(1) << endl;
    }
};

State statusG;
queue<Line> q;   
vector<Line> rows(N);
vector<Line> cols(N);

int main() {
    ifstream inputFile("input.txt");
    string str; 
    if (inputFile.is_open()) {
        while(getline(inputFile, str)){
            FileIO::loadCase(inputFile, str, q, rows, cols);
            propagate();
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




bool complete(vector<Line>& rows){
    for(auto r: rows){
        Line line = r;
        for(int i = 1; i <= N; i++){
            if(line.s[i] == 'u')
                return false;
        }
    }
    return true;
};

void propagate(){
    while(!q.empty()){
        Line cur = q.front();
        q.pop();
        if(!cur.fix(N,cur.d.size() - 1)){
            statusG = State::CONFLICT;
            return;
        }
        string origin = cur.s;
        cur.s = "X" + cur.paint(N, cur.d.size() - 1); 
        if(origin != cur.s){
            for(int i = 1; i <= N; i++){
                if(origin[i] != cur.s[i]){
                    if(cur.isRow){
                        cols[i-1].s[cur.id+1] = cur.s[i];
                        q.push((cols[i-1]));
                            
                    }else{
                        rows[i-1].s[cur.id+1] = cur.s[i];
                        q.push((rows[i-1]));
                    }
                }
            }
            if(cur.isRow){
                for(int i = 1; i <= N; i++){
                    if(cur.s[i] != 'u'){
                        rows[cur.id].s[i] = cur.s[i];
                    }
                }
            }else{
                for(int i = 1; i <= N; i++){
                    if(cur.s[i] != 'u'){
                        cols[cur.id].s[i] = cur.s[i];
                    }
                }
            }
        }
    }   
    if(complete(rows)){
        statusG = State::SOLVED;
    }else{
        statusG = State::INCOMPLETE;
    }
    print(rows);

};

// void FP1(){
//     while(true){
//         propagate();
//         if(statusG == State::CONFLICT || statusG == State:: SOLVED){
//             return;
//         }
//         for(){  //* 要補
//             if(statusG == State:: CONFLICT || statusG == State:: SOLVED){
//                 return;
//             }
//             if(statusG == State:: PAINTED){
//                 break;
//             }
//         }
//     }
    
// };


void probe(){

};