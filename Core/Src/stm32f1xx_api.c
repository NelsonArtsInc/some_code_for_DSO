#include "stm32f1xx_api.h"

#include <math.h>
#include <string.h>
#include "tiny_printf.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal.h"

extern UART_HandleTypeDef huart1;

UART_HandleTypeDef* UARTChannels[1];
int writeSinkChannelId;
int hardfaultSinkChannelId;

typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;   // Link register.
    uint32_t pc;   // Program counter.
    uint32_t psr;  // Program status register.
} ProcessorStackFrame;

void initializeAPI()
{
	UARTChannels[0] = &huart1;
	writeSinkChannelId = 0; // default write channel
	hardfaultSinkChannelId = 0;
	initializeBuzzer();
	initializeControls();
}

void dumpProcessorTrace(ProcessorStackFrame* frame)
{
    uint32_t cfsr = SCB->CFSR;
    uint32_t hfsr = SCB->HFSR;
    uint32_t dfsr = SCB->DFSR;
    uint32_t afsr = SCB->AFSR;
    uint32_t mmfar = SCB->MMFAR;
    uint32_t bfar = SCB->BFAR;

    volatile uint32_t fault_r0   = frame->r0;
    volatile uint32_t fault_r1   = frame->r1;
    volatile uint32_t fault_r2   = frame->r2;
    volatile uint32_t fault_r3   = frame->r3;
    volatile uint32_t fault_r12  = frame->r12;
    volatile uint32_t fault_lr   = frame->lr;
    volatile uint32_t fault_pc   = frame->pc;
    volatile uint32_t fault_psr  = frame->psr;
    volatile uint32_t fault_cfsr = cfsr;
    volatile uint32_t fault_hfsr = hfsr;
    volatile uint32_t fault_dfsr = dfsr;
    volatile uint32_t fault_afsr = afsr;
    volatile uint32_t fault_mmfar = mmfar;
    volatile uint32_t fault_bfar  = bfar;

    UNUSED(fault_r0);
    UNUSED(fault_r1);
    UNUSED(fault_r2);
    UNUSED(fault_r3);
    UNUSED(fault_r12);
    UNUSED(fault_lr);
    UNUSED(fault_pc);
    UNUSED(fault_psr);
    UNUSED(fault_cfsr);
    UNUSED(fault_hfsr);
    UNUSED(fault_dfsr);
    UNUSED(fault_afsr);
    UNUSED(fault_mmfar);
    UNUSED(fault_bfar);

	const char * msg = "HARD FAULT\n\r";
	UARTTransmit(hardfaultSinkChannelId, msg, strlen(msg), NULL);

    while (1) {} // For debugging, place breakpoint here and investigate variables
}

void hardfaultInternalHandler()
{
    __asm volatile (
        "TST lr, #4            \n" // Test bit 2 of LR to determine the current stack pointer.
        "ITE EQ                \n"
        "MRSEQ r0, MSP         \n" // If 0, use MSP.
        "MRSNE r0, PSP         \n" // Else, use PSP.
        "B dumpProcessorTrace  \n" // Branch to a C function that will print the fault info.
    );
}

bool writeSink(const char * data, int dataSize)
{
	bool isOk;
	UARTTransmit(writeSinkChannelId, data, dataSize, &isOk);
	return isOk;
}

void printTimeStamp(uint32_t timeMs)
{
	int hours = timeMs / 3600000;
	timeMs %= 3600000;
	int minutes = timeMs / 60000;
	timeMs %= 60000;
	int seconds = timeMs / 1000;
	int milliseconds = timeMs % 1000;
	printf("[%02i:%02i:%02i.%03i] ", hours, minutes, seconds, milliseconds);
}

void logString(const char * data, ...)
{
	printTimeStamp(getTick());

	va_list va;
	va_start(va, data);
	(void)vprintf(data, va);
	va_end(va);

	printf("\r\n");
}

uint32_t getTick()
{
	return HAL_GetTick();
}

void delayMS(uint32_t ms)
{
	HAL_Delay(ms);
}

void delayUS(uint32_t us)
{
	us = round(us * (SystemCoreClock / 6000000.0f));
	us--;
	__asm volatile(
		"mov r0, %[us]  \n"
		"1: subs r0, #1 \n"
		"bhi 1b         \n"
		:
		: [us] "r" (us)
		: "r0"
	);
}

void UARTTransmit(int channelId, const char* data, int dataSize, bool* isOk)
{
//	ASSERT(channelId < sizeof(UARTChannels));
	HAL_StatusTypeDef status = HAL_UART_Transmit(UARTChannels[channelId], (uint8_t*)data, dataSize, UART_TRANSMIT_TIMEOUT_MS);
	if(isOk != NULL) {
		*isOk = status == HAL_OK ? 1 : 0;
	}
}
void LED_ON(bool state){
HAL_GPIO_WritePin(EXTERN_LED_GPIO_Port, EXTERN_LED_Pin, !state);}
