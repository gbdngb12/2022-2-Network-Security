import socket

host = "127.0.0.1"
port = 10001

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


sock.connect((host,port))
with open('example.txt') as f:
    #message = b'This is the message.'
    message = f.read().encode()
    sock.sendto(message,(host,port))
    #print("sending {}".format(message))
    recevied = sock.recv(65565)
    print("Received : ", recevied)

sock.close()