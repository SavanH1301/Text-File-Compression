
---

## Huffman

> These two programs mirror your earlier logic (bit-level I/O, pseudo-EOF `char(129)`, `codes.txt` mapping). They’ve been cleaned up and commented.

### `huffman/huffman_compress.cpp`
```cpp
// Huffman Compressor: reads a text file, writes <filename>.cmp (binary) and codes.txt (map)
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <iomanip>
#include <cstring>
#include <string>
#include <utility>

using namespace std;

static const unsigned char PSEUDO_EOF = 129;
static const unsigned char INTERNAL_NODE_MARK = 254;

string codesTableLineBuffer;
vector<pair<char,string>> charactermap; // (character, huffman code)
vector<int> bitstream;                  // bits to write
int distinctCount = 0;                  // count of distinct characters
int tableCount   = 0;                   // count used for printing
priority_queue<struct MinHeapNode*, vector<struct MinHeapNode*>, struct compare> minHeap;

class BitwiseWrite {
    char buf;
    int  nbits;
    ostream& out;
public:
    explicit BitwiseWrite(ostream& os) : buf(0), nbits(0), out(os) {}
    void flush() { out.put(buf); out.flush(); buf = 0; nbits = 0; }
    void writeBit(int b) {
        if (b == -1) { flush(); return; }
        if (nbits == 8) flush();
        unsigned char mask = 1u << (7 - nbits);
        if (b == 1) buf |= mask;
        else        buf &= static_cast<unsigned char>(~mask);
        ++nbits;
    }
};

// ---------------- Huffman tree ----------------
struct MinHeapNode {
    int data;    // store char as int
    int freq;
    MinHeapNode *left, *right;
    MinHeapNode(int d, int f) : data(d), freq(f), left(nullptr), right(nullptr) {}
};

struct compare {
    bool operator()(MinHeapNode* l, MinHeapNode* r) const { return l->freq > r->freq; }
};

void emitCodes(MinHeapNode* root, const string& prefix) {
    if (!root) return;
    if (root->data != INTERNAL_NODE_MARK) {
        char ch = static_cast<char>(root->data);
        cout << "\t    " << (ch == ' ' ? string("Space") : string(1, ch))
             << "\t\t  " << prefix << "\n";

        // store printable line for codes.txt (use '_' for space)
        if (ch == ' ') codesTableLineBuffer += string("_ ") + prefix + "|\n";
        else           codesTableLineBuffer += string(1,ch) + " " + prefix + "|\n";

        for (auto &p : charactermap)
            if (p.first == ch) p.second = prefix;
    }
    emitCodes(root->left,  prefix + "0");
    emitCodes(root->right, prefix + "1");
}

void printTree(ostream& out, MinHeapNode* root, int indent = 0) {
    if (!root) return;
    printTree(out, root->right, indent + 4);
    out << setw(indent) << " " << static_cast<char>(root->data) << "\n";
    printTree(out, root->left, indent + 4);
}

void buildHuffman(int data[], int freq[], int size) {
    for (int i = 0; i < size; ++i) minHeap.push(new MinHeapNode(data[i], freq[i]));
    while (minHeap.size() > 1) {
        auto* l = minHeap.top(); minHeap.pop();
        auto* r = minHeap.top(); minHeap.pop();
        auto* top = new MinHeapNode(INTERNAL_NODE_MARK, l->freq + r->freq);
        top->left = l; top->right = r;
        minHeap.push(top);
    }
    emitCodes(minHeap.top(), "");
}

void writeCodesTxt(const string& raw) {
    ofstream out("codes.txt");
    for (char c : raw) {
        if (c != '|') out << c;
        else out << '\n';
    }
    out.close();
}

void stringBitsToVector() {
    for (auto& pr : charactermap) {
        for (char b : pr.second) bitstream.push_back(b == '1' ? 1 : 0);
    }
    bitstream.push_back(-1); // flush marker for writer
}

// ---------------- Helpers ----------------
void calcFreqFromFile(int freq[256], const string& filename) {
    ifstream fin(filename, ios::binary);
    if (!fin) { cerr << "Error: cannot open input file.\n"; exit(1); }
    char ch;
    vector<bool> seen(256, false);
    while (fin.get(ch)) {
        unsigned char uc = static_cast<unsigned char>(ch);
        if (uc >= 32 && uc < 255) {
            if (!seen[uc]) { charactermap.emplace_back(static_cast<char>(uc), ""); seen[uc] = true; ++distinctCount; }
        }
        ++freq[uc];
    }
    // add PSEUDO_EOF to table
    charactermap.emplace_back(static_cast<char>(PSEUDO_EOF), "");
    ++distinctCount;
}

void separateOccurring(const int freq[256], int actual[], int ascii[], int& countOut) {
    int k = 0, y = 0;
    for (int j = 32; j < 254; ++j) {
        if (freq[j] != 0) { actual[k] = freq[j]; ascii[y] = j; ++y; ++k; ++countOut; }
    }
    actual[y] = 1;         // pseudo-EOF
    ascii[k]  = PSEUDO_EOF;
    ++countOut;
}

void printFreqTable(const int actual[], const int ascii[], int count1) {
    cout << "\n\tFrequency Table\n\n";
    cout << "\t====================================\n";
    cout << "\tCharacter\t\tFrequency\n";
    cout << "\t====================================\n";
    for (int i = 0; i < count1; ++i) {
        char ch = static_cast<char>(ascii[i]);
        if (ch == ' ') cout << "\t    Space\t\t  " << actual[i] << "\n";
        else           cout << "\t    " << ch << "\t\t\t  " << actual[i] << "\n";
    }
}

void printCompressionStats(size_t originalBytes, size_t compressedBits) {
    size_t compressedBytes = compressedBits / 8;
    cout << "\nOriginal Size        : " << originalBytes   << " bytes";
    cout << "\nCompressed Size      : " << compressedBytes << " bytes";
    if (originalBytes > 0) {
        long double ratio = (static_cast<long double>(compressedBytes) / originalBytes) * 100.0L;
        cout << "\nCompression Ratio    : " << fixed << setprecision(2) << ratio << "%";
    }
    cout << "\n";
}

// ---------------- main ----------------
int main() {
    cout << "\n\t====================================";
    cout << "\n\t\t  Huffman Compressor\n";
    cout << "\t====================================\n";

    string filename;
    cout << "\nEnter name of text file to compress: ";
    cin  >> filename;

    int freq[256] = {0}, ascii[256] = {0}, actual[256] = {0};
    calcFreqFromFile(freq, filename);

    int count1 = 0;
    separateOccurring(freq, actual, ascii, count1);

    printFreqTable(actual, ascii, count1);
    cout << "\nCharacter → Huffman Code\n";
    cout << "\n\tCharacter\t\tCode\n";

    buildHuffman(ascii, actual, count1);
    stringBitsToVector();

    // change extension to .cmp
    string outname = filename;
    if (outname.size() >= 3) outname.replace(outname.size()-3, 3, "cmp");
    else outname += ".cmp";

    cout << "\n\nHuffman Encoding Tree:\n";
    printTree(cout, minHeap.top(), 0);

    writeCodesTxt(codesTableLineBuffer);

    ofstream fout(outname, ios::binary);
    if (!fout) { cerr << "Error: cannot open output file.\n"; return 1; }

    BitwiseWrite writer(fout);
    for (int b : bitstream) writer.writeBit(b);

    // Stats (rough: use number of distinct chars as proxy for demo)
    printCompressionStats(static_cast<size_t>(distinctCount - 1), bitstream.size());

    cout << "Wrote: " << outname << " and codes.txt\n";
    cout << "Done.\n";
    return 0;
}
