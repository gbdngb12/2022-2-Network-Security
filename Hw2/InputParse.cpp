#include <iostream>
#include <map>
#include <memory>
#include <stack>

using namespace std;

namespace Operator {
map<char, int> operatorMap = {
    {'+', 1},
    {'-', 1},
    {'*', 2},
    {'/', 2},
    {'(', 3},
    {')', 4}};
}
class InfixToPostfix {
   private:
    stack<char> mStack = stack<char>();

    void pushOperator(const char &op) {
        // op is Valid Key
        int top = Operator::operatorMap[mStack.top()];
        int cur = Operator::operatorMap[op];
        if (Operator::operatorMap[')'] == cur) {
            while (top != Operator::operatorMap['(']) {
                cout << mStack.top();
                mStack.pop();
                if (!mStack.empty()) {
                    top = Operator::operatorMap[mStack.top()];
                } else {
                    return;
                }
                continue;
            }
            mStack.pop();
            return;
        }
        while (top >= cur) {
            if (Operator::operatorMap['('] == top) {
                mStack.push(op);
                return;
            }
            cout << mStack.top();
            mStack.pop();
            if (!mStack.empty()) {
                top = Operator::operatorMap[mStack.top()];
                continue;
            } else {
                break;
            }
        }
        mStack.push(op);
        return;
    }

   public:
    void infixToPostfix(string &infix) {
        for (char &s : infix) {
            if (isOperator(static_cast<const char &>(s))) {  // is Operator!
                if (mStack.empty()) {                        // if Stack is Empty push
                    mStack.push(s);
                } else {  // if Stack is Not Empty
                    pushOperator(static_cast<const char &>(s));
                }
            } else {  // is Not Operator
                cout << s;
            }
        }
        while (!mStack.empty()) {
            cout << mStack.top();
            mStack.pop();
        }
    }
    static bool isOperator(const char &c) {
        if (Operator::operatorMap.count(c)) {  // if is Operator
            return true;
        } else {
            return false;  // if is Not Operator
        }
    }
};

class Node {
    private :
    int charToInt(const char &c) {
        return c - 48;
    }
   public:
    char mexp;
    Node *mleft;
    Node *mright;
    static char intToChar(const int& n) {
        return n + 48;
    }
    Node() = default;
    Node(const char &exp) {
        if (InfixToPostfix::isOperator(exp)) {
            mexp = exp;
        } else {
            mexp = charToInt(exp);
        }
    }
};

class MakeExpTree {
   private:
    stack<Node *> mStack = stack<Node *>();
    const Node *mRoot = nullptr;
    auto createNode(const char &exp) {
        auto node = new Node(exp);
        node->mleft = nullptr;
        node->mright = nullptr;
        return node;
    }

    auto makeTree(const char &op, Node *const leftNode, Node *const rightNode) {
        auto rootNode = createNode(op);
        rootNode->mleft = leftNode;
        rootNode->mright = rightNode;
        return rootNode;
    }

    void freeTree(const Node* root) {
        if(root) {
            freeTree(root->mleft);
            freeTree(root->mright);
            if(InfixToPostfix::isOperator(root->mexp))
            {
                cout << root->mexp << " " ;
                delete root;
            }
            else {
                cout <<  Node::intToChar(root->mexp) << " ";
                delete root;
            } 
        }
    }
   public:

    ~MakeExpTree() {
        freeTree(mRoot);
    }
    const Node *makeExpTree(const string &postFix) {
        for (const char &s : postFix) {
            if (InfixToPostfix::isOperator(static_cast<const char &>(s))) {  // if Is Operator
                Node *rightNode = mStack.top();
                mStack.pop();
                Node *leftNode = mStack.top();
                mStack.pop();
                mStack.push(makeTree(s, leftNode, rightNode));
            } else {  // if is Not Operator
                Node *operandNode = createNode(s);
                mStack.push(operandNode);
            }
        }
        mRoot = mStack.top();
        mStack.pop();
        return mRoot;
    }

    float calc(const Node *const root) {
        if (root->mleft == nullptr && root->mright == nullptr)
            return root->mexp;

        float leftOperand = calc(root->mleft);
        float rightOperand = calc(root->mright);
        switch (root->mexp) {
            case '+':
                return leftOperand + rightOperand; //Call Server
            case '-':
                return leftOperand - rightOperand; //Call Server
            case '*':
                return leftOperand * rightOperand; //Call Server
            case '/':
                return leftOperand / rightOperand; //Call Server
            default:
                cout << "What is This ?!?" << root->mexp << endl;
                exit(1);
        }
    }
};


int main() {
    // string testString = "A+B/C*D*(E+F)";
    // testString = "(3+5)*2";
    // auto test = make_unique<InfixToPostfix>();
    // test->infixToPostfix(testString);
    string testString = "183/2*13+*+";
    auto test = make_unique<MakeExpTree>();
    const Node *root = test->makeExpTree(testString);
    cout << test->calc(root) << endl;
    return 0;
}