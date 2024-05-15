from flask import Flask, render_template, send_file
from flask_socketio import SocketIO, emit
from threading import Lock
import time
import socket
import select
import threading
import time
import json
import re

app = Flask(__name__)
socketio = SocketIO(app)

is_started = False
thread = None
thread_lock = Lock()

server_ip = '127.0.0.1'
server_port = 2323
buffer_size = 4096

parsed_json = {}
graph_json = {}
image_json = {}

def find_json(data):
    str_data = data.decode('latin-1', errors='ignore')
    start = str_data.find('{') 
    end = str_data.rfind('}')
    
    if start >= 0 and end >= 0 and start < end:
        json_str = str_data[start:end + 1]
        return json_str
    return None

def tcp_listener():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.settimeout(None)
    sock.connect((server_ip, server_port))

    try:
        http_request = f"GET /Stream HTTP/1.1\r\nHost: {server_ip}\r\nConnection: keep-alive\r\n\r\n"
        sock.send(http_request.encode())
        
        while True:
            readable, _, _ = select.select([sock], [], [], 10)  # 10-second timeout
            if readable:
                data = sock.recv(buffer_size)   
                if data:
                    json_str = find_json(data)
                    if json_str:
                        try:
                            global parsed_json
                            global graph_json
                            global image_json
                            parsed_json = json.loads(json_str)
                            print("Extracted JSON data:")
                            print(parsed_json)
                            if "imagePath" in parsed_json:
                                image_json = parsed_json
                                socketio.emit('image_update', image_json)
                            else:
                                graph_json = parsed_json
                                socketio.emit('graph_update', graph_json)

                        except json.JSONDecodeError:
                            print("Invalid JSON content found.")
                    else:
                        print("No data found or binary content received:")
                else:
                    break
            else:
                continue
    finally:
        global is_started
        is_started = False
        sock.close()

@app.route('/image')
def get_image():
    global parsed_json
    return send_file(parsed_json['imagePath'])

@app.route('/')
def index():
    global thread
    global is_started

    with thread_lock:
        if thread is None and not is_started:
            is_started = True
            thread = socketio.start_background_task(tcp_listener)
        else:
            socketio.emit('image_update', image_json)
            socketio.emit('graph_update', graph_json)

    return render_template('index.html')

if __name__ == '__main__':
    socketio.run(app, debug=True, host='0.0.0.0', port=8000)