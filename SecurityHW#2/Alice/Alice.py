import socket
import rsa


host = "127.0.0.1"
port = 10001

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


(pubKey, privKey) = rsa.newkeys(512)

n = str(pubKey.n).encode('utf-8')
e = str(pubKey.e).encode('utf-8')

sock.connect((host,port))

message = n + b'/' + e
sock.sendto(message,(host,port))



recevied = sock.recv(65565)
rawData = rsa.decrypt(recevied,privKey)
print("Received : ", rawData)
with open('example.txt', 'w') as f:
    f.write((rawData).decode('utf-8'))


sock.close()