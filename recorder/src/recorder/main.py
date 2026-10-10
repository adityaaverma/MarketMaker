"""Entrypoint, asyncio loop.

Run from inside recorder/ (venv active) with:
    python -m recorder.main
"""

import asyncio
import signal

import aiohttp
import yaml

from .snapshots import snapshot_all, snapshot_loop
from .writer import Writer
from .ws_client import build_stream_url, stream


class _Shutdown(Exception):
    """Raised internally to unwind the TaskGroup on SIGINT/SIGTERM."""


async def _wait_for_stop(stop_event: asyncio.Event) -> None:
    await stop_event.wait()
    raise _Shutdown


async def run(config: dict) -> None:
    symbols = config["symbols"]
    streams = config["streams"]
    interval_seconds = config["snapshot"]["interval_seconds"]

    writer = Writer()
    url = build_stream_url(symbols, streams)
    stop_event = asyncio.Event()

    loop = asyncio.get_running_loop()
    for sig in (signal.SIGINT, signal.SIGTERM):
        loop.add_signal_handler(sig, stop_event.set)

    async def on_reconnect() -> None:
        async with aiohttp.ClientSession() as session:
            await snapshot_all(session, symbols, writer)

    try:
        async with asyncio.TaskGroup() as tg:
            tg.create_task(stream(url, writer.write, on_reconnect=on_reconnect))
            tg.create_task(snapshot_loop(symbols, writer, interval_seconds=interval_seconds))
            tg.create_task(_wait_for_stop(stop_event))
    except* _Shutdown:
        pass
    finally:
        writer.close()
        print("writer closed, shutdown complete.")


def main() -> None:
    with open("config.yaml") as f:
        config = yaml.safe_load(f)
    asyncio.run(run(config))


if __name__ == "__main__":
    main()
