#include <fstream>  // Include fstream for file operations
#include <iostream>

int main() {

	std::string text;
	text.append("POST /test HTTP/1.1\n");
    text.append("Host: foo.example\n");
    text.append("Content-Type: application/x-www-form-urlencoded\n");
    text.append("Content-Length: 27\n");
    text.append("\r\n\r\n");
    text.append("field1=value1&field2=value2");

	std::cout << text << std::endl;

    // Create an output file stream object to write to a file named "request"
    std::ofstream file("request");

    // Check if the file opened successfully
    if (file.is_open()) {
        // Write the text to the file
		file << text;
        file.close();
    } else {
        // Handle the error if the file could not be opened
        return 1;
    }

    return 0;
}

