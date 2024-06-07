#include<bits/stdc++.h>
using namespace std;

// helper functions

string to_upper(string str) {
    string result;
    for (char c : str) {
        result += toupper(c);
    }
    return result;
}

vector<string> split(const string& input, char delimiter) {
    vector<string> parts;
    stringstream ss(input);
    string part;

    while (getline(ss, part, delimiter)) {
        parts.push_back(part);
    }

    return parts;
}

string strip(string input) {
    size_t start = input.find_first_not_of(" \t\n\r\f\v");
    if (start == string::npos)
        return ""; // Empty string or all whitespace

    size_t end = input.find_last_not_of(" \t\n\r\f\v");
    return input.substr(start, end - start + 1);
}

bool contains(string str, char ch){
    size_t pos = str.find(ch);
    if (pos != string::npos) {
        return true;
    }
    return false;
}

vector<string> split_pair(string str, char delimiter) {
    vector<string> parts;
    size_t pos = str.find(delimiter);
    if (pos != string::npos) {
        parts.push_back(str.substr(0, pos));
        parts.push_back(str.substr(pos + 1));
    } else {
        parts.push_back(str);
        parts.push_back("");
    }
    return parts;
}

// Classes

class Statement {
public:
    string line;
    string label, op;
    vector<string> args;
    string comment;

    Statement(){
        label = op = comment = "";
        args.clear();
    }

    void process(){
        op = strip(to_upper(op));
        for(auto &arg: args){
            arg = strip(to_upper(arg));
        }
    }

    void print(){
        cout<<"Label: "<<label<<endl;
        cout<<"Op: "<<op<<endl;
        cout<<"Comment: "<<comment<<endl;
    }
};

// Parser for code.asm

vector<Statement> parse_assembly(string filename) {
    vector<Statement> statements;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return statements;
    }

    string line;
    while (getline(file, line)) {
        Statement statement;

        statement.line = line;

        line = strip(line);

        //cout<<line<<endl;

        if(line == ""){
            statements.push_back(statement);
            continue;
        }

        // Splitting the line into label, operation, and comment
        vector<string> parts = split_pair(line, ';');

        statement.comment = strip(parts[1]);

        string main_part = strip(parts[0]);

        if(main_part == ""){
            statements.push_back(statement);
            continue;
        }

        if(contains(main_part, ':')){
            parts = split_pair(main_part, ':');
            statement.label = strip(parts[0]);
            main_part = parts[1];
        }

        if(main_part == ""){
            statements.push_back(statement);
            continue;
        }

        parts = split_pair(main_part, ' ');
        
        statement.op = parts[0];

        string arg_part = parts[1];

        auto args = split(arg_part, ',');

        for(auto arg: args){
            statement.args.push_back(strip(arg));
        }

        statement.process();

        statements.push_back(statement);
    }

    file.close();
    return statements;
}

// Optimizer

int main() {

    ofstream opt_out("optimized_code.asm");

    vector<Statement> statements = parse_assembly("code.asm");

    int n = statements.size();

    vector<int>marked(n+5,0);

    int non_instructions = 0;

    for(int i=0; i<n; i++){
        auto s1 = statements[i];

        //s1.print();

        if(s1.op == "" || s1.op[0]=='.'){
            non_instructions++;
        }

        // removing unnecessary add/subtract by 0
        if(s1.op == "ADD" || s1.op == "SUB"){
            if( s1.args[1] == "0" ){
                marked[i] = 1;
            }
        }

        if(i<n-1){
            auto s2 = statements[i+1];

            //cout<<s2.label<<endl;

            // removing redundant MOV instructions
            if(s1.op == "MOV" && s2.op == "MOV"){
                if(s1.args[0] == s2.args[1] && s1.args[1] == s2.args[0]){
                    marked[i] = 1;
                    marked[i+1] = 1;
                }
            }

            // removing consecutive push pop to same register
            if(s1.op == "PUSH" && s2.op == "POP"){
                if(s1.args[0] == s2.args[0]){
                    marked[i] = 1;
                    marked[i+1] = 1;
                }
            }

            // removing unnecessary jumps
            if(s1.op == "JMP"){
                if(s2.label != ""){
                    if(s1.args[0] == s2.label){
                        marked[i] = 1;
                    }
                }
            }
        }
    }

    int cnt = 0;

    for(int i=0; i<n; i++){
        if( ! marked[i] ){
            opt_out<<statements[i].line<<endl;
            cnt++;
        }
    }

    bool verbose = true;

    if(verbose){
        
        cout<<"\nOptimized intermediate code written to optimized_code.asm file"<<endl;

        double pct = ((cnt - non_instructions)/((n-non_instructions)*1.0))*100.0;

        cout<<"\nRemoved "<<n-cnt<<" instructions out of "<<n-non_instructions<<" instructions"<<endl;
        cout<<"Code shrinked to "<<pct<<"\% of original code\n"<<endl;

    }

    opt_out.close();

    return 0;
}
