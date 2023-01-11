#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <deque>

#define MAX_ALPHABET_SIZE 256

using namespace std;

class SuffixArray
{
public:
    SuffixArray();
    ~SuffixArray();

    bool addStringFromFile(string);
    bool addString(string);

    void addFileName(string);

    void makeSuffixArray();
    vector<int> makeLCPArray();

    int findLongestCommonStrand(const unsigned int);

    int calcParentString(int);
    void printSuffixArray();

private:
    void skewSuffixArray(const vector<int>&, vector<int>&, int, int, int);
    void radixSort(vector<int>&, vector<int>&, const vector<int>&, int, int, int, int);

    template <typename T> void printVector(vector<T> v);

    template <typename T> void printSubStringVector(vector<T> v, int offset, int length, bool inHex = false);

private:
    vector<int>* m_suffixArray;

    int m_stringCount;
    vector<int>* m_concatedString;

    vector<string> *m_fileNames;

    int m_sentinelCharacter = -1;
};
