#include "display_driver.h"

unsigned char display[12];
unsigned char display_dp[12];

volatile unsigned char valid_time = 0;	
volatile unsigned char valid_secs = 0;
volatile unsigned char bad_signal = 0;	

volatile char mode = 0;
volatile char menu = 0;

//					  gfedcba
char segcode[] = { 0b00111111,	// 0
				   0b00000110,  // 1
				   0b01011011,  // 2
				   0b01001111,  // 3
				   0b01100110,  // 4

				   0b01101101,  // 5
				   0b01111101,  // 6
				   0b00000111,  // 7
				   0b01111111,  // 8
				   0b01101111,  // 9

				   0b01110111,	// A 10
				   0b01111100,	// B 11
				   0b00111001,  // C 12
				   0b01011110,  // D 13
				   0b01111001,  // E 14
				   0b01110001,  // F 15
				   
				   0b01111101,  // G 16
				   0b01110100,  // H 17
				   0b00000100,  // I 18
				   0b00011110,  // J 19				   
				   0,  			//   20
				   0b00111000,  // L 21
				   0b00000000,  //   22
				   0b01010100,  // N 23
				   0b01011100,  // O 24,
				   0b01110011,  // P 25,
				   0,			
				   0b01010000,  // r 27
				   0,
				   0b01111000,	// T 29
				   0b00111110,  // U 30
				   0,
				   0,
				   0,
				   0,
				   0,
				   0b01000000, // - 36
				   0b00000000, //   37
				   0b01100011, // ° 38
				   0b00110111, // "N" 39
				   0b00110000, // "I" 40
};


uint16_t scaler = 0;

/**********************************************************/
ISR(TIMER0_OVF_vect) {
/**********************************************************/
	
	if (scaler == 0) {
		show_display();
		scaler = 0 - (BRIGHTNESS_MIN + (BRIGHTNESS_STEP * settings[SETTING_BRIGHTNESS]));
	}
	scaler++;
}


/***************************************************************************/
inline char get_segment_status(char digit, char segment) {
/***************************************************************************/
	char retvala = (segcode[display[digit]] & (1 << segment)) > 0;
	char retvalb = display_dp[digit] > 0;
	if (segment == 7) {
		return retvalb;
	} else {
		return retvala;
	}
}




/***************************************************************************/
inline void show_led() {
/***************************************************************************/
	// DCF-77 LED
	if (PIND & (1<<PD2)) SBI(PORTA,3);
	else CBI(PORTA,3);
}

/***************************************************************************/
inline void show_display() {
/***************************************************************************/

	char seg = 0;

	if (mode == 0) {

		switch (settings[SETTING_DISPLAY_MODE]) {
			case DM_DCF: 	display_mode0(); break;
			case DM_UTC: 	display_mode1(); break;
			case DM_EPOCH: 	display_mode2(); break;
		}

	} else {

		display_menu();

	}

	for (seg=0; seg<8; seg++) {

		// Determine which digits are burning for this segment

		char tempc = 0;
		char tempb = 0;

		char c;		
		for (c=0;c<8;c++) tempc |= get_segment_status(c,seg) << c;
		for (c=8;c<12;c++) tempb |= get_segment_status(c,seg) << (c-8);

		// Alles off (all segments = high)
		PORTA |= (1<<7); 
		PORTD = 0xff;
		PORTC = tempc;
		PORTB = tempb;

		// Segments (LOW=active)
		if (seg == 2) PORTA &= ~(1<<7);
		else PORTD &= ~(1 << seg);

	}

	delayloop16(200);

	PORTA |= (1<<7); 
	PORTD = 0xff;

}

#define FIRST_DIG(val) (val / 10)
#define SECOND_DIG(val) (val - (val / 10) * 10)

/***************************************************************************/
inline void display_mode0() {
/***************************************************************************/
	// Original ACS-77

	memset(display_dp,0,12);

	if (valid_time) {
		display[0] = FIRST_DIG(dcf77.hh);
		display[1] = SECOND_DIG(dcf77.hh);	
		display[2] = FIRST_DIG(dcf77.mm);
		display[3] = SECOND_DIG(dcf77.mm);
		display[4] = FIRST_DIG(dcf77.ss);
		display[5] = SECOND_DIG(dcf77.ss);
		display[6] = FIRST_DIG(dcf77.day);
		display[7] = SECOND_DIG(dcf77.day);
		display[8] = FIRST_DIG(dcf77.mon);
		display[9] = SECOND_DIG(dcf77.mon);
		display[10] = FIRST_DIG(dcf77.year);
		display[11] = SECOND_DIG(dcf77.year);
		display_dp[7] = 1;
		display_dp[9] = 1;
		if (bad_signal) {
			display_dp[6] = 1;
			display_dp[8] = 1;
			display_dp[10] = 1;
			display_dp[11] = 1;
		}
	} else {
		// ACS-77
		if (!valid_secs) {
			display[0] = 10;
			display[1] = 12;	
			display[2] = 5;
			display[3] = 36;
			display[4] = 7;
			display[5] = 7;
		} else {
			display[0] = 37;
			display[1] = 37;	
			display[2] = 37;
			display[3] = 37;
			display[4] = FIRST_DIG(rx_bit_counter);
			display[5] = SECOND_DIG(rx_bit_counter);
		}

		// NI2006
		display[6] = 39;
		display[7] = 40;
		display[8] = 2;
		display[9] = 0;
		display[10] = 0;
		display[11] = 6;		
	}
}

//char months[] = { 31,28,31,30,31,30, 31,31,30,31,30,31 };


/***************************************************************************/
inline void display_mode1() {
/***************************************************************************/
	// UTC

	memset(display_dp,0,12);

	if (valid_time) {
		display[0] = FIRST_DIG(utc.hh);
		display[1] = SECOND_DIG(utc.hh);	
		display[2] = FIRST_DIG(utc.mm);
		display[3] = SECOND_DIG(utc.mm);
		display[4] = FIRST_DIG(utc.ss);
		display[5] = SECOND_DIG(utc.ss);
		display[6] = FIRST_DIG(utc.day);
		display[7] = SECOND_DIG(utc.day);
		display[8] = FIRST_DIG(utc.mon);
		display[9] = SECOND_DIG(utc.mon);
		display[10] = FIRST_DIG(utc.year);
		display[11] = SECOND_DIG(utc.year);
		display_dp[7] = 1;
		display_dp[9] = 1;
		if (bad_signal) {
			display_dp[6] = 1;
			display_dp[8] = 1;
			display_dp[10] = 1;
			display_dp[11] = 1;
		}
	} else {
		// ACS-77
		if (!valid_secs) {
			display[0] = 10;
			display[1] = 12;	
			display[2] = 5;
			display[3] = 36;
			display[4] = 7;
			display[5] = 7;
		} else {
			display[0] = 37;
			display[1] = 37;	
			display[2] = 37;
			display[3] = 37;
			display[4] = FIRST_DIG(rx_bit_counter);
			display[5] = SECOND_DIG(rx_bit_counter);
		}

		// NI2006
		display[6] = 39;
		display[7] = 40;
		display[8] = 2;
		display[9] = 0;
		display[10] = 0;
		display[11] = 6;		
	}

}


/***************************************************************************/
inline void display_mode2() {
/***************************************************************************/

	// EPOCH

	memset(display_dp,0,12);

	if (valid_time) {

		display[0] = 37;
		display[1] = 37;

		uint32_t temp1 = y2k_timestamp;
		
		temp1 += 946684800;

		uint32_t temp2 = 0;

		temp2 = temp1 / 1000000000;	display[2]  = temp2;temp1 -= temp2 * 1000000000;
		temp2 = temp1 / 100000000;	display[3]  = temp2;temp1 -= temp2 * 100000000;
		temp2 = temp1 / 10000000;	display[4]  = temp2;temp1 -= temp2 * 10000000;
		temp2 = temp1 / 1000000;	display[5]  = temp2;temp1 -= temp2 * 1000000;
		temp2 = temp1 / 100000;		display[6]  = temp2;temp1 -= temp2 * 100000;
		temp2 = temp1 / 10000;		display[7]  = temp2;temp1 -= temp2 * 10000;
		temp2 = temp1 / 1000;		display[8]  = temp2;temp1 -= temp2 * 1000;
		temp2 = temp1 / 100;		display[9]  = temp2;temp1 -= temp2 * 100;
		temp2 = temp1 / 10;			display[10] = temp2;temp1 -= temp2 * 10;
		display[11] = temp1;

	} else {
		// ACS-77
		if (!valid_secs) {
			display[0] = 10;
			display[1] = 12;	
			display[2] = 5;
			display[3] = 36;
			display[4] = 7;
			display[5] = 7;
		} else {
			display[0] = 37;
			display[1] = 37;	
			display[2] = 37;
			display[3] = 37;
			display[4] = FIRST_DIG(rx_bit_counter);
			display[5] = SECOND_DIG(rx_bit_counter);
		}

		// NI2006
		display[6] = 39;
		display[7] = 40;
		display[8] = 2;
		display[9] = 0;
		display[10] = 0;
		display[11] = 6;		
	}
}


/***************************************************************************/
inline void display_menu() {
/***************************************************************************/
	// Menu

	memset(display_dp,0,12);
	memset(display,37,12);


	switch(mode-1) {
		
		case SETTING_DISPLAY_MODE:	// SETTING_DISPLAY_MODE
			
			display[0] = 13;
			display[1] = 18;	
			display[2] = 5;
			display[3] = 25;


			switch(settings[SETTING_DISPLAY_MODE]) {
				
				case DM_DCF:
			
					display[9] = 13;
					display[10] = 12;
					display[11] = 15;		
					break;

				case DM_UTC:

					display[9] = 30;
					display[10] = 29;
					display[11] = 12;		
					break;

				case DM_EPOCH:

					display[6] = 37;
					display[7] = 14;
					display[8] = 25;
					display[9] = 24;
					display[10] = 12;
					display[11] = 17;		
					break;

			}
		break;

		case SETTING_GONG_INTERVAL:	// SETTING_GONG_INTERVAL
			
			display[0] = 16;
			display[1] = 24;	
			display[2] = 23;
			display[3] = 36;
			display[4] = 18;
			display[5] = 23;

			display[7] = settings[SETTING_GONG_INTERVAL];
			display[9] = 5;
			display[10] = 14;
			display[11] = 12;		
			break;
		
		case SETTING_GONG_ENABLE:	// SETTING_GONG_ENABLE
			
			display[0] = 16;
			display[1] = 24;
			display[2] = 23;
			display[3] = 16;	

			if ( settings[SETTING_GONG_ENABLE] ) {
				display[10] = 24;
				display[11] = 23;		
			} else {
				display[9] = 24;
				display[10] = 15;		
				display[11] = 15;		
			}
			break;
		
		case SETTING_WELCOMEGONG:	// SETTING_WELCOMEGONG
			
			display[0] = 24;
			display[1] = 23;
			display[2] = 16;
			display[3] = 24;	
			display[4] = 23;
			display[5] = 16;

			if ( settings[SETTING_WELCOMEGONG] ) {
				display[10] = 24;
				display[11] = 23;		
			} else {
				display[9] = 24;
				display[10] = 15;		
				display[11] = 15;		
			}
			break;

		case SETTING_BRIGHTNESS: // SETTING_BRIGHTNESS

			display[0] = 11;
			display[1] = 27;
			display[2] = 18;
			display[3] = 16;	
			display[4] = 17;
			display[5] = 29;
			
			display[10] = FIRST_DIG(settings[SETTING_BRIGHTNESS]);
			display[11] = SECOND_DIG(settings[SETTING_BRIGHTNESS]);

		default:

			break;

	}
			
}

/**********************************************************/
void init_display() {
/**********************************************************/
    //Timer init for display refresh
	TCCR0 = (1<<CS00) || (1<<CS02);			
	TIMSK |= (1<<TOIE0);
	sei();					
}
