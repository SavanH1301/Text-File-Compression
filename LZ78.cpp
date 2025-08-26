#include <bits/stdc++.h>
using namespace std;

struct LZ78Token {
    int index;
    char nextChar;
};

vector<LZ78Token> lz78_compress(const string &text) {
    vector<LZ78Token> tokens;
    unordered_map<string, int> dict;
    int dictIndex = 1;
    string current;

    for (char c : text) {
        current += c;
        if (dict.find(current) == dict.end()) {
            string prefix = current.substr(0, current.size() - 1);
            int index = prefix.empty() ? 0 : dict[prefix];
            tokens.push_back({index, c});
            dict[current] = dictIndex++;
            current.clear();
        }
    }
    if (!current.empty()) {
        int index = dict[current.substr(0, current.size() - 1)];
        tokens.push_back({index, current.back()});
    }
    return tokens;
}

string lz78_decompress(const vector<LZ78Token> &tokens) {
    vector<string> dict(1, ""); // dict[0] is empty string
    string result;

    for (auto &token : tokens) {
        string entry = dict[token.index] + token.nextChar;
        result += entry;
        dict.push_back(entry);
    }
    return result;
}

int main() {
    string text;
    cout << "Enter text to compress: ";
    getline(cin, text);

    auto compressed = lz78_compress(text);
    cout << "\nCompressed (index,nextChar):\n";
    for (auto &token : compressed) {
        cout << "(" << token.index << "," << token.nextChar << ") ";
    }

    string decompressed = lz78_decompress(compressed);
    cout << "\n\nDecompressed Text: " << decompressed << "\n";

    return 0;
}
