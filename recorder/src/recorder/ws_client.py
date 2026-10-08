import websockets


"""Websocket connection management.

TODO (Phase 0):
- connect to combined stream
- reconnect with exponential backoff
- heartbeat / ping-pong handling
- log every disconnect with timestamp to a gaps file
"""

while True:
    try:
        async with websockets.connet()