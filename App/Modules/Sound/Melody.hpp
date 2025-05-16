#pragma once

#include <memory>
#include <vector>

namespace Core::Sound {

class SoundManager;

// Класс Melody для хранения и воспроизведения мелодии
class Melody : public std::enable_shared_from_this<Melody>
{
	std::weak_ptr<SoundManager> _weakManager; // Слабая ссылка на менеджер звука

public:
	// Структура одной ноты
	struct Note
	{
		uint32_t duration; // Длительность ноты
		uint32_t freq;     // Частота ноты
	};

	Melody() = default;
	~Melody() = default;

	// Создать мелодию из строки
	static std::shared_ptr<Melody> createMelody(const char* rawMelody, int rate, const std::weak_ptr<SoundManager>& soundManager);

	void play();                  // Воспроизвести мелодию
	bool isPlayed() const;        // Проверить, проигрывается ли мелодия
	void stop();                  // Остановить воспроизведение
	std::size_t countOfNotes() const; // Количество нот
	Note getNoteByIndex(int i) const; // Получить ноту по индексу
	int getRate() const;              // Получить частоту воспроизведения

private:
	std::vector<Note> _notes; // Массив нот
	int _rate;                // Частота воспроизведения
};

}
