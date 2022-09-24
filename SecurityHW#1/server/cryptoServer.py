from audioop import add
import socket
from Crypto.Util.Padding import pad, unpad
from Crypto.Cipher import DES
from Crypto.Cipher import DES3
from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes


def desEcb(data,child,address) :
    key = data[13:]
    decipher = DES.new(key, DES.MODE_ECB)
    if len(key) == 8 :
        child.sendto(b'/admit', address)
        cryptoData = child.recv(65565)
        #print("cryptoData:",cryptoData)
        decryptoData = decipher.decrypt(cryptoData) 
        decryptoData = unpad(decryptoData, DES.block_size)
        print("received!",decryptoData)
        saveFile("DES_ECB",decryptoData)
    else :
        return

def desCbc(data,child,address) :
    key = data[13:21]
    v = data[21:]
    if len(key) == 8 and len(v) == 8 :
        decipher = DES.new(key,DES.MODE_CBC,v)
        child.sendto(b'/admit', address)
        cryptoData = child.recv(65565)
        decryptoData = decipher.decrypt(cryptoData)
        decryptoData = unpad(decryptoData, DES.block_size)
        print("received!",decryptoData)
        saveFile("DES_CBC",decryptoData)
    else :
        return


def des3Ecb(data,child,address) :
    key = data[14:]
    if len(key) == 24:
        decipher = DES3.new(key,DES3.MODE_ECB)
        child.sendto(b'/admit', address)
        cryptoData = child.recv(65565)
        decryptoData = decipher.decrypt(cryptoData)
        decryptoData = unpad(decryptoData,DES3.block_size)
        print("received!",decryptoData)
        saveFile("DES3_ECB",decryptoData)
    else :
        return
    

def des3Cbc(data,child,address) :
    key = data[14:38]
    v = data[38:]
    if len(key) == 24 and len(v) == 8 :
        decipher = DES3.new(key,DES3.MODE_CBC,v)
        child.sendto(b'/admit', address)
        cryptoData = child.recv(65565)
        decryptoData = decipher.decrypt(cryptoData)
        decryptoData = unpad(decryptoData, DES3.block_size)
        print("received!",decryptoData)
        saveFile("DES3_CBC",decryptoData)
    else :
        return


def aesEcb(data,child,address) :
    key = data[13:]
    if len(key) == 16:
        decipher =  AES.new(key, AES.MODE_ECB)
        child.sendto(b'/admit', address)
        cryptoData = child.recv(65565)
        decryptoData = decipher.decrypt(cryptoData)
        decryptoData = unpad(decryptoData, AES.block_size)
        print("received!",decryptoData)
        saveFile("AES_ECB",decryptoData)
    else :
        return

def aesCbc(data,child,address) :
    key = data[13:29]
    v = data[29:]
    if len(key) == 16 and len(v) == 16 :
        decipher = AES.new(key,AES.MODE_CBC,v)
        child.sendto(b'/admit', address)
        cryptoData = child.recv(65565)
        decryptoData = decipher.decrypt(cryptoData)
        decryptoData = unpad(decryptoData, AES.block_size)
        print("received!",decryptoData)
        saveFile("AES_CBC",decryptoData)
    else :
        return

def saveFile(type,data) :
    with open(type + 'example.txt', 'w') as f:
        f.write(data.decode())
    

host = "127.0.0.1"
port = 10002

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

    print("received data: {}({}) bytes from {}".format(data,len(data),address))
    if data.startswith(b"/key/DES/ECB") == True :#and len(data.decode()) == 13 :
        desEcb(data,child,address)
    if data.startswith(b"/key/DES/CBC") == True :
        desCbc(data,child,address)
    if data.startswith(b"/key/DES3/ECB") == True :
        des3Ecb(data,child,address)
    if data.startswith(b"/key/DES3/CBC") == True :
        des3Cbc(data,child,address)
    if data.startswith(b"/key/AES/ECB") == True :
        aesEcb(data,child,address)
    if data.startswith(b"/key/AES/CBC") == True :
        aesCbc(data,child,address)
    print()
    print()
    
    child.close()
    parent.close()