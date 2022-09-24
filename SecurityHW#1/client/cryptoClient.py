import socket
from Crypto.Util.Padding import pad, unpad
from Crypto.Cipher import DES
from Crypto.Cipher import DES3
from Crypto.Cipher import AES
from Crypto.Random import get_random_bytes


def desEcb(sock, host, port, fileData):
    key = b'8bit key'
    cipher = DES.new(key, DES.MODE_ECB)
    encryptPayload = cipher.encrypt(pad(fileData, DES.block_size))
    sock.sendto(b'/key/DES/ECB ' + key, (host, port))
    admit = sock.recv(65565)
    if admit.decode() == "/admit":
        sock.sendto(encryptPayload, (host, port))


def desCbc(sock, host, port, fileData):
    key = b'8bit key'
    v = b'12345678'
    cipher = DES.new(key, DES.MODE_CBC, v)
    encryptPayload = cipher.encrypt(pad(fileData, DES.block_size))
    sock.sendto(b'/key/DES/CBC ' + key + v, (host, port))
    admit = sock.recv(65565)
    if admit.decode() == "/admit":
        sock.sendto(encryptPayload, (host, port))


def des3Ecb(sock, host, port, fileData):
    key = b'This is twenty-four key!'
    cipher = DES3.new(key, DES3.MODE_ECB)
    encryptPayload = cipher.encrypt(pad(fileData, DES3.block_size))
    sock.sendto(b'/key/DES3/ECB ' + key, (host, port))
    admit = sock.recv(65565)
    if admit.decode() == "/admit":
        sock.sendto(encryptPayload, (host, port))


def des3Cbc(sock, host, port, fileData):
    while True:
        try:
            key = DES3.adjust_key_parity(get_random_bytes(24))
            break
        except ValueError:
            pass
    v = b'12345678'
    cipher = DES3.new(key, DES3.MODE_CBC, v)
    encryptPayload = cipher.encrypt(pad(fileData, DES3.block_size))
    sock.sendto(b'/key/DES3/CBC ' + key + v, (host, port))
    admit = sock.recv(65565)
    if admit.decode() == "/admit":
        sock.sendto(encryptPayload, (host, port))


def aesEcb(sock, host, port, fileData):
    key = b'Sixteen byte key'
    cipher = AES.new(key, AES.MODE_ECB)
    encryptPayload = cipher.encrypt(pad(fileData,AES.block_size))
    sock.sendto(b'/key/AES/ECB ' + key, (host, port))
    admit = sock.recv(65565)
    if admit.decode() == "/admit":
        sock.sendto(encryptPayload, (host, port))


def aesCbc(sock, host, port, fileData):
    key = b'Sixteen byte key'
    v = b'Sixteen   vector'
    cipher = AES.new(key, AES.MODE_CBC, v)
    encryptPayload = cipher.encrypt(pad(fileData,AES.block_size))
    sock.sendto(b'/key/AES/CBC ' + key + v, (host, port))
    admit = sock.recv(65565)
    if admit.decode() == "/admit":
        sock.sendto(encryptPayload, (host, port))


host = "127.0.0.1"
port = 10002

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


sock.connect((host, port))
with open('example.txt') as f:
    fileData = f.read().encode()
    print("1. DES_ECB\n2. DES_CBC\n3. 3DES_ECB\n4. 3DES_CBC\n5. AES_ECB\n6. AES_CBC")
    inputData = int(input())
    if inputData == 1:
        desEcb(sock, host, port, fileData)
    elif inputData == 2:
        desCbc(sock, host, port, fileData)
    elif inputData == 3:
        des3Ecb(sock, host, port, fileData)
    elif inputData == 4:
        des3Cbc(sock, host, port, fileData)
    elif inputData == 5:
        aesEcb(sock, host, port, fileData)
    elif inputData == 6:
        aesCbc(sock, host, port, fileData)
    else:
        print('uknown Input')
        exit(0)

sock.close()
