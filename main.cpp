#define N 25
#include "FileIO.h"
#include "Grid.h"
#include <unordered_set>
using namespace std;



Line generateLine(vector<int>&, bool, int);
bool complete(vector<Line> &);
void propagate(Grid& , queue<Line>&, bool&);
void FP1(Grid& , queue<Line>&);
void probe(Grid& , string , bool&);
Grid probeG(Grid& , string , int , bool&);
pair<int, int> getIdx(string );
vector<Line> merge(Grid& , vector<Line>& , vector<Line>& , bool& );
void backtracking(Grid& , queue<Line>& q);


unordered_set<string> getUnpainted(Grid& g){  //* row_idx - col_idx 0-index
    unordered_set<string> unpainted;
    for(int i = 0; i < 25; i++){
        for(int j = 1; j <= 25; j++){
            if(g.rows[i].s[j] == 'u'){
                string pixel = to_string(i) + "-" + to_string(j-1);
                unpainted.insert(pixel); 
            }
        }
    }
    return unpainted;
}

void print(vector<Line>& rows){
    for(auto r: rows){
        Line line = r;
        for(auto& ch : line.s.substr(1)){
            cout << ch << " ";
        }
        cout << endl;
    }
};
  

int main() {
    ifstream inputFile("input.txt");
    string str; 
    if (inputFile.is_open()) {
        while(getline(inputFile, str)){
            Grid g;
            queue<Line> q; 
            g.rows.resize(N);
            g.cols.resize(N);
            FileIO::loadCase(inputFile, str, q, g.rows, g.cols);
            backtracking(g, q);
            print(g.rows);
            g.rows.clear();
            g.cols.clear();
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

void propagate(Grid& g, queue<Line>& q, bool& update){
    while(!q.empty()){
        Line cur = q.front();
        q.pop();
        if(!cur.fix(N,cur.d.size() - 1)){
            g.status = State::CONFLICT;
            return;
        }
        string origin = cur.s;
        cur.s = "X" + cur.paint(N, cur.d.size() - 1); 
        if(origin != cur.s){
            for(int i = 1; i <= N; i++){
                if(origin[i] != cur.s[i]){
                    update = true;
                    if(cur.isRow){
                        g.cols[i-1].s[cur.id+1] = cur.s[i];
                        q.push((g.cols[i-1]));
                            
                    }else{
                        g.rows[i-1].s[cur.id+1] = cur.s[i];
                        q.push((g.rows[i-1]));
                    }
                }
            }
            if(cur.isRow){
                for(int i = 1; i <= N; i++){
                    if(cur.s[i] != 'u'){
                        g.rows[cur.id].s[i] = cur.s[i];
                    }
                }
            }else{
                for(int i = 1; i <= N; i++){
                    if(cur.s[i] != 'u'){
                        g.cols[cur.id].s[i] = cur.s[i];
                    }
                }
            }
        }
    }   
    if(complete(g.rows)){
        g.status = State::SOLVED;
    }else{
        g.status = State::INCOMPLETE;
    }
};

void FP1(Grid& g, queue<Line>& q){
    bool update = false;
    do{
        update = false;
        propagate(g, q, update);
        if(g.status == State::CONFLICT || g.status == State::SOLVED){
            return;
        }
        for(auto& pixel: getUnpainted(g)){  
            probe(g, pixel, update);
            if(g.status == State::CONFLICT || g.status == State::SOLVED){
                return;
            }
            if(g.status == State::PAINTED){
                break;
            }
        }
    }while(update);
};


void probe(Grid& g, string pixel, bool& update){
    bool update_zero = false, update_one = false;
    Grid GP0 = probeG(g, pixel, 0, update_zero);
    Grid GP1 = probeG(g, pixel, 1, update_one);
    cout << (GP0.status == State::CONFLICT)<< " " << (GP1.status == State::CONFLICT) << endl;
    if(GP0.status == State::CONFLICT && GP1.status == State::CONFLICT){
        g.status = State::CONFLICT;
        return;
    }else if(GP0.status == State::CONFLICT){
        g.rows = GP1.rows;
        g.cols = GP1.cols;
        update = update_one;
    }else if(GP1.status == State::CONFLICT){
        g.rows = GP0.rows;
        g.cols = GP0.cols;
        update = update_zero;
    }else{
        bool update_merge = false;
        g.rows = merge(g, GP0.rows, GP1.rows, update_merge);
        g.cols = merge(g, GP0.cols, GP1.cols, update_merge);
        update = update_merge;
    }
    
    if(update){
        g.status = State:: PAINTED;
    }else{
        g.status = State:: INCOMPLETE;
    }
};


Grid probeG(Grid& g, string pixel, int filled, bool& update){ 
    Grid newG = g;
    queue<Line> q;
    pair<int, int> idx = getIdx(pixel);
    int row_idx = idx.first;
    int col_idx = idx.second;
    newG.rows[row_idx].s[col_idx+1] = filled - '0';
    newG.cols[col_idx].s[row_idx+1] = filled - '0';
    q.push(newG.rows[row_idx]);
    q.push(newG.cols[col_idx]);
    propagate(newG, q, update);
    return newG;
};

pair<int, int> getIdx(string pixel){
    stringstream ss(pixel);
    string token;
    int i = 0;
    pair<int, int> idx;
    while(getline(ss, token, '-')){
        if(i == 0){
            idx.first = stoi(token);
        }else{
            idx.second = stoi(token);
        }
        i++;
    }
    return idx;
};


vector<Line> merge(Grid& g ,vector<Line>& filled_zero, vector<Line>& filled_one, bool& update_merge){
    vector<Line> merged(25);
    if(filled_one[0].isRow){
        merged = g.rows;
    }else{
        merged = g.cols;
    }
    for(int i = 0; i < 25; i++){
        for(int j = 1; j <= 25; j++){
            if(merged[i].s[j] == 'u' && filled_one[i].s[j] != 'u' && filled_one[i].s[j] == filled_zero[i].s[j]){
                merged[i].s[j] = filled_one[i].s[j];
                update_merge = true;
            }
        }
    }
    return merged;
};

void backtracking(Grid& g, queue<Line>& q){
    FP1(g, q);
    if(g.status == State::CONFLICT){
        return;
    }
    if(g.status == State::SOLVED){
        return;
    }
    bool update = false;
    for(auto& pixel: getUnpainted(g)){
        Grid GP0 = probeG(g, pixel, 0, update);
        backtracking(GP0, q);
        Grid GP1 = probeG(g, pixel, 1, update);
        backtracking(GP1, q);
    }
}