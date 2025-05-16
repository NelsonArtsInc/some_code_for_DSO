#pragma once

#include <memory>
#include <vector>

namespace Core::Sound {

class SoundManager;

class Melody : public std::enable_shared_from_this<Melody>
{
	std::weak_ptr<SoundManager> _weakManager;

public:
	struct Note
	{
		uint32_t duration;
		uint32_t freq;
	};

	Melody() = default;
	~Melody() = default;
	static std::shared_ptr<Melody> createMelody(const char* rawMelody, int rate, const std::weak_ptr<SoundManager>& soundManager);

	void play();
	bool isPlayed() const;
	void stop();
	std::size_t countOfNotes() const;
	Note getNoteByIndex(int i) const;
	int getRate() const;

private:
	std::vector<Note> _notes;
	int _rate;
};

}
