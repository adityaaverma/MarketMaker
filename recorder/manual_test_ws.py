"""Manual smoke test for ws_client.stream() — not a pytest test.

Run it, watch messages print, then kill your Wi-Fi for 10-15s and watch it
reconnect. Check data/gaps.jsonl afterwards for the logged gap.
"""

import asyncio
import datetime

from recorder.ws_client import build_stream_url, stream

url = build_stream_url(["btcusdt"], ["bookTicker"])


def printer(msg, recv_ns):
    ts = datetime.datetime.fromtimestamp(recv_ns / 1e9).strftime("%H:%M:%S.%f")[:-3]
    print(ts, msg[:100])


asyncio.run(stream(url, printer))
