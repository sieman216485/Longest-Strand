#include "SuffixArray.h"

#include <filesystem>

SuffixArray::SuffixArray()
{
    m_suffixArray = new vector<int>;

    m_stringCount = 0;
    m_concatedString = new vector<int>;

    m_fileNames = new vector<string>;
}

SuffixArray::~SuffixArray()
{
    delete m_suffixArray;

    delete m_concatedString;

    delete m_fileNames;
}

bool SuffixArray::addString(string inputString)
{
    for (auto it = inputString.cbegin(); it != inputString.cend(); it++)
    {
        m_concatedString->push_back(*it);
    }
    m_concatedString->push_back(m_sentinelCharacter);

    m_sentinelCharacter--;
    m_stringCount++;

    return true;
}

bool SuffixArray::addStringFromFile(string fileName)
{
    ifstream fin(fileName, ios::binary);
    if (!fin)
    {
        cerr << "Could not read file from: " << fileName << endl;
        return false;
    }

    fin.unsetf(ios::skipws);

    fin.seekg(0, ios::end);
    streampos length = fin.tellg();
    fin.seekg(0, ios::beg);

    m_concatedString->reserve(length);

    int i = 0;
    char temp;
    while (i < length)
    {
        fin.read(&temp, sizeof(temp));
        m_concatedString->push_back(reinterpret_cast<unsigned char&>(temp));
        i++;
    }

    m_concatedString->push_back(m_sentinelCharacter);

    addFileName(fileName);

    m_sentinelCharacter--;
    m_stringCount++;

    return true;
}

void SuffixArray::addFileName(string filePath)
{
    filesystem::path p(filePath);

    m_fileNames->push_back(p.filename());
}

void SuffixArray::makeSuffixArray()
{
    m_suffixArray->resize(m_concatedString->size());

    m_concatedString->push_back(m_sentinelCharacter);
    m_concatedString->push_back(m_sentinelCharacter);
    m_concatedString->push_back(m_sentinelCharacter);

    skewSuffixArray(*m_concatedString, *m_suffixArray, m_concatedString->size() - 3, MAX_ALPHABET_SIZE, m_stringCount + 1);

    m_concatedString->pop_back();
    m_concatedString->pop_back();
    m_concatedString->pop_back();
}

/*
    Construct LCP Array using Kasai's Algorithim
*/
vector<int> SuffixArray::makeLCPArray()
{
    int length = m_suffixArray->size();

    vector<int> lcp(length, 0);
    vector<int> invertedSuffixArray(length, 0);

    int i;
    // Construct Inverted Suffix Array
    for (i = 0; i < length; i++)
        invertedSuffixArray[(*m_suffixArray)[i]] = i;

    int l = 0;
    // Kasai Algorithim
    for (i = 0; i < length - 1; i++)
    {
        int k = invertedSuffixArray[i];
        int j = (*m_suffixArray)[k - 1];

        while ((*m_concatedString)[i + l] == (*m_concatedString)[j + l])
            l += 1;

        lcp[k] = l;

        if (l > 0)
            l -= 1;
    }

    return lcp;
}

/*
    Finds Longest Common Strand that appears in k substrings
    Returns length of LCS and saves offsets to a set
*/
int SuffixArray::findLongestCommonStrand(const unsigned int k)
{
    if (k < 1 || k > m_stringCount)
        cout << "Given invalid k-value: " << k << endl;

    // Initialize LCP Array
    vector<int> lcp = makeLCPArray();
    // printVector(lcp);

    unsigned int length = lcp.size();

    set<int> offsets;

    unsigned int window1 = m_stringCount;
    unsigned int window2 = m_stringCount;

    map<int, int> suffixSourcesMap;
    suffixSourcesMap[calcParentString((*m_suffixArray)[window1])]++;

    int max = 0;
    deque<int> window;

    auto decrementIndex = [](map<int, int> &map, int index) {
        if (map[index] - 1 <= 0)
            map.erase(index);
        else
            map[index] -= 1;
    };

    auto updateMaxValue = [&](int index1, int index2) {
        if ((*m_concatedString)[index1] == (*m_concatedString)[index2])
        {
            if (lcp[window.front()] > max)
            {
                max = lcp[window.front()];
                offsets.clear();
                offsets.insert(index1);
                offsets.insert(index2);
            }
            else if (lcp[window.front()] == max)
            {
                offsets.insert(index2);
            }
        }
    };

    while (window1 < length && window2 < length)
    {
        int suffix1 = (*m_suffixArray)[window1];
        int suffix2 = (*m_suffixArray)[window2];
        if (suffixSourcesMap.size() >= k)
        {
            updateMaxValue(suffix1, suffix2);

            decrementIndex(suffixSourcesMap, calcParentString(suffix1));
            window1++;

            if (window.front() <= window1)
                window.pop_front();
        }
        else if (++window2 < length)
        {
            suffixSourcesMap[calcParentString((*m_suffixArray)[window2])]++;

            while (!window.empty() && lcp[window.back()] >= lcp[window2])
                window.pop_back();
            window.push_back(window2);
        }
    }

    int suffix2 = *m_suffixArray->crbegin();
    while (++window1 < length && suffixSourcesMap.size() >= k)
    {
        int suffix1 = (*m_suffixArray)[window1];
        decrementIndex(suffixSourcesMap, calcParentString(suffix1 - 1));
        if (window.front() <= window1)
            window.pop_front();

        updateMaxValue(suffix1, suffix2);
    }

    // Print out results
    cout << "Length of the strand: " << max << endl;

    // cout << "The longest strand: ";
    // printSubStringVector(*m_concatedString, 0, max);

    int fileIndex;
    int fileOffset;
    for (auto it = offsets.begin(); it != offsets.end(); it++)
    {
        fileIndex = calcParentString(*it);
        cout << "File name & index: " << (*m_fileNames)[fileIndex - 1] << "(" << fileIndex << ")" << endl;

        if (fileIndex == 1)
            fileOffset = *it;
        else
            fileOffset = *it - (*m_suffixArray)[m_stringCount - fileIndex + 1] - 1;

        cout << "Offset: " << fileOffset << endl;
    }

    return max;
}

/*
    Returns which string a suffix is in
    i.e. first string, second string
*/
int SuffixArray::calcParentString(int suffixOffset)
{
    if (suffixOffset > (*m_suffixArray)[0])
    {
        cerr << "Offset is out of range: " << suffixOffset << endl;
        return -1;
    }

    if (m_stringCount <= 1)
        return m_stringCount;

    int high = m_stringCount - 1;
    int low = 0;
    int mid = 0;

    // Binary Search to find closest value
    while (low <= high)
    {
        mid = low + (high - low) / 2;
        if ((*m_suffixArray)[mid] == suffixOffset)
            return m_stringCount - mid;
        if ((*m_suffixArray)[mid] > suffixOffset)
            low = mid + 1;
        else
            high = mid - 1;
    }
    // Edge-case where offset is greater then the closest value
    if (suffixOffset > (*m_suffixArray)[mid])
        mid--;
    // Found the parent string
    return m_stringCount - mid;
}

/*
    Skew-Algorithim for constructing Suffix Arrays of an integer alphabet given a source from s[1...n]
    Assumes s[n] = s[n+1] = s[n+2] = unused sentinelCharacter
    Supports for a source consisting of different sentinel characters (for multiple given sources)
*/
void SuffixArray::skewSuffixArray(const vector<int> &s, vector<int> &SA, int length, int alphabetSize, int alphabetOffset)
{
    int n0 = (length + 2) / 3, n1 = (length + 1) / 3, n2 = length / 3, n02 = n0 + n2;
    vector<int> s12(n02 + 3); s12[n02] = s12[n02 + 1] = s12[n02 + 2] = m_sentinelCharacter;
    vector<int> SA12(n02 + 3); SA12[n02] = SA12[n02 + 1] = SA12[n02 + 2] = m_sentinelCharacter;
    vector<int> s0(n0);
    vector<int> SA0(n0);

    // Create Triple Indexes for s12
    for (int i = 0, j = 0; i < length + (n0 - n1); i++)
        if (i % 3 != 0)
            s12[j++] = i;

    // RadixSort to construct SA12
    radixSort(s12, SA12, s, 2, n02, alphabetSize, alphabetOffset);
    radixSort(SA12, s12, s, 1, n02, alphabetSize, alphabetOffset);
    radixSort(s12, SA12, s, 0, n02, alphabetSize, alphabetOffset);

    int name = 0, c0 = -1, c1 = -1, c2 = -1;

    for (int i = 0; i < n02; i++)
    {
        if (s[SA12[i]] != c0 || s[SA12[i] + 1] != c1 || s[SA12[i] + 2] != c2)
        {
            name++;
            c0 = s[SA12[i]]; c1 = s[SA12[i] + 1]; c2 = s[SA12[i] + 2];
        }
        if (SA12[i] % 3 == 1)
            s12[SA12[i] / 3] = name;
        else
            s12[SA12[i] / 3 + n0] = name;
    }

    // Create Suffix Array for s12
    // Recursion if names are not unique
    if (name < n02)
    {
        skewSuffixArray(s12, SA12, n02, name, alphabetOffset);
        for (int i = 0; i < n02; i++)
            s12[SA12[i]] = i + 1;
    }
    else
        for (int i = 0; i < n02; i++)
            SA12[s12[i] - 1] = i;

    // Construct suffix array for s0 and sort
    for (int i = 0, j = 0; i < n02; i++)
        if (SA12[i] < n0)
            s0[j++] = 3 * SA12[i];
    radixSort(s0, SA0, s, 0, n0, alphabetSize, alphabetOffset);

    // Merge SA0 and SA12 suffixes
    auto leqPair = [](int a1, int a2, int b1, int b2) -> bool {
        return a1 < b1 || a1 == b1 && a2 <= b2;
    };

    auto leqTriple = [leqPair](int a1, int a2, int a3, int b1, int b2, int b3) -> bool {
        return a1 < b1 || a1 == b1 && leqPair(a2, a3, b2, b3);
    };

    for (int p = 0, t = n0 - n1, k = 0; k < length; k++)
    {
        int i = SA12[t] < n0 ? SA12[t] * 3 + 1 : (SA12[t] - n0) * 3 + 2; // Offset of 12 suffix
        int j = SA0[p]; // Offset of 0 suffix
        if (SA12[t] < n0 ? leqPair(s[i], s12[SA12[t] + n0],s[j],s12[j / 3]) :
            leqTriple(s[i], s[i+1], s12[SA12[t] - n0 + 1], s[j],s[j + 1],s12[j / 3 + n0]))
        {
            SA[k] = i;
            t++;
            if (t == n02)
                for (k++; p < n0; p++, k++)
                    SA[k] = SA0[p];
        }
        else
        {
            SA[k] = j;
            p++;
            if (p == n0)
                for (k++; t < n02; t++, k++)
                    SA[k] = SA12[t] < n0 ? SA12[t] * 3 + 1 : (SA12[t] - n0) * 3 + 2;
        }
    }
}

/*
    From Skew Algorithim Implementation
    Allows for alphabet that includes negative integers
    Assumes alphabetOffset offsets the smallest character of the alphabet to 0
*/
void SuffixArray::radixSort(vector<int>& a, vector<int>& b, const vector<int> &r, int offset, int length, int alphabetSize, int alphabetOffset)
{
    vector<int> count(alphabetSize + 1 + alphabetOffset, 0);

    int i;
    for (i = 0; i < length; i++)
        count[r[a[i] + offset] + alphabetOffset]++;

    int sum;
    for (i = 0, sum = 0; i <= alphabetSize + alphabetOffset; i++)
    {
        int t = count[i];
        count[i] = sum;
        sum += t;
    }

    for (int i = 0; i < length; i++)
        b[count[r[a[i] + offset] + alphabetOffset]++] = a[i];

}

template <typename T> void SuffixArray::printVector(vector<T> v)
{
    for (auto i = v.begin(); i != v.end(); i++)
        cout << *i << ' ';

    cout << endl;
}

template <typename T> void SuffixArray::printSubStringVector(vector<T> v, int offset, int length, bool inHex)
{
    if (offset + length <= v.size())
    {
        for (auto i = offset; i < offset + length; i++)
        {
            if (inHex)
                cout << hex << v.at(i);
            else
                cout << (char)v.at(i);
        }

        cout << endl;
    }
    else
    {
        cerr << "Given offset & length for SubVector Print out of range" << endl;
        cerr << "Offset: " << offset << endl;
        cerr << "Length: " << length << endl;
        cerr << "Vector Size: " << v.size() << endl;
    }
}

void SuffixArray::printSuffixArray()
{
    cout << "Suffix Array : ";
    printVector(*m_suffixArray);
}
