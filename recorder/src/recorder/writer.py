"""Hourly-rotated, zstd-compressed JSONL writer.

Each incoming combined-stream message is wrapped with its local receive
time (ns) and appended as one JSON line to a file routed by symbol/stream,
following: data/raw/{exchange}/{symbol}/{stream}/YYYY-MM-DD/HH.jsonl.zst

Rotation is implicit: the path is recomputed from recv_ns on every write,
so crossing an hour boundary naturally opens a new file.
"""

import datetime
import json
import os

import zstandard

RAW_DIR = "data/raw/binance"


def _stream_to_path(stream_name: str, recv_ns: int) -> str:
    # stream_name looks like "btcusdt@bookTicker" or "btcusdt@depth@100ms"
    symbol, _, rest = stream_name.partition("@")
    dt = datetime.datetime.fromtimestamp(recv_ns / 1e9, tz=datetime.timezone.utc)
    return os.path.join(
        RAW_DIR,
        symbol.upper(),
        rest,
        dt.strftime("%Y-%m-%d"),
        dt.strftime("%H") + ".jsonl.zst",
    )


class Writer:
    def __init__(self):
        self._open = {}  # path -> (zstd stream_writer, underlying file)

    def write(self, raw_message: str, recv_ns: int) -> None:
        envelope = json.loads(raw_message)
        stream_name = envelope.get("stream", "unknown")
        path = _stream_to_path(stream_name, recv_ns)

        if path not in self._open:
            self._rotate_in(path)

        compressor, _ = self._open[path]
        record = {"recv_ns": recv_ns, "msg": envelope}
        compressor.write((json.dumps(record) + "\n").encode())

    def _rotate_in(self, path: str) -> None:
        os.makedirs(os.path.dirname(path), exist_ok=True)
        f = open(path, "ab")
        compressor = zstandard.ZstdCompressor(level=3).stream_writer(f)
        self._open[path] = (compressor, f)

    def close(self) -> None:
        for compressor, f in self._open.values():
            compressor.flush(zstandard.FLUSH_FRAME)
            f.close()
        self._open.clear()
