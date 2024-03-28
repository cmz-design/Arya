#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include "utils.h"

using namespace std;

struct words {
    long n; // total number of characters
    char* Chars;  // array storing all strings
    long m; // number of substrings
    char** Strings; // pointers to strings (all should be null terminated)
    words() {}
    words(char* C, long nn, char** S, long mm)
        : Chars(C), n(nn), Strings(S), m(mm) {}
    void del() {free(Chars); free(Strings);}
};

inline bool isSpace(char c) {
    switch (c)  {
    case '\r': 
    case '\t': 
    case '\n': 
    case 0:
    case ' ' : return true;
    default : return false;
    }
};

_seq<char> readStringFromFile(const string& filename) {
    ifstream file (filename, ios::in | ios::binary | ios::ate);
    if (!file.is_open()) {
        std::cout << "Unable to open file: " << filename << std::endl;
        abort();
    }
    long end = file.tellg();
    file.seekg (0, ios::beg);
    long n = end - file.tellg();
    char* bytes = newA(char,n+1);
    file.read (bytes,n);
    file.close();
    return _seq<char>(bytes,n);
}


// parallel code for converting a string to words
words stringToWords(char *Str, long n) {
    cilk_for (long i=0; i < n; i++) 
        if (isSpace(Str[i])) Str[i] = 0; 

    // mark start of words
    bool *FL = newA(bool,n);
    FL[0] = Str[0];
    cilk_for (long i=1; i < n; i++) FL[i] = Str[i] && !Str[i-1];

    // offset for each start of word
    _seq<long> Off = sequence::packIndex<long>(FL, n);
    long m = Off.n;
    long *offsets = Off.A;

    // pointer to each start of word
    char **SA = newA(char*, m);
    cilk_for (long j=0; j < m; j++) SA[j] = Str+offsets[j];

    free(offsets); free(FL);
    return words(Str,n,SA,m);
}

// Function to read edges from input file

vector<pair<int, int>> readEdges(const string& filename) {
    _seq<char> S = readStringFromFile(filename);
    char* S2 = newA(char,S.n);
    //ignore starting lines with '#' and find where to start in file 
    long k=0;
    while(1) {
        if(S.A[k] == '#') {
    while(S.A[k++] != '\n') continue;
        }
        if(k >= S.n || S.A[k] != '#') break; 
    }
    cilk_for(long i=0;i<S.n-k;i++) S2[i] = S.A[k+i];
    S.del();

    words W = stringToWords(S2, S.n-k);
    long n = W.m/2;
    long m = 2 * n; // Double the number of edges to account for reverse edges
    vector<pair<int, int>> edges;

    {cilk_for(long i=0; i < n; i++){
        int start=atoi(W.Strings[2*i]);
        int end=atoi(W.Strings[2*i + 1]);
        edges.push_back({start, end});
        edges.push_back({end, start});
        }
    }
    cout << "Read finish" << endl;
    W.del();
    sort(edges.begin(), edges.end());
    cout << "Sort finish" << endl;
    edges.erase(unique(edges.begin(), edges.end()), edges.end()); // 去除重复的边
    cout << "Delete finish" << endl;
    return edges;
}

// Function to write edges and their reverse to output file
void writeEdges(const string& filename, const vector<pair<int, int>>& edges) {
    ofstream outputFile(filename);
    for (const auto& edge : edges) {
        outputFile << edge.first << " " <<  edge.second << endl;
    }
    outputFile.close();
    cout << "Write finish" << endl;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " input_filename output_filename" << endl;
        return 1;
    }

    string inputFilename = argv[1];  // Input file containing graph edges
    string outputFilename = argv[2];  // Output file to write edges and their reverses
    
    auto start = chrono::steady_clock::now();

    // Read edges from input file
    vector<pair<int, int>> edges = readEdges(inputFilename);

    // Write edges to output file
    writeEdges(outputFilename, edges);

    auto end_time = chrono::steady_clock::now(); // 记录结束时间
    auto elapsed_time = chrono::duration<double>(end_time - start); // 计算时间间隔
    double elapsedSeconds = elapsed_time.count();
    cout << "Edges and their reverses written to " << outputFilename << endl;
    int hours = elapsedSeconds / 3600;
    int minutes = (elapsedSeconds - hours * 3600) / 60;
    int seconds = elapsedSeconds - hours * 3600 - minutes * 60;
    cout << "Time elapsed: " << hours << " hours, " << minutes << " minutes, " << seconds << " seconds." << endl;

    return 0;
}
