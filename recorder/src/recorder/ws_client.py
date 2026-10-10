"""Websocket connection management.

Reconnect-with-backoff and ping/pong heartbeat are both handled by the
`websockets` library itself when `websockets.connect(url)` is used as an
async iterator (`async for ws in websockets.connect(url)`) — it yields a
fresh connection every time the previous one drops, waiting according to
`reconnect_delays` (exponential backoff by default) between attempts.
Our job is just: detect the drop, log the gap, keep consuming messages.
"""

import json
import time

import websockets

GAPS_LOG = "data/gaps.jsonl"


def build_stream_url(symbols: list[str], streams: list[str]) -> str:
    parts = [f"{s.lower()}@{st}" for s in symbols for st in streams]
    return "wss://fstream.binance.com/stream?streams=" + "/".join(parts)


def log_gap(disconnected_at: float, reconnected_at: float) -> None:
    with open(GAPS_LOG, "a") as f:
        f.write(
            json.dumps(
                {
                    "disconnected_at": disconnected_at,
                    "reconnected_at": reconnected_at,
                    "gap_seconds": reconnected_at - disconnected_at,
                }
            )
            + "\n"
        )


async def stream(url: str, on_message, on_reconnect=None) -> None:
    """Connect to `url` and call on_message(raw_message, recv_time_ns) for
    every message received. Reconnects forever; never returns normally.

    Gaps are measured from the last message actually received to the next
    message actually received — not from when ConnectionClosed happens to
    get raised. The ping/pong keepalive can take up to
    ping_interval + ping_timeout seconds to even notice a dead connection,
    so timestamping off ConnectionClosed would badly undercount real
    downtime (confirmed experimentally: a ~25s Wi-Fi outage only produced a
    ~0.5s gap when measured that way).

    If given, `on_reconnect` (an async callable, no args) is awaited every
    time a new connection is established after the first one — used to
    trigger a fresh REST snapshot, since the book sync starting point needs
    one right after any reconnect.
    """
    last_received_at = time.time()
    first_connection = True

    async for ws in websockets.connect(url):
        gap = time.time() - last_received_at
        if not first_connection:
            if gap > 1:
                log_gap(last_received_at, time.time())
            if on_reconnect is not None:
                await on_reconnect()
        first_connection = False

        try:
            async for raw_message in ws:
                last_received_at = time.time()
                on_message(raw_message, time.time_ns())
        except websockets.ConnectionClosed:
            continue  # the async-for above opens a new connection


