#include<iostream>
#include<algorithm>
using namespace std;

class Solution {
public:
    int minDistance(string word1, string word2) {
        int m = word1.length();
        int n = word2.length();

        int** _minDist = new int*[m + 1];
        for (int i = 0; i <= m; i++) {
            _minDist[i] = new int[n + 1];
        }

        for (int i = 0; i <= m; i++) {
            _minDist[i][0] = i;
        }

        for (int j = 0; j <= n; j++) {
            _minDist[0][j] = j;
        }

        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                _minDist[i][j] = min(_minDist[i-1][j] + 1, _minDist[i][j-1] + 1); //one addition or removal
                if (word1[i - 1] == word2[j - 1]) {
                    _minDist[i][j] = min(_minDist[i][j], _minDist[i-1][j-1]); // same
                } else {
                    _minDist[i][j] = min(_minDist[i][j], _minDist[i-1][j-1] + 1); // one replacement
                }
            }
        }

        return _minDist[m][n];
    }
};

int main() {
    Solution sol;
    cout << R"(minDist("horse", "ros") = )" << sol.minDistance("horse", "ros") << endl;
    cout << R"(minDist("intention", "execution") = )" << sol.minDistance("intention", "execution") << endl;
    cout << R"(minDist("thisisatestsentence", "thisisasentence") = )" << sol.minDistance("thisisatestsentence", "thisisasentence") << endl;
    cout << R"(minDist("sea", "eat") = )" << sol.minDistance("sea", "eat") << endl;
    cout << R"(minDist("park", "spake") = )" << sol.minDistance("park", "spake") << endl;
    cout << R"(minDist("abcdefghij", "klmnopqrsa") = )" << sol.minDistance("abcdefghij", "klmnopqrsa") << endl;
    cout << R"(minDist("pneumonoultramicroscopicsilicovolcanoconiosis", "unmicroscopically") = )" << sol.minDistance("pneumonoultramicroscopicsilicovolcanoconiosis", "unmicroscopically") << endl;
    cout << R"(minDist("a", "a") = )" << sol.minDistance("a", "a") << endl;
}