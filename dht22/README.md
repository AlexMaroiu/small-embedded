In this directory, i have made multiples ways to have the data of the temperature sensor (DHT22) send to a browser app:
    - Server on the esp32 board directly
    - Pc server with esp32 sending data as a HTTP POST request (board will enter sleep to use less power)
    - PC server with WebSockets with esp32 sending data as a HTTP POST request
    - PC server with esp32 send raw TCP/IP data
    - PC Websocket server with esp32 sending data with a websocket
