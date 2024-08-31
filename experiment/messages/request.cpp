#include <fstream>  // Include fstream for file operations

int main() {
    // Define the text as a char array with each character specified individually
    const char text[] = {
        'P', 'O', 'S', 'T', ' ', '/', 't', 'e', 's', 't', ' ', 'H', 'T', 'T', 'P', '/', '1', '.', '1', '\n',
        'H', 'o', 's', 't', ':', ' ', 'f', 'o', 'o', '.', 'e', 'x', 'a', 'm', 'p', 'l', 'e', '\n',
        'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'T', 'y', 'p', 'e', ':', ' ', 'a', 'p', 'p', 'l', 'i', 'c', 'a', 't', 'i', 'o', 'n', '/', 'x', '-', 'w', 'w', 'w', '-', 'f', 'o', 'r', 'm', '-', 'u', 'r', 'l', 'e', 'n', 'c', 'o', 'd', 'e', 'd', '\n',
        'C', 'o', 'n', 't', 'e', 'n', 't', '-', 'L', 'e', 'n', 'g', 't', 'h', ':', ' ', '2', '7', '\n',
        '\r','\n', '\r', '\n',
        'f', 'i', 'e', 'l', 'd', '1', '=', 'v', 'a', 'l', 'u', 'e', '1', '&', 'f', 'i', 'e', 'l', 'd', '2', '=', 'v', 'a', 'l', 'u', 'e', '2'
    };

    // Create an output file stream object to write to a file named "request"
    std::ofstream file("request");

    // Check if the file opened successfully
    if (file.is_open()) {
        // Write the text to the file
        file.write(text, sizeof(text));  // Use write method to handle binary data
        file.close();
    } else {
        // Handle the error if the file could not be opened
        return 1;
    }

    return 0;
}

