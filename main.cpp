#include "FileInput.h"
#include "Grid.h"

#include <unordered_set>
#include <time.h>

Line generateLine(vector<int>&, bool, int);
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
    for(int i = 0; i < N; i++){
        for(int j = 1; j <= N; j++){
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

bool isSolved(vector<Line*>& rows){
    for(auto r: rows){
        Line& line = *r;
        vector<int> actual;
        actual.push_back(-1);
        int cnt = 0;
        for(int i = 1; i <= N; i++){
            if(line.s[i] == '1'){
                cnt++;
            }else{
                if(cnt != 0)
                    actual.push_back(cnt);
                cnt = 0;
            }
        }
        if(cnt != 0)
            actual.push_back(cnt);
        for(int i = 1; i < line.d.size(); i++){
            if(line.d[i] != actual[i])
                return false;
        }
        
    }
    return true;
};

int main() {
    ifstream inputFile("input.txt");
    string str; 
    if (inputFile.is_open()) {
        while(getline(inputFile, str)){
            string test_case = str;
            clock_t start = clock();
            Grid g;
            queue<Line*> q; 
            g.rows.resize(N);
            g.cols.resize(N);
            FileInput::loadCase(inputFile, str, q, g.rows, g.cols);
            backtracking(g,q);
            clock_t end = clock();
            double time_spent = 0.0;
            time_spent += (double)(end - start) / CLOCKS_PER_SEC;
            if(g.status == State:: SOLVED)
                cout << "Case "<< test_case << ": Solved; time_spent: "<< time_spent << " s" << endl;
            // cout << isSolved(g.rows) << endl;
            // print(g.rows);
        }
        inputFile.close();
    } else {
        cerr << "Failed to open the file for reading." << endl;
    }
    return 0;
}


void propagate(Grid& g, queue<Line*>& q, bool& update){
    while(!q.empty()){
        Line& cur = *(q.front());
        q.pop();
        cur.exist = false;
        if(!cur.fix(N,cur.d.size() - 1)){
            g.status = State:: CONFLICT;
            return;
        }
        string origin = cur.s;
        cur.s = cur.paint(N, cur.d.size() - 1); 
        if(origin != cur.s){
            for(int i = 1; i <= N; i++){
                if(origin[i] != cur.s[i]){
                    if(cur.isRow){
                        g.cols[i-1]->s[cur.id+1] = cur.s[i];
                        if(!g.cols[i-1]->exist) {
                            q.push((g.cols[i-1]));
                            g.cols[i-1]->exist = true;
                        }

                    }else{
                        g.rows[i-1]->s[cur.id+1] = cur.s[i];
                        if(!g.rows[i-1]->exist) {
                            q.push((g.rows[i-1]));
                            g.rows[i-1]->exist = true;
                        }
                    }
                }
            }
        }
    }   
    if(isSolved(g.rows))
        g.status = State:: SOLVED;
    else
        g.status = State:: INCOMPLETE;
};

void FP1(Grid& g, queue<Line*>& q){
    bool update = false;
    do{
        update = false;
        propagate(g, q, update);
        if(g.status == State::CONFLICT || g.status == State::SOLVED)
            return;
        for(auto& pixel: getUnpainted(g)){  
            probe(g, pixel, update);
            if(g.status == State::CONFLICT || g.status == State::SOLVED)
                return;
            if(g.status == State::PAINTED)
                break;
        }
    }while(update);
};

void probe(Grid& g, string pixel, bool& update){
    Grid GP0 = probeG(g, pixel, '0', update);
    Grid GP1 = probeG(g, pixel, '1', update);
    if(GP0.status == State::CONFLICT && GP1.status == State::CONFLICT){
        g.status = State::CONFLICT;
        return;
    }else if(GP0.status == State::CONFLICT){
        g.rows = GP1.rows;
        g.cols = GP1.cols;
        update = true;
    }else if(GP1.status == State::CONFLICT){
        g.rows = GP0.rows;
        g.cols = GP0.cols;
        update = true;
    }else{
        g.rows = merge(g, GP0.rows, GP1.rows, update);
        g.cols = merge(g, GP0.cols, GP1.cols, update);
    }
    if(update)
        g.status = State:: PAINTED;
    else
        g.status = State:: INCOMPLETE;
};

Grid copyG(Grid& g){
    Grid newG;
    newG.status = g.status;
    vector<Line*> rows(N);
    vector<Line*> cols(N);
    for(int i = 0; i < N; i++){
        Line* line = new Line(g.rows[i]->s, g.rows[i]->d, g.rows[i]->isRow, g.rows[i]->id, g.rows[i]->exist);
        rows[i] = line;
    }
    for(int i = 0; i < N; i++){
        Line* line = new Line(g.cols[i]->s, g.cols[i]->d, g.cols[i]->isRow, g.cols[i]->id, g.cols[i]->exist);
        cols[i] = line;
    }
    newG.rows = rows;
    newG.cols = cols;
    return newG;
};

Grid probeG(Grid& g, string pixel, char filled, bool& update){ 
    Grid newG = copyG(g);
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
        if(i == 0)
            idx.first = stoi(token);
        else
            idx.second = stoi(token);
        i++;
    }
    return idx;
};

vector<Line*> merge(Grid& g ,vector<Line*>& filled_zero, vector<Line*>& filled_one, bool& update_merge){
    vector<Line*> merged(N);
    if(filled_one[0]->isRow){
        merged = g.rows;
    }else{
        merged = g.cols;
    }
    for(int i = 0; i < N; i++){
        for(int j = 1; j <= N; j++){
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
    if(g.status == State::CONFLICT)
        return;
    if(g.status == State::SOLVED)
        return;
    for(auto& pixel: getUnpainted(g)){
        updatePixel(g, pixel, '0');
        backtracking(g, q);
        if(g.status == State::SOLVED)
            return;
        updatePixel(g, pixel, '1');
        backtracking(g, q);
    }
};