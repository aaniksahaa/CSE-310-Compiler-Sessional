#include<bits/stdc++.h>
using namespace std;


class Statement {
public:
    string label, op;
    vector<string>args;
    string comment;
};

vector<Line> parseASMFile(const std::string& filename) {
    //cout<<"here"<<endl;
    vector<Line> lines;
    ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return lines;
    }

    string line;
    while (getline(file, line)) {
        istringstream iss(line);
        string left, core, right;
        
        int i = 0;
        int n = line.size();

        while(i<n && (line[i]=='\t' || line[i] == ' ')){
            left += line[i];
            i++;
        }

        while(i<n && (line[i]!='\t' && line[i] != ';') ){
            core += line[i];
            i++;
        }

        while(i<n){
            right += line[i];
            i++;
        }

        lines.emplace_back(left, core, right);
    }

    file.close();
    return lines;
}

// Function to split a string based on a delimiter
vector<string> split(string input, char delimiter) {
    vector<string> parts;
    stringstream ss(input);
    string part;

    while (getline(ss, part, delimiter)) {
        parts.push_back(part);
    }

    return parts;
}

// Function to strip leading and trailing whitespace from a string
std::string strip(const std::string& input) {
    size_t start = input.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos)
        return ""; // Empty string or all whitespace

    size_t end = input.find_last_not_of(" \t\n\r\f\v");
    return input.substr(start, end - start + 1);
}

string toUpper(string str) {
    string result;
    for (char c : str) {
        result += toupper(c);
    }
    return result;
}

int main() {
    std::vector<Line> lines = parseASMFile("code.asm");

    // for (auto line : lines) {
    //     line.print();
    // }

    // Step 1:

    // Removing redundant addition and multiplication operations
    // ADD AX, 0 
    // SUB AX, 0 

    // vector<Line>temp_lines;

    // for(auto line: lines){
    //     string op = toUpper(line.core.substr(0,3));
    //     if(op == "ADD" || op == "SUB"){

    //     }
    // }

    string ans = strip("     a      b   , ");
    cout<<ans<<endl;

    return 0;
}
