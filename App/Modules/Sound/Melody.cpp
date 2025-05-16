#include "Melody.hpp"

#include <cmath>
#include "Definitions.hpp"
#include "SoundManager.hpp"

namespace {

// Парсинг строки мелодии в массив нот
std::vector<Core::Sound::Melody::Note> parseMelody(const char* bytes)
{
	std::vector<Core::Sound::Melody::Note> result;
	int index = 0;
	int note = -1;
	bool isDurationDefined = false;
	bool isLongNote = false;
	Core::Sound::Melody::Note tmpNote;
	while(bytes[index] != '\0') {
		auto byte = bytes[index];
		if(byte == ' ') {
			if(isLongNote) {
				tmpNote.duration = std::round(tmpNote.duration * 3.0f/2.0f);
			}
			result.push_back(tmpNote);
			isDurationDefined = false;
			isLongNote = false;
			note = -1;
			tmpNote = {};

		} else if('0' <= byte && byte <= '9') {
			if(isDurationDefined) {
				tmpNote.freq = Core::Sound::NOTES_FREQ[note] >> (UPPER_OCTAVE - (byte & 0x0F));
			} else if(tmpNote.duration == 0) {
				tmpNote.duration = (byte & 0x0F);
			} else {
				tmpNote.duration = tmpNote.duration * 10 + (byte & 0x0F);
			}
		} else if(byte == '-') {
			tmpNote.freq = 0;
			isDurationDefined = true;
		} else if(byte == '#') {
			if(note < SEMITONE_COUNT - 1) {
				note += 1;
			}
		} else if(('a' <= byte && byte <= 'g') || ('A' <= byte && byte <= 'G')) {
			if('a' <= byte && byte <= 'g') { // to upper case
				byte -= 'a' - 'A';
			}

			switch(byte)
			{
			case 'A': note += Core::Sound::A; break;
			case 'B': note += Core::Sound::B; break;
			case 'C': note += Core::Sound::C; break;
			case 'D': note += Core::Sound::D; break;
			case 'E': note += Core::Sound::E; break;
			case 'F': note += Core::Sound::F; break;
			case 'G': note += Core::Sound::G; break;
			default: break;
			}

			isDurationDefined = true;
		} else if(byte == '.') {
			isLongNote = true;
		}

		++index;
	}

	return result;
}

}

namespace Core::Sound {

// Создание мелодии из строки
std::shared_ptr<Melody> Melody::createMelody(const char* rawMelody, int rate, const std::weak_ptr<SoundManager>& soundManager)
{
	auto newMelody = std::make_shared<Melody>();
	newMelody->_weakManager = soundManager;
	newMelody->_rate = rate;
	newMelody->_notes = parseMelody(rawMelody);
	return newMelody;
}

// Воспроизвести мелодию
void Melody::play()
{
	if(auto manager = _weakManager.lock()) {
		manager->play(weak_from_this());
	}
}

// Проверить, проигрывается ли мелодия
bool Melody::isPlayed() const
{
	if(auto manager = _weakManager.lock()) {
		return manager->currentMelody().lock().get() != weak_from_this().lock().get();
	}
	return false;
}

// Остановить воспроизведение
void Melody::stop()
{
	if(auto manager = _weakManager.lock()) {
		manager->stop();
	}
}

// Получить количество нот
std::size_t Melody::countOfNotes() const
{
	return _notes.size();
}

// Получить ноту по индексу
Melody::Note Melody::getNoteByIndex(int i) const
{
	return _notes.at(i);
}

// Получить частоту воспроизведения
int Melody::getRate() const
{
	return _rate;
}

}
