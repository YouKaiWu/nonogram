#define N 25
#include "FileIO.h"
#include "Grid.h"
#include <unordered_set>
#include <time.h>
using namespace std;



Line generateLine(vector<int>&, bool, int);
bool complete(vector<Line*> &);
void propagate(Grid& , queue<Line*>&, bool&);
void FP1(Grid& , queue<Line*>&);
void probe(Grid& , string , bool&);
Grid probeG(Grid& , string , char , bool&);
pair<int, int> getIdx(string );
vector<Line*> merge(Grid& , vector<Line*>& , vector<Line*>& , bool& );
void updatePixel(Grid& , string , char );
void backtracking(Grid& , queue<Line*>& q);


vector<string> getUnpainted(Grid& g){  //* row_idx - col_idx 0-index
    vector<string> unpainted;
    for(int i = 0; i < 25; i++){
        for(int j = 1; j <= 25; j++){
            if(g.rows[i]->s[j] == 'u'){
                string pixel = to_string(i) + "-" + to_string(j-1);
                unpainted.push_back(pixel); 
            }
        }
    }
    return unpainted;
};

void print(vector<Line*>& rows){
    for(auto r: rows){
        Line& line = *r;
        for(auto& ch : line.s.substr(1)){
            cout << ch << " ";
        }
        cout << endl;
    }
};
  
// * 慢慢找去修改Propagate , fp1, backtracking
int main() {
    ifstream inputFile("input.txt");
    string str; 
    if (inputFile.is_open()) {
        int t = 1;
        while(getline(inputFile, str)){
            clock_t start = clock();
            Grid g;
            queue<Line*> q; 
            g.rows.resize(N);
            g.cols.resize(N);
            FileIO::loadCase(inputFile, str, q, g.rows, g.cols);
            backtracking(g,q);
            cout << static_cast<int>(g.status) << endl;
            if(g.status == State:: SOLVED){
                cout << "Case"<< t++ << ": Solved" << endl;
            }
            print(g.rows);
            clock_t end = clock();
            double time_spent = 0.0;
            time_spent += (double)(end - start) / CLOCKS_PER_SEC;
            cout << "time_spent: "<< time_spent << " s" << endl;
        }
        inputFile.close();
    } else {
        cerr << "Failed to open the file for reading." << endl;
    }
    return 0;
}




bool complete(vector<Line*>& rows){
    for(auto r: rows){
        Line& line = *r;
        for(int i = 1; i <= N; i++){
            if(line.s[i] == 'u')
                return false;
        }
    }
    return true;
};

void propagate(Grid& g, queue<Line*>& q, bool& update){
    while(!q.empty()){
        Line* cur = q.front();
        q.pop();
        (*cur).exist = false;
        if(!(*cur).fix(N,(*cur).d.size() - 1)){
            g.status = State:: CONFLICT;
            return;
        }
        string origin = (*cur).s;
        (*cur).s = "X" + (*cur).paint(N, (*cur).d.size() - 1); 
        if(origin != ((*cur).s)){
            for(int i = 1; i <= N; i++){
                if(origin[i] != (*cur).s[i]){
                    if((*cur).isRow){
                        (*(g.cols[i-1])).s[(*cur).id+1] = (*cur).s[i];
                        if(!(*(g.cols[i-1])).exist) {
                            q.push((g.cols[i-1]));
                            (*(g.cols[i-1])).exist = true;
                        }

                    }else{
                        (*(g.rows[i-1])).s[(*cur).id+1] = (*cur).s[i];
                        if(!(*(g.rows[i-1])).exist) {
                            q.push((g.rows[i-1]));
                            (*(g.rows[i-1])).exist = true;
                        }
                    }
                }
            }
        }
    }   
    if(complete(g.rows)){
        g.status = State:: SOLVED;
    }else{
        g.status = State:: INCOMPLETE;
    }
};

void FP1(Grid& g, queue<Line*>& q){
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
    Grid GP0 = probeG(g, pixel, '0', update_zero);
    Grid GP1 = probeG(g, pixel, '1', update_one);
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
        g.rows = merge(g, GP0.rows, GP1.rows, update);
        g.cols = merge(g, GP0.cols, GP1.cols, update);
    }
    
    if(update){
        g.status = State:: PAINTED;
    }else{
        g.status = State:: INCOMPLETE;
    }
};

// * 這邊複製要建新的指標，要改寫
Grid probeG(Grid& g, string pixel, char filled, bool& update){ 
    Grid newG = g;
    pair<int, int> idx = getIdx(pixel);
    int row_idx = idx.first;
    int col_idx = idx.second;
    newG.rows[row_idx]->s[col_idx+1] = filled;
    newG.rows[row_idx]->exist = true;
    newG.cols[col_idx]->s[row_idx+1] = filled;
    newG.cols[col_idx]->exist = true;
    queue<Line*> q;
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


vector<Line*> merge(Grid& g ,vector<Line*>& filled_zero, vector<Line*>& filled_one, bool& update_merge){
    vector<Line*> merged(25);
    if(filled_one[0]->isRow){
        merged = g.rows;
    }else{
        merged = g.cols;
    }
    for(int i = 0; i < 25; i++){
        for(int j = 1; j <= 25; j++){
            if(merged[i]->s[j] == 'u' && filled_one[i]->s[j] == filled_zero[i]->s[j] && filled_one[i]->s[j] != 'u' ){
                merged[i]->s[j] = filled_one[i]->s[j];
                update_merge = true;
            }
        }
    }
    return merged;
};

void updatePixel(Grid& g, string pixel, char filled){ 
    pair<int, int> idx = getIdx(pixel);
    int row_idx = idx.first;
    int col_idx = idx.second;
    g.rows[row_idx]->s[col_idx+1] = filled;
    g.cols[col_idx]->s[row_idx+1] = filled;
};

void backtracking(Grid& g, queue<Line*>& q){
    FP1(g, q);
    if(g.status == State::CONFLICT){
        return;
    }
    if(g.status == State::SOLVED){
        return;
    }
    bool update = false;
    for(auto& pixel: getUnpainted(g)){
        updatePixel(g, pixel, '0');
        backtracking(g, q);
        updatePixel(g, pixel, '1');
        backtracking(g, q);
    }
}