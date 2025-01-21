#include <vector>
#include <stack>

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
};

class Solution {
public:
    std::vector<int> preorderTraversal(TreeNode* root) {
        std::vector<int> result;
        if (!root) return result;

        std::stack<TreeNode*> nodes;

        nodes.push(root);
        while (!nodes.empty())
        {
            auto *pNode = nodes.top();
            nodes.pop();
            result.push_back(pNode->val);
            if (pNode->right) nodes.push(pNode->right);
            if (pNode->left) nodes.push(pNode->left);
        }

        return result;
    }

    std::vector<int> inorderTraversal(TreeNode* root) {
        std::vector<int> result;
        if (!root) return result;

        std::stack<TreeNode*> nodes;

        nodes.push(root);
        while (!nodes.empty())
        {
            auto *pNode = nodes.top();
            while (pNode->left)
            {
                pNode = pNode->left;
                nodes.push(pNode);
            }

            nodes.pop();
            result.push_back(pNode->val);

            while (!pNode->right && !nodes.empty())
            {
                pNode = nodes.top();
                nodes.pop();
                result.push_back(pNode->val);
            }

            if (pNode->right) nodes.push(pNode->right);
        }

        return result;
    }
};

int main(int argc, char const *argv[]) {
    return 0;
}
