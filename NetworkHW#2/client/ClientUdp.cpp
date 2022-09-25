#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

using namespace std;

class ClientUdpSocket {
   private:
    const int maxDataSize = 100; /* max number of bytes we can get at once */
    const int port = 20000;      /* the port client will be connecting to */
    int sockfd, numbytes;
    struct hostent* he;
    struct sockaddr_in their_addr; /* connector's address information */

   public:
    string sendPayloadReceiveData(const string& ipAddress, const string& payload) {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
            perror("socket");
            exit(1);
        }
        their_addr.sin_family = AF_INET;   /* host byte order */
        their_addr.sin_port = htons(port); /* short, network byte order */
        their_addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
        bzero(&(their_addr.sin_zero), 8); /* zero the rest of the struct */

        if (sendto(sockfd, payload.c_str(), payload.size(), 0, (struct sockaddr*)&their_addr, sizeof(struct sockaddr)) == -1) {
            perror("sendto");
            exit(1);
        }
        char buf[maxDataSize];
        if ((numbytes = recv(sockfd, buf, maxDataSize, 0)) == -1) {
            perror("recv");
            exit(1);
        }
        buf[numbytes] = '\0';
        if (close(sockfd) == -1) {
            perror("close");
            exit(1);
        }
        return string(buf);
    }
};

int main() {
    auto socketModule = make_unique<ClientUdpSocket>();
    string ipAddress = "127.0.0.1";
    cout << "Input expression" << endl;
    string inputExpression;
    cin >> inputExpression;
    const string&& result = socketModule->sendPayloadReceiveData(ipAddress, inputExpression);
    cout << "Result : " << result << endl;
    return 0;
}