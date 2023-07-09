import socket
import os


from dronecontrole import DronController
###########################################################################
# Import Drone traversing methode python and Drone controle code          #
###########################################################################


# Define server address and port
SERVER_ADDRESS = '192.168.225.43'
SERVER_PORT = 2222
EXIT = False
DRONE = DronController(20, 4)
# DRONE.startup()

def handle_client(client_socket, client_address):
    global EXIT
    message = ""
    while True:
        data = client_socket.recv(1024).decode('utf-8').strip()
        if data is not None:
            # Receive data from the client
            message += data

            # Check if the message ends with "END"
            if message.endswith("END"):
                # Process the complete message (you can add your custom logic here)
                message = message[:-3]  # Remove "END" from the message

                # Check if the received message is "LOGOUT"
                if message == "LOGOUT" or message == "EXIT":
                    # Send a response to the client
                    response = "Goodbye!"
                    client_socket.send(response.encode('utf-8'))
                    client_socket.shutdown(socket.SHUT_WR)
                    if message == "EXIT":
                        EXIT = True
                    ###########################################################################
                    # Clean up DRON data for next task                                        #
                    ###########################################################################
                    break  # Terminate the connection and exit the loop
                
                droneDataPath = os.join(os.getcwd(), 'drone.data')
                with open(droneDataPath, 'wb') as f:
                    f.write(message)

                # Send a response back to the client
                #####################################################################################################
                response = "Hello from the server!" # This should be placed with Drone travers                      #
                # Initialy go to DRONE start point before starting traversing                                       #
                # DRONE.goto(home.X, home.Y)                                                                        #
                # Use only DRONE.goto(vertex.X, vertex.Y) function and when travers was done                        #
                # use DRONE.missinon_end() to done processing and LAND Drone, then sand response back to the client #
                #####################################################################################################
                client_socket.send(response.encode('utf-8'))
                client_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
                message = ""  # Reset the message for the next iteration

    # Close the client socket
    client_socket.close()
    print(f"Closed connection with {client_address}")

# Create a TCP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Bind the server socket to the specified address and port
server_socket.bind((SERVER_ADDRESS, SERVER_PORT))

# Listen for incoming connections
server_socket.listen(1)
print("Server is listening for incoming connections...")

while True:
    # Accept a client connection
    client_socket, client_address = server_socket.accept()
    print(f"Accepted connection from {client_address}")

    # Handle the client connection in a separate function
    handle_client(client_socket, client_address)
    if EXIT:
        exit(0)
