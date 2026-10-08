"""Entrypoint, asyncio loop.

TODO (Phase 0):
- load config.yaml
- start one ws_client task per symbol/stream combo (or one combined stream)
- wire up writer, snapshots, health reporting
- graceful shutdown on SIGINT/SIGTERM
"""
