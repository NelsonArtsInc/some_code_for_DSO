#include "SoundManager.hpp"

#include "stm32f1xx_api.h"
#include "Definitions.hpp"

namespace {

// Индекс текущей ноты
int _currerntNote = 0;
// Слабая ссылка на текущую проигрываемую мелодию
std::weak_ptr<Core::Sound::Melody> _currentMelodyWeak;

// Запуск следующей ноты мелодии
void startNextNote()
{
	if(auto melody = _currentMelodyWeak.lock()) {
		if(_currerntNote < melody->countOfNotes()) {
			auto note = melody->getNoteByIndex(_currerntNote++);
			auto duration = 60000 / melody->getRate() * 4 / note.duration;
			playSoundDeferred(_currerntNote == 1 ? 0 : PAUSE_BETWEEN_NOTES_MS, note.freq, 100, duration);
		}
	}
}

}

// Обработчик окончания проигрывания звука (вызывается из HAL)
void onBuzzerStop()
{
	startNextNote();
}

namespace Core::Sound {

// Запустить проигрывание мелодии
void SoundManager::play(const std::weak_ptr<Melody>& melody)
{
	_currentMelodyWeak = melody;
	_currerntNote = 0;

	startNextNote();
}

// Проверить, занят ли звуковой менеджер
bool SoundManager::isBusy() const
{
	return !_currentMelodyWeak.expired();
}

// Остановить проигрывание
void SoundManager::stop()
{
	_currentMelodyWeak.reset();
}

// Получить текущую проигрываемую мелодию
std::weak_ptr<Melody> SoundManager::currentMelody() const
{
	return _currentMelodyWeak;
}

}
