# Bootleg Radio — Phase 1 & 2 test notes

Branch: `538-bootleg-radio-8mu-remote`
Firmware: `UF2/bootleg_radio.uf2`
Phase 1 commit: `49eeece7` (speed range, one-shot, bank LEDs)
Phase 2 commit: see the commit that adds this file (loop/station fades)

Tested by: ____________________  Date: ____________

## Setup

- USB stick with a few WAVs: one long file (> 1 minute), one short loop
  (100–500 ms of sustained tone), one drum/percussion loop, and one quiet file.
- `settings.txt` in the root. Edit it, then **reinsert the stick or power-cycle**
  so the card re-reads it.
- 8mu: TRS output set to **Type A**, stereo TRS cable to **Pulse in 2**, powered
  from its own charger/power bank.
- Have a copy of the default settings handy:

```ini
mode = sync
loopMode = loop
bank = 0
speedRange = 2oct
clockBpm = 120
triPulses = 40
pulse1 = gate
pulse2 = tick
led = verbose
remote = auto
```

- Flash `UF2/bootleg_radio.uf2` with the usual Workshop Computer workflow.
- If anything gets stuck, power-cycle the Computer and reinsert the stick.

## Phase 1 — settings and UI

### 1. Speed range

| Setting | Expected |
|---|---|
| `2oct` (default) | X knob full travel is roughly −2..+2 octaves; centre is 1× |
| `1oct` | Full travel is roughly −1..+1 octave; centre is 1× |
| `fine` | Full travel is about ±1 semitone; useful as a gentle pitch bend |
| `off` | Playback is fixed at 1×; X, CV in 2 and 8mu F1 do nothing |

- [ ] `2oct`: X centre is normal speed; extremes are clearly ±2 octaves.
- [ ] `1oct`: extremes are about half the old pitch/speed range.
- [ ] `fine`: moving X end-to-end is a tiny pitch bend, not a tape stop.
- [ ] `off`: moving X, patching CV in 2, or moving 8mu F1 does not change speed.
- [ ] CV in 2 follows the chosen range.
- [ ] 8mu F1 changes the speed within the selected range.
- [ ] 8mu F2 above its dead zone overrides the range: off (8–37), fine (38–67),
      ±1 octave (68–97), ±2 octaves (98–127).
- [ ] Changing `speedRange` while playing lands on the new range without a
      crash. A jump is expected.
- [ ] Speed still links pitch and time in every range (no time-stretch).

### 2. One-shot

| Setting | Expected |
|---|---|
| `loopMode = loop` | Original behaviour: wraps forever |
| `loopMode = one-shot` | Plays the window once, stops, waits |

- [ ] `loop`: the station loops normally.
- [ ] `one-shot`: the station plays from loop start to loop end once, then
      stops. LED 2 goes off. It does **not** restart by itself.
- [ ] Switch-down tap retriggers and plays once again.
- [ ] 8mu B1 retriggers and plays once again.
- [ ] Pulse in 2 retriggers when `remote = auto` (before an 8mu is heard) or
      `remote = off`.
- [ ] Station change starts the new station and it plays once.
- [ ] Bank change starts the new bank's station and it plays once.
- [ ] `one-shot` + `Sync`: a new station starts at the same proportional
      position; if that is near the end, it may play only a little. Expected.
- [ ] Move F3/F4 while stopped: after the ~250 ms settle the loop edit
      retriggers, and the station plays once from the new window.
- [ ] The end of a one-shot fades out rather than cutting (Phase 2).

### 3. Bank LEDs

While the panel switch is **Up**, LEDs 0–4 show the **target** bank in binary;
LED 5 is off.

| Bank | LEDs 0–4 |
|---|---|
| 0 | all off |
| 1 | 0 on |
| 2 | 1 on |
| 3 | 0 + 1 on |
| 4 | 2 on |
| 8 | 3 on |
| 16 | 4 on |
| 31 | 0–4 on |

- [ ] Hold switch up and turn Main: the LEDs count in binary.
- [ ] The display follows the target before the switch is released.
- [ ] Release the switch: the normal level/status display returns.
- [ ] Bank 0 is all LEDs off (not a fault; it is bank 0).
- [ ] `led = quiet`: the bank display still works while the switch is up, and
      the normal quiet display returns on release.
- [ ] With the 8mu station fader active, holding switch up and moving F5 also
      shows the target bank.
- [ ] No audio glitch while the bank LEDs are active.

## Phase 2 — fades

The fades are always on. There is no setting to turn them off.

### 4. Loop seam

- [ ] A short loop of a sustained sine/tone: the wrap no longer clicks.
      A very small hole may remain; that is the USB reload, not the fade.
- [ ] A drum loop: the wrap does not snap. The first transient at the loop
      start may be slightly softened by the fade-in; expected.
- [ ] Slow speed (`1oct` or `2oct` near the bottom): still no click.
- [ ] Fast speed (`2oct` near the top): still no click.
- [ ] Very short loop (< ~20 ms): the fade shortens with the loop; a 1-frame
      loop is still a buzz. Expected.
- [ ] One-shot end: fades out rather than cutting.
- [ ] Loop end change (F4) then settle: retrigger and fade; no click. START
      above END gives reverse.

### 5. Station, bank and retrigger fades

Use a sustained tone so a cut would be obvious.

- [ ] Main knob station change: the output dips for about 5 ms, then the new
      station fades in. No hard click.
- [ ] CV in 1 station change: same.
- [ ] 8mu F5 station change: same.
- [ ] 8mu B2/B3 bank tap: same; the new bank starts as before.
- [ ] Panel switch-up bank change: same.
- [ ] Switch-down retrigger: fades and restarts; no click.
- [ ] 8mu B1 retrigger: fades and restarts; no click.
- [ ] Pulse in 2 retrigger (`remote = off` or `auto` before detection): fades
      and restarts.
- [ ] Rapid tuning across several stations: the card should keep up and land on
      a station. Requests may coalesce; it should not get stuck silent.
- [ ] Soft release (hold B4 ~1.5 s, or hold switch up ~2 s): the sound still holds where it
      is; the panel knobs take over when moved. The fade does not change this.
- [ ] One-shot restart: fades in; no click.
- [ ] Loop edit retrigger (F3/F4, settle): fades; no click.
- [ ] Fades still work at the extremes of `speedRange`.
- [ ] No new LED 5 underruns caused by the fades.

## Tuning static

Set `static = on` and start with `staticLevel = 50`.

- [ ] Changing station with Main produces a short crackle, then the new station
      fades in. The old station fades out first; the static fills the gap.
- [ ] CV in 1 station change: same.
- [ ] 8mu F5 station change: same.
- [ ] Panel bank change: same.
- [ ] 8mu B2/B3 bank tap: same.
- [ ] Hold 8mu B2: static turns off (one all-LED flash); the next station change is
      silent. Hold B3: static turns on again (four flashes). The change survives
      releasing the remote.
- [ ] `staticLevel = 0`: the burst is silent but station changes still work.
- [ ] `staticLevel = 100`: louder, but no hard clipping or nasty distortion.
- [ ] `static = off`: no burst.
- [ ] Retrigger (switch down, 8mu B1, Pulse in 2) does **not** trigger static.
- [ ] The loop seam does **not** trigger static.
- [ ] The one-shot end does **not** trigger static.
- [ ] CV and pulse outputs are unaffected by the static.
- [ ] Rapid tuning retriggers the burst rather than leaving a stuck noise.
- [ ] No new LED 5 underruns with static on.

## Regression checks from earlier phases

- [ ] F1 speed, F2 speed range, F3 loop start, F4 loop end, F5 station.
- [ ] F6 loop mode (loop/one-shot/ping-pong), F7 performance mode, F8 pulse 1
      mode. Clock BPM and pulse out 2 stay in settings.txt.
- [ ] B4 tap toggles Loop/Sync (2/3 flashes) and persists after release; B4
      hold ~1.5 s releases; hold switch up ~2 s also releases.
- [ ] B2/B3 tap changes bank on release; a hold turns static off/on and does not
      change bank.
- [ ] Bank buttons land on the relative station position when the station fader
      (F5) is active.
- [ ] CV still adds to station/speed/start (F5/F1/F3).
- [ ] Loop end: fader-only while remote; sticky on release when Audio in 2 is
      unpatched. START above END gives reverse.
- [ ] `remote = auto` still uses Pulse in 2 as a retrigger until an 8mu is heard.
- [ ] Power-up snapshot: power the 8mu after the card; nothing jumps until a
      single fader is moved.
- [ ] One-shot and loop modes survive a stick reinsert with the new settings.

## Results

| Test | Pass | Notes |
|---|---|---|
| Speed range — 2oct | | |
| Speed range — 1oct | | |
| Speed range — fine | | |
| Speed range — off | | |
| One-shot — stop/retrigger | | |
| One-shot — station/bank | | |
| One-shot — loop edit | | |
| Bank LEDs — binary | | |
| Bank LEDs — bank 0 | | |
| Loop seam — tone | | |
| Loop seam — drum | | |
| Loop seam — slow/fast | | |
| Loop seam — very short | | |
| Station fade — panel | | |
| Station fade — CV/8mu | | |
| Bank/retrigger fade | | |
| One-shot end fade | | |
| Rapid tuning | | |
| Soft release regression | | |
| Remote/loop edit regression | | |
| Tuning static — on/off | | |
| Tuning static — level | | |
| Tuning static — B2/B3 hold | | |
| Tuning static — no false triggers | | |
| Remote — F1 speed / F2 range | | |
| Remote — F5 station / bank relative | | |
| Remote — B4 mode / release | | |
| No new underruns | | |

## Known limits of this round

- The loop seam is a **fade**, not a true overlapping crossfade. The file reload
  gap can still be heard as a short hole.
- Station changes are a 5 ms fade-out + fade-in, not a two-sample crossfade. The
  request is deliberately delayed by about 5 ms so the fade is real.
- Very short loops get proportionally shorter fades; a one-frame loop is still a
  buzz.
- Reverse, ping-pong and the 8mu START/END and button map are covered in
  TESTING-reverse-pingpong.md. Background advance and motion are still out of
  scope.
- The 8mu link remains one-way; there is no feedback to the 8mu itself, though
  the card's LEDs show mode, static and takeover state.

If the loop seam is still too cut, the next step is the 20 ms head-buffer
crossfade. If fast retriggers feel sloppy, the 5 ms request delay can be
shortened or made a setting.
