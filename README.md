# Longest-Strand

## Problem

Given a large number of binary files, write a program that finds the
longest strand of bytes that is identical between two or more files.

Use the test set **samples/sample.*** files.

The program should display:
- the length of the strand
- the file names where the largest strand appears
- the offset where the strand appears in each file

## Compiling & Building
In order to build the project, you will need to install:
- **CMake**
- **Make**

Clone the Repository.
```bash
git clone "https://github.com/sieman216485/Longest-Strand"
```
Compile using CMake.
```bash
cmake .
```
Build using the Makefile.
```bash
make
```
Run to view test code.
```
./LongestStrand
```
## Algorithms & Complexity

**Time: O(n), Space: O(n)**

The class **SuffixArray** represents a main class that construct the Suffix Array & LCP Array and find the logest strand of bytes that is in at least 2 files.
It uses **Skew** algorithm to contruct the Suffix Array and uses **Kasai** algorithm to contruct the LCP Array from the Suffix Array.
**Sliding Window Minimum Algorithm** is used to find the longest strand of bytes from the LCP Array.
