#include <iostream>
#include <vector>
#include <functional>
using namespace std;

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

class Solution {
public:
    vector<vector<int>> pathSum(TreeNode *root, int targetSum) {
        vector<vector<int>> solutions;
        vector<int> currentSolution;

        std::function<void(TreeNode*, int)> findPaths = [&](TreeNode* root, int currentSum) {
            if (root == nullptr) {
                return;
            }

            currentSolution.push_back(root->val);

            if (root->left == nullptr && root->right == nullptr && root->val + currentSum == targetSum) {
                solutions.push_back(vector<int>(currentSolution.begin(), currentSolution.end()));
            }

            findPaths(root->left, root->val + currentSum);
            findPaths(root->right, root->val + currentSum);

            currentSolution.pop_back();
            return;
        };

        findPaths(root, 0);
        return solutions;
    }
};

int main() {
}
