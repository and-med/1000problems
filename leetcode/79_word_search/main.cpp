#include <iostream>
#include <vector>
#include <queue>
#include <set>
using namespace std;

class Solution {
public:
    bool dfs(vector<vector<char>>& board, string word, int x, int y, int dist) {
        if (dist >= word.length()) {
            return true;
        }

        if (x < 0 || x >= board.size() || y < 0 || y >= board[0].size() || board[x][y] != word[dist]) {
            return false;
        }

        char old = board[x][y];
        board[x][y] = '!';

        bool any = 
            dfs(board, word, x + 1, y, dist + 1) ||
            dfs(board, word, x - 1, y, dist + 1) ||
            dfs(board, word, x, y + 1, dist + 1) ||
            dfs(board, word, x, y - 1, dist + 1);

        board[x][y] = old;

        return any;
    }

    bool exist(vector<vector<char>>& board, string word) {
        int m = board.size();
        int n = board[0].size();

        for (int i = 0; i < m; i++) {
            for (int j = 0; j < n; j++) {
                if (dfs(board, word, i, j, 0)) {
                    return true;
                }
            }
        }

        return false;
    }
};

int main() {
    struct test{
        vector<vector<char>> board;
        string word;
        bool expected;

        test(const vector<vector<char>>& board, const string& word, bool expected): board(board), word(word), expected(expected) {}
    };

    vector<test> tests = {
        {
            {
                {'A','B','C','E'},
                {'S','F','C','S'},
                {'A','D','E','E'},
            },
            "ABCCED",
            true
        },
        {
            {
                {'A','B','C','E'},
                {'S','F','C','S'},
                {'A','D','E','E'},
            },
            "SEE",
            true
        },
        {
            {
                {'A','B','C','E'},
                {'S','F','C','S'},
                {'A','D','E','E'},
            },
            "ABCB",
            false
        },
        {
            {
                {'a', 'b'}
            },
            "ba",
            true
        },
        {
            {
                {'a', 'b'},
                {'c', 'd'},
            },
            "acdb",
            true
        },
    };
    
    for (int i = 0; i < tests.size(); i++) {
        auto t = tests[i];

        Solution sol;
        bool actual = sol.exist(t.board, t.word);

        if (actual == t.expected) {
            cout << "test " << i << ": was successful!" << endl;
        } else {
            cout << "test " << i << ": failed! expected: " << t.expected << " but got: " << actual << endl;
        }
    }
}
