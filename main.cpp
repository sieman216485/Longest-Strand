#include "SuffixArray.h"

int main()
{
    SuffixArray sa;

#if 0

    string testString1("aba");
    string testString2("xab");
    string testString3("1");
    string testString4("211abz");

    cout << "Test String 1: " << testString1 << endl;
    cout << "Test String 2: " << testString2 << endl;
    cout << "Test String 3: " << testString3 << endl;
    cout << "Test String 4: " << testString4 << endl;

    sa.addString(testString1);
    sa.addString(testString2);
    sa.addString(testString3);
    sa.addString(testString4);

#else

    int i;
    string fileName;
    for (i = 1; i <= 10; i++)
    {
        fileName = "samples/sample." + to_string(i);

        sa.addStringFromFile(fileName);
    }

#endif

    // sa.addStringFromFile("samples/sample.1");
    // sa.addStringFromFile("samples/sample.2");
    // sa.addStringFromFile("samples/sample.3");
    // sa.addStringFromFile("samples/sample.4");
    // sa.addStringFromFile("samples/sample.5");
    // sa.addStringFromFile("samples/sample.6");
    // sa.addStringFromFile("samples/sample.7");
    // sa.addStringFromFile("samples/sample.8");
    // sa.addStringFromFile("samples/sample.9");
    // sa.addStringFromFile("samples/sample.10");

    sa.makeSuffixArray();
    // sa.printSuffixArray();
    sa.findLongestCommonStrand(2);

    return 0;
}
