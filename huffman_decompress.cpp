// Huffman Decompressor: reads codes.txt and a .cmp binary, writes Decompressed.txt
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std;

static const unsigned char PSEUDO_EOF = 129;

class BitwiseRead {
    char buf;
    int  nbits;   // bits consumed in buf
    istream& in;
public:
    explicit BitwiseRead(istream& is) : buf(0), nbits(8), in(is) {}
    void fill() { buf = in.get(); nbits = 0; }
    int readBit() {
        if (nbits == 8) fill();
        unsigned char mask = 1u << (7 - nbits);
        ++nbits;
        return (buf & mask) ? 1 : 0;
    }
};

class Huffman {
    struct Node {
        char data;
        Node *left, *right;
        explicit Node(char c) : data(c), left(nullptr), right(nullptr) {}
    };
    using P = Node*;
    P root;
public:
    Huffman() : root(new Node('*')) {}
    void buildDecodingTree(ifstream& codeIn) {
        char ch; string code;
        while (codeIn >> ch >> code) {
            if (ch == '_') ch = ' ';
            insert(ch, code);
        }
    }
    char decode(BitwiseRead& in) {
        P p = root;
        while (true) {
            int bit = in.readBit();
            p = (bit == 1) ? p->right : p->left;
            if (!p->left || !p->right) break;  // reached leaf
        }
        return p->data;
    }
    void printTree(ostream& out, P node, int indent = 0) {
        if (!node) return;
        printTree(out, node->right, indent + 4);
        out << setw(indent) << " " << node->data << "\n";
        printTree(out, node->left, indent + 4);
    }
    void show(ostream& out) { printTree(out, root); }
private:
    void insert(char ch, const string& code) {
        P p = root;
        for (char b : code) {
            if (b == '0') { if (!p->left)  p->left  = new Node('*'); p = p->left;  }
            else if (b == '1') { if (!p->right) p->right = new Node('*'); p = p->right; }
            else { cerr << "Invalid bit in code file.\n"; exit(1); }
        }
        p->data = ch;
    }
};

int main() {
    cout << "\n\t====================================";
    cout << "\n\t\t  Huffman Decompressor\n";
    cout << "\t====================================\n";

    string codesFile, cmpFile;
    cout << "\nEnter name of code file (codes.txt): ";
    cin  >> codesFile;

    ifstream codeIn(codesFile);
    if (!codeIn) { cerr << "Error: cannot open code file.\n"; return 1; }

    Huffman h;
    h.buildDecodingTree(codeIn);

    cout << "\nHuffman decoding tree:\n";
    h.show(cout);

    cout << "\nEnter name of compressed file (.cmp): ";
    cin  >> cmpFile;

    ifstream in(cmpFile, ios::binary);
    if (!in) { cerr << "Error: cannot open compressed file.\n"; return 1; }

    ofstream out("Decompressed.txt");
    if (!out) { cerr << "Error: cannot create Decompressed.txt\n"; return 1; }

    BitwiseRead br(in);
    cout << "\nDecoding... please wait.\n";

    while (true) {
        char c = h.decode(br);
        if (static_cast<unsigned char>(c) == PSEUDO_EOF) break;
        out.put(c);
    }

    cout << "Decompression successful! Wrote Decompressed.txt\n";
    return 0;
}
