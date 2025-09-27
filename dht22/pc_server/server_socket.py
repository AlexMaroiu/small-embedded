import socket

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(("0.0.0.0", 5000))
server_socket.listen(1)
print("Server listening on port 5000...")

conn, addr = server_socket.accept()
print("Connected by", addr)

while True:
    data = conn.recv(1024)
    if not data:
        break
    print("Received:", data.decode())

conn.close()
print("closed")