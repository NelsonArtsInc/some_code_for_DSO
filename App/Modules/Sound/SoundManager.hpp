#pragma once

#include "Melody.hpp"

namespace Core::Sound {

class SoundManager : public std::enable_shared_from_this<SoundManager>
{
public:
	SoundManager() = default;
	std::shared_ptr<Melody> createMelody(const char * rawNotes, int rate)
	{
		return Melody::createMelody(rawNotes, rate, weak_from_this());
	}

	bool isBusy() const;
	void play(const std::weak_ptr<Melody>& melody);
	void stop();
	std::weak_ptr<Melody> currentMelody() const;
};

}
