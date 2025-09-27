from flask import Flask, request, jsonify
from flask_sock import Sock

app = Flask(__name__)
sock = Sock(app)

style = """<style>
              body {
                font-family: Arial, sans-serif;
                background: #f7f7f7;
                margin: 0;
                height: 100vh;
                display: flex;
                justify-content: center;
                align-items: center;
              }
              .card {
                background: white;
                padding: 20px 30px;
                border-radius: 12px;
                box-shadow: 0 4px 12px rgba(0,0,0,0.2);
                text-align: center;
                min-width: 250px;
              }
              h1 {
                margin-top: 0;
              }
            </style>"""


# Store latest data
latest_data = {}
# List of connected websocket clients
clients = []

@app.route('/data', methods=['POST'])
def receive_data():
    global latest_data
    latest_data = request.get_json()
    print("Received:", latest_data)

    # broadcast to all browser websocket clients
    for ws in clients:
        try:
            ws.send(str(latest_data))
        except:
            pass

    return jsonify({"status": "ok"}), 200

@sock.route('/ws')
def ws_handler(ws):
    clients.append(ws)
    print("Browser connected via WS")
    try:
        while True:
            data = ws.receive()  # we don't expect browser to send
            if data is None:
                break
    finally:
        clients.remove(ws)
        print("Browser disconnected")


@app.route('/')
def index():
    return f"""
    <!DOCTYPE html>
    <html>
    <head>
        <title>ESP32 Dashboard</title>
        {style}
    </head>
    <body>
        <div class="card">
            <h1>ESP32 Data</h1>
            <p id="temp">Temperature: --</p>
            <p id="hum">Humidity: --</p>
        </div>
        <script>
            let ws = new WebSocket("ws://" + location.hostname + ":5000/ws");
            ws.onmessage = function(event) {{
                let data = JSON.parse(event.data.replace(/'/g, '"')); // quick fix for Python dict format
                document.getElementById("temp").innerText = "Temperature: " + data.temperature + " °C";
                document.getElementById("hum").innerText = "Humidity: " + data.humidity + " %";
            }};
        </script>
    </body>
    </html>
    """

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=5000, debug=True)
