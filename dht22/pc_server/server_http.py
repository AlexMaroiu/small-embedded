from flask import Flask, request

app = Flask(__name__)

style = f"""<style>
              body {{
                font-family: Arial, sans-serif;
                background: #f7f7f7;
                margin: 0;
                height: 100vh;
                display: flex;
                justify-content: center;
                align-items: center;
              }}
              .card {{
                background: white;
                padding: 20px 30px;
                border-radius: 12px;
                box-shadow: 0 4px 12px rgba(0,0,0,0.2);
                text-align: center;
                min-width: 250px;
              }}
              h1 {{
                margin-top: 0;
              }}
            </style>"""

no_data = "<h1>No data received yet</h1>"
failed_data = "<h1>Sensor data failed<h1>"

sensor_data = {}

def compute_data():
    if sensor_data:
      if (sensor_data.get('temperature') == -127.0) or (sensor_data.get('humidity') == -127.0):
        return failed_data
      else:
        return f"""<h1>ESP32 Data</h1>
        <p><b>Temperature:</b> {sensor_data.get('temperature')} °C</p>
        <p><b>Humidity:</b> {sensor_data.get('humidity')} %</p>"""
    else:
        return no_data

@app.route('/data', methods=['POST'])
def receive_data():
    global sensor_data
    sensor_data = request.json
    print("Received:", sensor_data)
    return {"status": "ok"}

@app.route('/', methods=['GET'])
def show_data():
  return f"""
  <html>
    <head>
      <meta http-equiv="refresh" content="10">
      {style}
    </head>
    <body>
      <div class="card">
        {compute_data()}
      </div>
    </body>
  </html>
  """

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
