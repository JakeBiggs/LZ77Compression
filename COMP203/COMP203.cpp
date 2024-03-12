// COMP203.cpp : Defines the entry point for the application.
//

#include "COMP203.h"

using namespace std;

#include <iostream>
#include <vector>
#include <fstream>

// Define a struct to hold the LZ77 tokens
// These act as references to substrings within the input data
struct LZ77Token {
    int offset;
    int length;
    char next;
};

// Function to compress the input data using the LZ77 algorithm
vector<LZ77Token> lz77_compress(const vector<char>& input, int window_size) {
    vector<LZ77Token> output;
    int i = 0; //i represents the current position in the input data
    //The sliding window is being defined implicitly from position i-window_size to i

    // Loop over the input data
    while (i < input.size()) {
        int match_distance = 0;
        int match_length = 0;

        // Search for a match in the  sliding window
        for (int j = i - window_size; j < i; j++) {
            if (j < 0) continue;

            int k = 0;
            // Extend the match as far as possible
            //while (input[j + k] == input[i + k] && k < window_size) {
              //  k++;
            //}
            while (j + k < input.size() && i + k < input.size() && input[j + k] == input[i + k] && k < window_size) {
                k++;
                    if (j + k >= input.size() || i + k >= input.size()) {
                        break;
                    }
            }
            // If this match is longer than the previous best match, update the best match
            if (k > match_length) {
                match_distance = i - j;
                match_length = k;
            }
        }

        // Get the next character after the match
        // If at the end of the input, use a null character
        //char next = input[i + match_length];
        char next = (i + match_length < input.size()) ? input[i + match_length] : '\0'; 

        // Add the LZ77 token to the output
        output.push_back({ match_distance, match_length, next });

        // Move the window
        i += match_length + 1;
    }

    // Save the compressed data to a file
    ofstream outfile("compressed.bin", ios::binary);
    for (const LZ77Token& token : output) {
        uint16_t offset = token.offset;
        uint16_t length = token.length;
        char next = token.next;

        outfile.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        outfile.write(reinterpret_cast<const char*>(&length), sizeof(length));
        outfile.write(&next, sizeof(next));
    }
    outfile.close();

    return output;
}

string lz77_decompress(const vector<LZ77Token>& compressed) {
    string output;
    for (const LZ77Token& token : compressed) {
        if (token.length > 0) {
            // Copy the match from the specified distance back in the output
            int start = output.length() - token.offset;
            for (int i = 0; i < token.length && start + i < output.size(); ++i) {
                output += output[start + i];
            }

            //string substring = output.substr(start, token.length);
            //output += substring;
        }
        // Append the next character
        output += token.next;
    }
    return output;
}

vector<LZ77Token> read_compressed_binary(string filename) {
    // Read the compressed data from a file
    vector<LZ77Token> compressed;
    ifstream infile(filename, ios::binary);
    while (infile.good()) {
        uint16_t offset;
        uint16_t length;
        char next;

        infile.read(reinterpret_cast<char*>(&offset), sizeof(offset));
        infile.read(reinterpret_cast<char*>(&length), sizeof(length));
        infile.read(&next, sizeof(next));

        if (infile.good()) {
            compressed.push_back({ offset, length, next });
        }
    }
    infile.close();
    return compressed;
}

int main() {
    // Open the file in binary mode
    ifstream file("bee-moviex100.txt", ios::binary);
    if (!file.good()) {
        cout << "Error Opening File..." << strerror(errno) << endl;
        return 1;
    }
    // Read the entire file into a vector of chars
    vector<char> input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    int window_size = 8192;//4096; //Dont exceed uint16 

    // Compress the data
    vector<LZ77Token> compressed_data = lz77_compress(input, window_size);



    // Print the compressed data
    //for (const LZ77Token& token : compressed_data) {
      //  cout << "(" << token.offset << ", " << token.length << ", " << token.next << ")" << endl;
    //}
    //vector<LZ77Token> compressed = read_compressed_binary("compresed.bin");


    //Decompress the data
    string decompressed = lz77_decompress(read_compressed_binary("compressed.bin"));
    cout << "Decompressed Data: " << decompressed << endl;

    return 0;
}

