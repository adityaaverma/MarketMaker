"""Hourly-rotated, zstd-compressed JSONL writer.

TODO (Phase 0):
- write each raw message unchanged, plus local receive time in ns
- rotate files hourly
- zstd compression
- path convention: data/raw/{exchange}/{symbol}/{stream}/YYYY-MM-DD/HH.jsonl.zst
"""
