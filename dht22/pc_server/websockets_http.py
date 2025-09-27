from websocket_server import WebsocketServer
import threading
import json

# Store latest data
latest_data = {}

# Keep track of browser clients
browser_clients = []

# Called for any client connecting (ESP32 or browser)
def new_client(client, server):
    print(f"New client connected: {client['id']}")
    # For browsers, we will add to browser_clients list
    if client['id'] not in browser_clients:
        browser_clients.append(client)

# Called for any client disconnecting
def client_left(client, server):
    if client in browser_clients:
        browser_clients.remove(client)
    print(f"Client disconnected: {client['id']}")

# Called when a message is received
def message_received(client, server, message):
    global latest_data
    try:
        data = json.loads((message[14:])[:-1])
        latest_data = data
        print("Received:", data)
        # broadcast to all browser clients
        for c in browser_clients:
            server.send_message(c, json.dumps(data))
    except Exception as e:
        print("Error parsing JSON:", e)

# Start WebSocket server
server = WebsocketServer(host='0.0.0.0', port=5000)
server.set_fn_new_client(new_client)
server.set_fn_client_left(client_left)
server.set_fn_message_received(message_received)

# Run server in a separate thread
threading.Thread(target=server.run_forever, daemon=True).start()

print("WebSocket server running on ws://0.0.0.0:5000/")

# Minimal HTTP server to serve HTML dashboard
from http.server import HTTPServer, SimpleHTTPRequestHandler

class DashboardHandler(SimpleHTTPRequestHandler):
    def do_GET(self):
        if self.path == '/':
            self.send_response(200)
            self.send_header('Content-type', 'text/html')
            self.end_headers()
            # Dashboard HTML
            html = f"""
            <!DOCTYPE html>
            <html>
            <head>
                <title>ESP32 Dashboard</title>
                <style>
                    body {{ font-family: Arial; display:flex; justify-content:center; align-items:center; height:100vh; margin:0; background:#f7f7f7; }}
                    .card {{ background:white; padding:20px 30px; border-radius:12px; box-shadow:0 4px 12px rgba(0,0,0,0.2); text-align:center; min-width:250px; }}
                </style>
            </head>
            <body>
                <div class="card">
                    <h1>ESP32 Data</h1>
                    <p id="temp">Temperature: --</p>
                    <p id="hum">Humidity: --</p>
                </div>
                <script>
                    let ws = new WebSocket("ws://" + location.hostname + ":5000/");
                    ws.onmessage = function(event) {{
                        let data = JSON.parse(event.data);
                        document.getElementById("temp").innerText = "Temperature: " + data.temperature + " C";
                        document.getElementById("hum").innerText = "Humidity: " + data.humidity + " %";
                    }};
                </script>
            </body>
            </html>
            """
            self.wfile.write(html.encode('utf-8'))
        else:
            self.send_error(404)

httpd = HTTPServer(('0.0.0.0', 8000), DashboardHandler)
print("HTTP dashboard running on http://0.0.0.0:8000/")
httpd.serve_forever()
