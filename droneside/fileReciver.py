import socket

def save_file(file_data, file_name):
    with open(file_name, 'wb') as file:
        file.write(file_data)

def start_server(ip, port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((ip, port))
    server_socket.listen(1)

    print(f"Server listening on {ip}:{port}")

    client_socket, client_address = server_socket.accept()

    print(f"Connected by {client_address}")

    file_data = client_socket.recv(1024)
    file_name = 'received_file.txt'  # Change this to the desired file name and extension
    save_file(file_data, file_name)

    client_socket.close()
    server_socket.close()

if __name__ == "__main__":
    IP = "your_server_ip"  # Change this to your server IP address
    PORT = 12345  # Change this to your desired port number

    start_server(IP, PORT)