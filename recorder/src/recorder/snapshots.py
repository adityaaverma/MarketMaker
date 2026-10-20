"""Periodic + on-reconnect REST depth snapshots.

Reuses writer.py's routing by building the same envelope shape ws_client
produces ({"stream": "<symbol>@snapshot", "data": ...}), so the Writer
doesn't need to know snapshots exist as a separate thing.
"""

import asyncio
import json
import time

import aiohttp

from .rest_poll import get_with_backoff

REST_ENDPOINT = "https://fapi.binance.com/fapi/v1/depth"


async def fetch_snapshot(session: aiohttp.ClientSession, symbol: str, limit: int = 1000) -> dict:
    return await get_with_backoff(session, REST_ENDPOINT, {"symbol": symbol, "limit": limit})


async def snapshot_once(session: aiohttp.ClientSession, symbol: str, writer) -> None:
    data = await fetch_snapshot(session, symbol)
    envelope = {"stream": f"{symbol.lower()}@snapshot", "data": data}
    writer.write(json.dumps(envelope), time.time_ns())


async def snapshot_all(session: aiohttp.ClientSession, symbols: list[str], writer) -> None:
    for symbol in symbols:
        await snapshot_once(session, symbol, writer)


async def snapshot_loop(symbols: list[str], writer, interval_seconds: int = 3600) -> None:
    """Hourly snapshot loop. Call snapshot_all() separately right after a
    ws reconnect — this loop only covers the periodic half."""
    async with aiohttp.ClientSession() as session:
        while True:
            await snapshot_all(session, symbols, writer)
            await asyncio.sleep(interval_seconds)
