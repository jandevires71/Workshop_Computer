# Bootleg Radio — Phase 2a test notes: F7 performance and F8 pulse 1

Branch: `538-bootleg-radio-reverse-pingpong`

Firmware: `UF2/bootleg_radio.uf2`

This build restores the pre-crossfade firmware and adds the F7/F8 remap with
clocked **Scan** and **Beat-repeat**. Slice is reserved but not implemented yet.
No audio buffer was added; the card remains at roughly 92% RAM.

## Setup

- `settings.txt` in the root; reinsert/power-cycle after editing.
- 8mu: Type A, stereo TRS to Pulse in 2, own power.
- Start with:

```ini
clockBpm = 120
performanceMode = off
perfDiv = 1
loopMode = loop
```

- Patch Pulse in 1 if you want an external clock; leave it unpatched to use the
  internal clock.
- Use a bank with several short files for the scan tests.

## New 8mu map

| Control | Function |
|---|---|
| F1 | Speed |
| F2 | Speed range |
| F3 | Loop start |
| F4 | Loop end |
| F5 | Station / file |
| F6 | Loop mode: loop / one-shot / ping-pong |
| F7 | Performance mode |
| F8 | Pulse out 1 mode |

F7 zones:

| F7 | Mode |
|---|---|
| 0–7 | `settings.txt` (`performanceMode`) |
| 8–27 | Scan |
| 28–47 | Beat-repeat |
| 48–67 | Slice (reserved; currently no effect) |
| 68–127 | Off |

F8 zones:

| F8 | Pulse out 1 |
|---|---|
| 0–7 | `settings.txt` (`pulse1`) |
| 8–37 | Gate |
| 38–67 | Tick |
| 68–97 | Clock |
| 98–127 | Off |

Clock BPM and pulse out 2 are no longer on the 8mu; both are in `settings.txt`.
Pulse in 1 still overrides the clock tempo.

## 1. F7/F8 remap

- [ ] F7 at 0–7 uses `performanceMode`; the card plays normally with
      `performanceMode = off`.
- [ ] F7 8–27 selects Scan; F7 28–47 selects Beat-repeat.
- [ ] F7 48–67 selects the reserved Slice zone; it behaves as normal playback
      for now.
- [ ] F7 68–127 is Off.
- [ ] F8 0–7 uses `pulse1`; 8–37 gate, 38–67 tick, 68–97 clock, 98–127 off.
- [ ] Moving F7 or F8 flashes LED 3 once when the remote takes over.
- [ ] Releasing the remote returns F7 to `performanceMode` and F8 to `pulse1`.
- [ ] Clock BPM still follows `clockBpm`; Pulse in 1 still overrides it.

## 2. Scan

Set `clockBpm = 120`, `perfDiv = 1`, F7 8–27. Use the internal clock first.

- [ ] F5 selects a station before enabling Scan. Scan starts from that station.
- [ ] Each clock pulse advances one station; the station fade and static behave
      as a normal station change.
- [ ] While Scan is active, moving F5, Main or CV in 1 does not change the
      station.
- [ ] Leaving Scan (F7 out of 8–27) hands station selection back to F5/Main from
      the station the scan stopped on.
- [ ] Scan wraps from the last station to the first.
- [ ] `perfDiv = 2`: one station change every two clock pulses.
- [ ] `perfDiv = 4`, `8`, `16`: same relationship; no runaway.
- [ ] External clock on Pulse in 1: Scan follows it; when the external clock
      stops for a second, Scan returns to the internal clock.
- [ ] Scan with F6 loop / one-shot / ping-pong: the new station follows the
      selected loop mode.
- [ ] Scan with Sync mode: the new station lands at the synchronised position.
- [ ] Scan with reverse or ping-pong direction: the new station keeps the
      selected direction.
- [ ] Bank change while Scan is active: the new bank starts at station 0 and the
      scan continues from there.
- [ ] Rapid clock: no stuck silence, no missed station permanently, no LED 5
      underruns beyond what the stick normally shows.
- [ ] Leaving Scan does not leave `performanceMode` latched if F7 is returned to
      0–7 with `performanceMode = off`.

## 3. Beat-repeat

Set F7 28–47, `perfDiv = 1`.

- [ ] Each clock pulse retriggers the current loop; the loop restart fades.
- [ ] The tuning static does not fire on beat-repeat (it is a retrigger, not a
      station change).
- [ ] F6 loop: the retriggered loop continues looping.
- [ ] F6 one-shot: the retriggered pass plays once and waits.
- [ ] `perfDiv = 2`, `4`, `8`, `16`: repeat rate divides correctly.
- [ ] External clock on Pulse in 1: repeat follows it; internal fallback works.
- [ ] While Beat-repeat is active, F5/Main/CV1 still select stations normally.
- [ ] Changing F7 from Repeat to Scan or Off does not leave a pending retrigger
      stuck.
- [ ] Rapid clock: no clicks beyond the normal retrigger fade, no stuck silence.

## 4. Combined checks

- [ ] F6 one-shot + Scan: each clock advances to a new station, which plays once.
- [ ] F6 one-shot + Repeat: each clock retriggers the one-shot.
- [ ] F7 Scan + F8 clock pulse mode: pulse out 1 follows the same clock.
- [ ] F7 Repeat + F8 gate/tick: the pulse outputs still describe the loop.
- [ ] F7 Off returns to normal station/loop behaviour.
- [ ] `performanceMode` in settings is used when F7 is 0–7.
- [ ] `perfDiv` in settings is used when F7 is 0–7.

## 5. Regression

- [ ] F1 speed, F2 speed range, F3/F4 start/end, F5 station still work.
- [ ] F6 loop / one-shot / ping-pong still work.
- [ ] B4 tap Loop/Sync; B4 hold release; B2/B3 bank/static; B1 retrigger.
- [ ] Soft release, bank-relative station, loop edit retrigger.
- [ ] Tuning static on station/bank changes, not on retrigger or loop seam.
- [ ] CV sums, loop-end sticky, `remote = auto/on/off`.
- [ ] No new LED 5 underruns or resets at normal speed.
- [ ] The crossfade is gone again; the old short loop fade is expected.

## Results

| Test | Pass | Notes |
|---|---|---|
| F7/F8 remap | | |
| F7 0–7 settings | | |
| F8 pulse 1 zones | | |
| Scan — forward/wrap | | |
| Scan — F5 ignored | | |
| Scan — leave/reclaim | | |
| Scan — perfDiv | | |
| Scan — external clock | | |
| Scan — loop modes | | |
| Beat-repeat — loop | | |
| Beat-repeat — one-shot | | |
| Beat-repeat — perfDiv | | |
| Beat-repeat — external clock | | |
| Clock BPM settings | | |
| F8 pulse 1 mode | | |
| Combined scan/repeat | | |
| Regression | | |
| No new underruns | | |

## Known limits of this round

- Slice is reserved; F7 48–67 does nothing yet.
- Scan is forward-only and wraps at the end of the bank.
- Scan owns station selection while active; F5/Main/CV1 are ignored until it is
  turned off.
- Clock BPM has no 8mu fader; use `settings.txt` or Pulse in 1.
- The crossfade is not in this build.
- Freeze/live remain out of scope.
