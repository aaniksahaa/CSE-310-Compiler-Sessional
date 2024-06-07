#include <iostream>
#include <fstream>

int main() {
    // Open a file named "output.txt" for writing
    std::ofstream outputFile("output.txt");

    // Check if the file is successfully opened
    if (!outputFile.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
        return 1; // Return an error code
    }

    // Write content to the file
    outputFile << "Hello, this is some content written to the file." << std::endl;
    outputFile << "You can add more lines as needed." << std::endl;

    // Close the file
    outputFile.close();

    // Inform the user that the operation was successful
    std::cout << "File write successful. Check 'output.txt'." << std::endl;

    return 0; // Return success code
}
