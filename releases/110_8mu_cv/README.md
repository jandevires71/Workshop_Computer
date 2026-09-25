# 8mu CV

*A Music Thing [8mu](https://www.musicthing.co.uk/8mu.html) as four voltages and two pulses.*

A program card for the Music Thing Modular Workshop System Computer. Plug an
8mu into the Computer's front USB-C jack and its six lefthand faders become:

| 8mu control | Jack | What it does |
|---|---|---|
| **Fader 1** | Audio Out 1 | Voltage, centre = 0V |
| **Fader 2** | Audio Out 2 | Voltage, centre = 0V |
| **Fader 3** | CV Out 1 | Voltage, centre = 0V |
| **Fader 4** | CV Out 2 | Voltage, centre = 0V |
| **Fader 5** | Pulse Out 1 | Square wave rate, 0.1Hz to 20Hz |
| **Fader 6** | Pulse Out 2 | Square wave rate, 0.1Hz to 20Hz |

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

## The pulses

Pulse Out 1 and Pulse Out 2 are independent square waves with a fixed 50%
duty cycle. Fader 5 and fader 6 set their rates between 0.1Hz and 20Hz, with
an exponential response so that equal movements of the fader multiply the rate
by the same amount. That matches the way speed is heard: the bottom of the
fader is a slow blink, the top is a fast pulse.

Patch them into clocks, triggers, gates, or anything that wants a rhythmic
on/off. At the bottom they are slow enough to use as an LFO.

## Playing it without a controller

With no 8mu attached, the three panel knobs take over, in two pages selected
by the switch:

| Switch | Main | X | Y |
|---|---|---|---|
| **Middle** | Audio Out 1 | Audio Out 2 | CV Out 1 |
| **Up** | CV Out 2 | Pulse Out 1 rate | Pulse Out 2 rate |

The 8mu always wins while it is connected, so the panel and the controller can
never fight over a parameter. Unplug it and the knobs pick up from wherever
the faders last left things, rather than jumping.

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
