"""Run the real pipeline (ws_client + writer) for ~1 hour so we can measure
actual disk usage. Not the final main.py — no health reporting, no clean
signal handling beyond Ctrl+C. Run from inside recorder/ with the venv active.
"""

import asyncio
import time

import yaml

from recorder.writer import Writer
from recorder.ws_client import build_stream_url, stream

with open("config.yaml") as f:
    config = yaml.safe_load(f)

url = build_stream_url(config["symbols"], config["streams"])
writer = Writer()

TEST_DURATION_SECONDS = 60 * 60
PROGRESS_EVERY_SECONDS = 60


async def main():
    start = time.time()
    task = asyncio.create_task(stream(url, writer.write))

    try:
        while time.time() - start < TEST_DURATION_SECONDS:
            await asyncio.sleep(PROGRESS_EVERY_SECONDS)
            print(f"{int(time.time() - start)}s elapsed...")
    finally:
        task.cancel()
        try:
            await task
        except asyncio.CancelledError:
            pass
        writer.close()
        print("done, writer closed.")


asyncio.run(main())
