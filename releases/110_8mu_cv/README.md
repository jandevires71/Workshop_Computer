# 8mu CV

*A Music Thing [8mu](https://www.musicthing.co.uk/8mu.html) as four voltages and two pulses.*

A program card for the Music Thing Modular Workshop System Computer. Plug an
8mu into the Computer's front USB-C jack and its eight lefthand faders and
four top buttons become:

| 8mu control | Jack | What it does |
|---|---|---|
| **Fader 1** | Audio Out 1 | Voltage, centre = 0V (or LFO rate) |
| **Fader 2** | Audio Out 2 | Voltage, centre = 0V (or LFO rate) |
| **Fader 3** | CV Out 1 | Voltage, centre = 0V (or LFO rate) |
| **Fader 4** | CV Out 2 | Voltage, centre = 0V (or LFO rate) |
| **Fader 5** | Pulse Out 1 | Rate, 0.1Hz to 20Hz |
| **Fader 6** | Pulse Out 2 | Rate, 0.1Hz to 20Hz |
| **Fader 7** | Pulse Out 1 | Width, centre is a 50% square |
| **Fader 8** | Pulse Out 2 | Width, centre is a 50% square |
| **Button 1** (C2) | Audio Out 1 | Triangle LFO on/off |
| **Button 2** (C3) | Audio Out 2 | Triangle LFO on/off |
| **Button 3** (C4) | CV Out 1 | Triangle LFO on/off |
| **Button 4** (C5) | CV Out 2 | Triangle LFO on/off |

The audio outputs are DC-coupled on this hardware, so they carry a steady
voltage just as well as sound does. That is what makes four voltage outputs
possible from two audio jacks and two CV jacks, rather than the usual two.
None of them are calibrated, which does not matter here: this card is a source
of control voltages, not a pitch reference.

## The voltages

Each of the four voltage outputs follows its fader from about -5V at the
bottom, through 0V in the middle, to about +5V at the top. The voltage is
smoothed very slightly, so the steps in the 8mu's 7-bit faders do not click
into whatever the voltage is controlling.

Use them for offsets, for holding a filter or oscillator at a position, or as
a manual voltage source you can dial in by hand.

## LFOs

Any of the four voltage outputs can be a **triangle LFO** instead of a steady
voltage. A tap on one of the four top buttons switches that output's LFO on or
off.

Switching one **on** captures the voltage the output is already sitting at and
oscillates around it, and the fader that was setting the level now sets the
**speed** (0.1Hz to 20Hz, exponential, the same feel as the pulse rates). So
the move is: dial in a voltage, tap the button, and it comes alive around that
point while your finger takes over the rate.

The triangle always swings the full distance to whichever supply rail is
nearer, so a voltage set close to a rail simply moves less. It can never clip.

Switching one **off** glides the output back to the captured voltage. The
fader does not grab the level immediately, because it is sitting at a rate
position - it stays locked until you move it to meet the captured value, so
nothing jumps.

**Holding any one of the four buttons for a second and a half stops all four
LFOs at once.** This is the way back if the card is running without the 8mu:
the on/off state deliberately survives the controller being unplugged, so
without this a card left in LFO mode would have no way to stop.

The LFOs keep running if the 8mu is unplugged, and their rates then follow
whatever the active panel page feeds the four left-hand parameters.

## The pulses

Pulse Out 1 and Pulse Out 2 are independent pulse streams. Fader 5 and fader 6
set their rates between 0.1Hz and 20Hz, with an exponential response so that
equal movements of the fader multiply the rate by the same amount. That matches
the way speed is heard: the bottom of the fader is a slow blink, the top is a
fast pulse.

Fader 7 and fader 8 set their **width** - how much of each cycle the output
stays high. The middle of the fader is a 50% square, the way up widens it and
the way down narrows it:

| Fader 7 / 8 | Duty | What it is good for |
|---|---|---|
| Bottom (~2%) | very narrow | a sharp trigger or clock edge |
| Middle (50%) | square | the classic LFO / gate shape |
| Top (~98%) | very wide | a gate that holds a note or envelope open nearly all the time |

The width never quite reaches zero or full, so there is always a pulse being
produced and never a dead or stuck output. Like the voltages, the width is
smoothed slightly, so sweeping the fader does not stretch a single pulse as it
passes.

Width is reachable only from the 8mu: the panel has six control slots and the
card has eight parameters. Without a controller both widths stay at a plain
50% square.

Patch them into clocks, triggers, gates, or anything that wants a rhythmic
on/off. At the bottom they are slow enough to use as an LFO; widen the pulses
for trigger duties and narrow-shape them into gate streams.

## Playing it without a controller

With no 8mu attached, the three panel knobs take over, in two pages selected
by the switch:

| Switch | Main | X | Y |
|---|---|---|---|
| **Middle** | Audio Out 1 | Audio Out 2 | CV Out 1 |
| **Up** | CV Out 2 | Pulse Out 1 rate | Pulse Out 2 rate |

The 8mu always wins while it is connected, so the panel and the controller can
never fight over a parameter. Unplug it and the knobs pick up from wherever
the faders last left things, rather than jumping. The two pulse widths are not
on the panel, so they stay at 50% while no controller is attached, and the LFO
on/off state is kept - press and hold any top button to stop them.

## Panel LEDs

| LED | Meaning |
|---|---|
| 0 | Lit while an 8mu is connected |
| 1 | Brightness follows the audio-out voltage on fader 1 |
| 2 | Follows Pulse Out 1 |
| 3 | Follows Pulse Out 2 |
| 4 | Lit on the panel's second page (switch up, no 8mu) |
| 5 | Brightness follows the CV-out voltage on fader 3 |

## Requirements

The 8mu is a USB device, so the Computer has to act as a USB host: this needs
a **Rev 1.1 or later** board, and nothing else may be plugged into the
Computer's front USB socket. The card is a pure MIDI listener - it never
writes to the 8mu's LEDs or settings, so the controller behaves exactly as it
does anywhere else.

## Building

```bash
cmake -B build -G Ninja
cmake --build build
```

Drag `build/8mu_cv.uf2` onto the Pico in bootloader mode, or use the binary
committed at `UF2/8mu_cv.uf2`.

## Credits

- **Chris Johnson** — the ComputerCard library and `EightMU.h`, which carries
  the USB MIDI host driver and the TinyUSB host configuration.
- **rppicomidi** — the `usb_midi_host` driver, MIT, vendored inside
  `EightMU.h`.
- **Tom Whitwell / Music Thing Modular** — the Workshop System and the 8mu.

## License

MIT. See `LICENSE`, which records the vendored components and their terms.
