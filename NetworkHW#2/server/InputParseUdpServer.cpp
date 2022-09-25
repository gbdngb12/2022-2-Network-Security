//#include "InputParseServer.h"
/*
    C Language Header for Socket Programming
*/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
/*
    C++ Language Header for Input Parse
*/
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <vector>

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
    vector<string> resultExp;
    void pushOperator(const char &op) {
        // op is Valid Key
        int top = Operator::operatorMap[mStack.top()];
        int cur = Operator::operatorMap[op];
        if (Operator::operatorMap[')'] == cur) {
            while (top != Operator::operatorMap['(']) {
                // resultExp += mStack.top();
                resultExp.push_back(to_string(mStack.top()));
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
            // resultExp += mStack.top();
            resultExp.push_back(to_string(mStack.top()));
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

    bool checkNegative(const string &infix, int pos) {
        //앞에연산자가 있거나 첫글자일경우
        if (infix[pos] != '-')
            return false;
        else {
            if (pos == 0)
                return true;                        //처음에 -인경우 음수
            else if (isOperator(infix[pos - 1])) {  //앞글자가 연산자인경우 음수
                return true;
            }
            return false;
        }
    }

   public:
    const vector<string> &infixToPostfix(string &infix) {
        string tmp;
        int index = 0;
        for (char &s : infix) {
            if (!checkNegative(infix, index) && isOperator(static_cast<const char &>(s))) {  // is Operator!
                if (!tmp.empty()) {
                    resultExp.push_back(tmp);
                    tmp.clear();
                }
                if (mStack.empty()) {  // if Stack is Empty push
                    mStack.push(s);
                } else {  // if Stack is Not Empty
                    pushOperator(static_cast<const char &>(s));
                }
            } else {  // is Not Operator
                tmp += s;
            }
            index++;
        }
        if (!tmp.empty()) {
            resultExp.push_back(tmp);
            tmp.clear();
        }
        while (!mStack.empty()) {
            // cout << mStack.top();
            // resultExp += mStack.top();
            resultExp.push_back(to_string(mStack.top()));
            mStack.pop();
        }
        return resultExp;
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
   public:
    string mexp;
    Node *mleft;
    Node *mright;
    static char intToChar(const int &n) {
        return n + 48;
    }
    Node() = default;
    Node(const string &exp) {
        mexp = exp;
    }
};

class ListenUDPServer {
   private:
    const int listenInputParsePort = 20000;
    const int sendAddSubPort = 20001;
    const int sendMulDivPort = 20002;
    const int maxDataSize = 100;
    int socketFd;                  /* listen on sockfd, new connection on newfd */
    struct sockaddr_in myAddr;     /* my address information */
    struct sockaddr_in their_addr; /* connector's address information */
    int numbytes;
    socklen_t addrlen;
    const char *serverAddress = "127.0.0.1";
    string createPayload(const float &leftOperand, const float &rightOperand, const char &op) {
        return to_string(leftOperand) + " " + to_string(op) + " " + to_string(rightOperand);
    }

    string getData(const int &port, const char *payload, int size) {
        if ((socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
            perror("socket");
            exit(1);
        }
        their_addr.sin_family = AF_INET;   /* host byte order */
        their_addr.sin_port = htons(port); /* short, network byte order */
        their_addr.sin_addr.s_addr = inet_addr(serverAddress);
        bzero(&(their_addr.sin_zero), 8); /* zero the rest of the struct */

        if (sendto(socketFd, payload, size, 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
            perror("sendto");
            exit(1);
        }
        char buf[maxDataSize];
        if ((numbytes = recv(socketFd, buf, maxDataSize, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        if (close(socketFd) == -1) {
            perror("close");
            exit(1);
        }
        return string(buf);
    }

   protected:
    float getAddSubData(const float &leftOperand, const float &rightOperand, const char &op) {
        string &&exp = createPayload(leftOperand, rightOperand, op);
        string &&res = getData(sendAddSubPort, exp.c_str(), exp.size());
        cout << res << endl;
        return stof(res);
    }
    float getMulDivData(const float &leftOperand, const float &rightOperand, const char &op) {
        string &&exp = createPayload(leftOperand, rightOperand, op);
        string &&res = getData(sendMulDivPort, exp.c_str(), exp.size());
        cout << res << endl;
        return stof(res);
    }

   public:
    void startListen(auto makeTreeModule) {
        while (1) {                                                             /* main accept() loop */
            if ((socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {  // 1. Create TCP Socket
                perror("socket");
                exit(1);
            }

            myAddr.sin_family = AF_INET;                                                    /* host byte order */
            myAddr.sin_port = htons(listenInputParsePort); /* short, network byte order */  // 2. Set Server Port 3490
            myAddr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */                 // 3. Set Server IP Address
            bzero(&(myAddr.sin_zero), 8);                                                   /* zero the rest of the struct */
            char buf[maxDataSize];
            socklen_t reuseAddress = 1;
            if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseAddress, sizeof(reuseAddress)) == -1) {
                perror("setsockopt");  // Port Reuse Option!
            }

            if (bind(socketFd, (struct sockaddr *)&myAddr, sizeof(struct sockaddr)) == -1) {  // 4. Bind IP, Port
                perror("bind");
                exit(1);
            }
            socklen_t addr_len = sizeof(struct sockaddr);
            if ((numbytes = recvfrom(socketFd, buf, maxDataSize, 0,
                                     (struct sockaddr *)&their_addr, &addr_len)) == -1) {
                perror("recvfrom");
                exit(1);
            }
            buf[numbytes] = '\0';
            cout << "Input: " << buf << endl;
            string userInput(buf);
            auto infixToPostModule = make_unique<InfixToPostfix>();
            const vector<string> &tmp = infixToPostModule->infixToPostfix(userInput);
            const Node *postRoot = makeTreeModule->makeExpTree(tmp);
            string result = to_string(makeTreeModule->calc(postRoot));
            //string result = "test";
            if (sendto(socketFd, result.c_str(), result.size(), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
                perror("send");
            }
            cout << "Conncetion Successful! Message! :" << buf << endl;
            close(socketFd);
        }
    }
};

class MakeExpTree : ListenUDPServer {
   private:
    stack<Node *> mStack = stack<Node *>();
    const Node *mRoot = nullptr;
    auto createNode(const string &exp) {
        auto node = new Node(exp);
        node->mleft = nullptr;
        node->mright = nullptr;
        return node;
    }

    auto makeTree(const string &op, Node *const leftNode, Node *const rightNode) {
        auto rootNode = createNode(op);
        rootNode->mleft = leftNode;
        rootNode->mright = rightNode;
        return rootNode;
    }

    void freeTree(const Node *root) {
        if (root) {
            freeTree(root->mleft);
            freeTree(root->mright);
            if (InfixToPostfix::isOperator(static_cast<const char &>(stoi(root->mexp)))) {
                cout << root->mexp << " ";
                delete root;
            } else {
                cout << Node::intToChar(static_cast<const char &>(stoi(root->mexp))) << " ";
                delete root;
            }
        }
    }

   public:
    ~MakeExpTree() {
        freeTree(mRoot);
    }
    const Node *makeExpTree(const vector<string> &postFix) {
        for (const string &s : postFix) {
            if (InfixToPostfix::isOperator(static_cast<const char &>(stoi(s)))) {  // if Is Operator
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
    void postOrder(const Node *root) {
        if (root) {
            postOrder(root->mleft);
            postOrder(root->mright);
            cout << root->mexp << " ";
        }
    }

    float calc(const Node *const root) {
        if (root->mleft == nullptr && root->mright == nullptr)
            return stof(root->mexp);
        float leftOperand = calc(root->mleft);
        float rightOperand = calc(root->mright);
        switch (static_cast<char>(stoi(root->mexp))) {
            case '+':
                // cout<<"test "<< leftOperand << root->mexp <<rightOperand<<endl;
                return ListenUDPServer::getAddSubData(leftOperand, rightOperand, '+');  // leftOperand + rightOperand;  // Call Server
            case '-':
                // cout<<"test "<< leftOperand << root->mexp <<rightOperand<<endl;
                return ListenUDPServer::getAddSubData(leftOperand, rightOperand, '-');  // Call Server
            case '*':
                // cout<<"test "<< leftOperand << root->mexp <<rightOperand<<endl;
                return ListenUDPServer::getMulDivData(leftOperand, rightOperand, '*');  // Call Server
            case '/':
                // cout<<"test "<< leftOperand << root->mexp <<rightOperand<<endl;
                return ListenUDPServer::getMulDivData(leftOperand, rightOperand, '/');  // Call Server
            default:
                cout << "What is This ?!?" << root->mexp << endl;
                exit(1);
        }
    }
};

int main() {
    auto test = make_unique<ListenUDPServer>();
    auto makeTreeModule = make_shared<MakeExpTree>();
    test->startListen(makeTreeModule);
    return 0;
}