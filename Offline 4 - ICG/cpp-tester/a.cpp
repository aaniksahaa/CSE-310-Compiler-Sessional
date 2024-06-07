#include <iostream>
#include <vector>
#include <sstream>

using namespace std;

string left_pad(string s)
{
	vector<string> lines;
    istringstream iss(s);
    string line;

    while (getline(iss, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    string result;
    for (const auto& line : lines) {
        result += "\t" + line + "\n";
    }

    return result;
}

int main() {
    string s = "a x\nb y\n\nc z\n\n\n";

    // Print the original and result strings
    std::cout << "Original String:\n" << s << "\n\n";
    std::cout << "Processed String:\n" << left_pad(s) << std::endl;

    return 0;
}
