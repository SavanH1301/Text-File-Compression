#include <bits/stdc++.h>
using namespace std;

struct LZ77Token {
    int offset;
    int length;
    char nextChar;
};

vector<LZ77Token> lz77_compress(const string &text, int windowSize = 20) {
    vector<LZ77Token> tokens;
    int n = text.size();
    int i = 0;

    while (i < n) {
        int matchLength = 0, matchOffset = 0;
        int start = max(0, i - windowSize);

        // Search for the longest match in the sliding window
        for (int j = start; j < i; j++) {
            int k = 0;
            while (i + k < n && text[j + k] == text[i + k]) k++;
            if (k > matchLength) {
                matchLength = k;
                matchOffset = i - j;
            }
        }

        char nextChar = (i + matchLength < n) ? text[i + matchLength] : '\0';
        tokens.push_back({matchOffset, matchLength, nextChar});
        i += matchLength + 1;
    }

    return tokens;
}

string lz77_decompress(const vector<LZ77Token> &tokens) {
    string result;
    for (auto &token : tokens) {
        if (token.offset > 0) {
            int start = result.size() - token.offset;
            for (int i = 0; i < token.length; i++) {
                result.push_back(result[start + i]);
            }
        }
        if (token.nextChar != '\0') {
            result.push_back(token.nextChar);
        }
    }
    return result;
}

int main() {
    string text;
    cout << "Enter text to compress: ";
    getline(cin, text);

    auto compressed = lz77_compress(text);
    cout << "\nCompressed (offset,length,nextChar):\n";
    for (auto &token : compressed) {
        cout << "(" << token.offset << "," << token.length << ","
             << (token.nextChar ? token.nextChar : '#') << ") ";
    }

    string decompressed = lz77_decompress(compressed);
    cout << "\n\nDecompressed Text: " << decompressed << "\n";

    return 0;
}
