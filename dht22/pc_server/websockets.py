from websocket_server import WebsocketServer

def new_client(client, server):
    print(f"New client connected: {client['id']}")

def message_received(client, server, message):
    print(f"Message from ESP32: {message}")

server = WebsocketServer(host='0.0.0.0', port=5000)
server.set_fn_new_client(new_client)
server.set_fn_message_received(message_received)
server.run_forever()
