#pragma once
#include <avr/interrupt.h>
#include "tools.h"
#include "settings.h"
#include "dcf77_new.h"

//auf Anzeigen darzustellende Zeit

struct time {
	uint8_t ss;
	uint8_t mm;
	uint8_t hh;
	uint8_t day;
	uint8_t mon;
	uint8_t year;
};

extern volatile unsigned char valid_time;
extern volatile unsigned char valid_secs;
extern volatile unsigned char bad_signal;

extern volatile char mode;
extern volatile char menu;

inline void show_display();
inline void show_led();
inline void init_display();
inline char get_segment_status(char digit, char segment);

inline void display_mode0();
inline void display_mode1();
inline void display_mode2();

inline void display_menu();
