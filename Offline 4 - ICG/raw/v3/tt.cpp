#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> splitPair(string str, char delimiter) {

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

int main() {
    string input = "Hello,World , dfsd  ";
    char delimiter = ',';
    
    vector<string> result = splitPair(input, delimiter);
    
    cout << "First part: " << result[0] << endl;
    cout << "Second part: " << result[1] << endl;

    return 0;
}
