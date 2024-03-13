import socket

def ascii2hex(ascii_message):
    # Convert ASCII characters to hexadecimal representation
    hex_message = ''.join(format(ord(c), '02x') for c in ascii_message)
    return hex_message

def hex2bytes(hex_message):
    # Convert hexadecimal representation to ASCII characters
    hex_message = hex_message.replace(' ', '')
    message = bytes.fromhex(hex_message)
    return message


# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('192.168.206.42', 502)
print('connecting to {} port {}'.format(*server_address))
sock.connect(server_address)

while True:
    function = input("Presiona h para ver registros, s para cambiar un registro: ")
    if function == "h":
        register = input("presiona el numero de registros a imprimir: ")
        msg = hex2bytes("6132 0000 0000 0006 0103 0000 000{} 5555".format(register))
    elif function  == "s":
        register = input("presiona en numero del registro a cambiar: ")
        value = input("presiona el nuevo valor del registro: ")
        msg = hex2bytes("6132 0000 0000 0006 0106 000{} 000{} 5555".format(register,value))
    else:
        print("entrada inválida")
        continue
    print(msg)
    sock.sendall(msg)

    # Look for the response
    amount_received = 0
    amount_expected = len(msg) + 10

    dataString = ""
    data = b''
    while dataString[-2:] != "UU":
        newByte = sock.recv(1)
        data += newByte
        dataString += newByte.decode()
    a2 = dataString.index("a2")+2
    UU = dataString.index("UU")
    print(data[a2:UU].hex())
        # print('received {!r}'.format(data.hex()))
print('closing socket')
sock.close()
