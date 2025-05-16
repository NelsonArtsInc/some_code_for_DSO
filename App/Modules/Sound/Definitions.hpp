#pragma once

namespace Core::Sound {

#define SEMITONE_COUNT 12
#define UPPER_OCTAVE 5
#define PAUSE_BETWEEN_NOTES_MS 5

enum NOTES
{
	C = 0,
	D = 2,
	E = 4,
	F = 5,
	G = 7,
	A = 9,
	B = 12
};
static constexpr uint32_t NOTES_FREQ[SEMITONE_COUNT] = {
	418608, // C
	443488, // C# -_-
	469856, // D
	497808, // D#
	527408, // E
	558768, // F
	591984, // F#
	627200, // G
	664480, // G#
	704000, // A
	745856, // A#
	790208  // B
};

}
