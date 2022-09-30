import socket
import rsa
from Crypto.Cipher import AES
from Crypto.Util.Padding import pad, unpad


def toPubKey(data) :
    data = data.decode('utf-8').split('/')
    pubKeyn = int(data[0])
    pubKeye = int(data[1])

    pubKey = rsa.PublicKey(pubKeyn, pubKeye)
    return pubKey

def aesDecrypt(aesKey, data) :
    decipher =  AES.new(aesKey, AES.MODE_ECB)
    decryptoData = decipher.decrypt(data)
    decryptoData = unpad(decryptoData, AES.block_size)
    return decryptoData



host = "127.0.0.1"
port = 10001

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)



sock.connect((host,port))
sock.sendto(b"/requestFile",(host,port))
recevied = sock.recv(65565)
pubKey = toPubKey(recevied)


aesKey = b'Sixteen byte key'
aesKeyCrypto = rsa.encrypt(aesKey,pubKey)
sock.sendto(aesKeyCrypto,(host,port))


cryptoData = sock.recv(65565)
decryptoData = aesDecrypt(aesKey,cryptoData)


print("received!",decryptoData)

with open('example.txt', 'w') as f:
    f.write((decryptoData).decode('utf-8'))

sock.close()