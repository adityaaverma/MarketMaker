"""REST-polled data for things the websocket streams can't deliver.

- aggTrades: incremental poll via fromId, since the @aggTrade websocket
  stream accepts the subscription but never delivers a message (see
  docs/decisions/0001-aggtrade-stream-not-delivering.md).
- premiumIndex (funding rate, mark price, index price) and openInterest:
  perpetual-specific metrics with no realtime websocket stream at all on
  Binance Futures (openInterest) or sharing the same broken stream as
  aggTrade (premiumIndex's markPrice) — see
  docs/decisions/0002-normalized-event-format.md.

Reuses writer.py's routing by emitting the same envelope shape
({"stream": "<symbol>@<name>", "data": ...}) the websocket client produces.
"""

import asyncio
import json
import time

import aiohttp

BASE = "https://fapi.binance.com"


async def _get(session: aiohttp.ClientSession, path: str, params: dict) -> dict | list:
    async with session.get(BASE + path, params=params) as resp:
        resp.raise_for_status()
        return await resp.json()


async def poll_agg_trades_once(
    session: aiohttp.ClientSession, symbol: str, writer, last_id: int | None
) -> int | None:
    """One fetch starting after last_id (or most-recent, if last_id is None).
    Returns the new last_id (unchanged if there was nothing new)."""
    params = {"symbol": symbol, "limit": 1000}
    if last_id is not None:
        params["fromId"] = last_id + 1
    trades = await _get(session, "/fapi/v1/aggTrades", params)
    if not trades:
        return last_id
    for t in trades:
        envelope = {"stream": f"{symbol.lower()}@aggTrade", "data": t}
        writer.write(json.dumps(envelope), time.time_ns())
    return trades[-1]["a"]


async def _catch_up_one_symbol(
    session: aiohttp.ClientSession, symbol: str, writer, last_ids: dict[str, int | None]
) -> None:
    """Poll one symbol until caught up. A 429 here backs off using Binance's
    own Retry-After and keeps going — it must NOT propagate up, or it tears
    down the loop for every other symbol too and main.py's _resilient
    wrapper restarts the whole thing from scratch, losing last_ids for
    symbols that were perfectly fine (confirmed on the VM: one symbol's 429
    was repeatedly killing tracking for all three)."""
    while True:
        try:
            new_last = await poll_agg_trades_once(session, symbol, writer, last_ids[symbol])
        except aiohttp.ClientResponseError as e:
            if e.status != 429:
                raise
            retry_after = float(e.headers.get("Retry-After", 5)) if e.headers else 5.0
            print(f"aggTrades rate-limited on {symbol}, backing off {retry_after}s")
            await asyncio.sleep(retry_after)
            continue

        caught_up = new_last == last_ids[symbol]
        last_ids[symbol] = new_last
        if caught_up:
            return


async def agg_trades_loop(symbols: list[str], writer, interval_seconds: float = 3.0) -> None:
    last_ids: dict[str, int | None] = {s: None for s in symbols}
    async with aiohttp.ClientSession() as session:
        while True:
            for symbol in symbols:
                await _catch_up_one_symbol(session, symbol, writer, last_ids)
            await asyncio.sleep(interval_seconds)


async def poll_perp_metrics_once(session: aiohttp.ClientSession, symbol: str, writer) -> None:
    premium = await _get(session, "/fapi/v1/premiumIndex", {"symbol": symbol})
    writer.write(
        json.dumps({"stream": f"{symbol.lower()}@premiumIndex", "data": premium}), time.time_ns()
    )

    oi = await _get(session, "/fapi/v1/openInterest", {"symbol": symbol})
    writer.write(json.dumps({"stream": f"{symbol.lower()}@openInterest", "data": oi}), time.time_ns())


async def perp_metrics_loop(symbols: list[str], writer, interval_seconds: float = 300.0) -> None:
    async with aiohttp.ClientSession() as session:
        while True:
            for symbol in symbols:
                await poll_perp_metrics_once(session, symbol, writer)
            await asyncio.sleep(interval_seconds)
