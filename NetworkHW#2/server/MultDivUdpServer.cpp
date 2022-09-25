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
#include <iostream>
#include <memory>
#include <tuple>

using namespace std;

class ListenUdpServer {
   private:
    const int maxDataSize = 100;
    const int sendAddSubPort = 20002;
    int socketFd;                  /* listen on sockfd, new connection on newfd */
    struct sockaddr_in myAddr;     /* my address information */
    struct sockaddr_in their_addr; /* connector's address information */
    int numbytes;

    tuple<float, char, float> expTuple;

    void parseData(const string& data) {
        int pos = 0;
        string tmp;
        float leftOperand = 0.0;
        float rightOperand = 0.0;
        char op = NULL;
        int index = 0;
        for (const auto& c : data) {
            index++;
            if (c != ' ') {
                tmp += c;
            } else {
                if (pos == 0)
                    leftOperand = stof(tmp);
                else if (pos == 1) {
                    op = static_cast<char>(stoi(tmp));
                }
                tmp.clear();
                pos++;
                if (pos == 2) {
                    rightOperand = stof(data.substr(index));
                    break;
                }
            }
        }
        expTuple = make_tuple(leftOperand, op, rightOperand);
        cout << "Parse Output!" << get<0>(expTuple) << " " << get<1>(expTuple) << " " << get<2>(expTuple) << endl;
    }

    string calcData() {
        switch (get<1>(expTuple)) {
            case '*':
                return to_string(get<0>(expTuple) * get<2>(expTuple));
            case '/':
                return to_string(get<0>(expTuple) / get<2>(expTuple));
            default:
                cout << "Unknown Error!" << endl;
                exit(1);
        }
    }

   public:
    void startListen() {
        while (1) {
            if ((socketFd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {  // 1. Create TCP Socket
                perror("socket");
                exit(1);
            }
            myAddr.sin_family = AF_INET;                                                    /* host byte order */
            myAddr.sin_port = htons(sendAddSubPort); /* short, network byte order */  // 2. Set Server Port 3490
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
            parseData(string(buf));
            string&& result = calcData();
            if (sendto(socketFd, result.c_str(), result.size(), 0, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
                perror("send");
            }
            cout << "Conncetion Successful! Message! :" << buf << endl;
            close(socketFd);
        }
    }
};

int main() {
    auto test = make_unique<ListenUdpServer>();
    test->startListen();
    return 0;
}