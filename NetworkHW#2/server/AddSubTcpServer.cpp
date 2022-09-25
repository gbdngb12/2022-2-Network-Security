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
#include <tuple>
#include <memory>

using namespace std;

class ListenTCPServer {
   private:
    const int port = 10001;
    const int backlog = 10;
    const int maxDataSize = 100;
    int socketFd, newFd;          /* listen on sockfd, new connection on newfd */
    struct sockaddr_in myAddr;    /* my address information */
    struct sockaddr_in theirAddr; /* connector's address information */
    socklen_t sin_size;
    int numbytes;
    tuple<float, char, float> expTuple;

    void parseData(const string& data) {
        int pos = 0;
        string tmp;
        float leftOperand = 0.0;
        float rightOperand = 0.0;
        char op = NULL;
        int index = 0;
        for(const auto& c : data) {
            index++;
            if(c != ' ') {
                tmp += c;
            } else {
                if(pos == 0)
                    leftOperand = stof(tmp);
                else if(pos == 1) {
                    op = static_cast<char>(stoi(tmp));
                }
                tmp.clear();
                pos++;
                if(pos == 2) {
                    rightOperand = stof(data.substr(index));
                    break;
                }
            }
        }
        expTuple = make_tuple(leftOperand,op,rightOperand);
       cout << "Parse Output!" << get<0>(expTuple)<<" "<<get<1>(expTuple)<< " " <<get<2>(expTuple) << endl;
    }

    string calcData() {
        switch(get<1>(expTuple)){
            case '+' :
                return to_string(get<0>(expTuple) + get<2>(expTuple));
            case '-' :
                return to_string(get<0>(expTuple) - get<2>(expTuple));
            default :
                cout<< "Unknown Error!" <<endl;
                exit(1);
        }
    }

   public:
    void startListen() {
        if ((socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {  // 1. Create TCP Socket
            perror("socket");
            exit(1);
        }
        myAddr.sin_family = AF_INET;                                     /* host byte order */
        myAddr.sin_port = htons(port); /* short, network byte order */   // 2. Set Server Port 3490
        myAddr.sin_addr.s_addr = INADDR_ANY; /* auto-fill with my IP */  // 3. Set Server IP Address
        bzero(&(myAddr.sin_zero), 8);                                    /* zero the rest of the struct */
        socklen_t reuseAddress = 1;
        if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuseAddress, sizeof(reuseAddress)) == -1) {
            perror("setsockopt");  // Port Reuse Option!
        }

        if (bind(socketFd, (struct sockaddr *)&myAddr, sizeof(struct sockaddr)) == -1) {  // 4. Bind IP, Port
            perror("bind");
            exit(1);
        }

        if (listen(socketFd, backlog) == -1) {  // 5. Listen! => TCP 3단계 Connection
            perror("listen");
            exit(1);
        }

        while (1) { /* main accept() loop */
            sin_size = sizeof(struct sockaddr_in);
            if ((newFd = accept(socketFd, (struct sockaddr *)&theirAddr,
                                &sin_size)) == -1) {  // 6. Accept!
                perror("accept");
                continue;
            }
            cout << "server: got connection from " << inet_ntoa(theirAddr.sin_addr) << endl;
            if (!fork()) { /* this is the child process */  // 7. then, Child Process do Any Jobs..
                char buf[maxDataSize];
                if ((numbytes = recv(newFd, buf, maxDataSize, 0)) == -1) {
                    perror("recv");
                    exit(1);
                }
                buf[numbytes] = '\0';
                parseData(string(buf));
                string&& result = calcData();
                if (send(newFd, result.c_str(), result.size(), 0) == -1)
                    perror("send");
                close(newFd);
                exit(0);
            }
            close(newFd); /* parent doesn't need this */
            while (waitpid(-1, NULL, WNOHANG) > 0)
                ; /* clean up child processes */
        }
    }
};

int main() {
    auto test = make_unique<ListenTCPServer>();
    test->startListen();
    return 0;
}
