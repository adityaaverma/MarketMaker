#pragma once

// TODO (Phase 1): Binance local order book sync state machine.
// WAITING_SNAPSHOT -> SYNCING -> LIVE
// - buffer @depth events while waiting for the REST snapshot
// - drop events with u < lastUpdateId
// - first applied event must satisfy U <= lastUpdateId <= u
// - every next event's pu must equal the previous event's u, else resync
//   from a fresh snapshot
