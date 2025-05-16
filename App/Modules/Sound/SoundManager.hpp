#pragma once

#include "Melody.hpp"

namespace Core::Sound {

// Класс SoundManager для управления воспроизведением мелодий
class SoundManager : public std::enable_shared_from_this<SoundManager>
{
public:
	SoundManager() = default;

	// Создать мелодию из строки
	std::shared_ptr<Melody> createMelody(const char * rawNotes, int rate)
	{
		return Melody::createMelody(rawNotes, rate, weak_from_this());
	}

	bool isBusy() const;                       // Проверить, занят ли менеджер
	void play(const std::weak_ptr<Melody>& melody); // Запустить мелодию
	void stop();                               // Остановить воспроизведение
	std::weak_ptr<Melody> currentMelody() const;    // Получить текущую мелодию
};

}
