#pragma once

#include <avr/io.h>

#define SYSCLK 16000000

inline void delayloop16 (uint16_t count);

#define INT0_LOW			(0b00)
#define INT0_LOGIC_CHANGE	(0b01)
#define INT0_FALLING_EDGE	(0b10)
#define INT0_RISING_EDGE	(0b11)

#define INT1_LOW			(0b0000)
#define INT1_LOGIC_CHANGE	(0b0100)
#define INT1_FALLING_EDGE	(0b1000)
#define INT1_RISING_EDGE	(0b1100)

#define SBI(port,bit) port |= (1<<bit)
#define CBI(port,bit) port &= ~(1<<bit)
#define NOP asm volatile ("nop");

