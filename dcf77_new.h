#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>
#include "display_driver.h"

void init_dcf();
void add_one_sec(void);
void check_gong();
void welcome_gong();
void calculate_y2k_timestamp();
void calculate_utc();
void do_dcf77();

ISR(INT0_vect);
ISR(TIMER0_OVF_vect);
ISR(TIMER1_OVF_vect);

extern volatile struct time dcf77;	
extern volatile struct time utc;

extern volatile uint32_t y2k_timestamp;	
extern volatile uint8_t rx_bit_counter;


//Structur des dcf_rx_buffer
struct DCF77_Bits {
	unsigned long long M		:1	;
	unsigned long long O1		:1	;
	unsigned long long O2		:1	;
	unsigned long long O3		:1	;
	unsigned long long O4		:1	;
	unsigned long long O5		:1	;
	unsigned long long O6		:1	;
	unsigned long long O7		:1	;
	unsigned long long O8		:1	;
	unsigned long long O9		:1	;
	unsigned long long O10		:1	;
	unsigned long long O11		:1	;
	unsigned long long O12		:1	;
	unsigned long long O13		:1	;
	unsigned long long O14		:1	;
	unsigned long long R		:1	;
	unsigned long long A1		:1	;
	unsigned long long Z1		:1	;
	unsigned long long Z2		:1	;
	unsigned long long A2		:1	;
	unsigned long long S		:1	;
	unsigned long long Min		:7	;	//7 Bits für die Minuten
	unsigned long long P1		:1	;	//Parity Minuten
	unsigned long long Hour		:6	;	//6 Bits für die Stunden
	unsigned long long P2		:1	;	//Parity Stunden
	unsigned long long Day		:6	;	//6 Bits für den Tag
	unsigned long long Weekday	:3	;	//3 Bits für den Wochentag 
	unsigned long long Month	:5	;	//3 Bits für den Monat
	unsigned long long Year		:8	;	//8 Bits für das Jahr **eine 5 für das Jahr 2005**
	unsigned long long P3		:1	;	//Parity von P2
};
	
struct {
	char parity_err				:1	;	//Hilfs Parity
	char parity_P1				:1	;	//Berechnetes Parity P1
	char parity_P2				:1	;	//Berechnetes Parity P2
	char parity_P3				:1	;	//Berechnetes Parity P3
} flags;
	

