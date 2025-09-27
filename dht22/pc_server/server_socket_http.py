import socket
import threading
from flask import Flask
app = Flask(__name__)

latest_data = "No data yet"
# --- Socket server (ESP32 connects here) ---
def socket_server():
    global latest_data
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(("0.0.0.0", 5000))  # ESP32 connects to port 6000
    server_socket.listen(1)
    print("Socket server listening on port 5000...")

    conn, addr = server_socket.accept()
    print("Connected by", addr)

    while True:
        data = conn.recv(1024)
        if not data:
            break
        if data.decode() != "\r\n":
            latest_data = data.decode()
            
        print("Received:", latest_data)

    conn.close()

# --- HTTP server (browser connects here) ---
@app.route('/')
def index():
    if len(latest_data.split(":")) > 1:
        sensor_data = latest_data.split(":")
    else:
        sensor_data = ["00.00","00.00","00.00","00.00"]
    return f"""
    <html>
      <head>
        <meta http-equiv="refresh" content="5">
        <style>
          body {{
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            background: #f7f7f7;
          }}
          .card {{
            background: white;
            padding: 20px 30px;
            border-radius: 12px;
            box-shadow: 0 4px 12px rgba(0,0,0,0.2);
            text-align: center;
          }}
        </style>
      </head>
      <body>
        <div class="card">
          <h1>ESP32 Data</h1>
          <p><b>Temperature:</b> {sensor_data[1]} °C</p>
          <p><b>Humidity:</b> {sensor_data[3]} %</p
        </div>
      </body>
    </html>
    """

if __name__ == '__main__':
    threading.Thread(target=socket_server, daemon=True).start()
    app.run(host='0.0.0.0', port=5001)
