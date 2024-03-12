#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

// Define a struct to hold the LZ77 tokens
struct LZ77Token {
    int offset;
    int length;
    char next;
};

// Function to compress the input data using the LZ77 algorithm
vector<LZ77Token> lz77_compress(const vector<char>& input, int window_size) {
    vector<LZ77Token> output;
    int i = 0;

    // Loop over the input data
    while (i < input.size()) {
        int match_distance = 0;
        int match_length = 0;

        // Search for a match in the window
        for (int j = i - window_size; j < i; j++) {
            if (j < 0) continue;

            int k = 0;
            // Extend the match as far as possible
            while (input[j + k] == input[i + k] && k < window_size) {
                k++;
            }

            // If this match is longer than the previous best match, update the best match
            if (k > match_length) {
                match_distance = i - j;
                match_length = k;
            }
        }

        // Get the next character after the match
        char next = input[i + match_length];
        // Add the LZ77 token to the output
        output.push_back({ match_distance, match_length, next });

        // Move the window
        i += match_length + 1;
    }

    return output;
}

string lz77_decompress(const vector<LZ77Token>& compressed) {
    string output;
    for (const LZ77Token& token : compressed) {
        if (token.length > 0) {
            // Copy the match from the specified distance back in the output
            int start = output.length() - token.offset;
            for (int i = 0; i < token.length; ++i) {
                output += output[start + i];
            }
        }
        // Append the next character
        output += token.next;
    }
    return output;
}

int main() {
    // Open the file in binary mode
    ifstream file("input.txt", ios::binary);
    // Read the entire file into a vector of chars
    vector<char> input((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    int window_size = 4096;

    // Compress the data
    vector<LZ77Token> compressed = lz77_compress(input, window_size);

    // Print the compressed data
    for (const LZ77Token& token : compressed) {
        cout << "(" << token.offset << ", " << token.length << ", " << token.next << ")" << endl;
    }
    
    //Decompress the data
    string decompressed = lz77_decompress(compressed);
    cout << "Decompressed Data: " << decompressed << endl;

    return 0;
}
