# Bootleg Radio — 64 KB streaming ring experiment

Branch: `538-bootleg-radio-reverse-pingpong`

Firmware: `UF2/bootleg_radio.uf2`

This build changes **only** the streaming ring: 128 KB → **64 KB**. No slice
code, no crossfade, no parser changes. Phase 2a is otherwise unchanged.

RAM is now about **66.8%**, freeing roughly 64 KB for the USB host and any later
slice state. The trade-off is a smaller streaming buffer: about **0.34 s** of
stereo audio at 48 kHz, or 0.68 s mono.

## Why

The previous slice and crossfade builds failed with LED 5 and silence at around
92% RAM. Phase 2a works at 91.8%. The working hypothesis is that the USB host
needs more headroom than the ~20 KB left at 91.8%, and that the slice/crossfade
additions crossed the line. This build tests the hypothesis without adding any
feature.

## Test first

1. Boot and mount; the card should show its normal mounted/playing LEDs.
2. Play a long file and a short loop; no gaps or stutter at normal speed.
3. Change station, bank, speed, start/end and loop mode.
4. Reverse and ping-pong; the smaller ring is the stress case.
5. One-shot and retrigger.
6. Tuning static and the 8mu functions.
7. Try the slowest stick you have and the highest sample-rate file you use.
8. Watch for LED 5 underruns; note whether they appear at normal speed or only
   at extremes.
9. Pull and reinsert the stick; the card should recover.

## Results

| Test | Pass | Notes |
|---|---|---|
| Boot/mount | | |
| Long file | | |
| Short loop | | |
| Station/bank change | | |
| Speed/start/end | | |
| One-shot/retrigger | | |
| Reverse | | |
| Ping-pong | | |
| Static/8mu | | |
| Slow stick | | |
| High sample rate | | |
| Reinsert stick | | |
| No LED 5 underruns | | |

## Known limits

- This is a RAM experiment, not a feature build.
- The ring is half the previous size; slow sticks or high-rate files may show
  underruns where the 128 KB ring did not.
- If this is unstable, restore the 128 KB ring from `e6e1dc7c`.
- If it is stable, the minimal slice table can be added and should have plenty
  of RAM headroom.
