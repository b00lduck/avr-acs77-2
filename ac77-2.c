#include <avr/io.h>
#include "display_driver.h"
#include "dcf77_new.h"
#include "settings.h"

void init_avr();

char buttona_pushed = 0;
char buttonb_pushed = 0;

/***************************************************************************/
int main() {
/***************************************************************************/

	init_avr();	
			
	init_display();
	
	settings[SETTING_DISPLAY_MODE] = DM_TEST;
	settings[SETTING_BRIGHTNESS] = 10;
	
	welcome_gong();	
	
	load_settings();	
	init_dcf();	
		
	while (1) {

		if (((PINA & 0b00000010) == 0)) {
			if (buttonb_pushed == 0) {
				buttonb_pushed = 1;
				mode++;
				mode %= (NUM_SETTINGS+1);
				delayloop16(10000);		
			}
		} else {
			buttonb_pushed = 0;
			delayloop16(1000);		
		}

		if (((PINA & 0b00000001) == 0)) {
			if (buttona_pushed == 0) {
				buttona_pushed = 1;
				change_setting(mode-1);
				delayloop16(10000);		
			}
		} else {
			buttona_pushed = 0;
			delayloop16(1000);		
		}


		if (valid_time) {
			check_gong();
		} else {
			gong_off();		
			aux1_off();
			aux2_off();
			aux3_off();
			aux4_off();
		}

	}
}

/***************************************************************************/
void init_avr() {
/***************************************************************************/

	PORTA = 0b10000011;
	PORTB = 0b00000000;
	PORTC = 0b00000000;
	PORTD = 0b11111011;

	DDRA = 0b11111100;	// Seg2, Aux1, Aux2, Aux3, LED, Aux4, ButtonA, ButtonB
	DDRB = 0b00011111;	// ICSP reserve, Gong, CA11, CA10, CA9, CA8
	DDRC = 0b11111111;	// CA7-0
	DDRD = 0b11111011;	// Seg7,6,5,4,3, INT0, Seg1,0

}



