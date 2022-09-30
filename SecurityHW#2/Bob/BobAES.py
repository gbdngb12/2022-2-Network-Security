import socket
import rsa
from Crypto.Util.Padding import pad, unpad
from Crypto.Cipher import AES

def aesEncrypt(key, data) :
    cipher = AES.new(key, AES.MODE_ECB)
    return cipher.encrypt(pad(data,AES.block_size))

host = "127.0.0.1"
port = 10001


(pubKey, privKey) = rsa.newkeys(512)

n = str(pubKey.n).encode('utf-8')
e = str(pubKey.e).encode('utf-8')


while True :
    parent = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
    #TCP 소켓 객체 생성

    parent.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    #소켓객체 종료후 해당 포트 번호 재사용
    parent.bind((host,port))

    parent.listen(10)
    #3단계 연결 설정에 따라 동작하는 TCP 클라이언트10대로부터 연결 요청을 기다린다.(최대 10대 TCP 클라이언트 접속가능)

    (child, address) = parent.accept() #parent process에서 기다리다가 받으면 child process에 socket객체와 address에 넘김

    data = child.recv(65565)
    if data[0:12] == b"/requestFile":
        print("received data: {}({}) bytes from {}".format(data,len(data),address))
        child.sendto(n + b'/' + e, address)

    aesKeyCrypto = child.recv(65565)
    aesKey = rsa.decrypt(aesKeyCrypto,privKey)


    with open('example.txt') as f:
        message = f.read().encode()
        crypto = aesEncrypt(aesKey, message)
        child.sendto(crypto,address)
        child.close()
        
    parent.close()



