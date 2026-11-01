# Bootleg Radio (538) — stable state and roadmap

## Stable build

- **Firmware commit:** this clean PR
- **Branch:** `538-bootleg-radio-clean`
- **Tag:** `stable-2026-11-01.1`
- **Firmware:** `UF2/bootleg_radio.uf2`
- **RAM:** ~66.8% (64 KB streaming ring)
- **Tests:** `TESTING-phase2a-scan-repeat.md`, `TESTING-ring-64k.md`

This is the last known working build: Phase 2a, the 64 KB ring and the F6
one-shot/loop zone swap. The card boots, mounts the stick and plays. The
development history with the parked experiments lives on the local
`538-bootleg-radio-reverse-pingpong` branch; this PR is its clean squashed tree.

## What is in the stable build

- Radio Music-style station playback from the USB stick, with banks and LEDs.
- START/END loop window, reverse and ping-pong.
- One-shot, loop mode, tuning static and the 8mu TRS remote.
- F1 speed, F2 speed range, F3 start, F4 end, F5 station.
- F6 loop mode (one-shot 8–47, loop 48–87, ping-pong 88–127), F7 performance,
  F8 pulse out 1.
- F7 Scan and Beat-repeat on the divided clock; `perfDiv` 1–16.
- Clock BPM and pulse out 2 in `settings.txt`; Pulse in 1 overrides the tempo.
- B4 Loop/Sync tap and release hold; B2/B3 bank + static off/on holds.
- Two CV outs (random and triangle), two pulse outs, CV in 1/2 and audio in 1/2
  as described in the README.
- 64 KB streaming ring: about 0.34 s stereo or 0.68 s mono at 48 kHz.

## Parked, with reasons

### Slice mode — parked until the logic can be tested off-card

The intended Stage A was sixteen equal slices, F5 file, F3 slice, F4 ignored,
F6 loop/one-shot/ping-pong, F7 clock/`perfDiv`.

It failed on hardware at **67% RAM**, so this is **not** the USB-host RAM limit
that affected the crossfade. The slice/window logic needs a host-side test or a
much smaller one-step-at-a-time build before it goes back on the card.

If resumed:

1. Test the slice table, `ActiveWindow` and F7 request logic outside the card.
2. Add only F3 selection first, with no F7 clock and no file/table rebuild.
3. Add the F7 clock advance.
4. Add cue/smpl markers as a separate low-stack parser pass.
5. Add F4 slice-end trim.
6. Add transient/onset auto-slicing.

Do **not** repeat the previous approach of adding the table, parser and clock
together.

### Crossfade — shelved

The forward-loop crossfade was attempted at 96% RAM and then 93%; both failed to
boot. A smaller 10 ms version at 92% also failed. The slice failure at 67% shows
RAM was not the whole story, but the crossfade still needs a design that does not
touch the boot/mount path and can be tested in isolation.

### Freeze / live capture — later

Freeze/scrub/tape-stop and live capture would use the ring and the audio inputs.
They are attractive but need a deliberate performance mode and a RAM/UI plan.

### Per-file loop memory — later

Remembering start/end/speed/loop mode per file is small in RAM but adds
administration and persistence questions. The card currently never writes to
the stick; keep that unless there is a clear plan.

### Granular, effects, dual deck — later

These are larger architectural changes. Not part of the near-term roadmap.

## Guardrails

- **Boot/mount is the first test for every firmware change.** A build that does
  not mount is not a candidate.
- **Do not push RAM back toward the ~92% USB-host limit.** The stable build is at
  ~66.8%; the 64 KB ring created that headroom.
- **No WAV parser or file-path changes without a host-side test.** The slice
  failure at 67% proves that a feature can fail independently of RAM.
- **One feature at a time, with a boot test after each step.**
- **The 64 KB ring is a trade-off.** If slow sticks or high-rate files underrun,
  restoring `ringSize` to 128 KB is a small, isolated change; test the boot/mount
  before adding anything else.
- **No remote-only mode that cannot be reached from the panel.** Keep the card
  usable without the 8mu.

The stable tree is the one tagged `stable-2026-11-01.1`. The older development
branch with the failed experiments and reverts is local only and is not part of
this PR.

## If work resumes

1. Start from the stable tag, not from an experimental tip.
2. Confirm boot/mount before adding anything.
3. Prefer a host-side test for any slice/window/table logic.
4. Keep the 64 KB ring unless a specific test shows it is the problem.
5. Update the README, `info.yaml`, `settings.example.txt` and test notes with the
   feature.
