# Guitar Hero II - Speed/Timing Fixes

Two common issues affect all Xbox 360 static recompilations on Windows.

## 1. VdSwap Frame Limiter

Xbox 360 games typically call `VdSwap()` which blocks for vsync (~16.667ms).
The ReXGlue SDK implements this with `Sleep(16)`, but Windows `Sleep()` has
~15.6ms timer granularity, meaning `Sleep(16)` actually sleeps ~31ms.

**Fix**: Use `QueryPerformanceCounter` spin-loop for precise frame pacing.

This is typically handled in the ReXGlue SDK itself via the presenter, but
if frame timing issues appear, check:
- Is `timeBeginPeriod(1)` being called?
- Is the presenter using the correct swap interval?

## 2. Timebase Scaling (__rdtsc)

Xbox 360's timebase register runs at 49.875 MHz. The generated code uses
`__rdtsc()` which returns the x86 TSC at ~3-4 GHz. This 60-80x difference
causes games to run at wildly wrong speeds for any time-based logic.

**Fix**: Override `__rdtsc()` in `ppc_detail.h` to route through
`rex::chrono::Clock::QueryGuestTickCount()` which scales the host TSC
to match the Xbox 360's 49.875 MHz rate.

```cpp
// ppc/ppc_detail.h
#pragma once
#include <rex/chrono.h>
#define __rdtsc() rex::chrono::Clock::QueryGuestTickCount()
```

## Guitar Hero II Specific

GH2 is a rhythm game where timing precision is critical. Frame drops
or inconsistent timing will directly affect:
- Note highway scrolling speed
- Hit window accuracy
- Audio sync
- Star Power duration
- Score multiplier timing

Priority: Get frame timing rock-solid before tackling other issues.
