// 8mu CV
//
// A simple breakout for a Music Thing Modular 8mu USB MIDI controller.  The
// card turns the 8mu's eight lefthand faders into four control voltages and
// two pulse streams:
//
//   fader 1  ->  Audio Out 1   (bipolar voltage, centre is 0V)
//   fader 2  ->  Audio Out 2   (bipolar voltage, centre is 0V)
//   fader 3  ->  CV Out 1      (bipolar voltage, centre is 0V)
//   fader 4  ->  CV Out 2      (bipolar voltage, centre is 0V)
//   fader 5  ->  Pulse Out 1   rate, 0.1Hz to 20Hz
//   fader 6  ->  Pulse Out 2   rate, 0.1Hz to 20Hz
//   fader 7  ->  Pulse Out 1   width, centre is a 50% square
//   fader 8  ->  Pulse Out 2   width, centre is a 50% square
//
// The audio outputs are DC-coupled on this hardware, so they carry a steady
// voltage just as well as sound does.  That gives four independent voltage
// sources rather than the usual two CV outputs.  All four are uncalibrated,
// which is fine here: this is a source of control voltages, not a pitch
// reference, and the two audio outs could never be calibrated anyway.  The
// useful range is roughly -5V to +5V, with the middle of a fader at 0V.
//
// Play it without a controller too.  With no 8mu plugged in the three panel
// knobs take over, in two pages selected by the switch:
//
//   switch middle:  Main = Audio 1,  X = Audio 2,  Y = CV 1
//   switch up:      Main = CV 2,     X = Pulse 1,  Y = Pulse 2
//
// The panel has exactly six slots and the card has eight parameters, so pulse
// width is reachable only from the 8mu: with no controller those two stay at
// a 50% square, which is what the card did before faders 7 and 8 existed.
//
// The 8mu wins whenever it is connected, so the panel cannot fight it for a
// parameter.  Unplug the controller and the knobs pick up from where the
// faders last left things, rather than jumping.
//
// The 8mu is used purely as a MIDI source: this card never talks to its
// LEDs.  Plug it into the Computer's front USB-C jack, which needs a Rev 1.1+
// board and nothing else in that socket.  The Computer is the USB host.

#include "ComputerCard.h"
#include "EightMU.h"
#include "hardware/clocks.h"

#include <math.h>
#include <stdint.h>


class EightMUCVCard : public ComputerCard
{
public:
	EightMUCVCard()
	{
		// Work out the pulse-rate table once, before audio starts.  This is
		// the only floating point in the card, and it runs a single time at
		// power-on, so it costs nothing in the audio path.
		InitRateTable();

		// A USB device needs to be powered before it will enumerate, and the
		// Computer's host port takes a moment to settle.  Only start the USB
		// stack if the port really is in host mode; on older revisions, or
		// with a cable plugged into a computer, USBPowerState() says so and
		// there is nothing to do.
		sleep_us(150000);
		if (USBPowerState() == DFP)
		{
			mu.Start();   // claims core 1 and runs the USB host stack there
		}
	}

	virtual void ProcessSample()
	{
		// --- where do the eight parameters come from this sample? -----------
		//
		// The 8mu takes priority whenever it is mounted.  Without one, the
		// panel's two pages fill in whichever three parameters they cover;
		// the rest hold their last value, so nothing drops out when the
		// switch moves.  The two pulse widths are never on the panel, so
		// without a controller they stay where they were initialised: a
		// plain 50% square.
		if (mu.Connected())
		{
			for (int i = 0; i < 8; i++)
			{
				param[i] = mu.Fader(i);
			}
		}
		else if (SwitchVal() == Switch::Up)
		{
			param[3] = KnobVal(Knob::Main);   // CV 2
			param[4] = KnobVal(Knob::X);      // Pulse 1 rate
			param[5] = KnobVal(Knob::Y);      // Pulse 2 rate
		}
		else
		{
			param[0] = KnobVal(Knob::Main);   // Audio 1
			param[1] = KnobVal(Knob::X);      // Audio 2
			param[2] = KnobVal(Knob::Y);      // CV 1
		}

		// --- four DC voltages ----------------------------------------------
		//
		// Faders and knobs both rest near 2048 at their centre (a fader sends
		// CC 64 there), so subtracting 2048 turns a position into a bipolar
		// voltage with 0V in the middle.  The targets are slewed gently to
		// hide the steps in the 7-bit MIDI fader: a jump of one CC number
		// would otherwise be an audible/visible click in whatever the voltage
		// is controlling.
		for (int i = 0; i < 4; i++)
		{
			int32_t target = param[i] - 2048;
			if (target < -2048) target = -2048;
			if (target > 2047) target = 2047;

			// One-pole smoothing, held in Q8 so the small differences that
			// remain as it settles do not get truncated away.  Multiplication
			// rather than a left shift, because target can be negative and
			// shifting a negative value is undefined in C++.
			levelQ8[i] += ((target * 256) - levelQ8[i]) >> 8;
		}

		AudioOut1((int16_t)(levelQ8[0] >> 8));
		AudioOut2((int16_t)(levelQ8[1] >> 8));
		CVOut1((int16_t)(levelQ8[2] >> 8));
		CVOut2((int16_t)(levelQ8[3] >> 8));

		// --- two pulse streams ----------------------------------------------
		//
		// A 32-bit phase accumulator: add a per-sample increment and it wraps
		// once per cycle.  The rate increment table covers 0.1Hz to 20Hz, and
		// because the faders are 7-bit it has one entry per possible CC value.
		//
		// The output goes high for the first part of each cycle and low for
		// the rest, so the width fader sets the duty cycle: narrow at one end
		// is a trigger, centred is a square, wide at the other is a gate.  The
		// threshold is a top-20-bit value so it lines up exactly with the
		// phase, and it can never reach 0 or full scale, so there is always
		// some low and some high - a pulse is always produced.
		phase[0] += rateInc[param[4] >> 5];
		phase[1] += rateInc[param[5] >> 5];

		for (int i = 0; i < 2; i++)
		{
			// Bipolar width about the centre: 2048 is a 50% square, and the
			// extremes map to 2% and 98% of the cycle.
			int32_t widthQ12 = 2048 + (((param[6 + i] - 2048) * 1966) / 2048);

			// Slew it like the voltage levels.  Without this a fast fader
			// sweep would momentarily stretch or shorten a pulse, and the
			// pulse LEDs would flicker.
			widthQ8[i] += ((widthQ12 * 256) - widthQ8[i]) >> 8;

			uint32_t threshold = (uint32_t)(widthQ8[i] >> 8) << 20;
			pulseHigh[i] = phase[i] < threshold;
		}

		PulseOut1(pulseHigh[0]);
		PulseOut2(pulseHigh[1]);

		// --- panel feedback -------------------------------------------------
		//
		// LED 1 and LED 5 show the Audio Out 1 and CV Out 1 levels as a steady
		// brightness, which is enough to read the fader position at a glance.
		// The other two voltage outs have no LEDs to spare, but they follow
		// the same faders, so the panel is still a fair picture.
		LedOn(0, mu.Connected());                                   // controller mounted
		LedBrightness(1, LevelLed(levelQ8[0]));
		LedOn(2, pulseHigh[0]);                                     // pulse 1 following
		LedOn(3, pulseHigh[1]);                                     // pulse 2 following
		LedOn(4, !mu.Connected() && SwitchVal() == Switch::Up);     // panel page 2
		LedBrightness(5, LevelLed(levelQ8[2]));
	}

private:
	// The 8mu only has 7-bit faders (its CC values are 0-127), so the rate
	// table needs exactly that many entries.  param >> 5 reduces either a
	// fader (0-4064) or a knob (0-4095) to 0-127.
	static constexpr int kRateSteps = 128;

	// Phase increment per sample for a 32-bit accumulator at 48kHz.  The
	// musical choice here is an exponential fader response: equal movements
	// of the fader multiply the rate by the same factor, which is how the ear
	// hears speed, rather than adding a fixed number of Hz.
	int32_t rateInc[kRateSteps];

	// Panel/8mu position for each parameter (0-4095), remembered across
	// samples so the two panel pages can each own half of the parameters.
	// The last two are the pulse widths, which only the 8mu can reach; they
	// start at 2048 so a controllerless card boots to a 50% square.
	int32_t param[8] = {2048, 2048, 2048, 2048, 0, 0, 2048, 2048};

	// Smoothed voltage outputs, Q8 (-2048<<8 to 2047<<8).
	int32_t levelQ8[4] = {0, 0, 0, 0};

	// Smoothed pulse widths, Q8 (82<<8 to 4013<<8, a 2% to 98% duty cycle).
	int32_t widthQ8[2] = {2048 << 8, 2048 << 8};

	// Square-wave phase accumulators, one per pulse output.
	uint32_t phase[2] = {0, 0};

	// This sample's pulse outputs, kept so the panel LEDs show the same thing
	// the jacks do.
	bool pulseHigh[2] = {false, false};

	EightMU mu;

	// Brightness for a voltage-level LED: the absolute level (Q8), scaled to
	// the 0-4095 the LEDs expect.  Clamped at 4095 because LedBrightness
	// squares its argument, and 4096 would overflow the 16-bit product back
	// down to zero - a fully deflected fader would put its LED out.
	static uint16_t LevelLed(int32_t q8)
	{
		int32_t v = q8 < 0 ? -q8 : q8;
		int32_t led = v >> 7;
		if (led > 4095) led = 4095;
		return (uint16_t)led;
	}

	void InitRateTable()
	{
		const float fMin = 0.1f;
		const float fMax = 20.0f;
		const float ratio = powf(fMax / fMin, 1.0f / (float)(kRateSteps - 1));
		const float samplesPerSecond = 48000.0f;
		const float halfCycleToPhase = 4294967296.0f / samplesPerSecond;

		float f = fMin;
		for (int i = 0; i < kRateSteps; i++)
		{
			rateInc[i] = (int32_t)(f * halfCycleToPhase);
			f *= ratio;
		}
	}
};


int main()
{
	set_sys_clock_khz(200000, true);

	EightMUCVCard card;
	card.Run();
}
