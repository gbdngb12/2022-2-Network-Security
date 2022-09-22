#include <iostream>
#include <map>
#include <memory>
#include <stack>

using namespace std;

typedef struct node {
    int exp;
    struct node *left;
    struct node *right;
} node;

class InfixToPostfix {
   private:
    map<char, int> operatorMap = {
        {'+', 1},
        {'-', 1},
        {'*', 2},
        {'/', 2},
        {'(', 3},
        {')', 4}};
    stack<char> stackPointer = stack<char>();

    bool isOperator(const char &c) const {
        if (operatorMap.count(c)) {  // if is Operator
            return true;
        } else {
            return false;  // if is Not Operator
        }
    }

    void pushOperator(const char &op) {
        // op is Valid Key
        int top = operatorMap[stackPointer.top()];
        int cur = operatorMap[op];
        if (operatorMap[')'] == cur) {
            while (top != operatorMap['(']) {
                cout << stackPointer.top();
                stackPointer.pop();
                if (!stackPointer.empty()) {
                    top = operatorMap[stackPointer.top()];
                } else {
                    return;
                }
                continue;
            }
            stackPointer.pop();
            return;
        }
        while (top >= cur) {
            if (operatorMap['('] == top) {
                stackPointer.push(op);
                return;
            }
            cout << stackPointer.top();
            stackPointer.pop();
            if (!stackPointer.empty()) {
                top = operatorMap[stackPointer.top()];
                continue;
            } else {
                break;
            }
        }
        stackPointer.push(op);
        return;
    }

   public:
    void infixToPostfix(string &infix) {
        for (char &s : infix) {
            if (isOperator(static_cast<const char &>(s))) {  // is Operator!
                if (stackPointer.empty()) {                 // if Stack is Empty push
                    stackPointer.push(s);
                } else {  // if Stack is Not Empty
                    pushOperator(static_cast<const char &>(s));
                }
            } else {  // is Not Operator
                cout << s;
            }
        }
        while (!stackPointer.empty()) {
            cout << stackPointer.top();
            stackPointer.pop();
        }
    }
};

class MakeExpTree {

};


int main() {
    string testString = "A+B/C*D*(E+F)";
    auto test = make_unique<InfixToPostfix>();
    test->infixToPostfix(testString);
    return 0;
}