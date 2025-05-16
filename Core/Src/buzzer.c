#include "buzzer.h"

#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_conf.h"

// Максимально допустимая частота сигнала для буззера
#define MAX_FREQUENCY 2000000
// Максимальная частота таймера
#define MAX_FREQUENCY_OF_TIMER 100000000
// Максимальная громкость (в условных единицах)
#define MAX_VOLUME 100

// Продолжительность исходного сигнала (-1 — бесконечно)
static int32_t _originDuration = -1;
// Счетчик оставшейся продолжительности сигнала
static int32_t _durationCounter = -1;
// Указатель на используемый таймер
static TIM_HandleTypeDef* _timer;
// Идентификатор канала таймера
static int _timerChannelId;

// Переменные для отложенного воспроизведения звука
static uint32_t _deferredSoundMs = -1;
static uint32_t _deferredFreq = 0;
static uint32_t _deferredVolume = 0;
static int32_t _deferredDurationMs = 0;

// Включение/выключение прерываний таймера
static void switchInterrupts(bool enable)
{
    if(enable) {
        HAL_TIM_Base_Start_IT(_timer); // Запуск таймера с прерываниями
    } else {
        HAL_TIM_Base_Stop_IT(_timer);  // Остановка таймера с прерываниями
    }
}

// Включение/выключение PWM на таймере
static void switchPWM(bool enable)
{
    if(enable) {
        HAL_TIM_PWM_Start(_timer, _timerChannelId); // Запуск PWM
    } else {
        HAL_TIM_PWM_Stop(_timer, _timerChannelId);  // Остановка PWM
    }
}

// Установка частоты и громкости сигнала
static void setFreqAndVolume(uint32_t freq, uint32_t volume)
{
    if(freq == 0) {
        return; // Если частота нулевая — ничего не делаем
    }

    if(freq > MAX_FREQUENCY) {
        freq = MAX_FREQUENCY; // Ограничение частоты
    }

    if(volume > MAX_VOLUME) {
        volume = MAX_VOLUME; // Ограничение громкости
    }

    uint32_t ARRPeriod = MAX_FREQUENCY_OF_TIMER / freq;

    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    // Вычисление значения импульса для нужной громкости
    sConfigOC.Pulse = ARRPeriod * volume / (2 * 100);
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    _timer->Init.Period = ARRPeriod;
    HAL_TIM_Base_Init(_timer); // Инициализация таймера
    HAL_TIM_PWM_ConfigChannel(_timer, &sConfigOC, _timerChannelId); // Конфигурирование канала PWM
}

// Уменьшение счетчика времени, возвращает true если счетчик достиг нуля
static bool decreaseCounter(uint32_t* counterMs)
{
    if(*counterMs == -1) {
        return false; // Бесконечный счетчик
    } else if(*counterMs == 0) {
        *counterMs = -1;
        return true; // Счетчик достиг нуля
    } else if( *counterMs >= _timer->Init.Period) {
        *counterMs = *counterMs - _timer->Init.Period; // Уменьшаем счетчик
        return false;
    } else {
        *counterMs = 0;
        return true; // Счетчик достиг нуля
    }
}

// Инициализация буззера (таймера и канала)
void initializeBuzzer()
{
    _timer = BUZZER_INTERRUPTS_TIMER;
    _timerChannelId = BUZZER_TIMER_CHANNEL_ID;

    _timer->Init.Period = 100000; // Значение периода по умолчанию
    HAL_TIM_Base_Init(_timer);
}

// Включение или выключение буззера
void switchBuzzer(bool enable)
{
    if(enable) {
        if(_originDuration > 0) {
            _durationCounter = _originDuration;
            switchInterrupts(true); // Включаем прерывания если есть ограничение по времени
        }
        switchPWM(true); // Включаем PWM
    } else {
        switchPWM(false); // Выключаем PWM
    }
}

// Установка параметров буззера (частота, громкость, длительность)
void setBuzzerParameters(uint32_t freq, uint32_t volume, int32_t durationMs)
{
    _originDuration = durationMs * 1000; // Переводим миллисекунды в микросекунды
    setFreqAndVolume(freq, volume);
}

// Обработка прерывания таймера для буззера
void handleBuzzerTimerInterrupt()
{
    if(decreaseCounter(&_deferredSoundMs)) {
        playSound(_deferredFreq, _deferredVolume, _deferredDurationMs); // Воспроизводим отложенный звук
        if(_originDuration == -1) {
            switchInterrupts(false); // Отключаем прерывания если звук бесконечный
        }
        // Сброс отложенных параметров
        _deferredSoundMs = -1;
        _deferredFreq = 0;
        _deferredVolume = 0;
        _deferredDurationMs = 0;
        return;
    }

    if(decreaseCounter(&_durationCounter)) {
        switchPWM(false); // Останавливаем звук
        if(_deferredSoundMs == -1) {
            switchInterrupts(false); // Отключаем прерывания если нет отложенного звука
        }
        onBuzzerStop(); // Вызываем обработчик остановки буззера
    }
}

// Воспроизведение звука с заданными параметрами
void playSound(uint32_t freq, uint32_t volume, int32_t durationMs)
{
    setBuzzerParameters(freq, volume, durationMs);
    switchBuzzer(true);
}

// Отложенное воспроизведение звука через delay миллисекунд
void playSoundDeferred(uint32_t delay, uint32_t freq, uint32_t volume, int32_t durationMs)
{
    if(delay == 0) {
        playSound(freq, volume, durationMs); // Если задержка нулевая — сразу воспроизводим
        return;
    }
    // Сохраняем параметры для отложенного воспроизведения
    _deferredSoundMs = delay;
    _deferredFreq = freq;
    _deferredVolume = volume;
    _deferredDurationMs = durationMs;

    switchInterrupts(true); // Включаем прерывания для отслеживания задержки
}

// Слабая функция-обработчик остановки буззера (может быть переопределена)
__weak void onBuzzerStop()
{

}
