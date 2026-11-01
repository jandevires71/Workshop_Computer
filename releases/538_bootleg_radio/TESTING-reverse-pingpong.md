# Bootleg Radio — reverse & ping-pong test notes

Branch: `538-bootleg-radio-reverse-pingpong`
Firmware: `UF2/bootleg_radio.uf2`
Built on top of the tested Phase 1/2 + tuning-static build.

Tested by: ____________________  Date: ____________

## Setup

- USB stick with:
  - a short loop (100–500 ms) with a clear transient, for the turn/seam;
  - a long file (> 1 minute), for reverse block boundaries;
  - a drum loop, for direction;
  - a quiet sustained tone, for clicks.
- `settings.txt` in the root; reinsert/power-cycle after editing.
- 8mu: Type A, stereo TRS to Pulse in 2, own power.
- Defaults for this round:

```ini
mode = sync
loopMode = loop
speedRange = 2oct
static = off
```

## Reverse

Set `loopMode = loop` and put **START (F3) above END (F4)** — or, on the panel,
Y above the Audio in 2 end. That makes the loop play backwards.

- [ ] A short loop plays backwards. The first sound is the loop end, the last
      is the loop start.
- [ ] It loops cleanly backwards: no click at the wrap (a short dip is expected,
      same as the forward seam).
- [ ] Retrigger (switch down, 8mu B1, Pulse in 2) starts again at the **loop
      end** in reverse. No Sync landing is used.
- [ ] Station change starts the new station at its **loop end**, not at the
      proportional Sync position. This is the agreed simple behaviour.
- [ ] Bank change starts the new bank's station at its loop end.
- [ ] The loop start (F3/Y) and loop end (F4/Audio 2) still define the window;
      reverse plays inside the same window.
- [ ] The 8mu station fader (F5) and bank buttons (B2/B3 tap) still select
      stations; reverse does not change the selection.
- [ ] F1 speed still changes speed/pitch; the reverse direction is unaffected.
- [ ] `speedRange` (or the F2 range fader) works in reverse: `2oct`, `1oct`,
      `fine`, `off`.
- [ ] Slow and fast speeds: no clicks; no obvious block-boundary artefacts.
- [ ] A loop longer than one 8 KB block still plays smoothly through the block
      boundaries.
- [ ] A very short loop (a few frames) does not hang; it may buzz, as before.
- [ ] One-shot + reverse: plays the window once backwards, stops, LED 2 off;
      retrigger plays it again.
- [ ] Reverse on a stereo file: both channels are reversed together, not
      swapped.
- [ ] Reverse on a mono file: fine.
- [ ] No new LED 5 underruns at normal speed; note if 4× speed or a 192 kHz
      source underruns.
- [ ] Station/bank/retrigger fades still work in reverse.
- [ ] Tuning static (if `static = on`) fires on station/bank changes in reverse.

## Ping-pong

Set `loopMode = ping-pong`. The direction is set by the START/END order: if END
is left of START, the first pass is reverse; otherwise it starts forward. Then
it alternates.

- [ ] If START is below END, the first pass is forward from the loop start /
      Sync landing.
- [ ] If START is above END, the first pass is reverse from the loop end.
- [ ] At the end it turns and plays the other way.
- [ ] At the start it turns back, and repeats.
- [ ] The turns do not click. A small dip is expected because the file is
      reloaded and faded at each turn; note how audible it is.
- [ ] The loop start and end define the window in both directions.
- [ ] Retrigger starts the current station in the fader direction at the window
      edge.
- [ ] Station change starts in the fader direction; a forward first pass uses the
      Loop/Sync landing, a reverse first pass starts at the loop end.
- [ ] Bank change starts the new station in the fader direction.
- [ ] F1 speed, F2 speed range / `speedRange` work in both directions.
- [ ] The 8mu station (F5), bank (B2/B3 tap) and retrigger (B1) controls work.
- [ ] B4 tap toggles Loop/Sync, B4 hold releases; B2/B3 hold toggles static off/on
      without changing bank.
- [ ] One-shot is not combined with ping-pong (the setting is one or the other).
- [ ] The loop gate and wrap tick pulse outputs behave; note if the ping-pong
      turn does or does not produce a tick.
- [ ] No new underruns at normal speed.
- [ ] Tuning static fires on station/bank changes in either direction.

## Regression

- [ ] Phase 1/2: speed range, one-shot, bank LEDs, loop/station fades.
- [ ] Tuning static: on/off, level, no static on retrigger or loop seam.
- [ ] 8mu remote: F1 speed, F2 speed range, F3/F4 start/end, F5 station, F6–F8;
      B1 retrigger, B2/B3 bank/static, B4 mode/release; soft release, bank
      relative, loop edit retrigger.
- [ ] CV sums, loop-end sticky, `remote = auto/on/off`.
- [ ] F6 loop mode (loop/one-shot/ping-pong), F7 performance mode, F8 pulse 1
      mode; clock BPM and pulse out 2 stay in settings.txt.
- [ ] No audio dropouts or stuck silence after rapid station changes in reverse
      or ping-pong.

## Results

| Test | Pass | Notes |
|---|---|---|
| Reverse — short loop | | |
| Reverse — loop start/end | | |
| Reverse — retrigger/station | | |
| Reverse — bank | | |
| Reverse — speed range | | |
| Reverse — block boundaries | | |
| Reverse — one-shot | | |
| Reverse — stereo/mono | | |
| Reverse — fades/static | | |
| Ping-pong — forward/reverse turns | | |
| Ping-pong — turn dip | | |
| Ping-pong — retrigger/station/bank | | |
| Ping-pong — speed range | | |
| Ping-pong — remote | | |
| Ping-pong — pulse outs | | |
| No new underruns | | |
| Phase 1/2 regression | | |
| Static regression | | |
| 8mu — F1/F2/F5 remap | | |
| 8mu — B2/B3 static holds | | |
| 8mu — B4 mode/release | | |

## Known limits of this round

- Reverse is implemented by reading 8 KB blocks backwards from the USB stick,
  reversing each block, and playing the ring forward. It keeps the existing
  ring/stream engine, but the USB stick is doing seeks rather than one long
  sequential read. 4× speed with high-rate source files is the stress case.
- Reverse ignores the Sync landing: a new station/retrigger starts at the window
  end. This is the agreed simple behaviour.
- Ping-pong turns reload the file, so the fade produces a small dip at each
  turn. It is not a seamless bidirectional buffer.
- Ping-pong starts in the direction of the START/END order, then alternates.
- One-shot and ping-pong are mutually exclusive loop modes.
- B2/B3 bank changes happen on release, so a 1s hold can mean static instead.
  B4 needs a 1.5s hold to release; a tap toggles Loop/Sync. The short/long logic
  relies on the 8mu sending note-off; a bank tap that does nothing would point at
  a missing note-off.
- F2's speed-range override has a 0–7 dead zone that defers to `speedRange`.
- The 8mu remains one-way; there is no feedback to the 8mu itself. The card's
  LEDs show the mode (2/3 flashes), static (1/4), takeover and release.
