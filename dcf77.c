#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "display_driver.h"
#include "dcf77.h"
#include "settings.h"

volatile unsigned char timecount = 0;
volatile unsigned short breakcount = 0;
volatile unsigned char dcf_active = 0;
volatile unsigned char array_count = 0;
volatile unsigned char dcf_array [60];
volatile unsigned char dcf_min_old = 0;
volatile unsigned char dcf_std_old = 0;

unsigned short sekunde = 0;
unsigned char minute = 0;
unsigned char stunde = 0;
unsigned char tag = 0;
unsigned char monat = 0;
unsigned char jahr = 0;

/***************************************************************************************/
unsigned char plausibilitaetscheck ( unsigned char std_new, unsigned char std_old,
									 unsigned char min_new, unsigned char min_old) {
/***************************************************************************************/
	
	// Hochzählen der zuletzt empfangenen Zeit um eine Minute
	// Vergleich der hochgezählten Zeit mit der neu empfangenen
	// wenn sie gleich sind, kann Zeit übernommen werden
	// sonst wird alte Zeit verworfen und durch neue ersetzt,
	// allerdings wird sie nicht auf die Anzeigen übernommen

	//	unsigned char min_old_x = min_old + 1;	
	//	unsigned char std_old_x = std_old;		
													
	return 1;

}

/**********************************************************/
void dcf77_exec () {
/**********************************************************/

	minute = 0;
	stunde = 0;
	tag = 0;
	monat = 0;
	jahr = 0;
	unsigned char parity = 0;

	//Tage
	if (dcf_array [36] == 1) { tag = 1;    parity++; }
	if (dcf_array [37] == 1) { tag += 2;   parity++; }
	if (dcf_array [38] == 1) { tag += 4;   parity++; }
	if (dcf_array [39] == 1) { tag += 8;   parity++; }
	if (dcf_array [40] == 1) { tag += 10;  parity++; }
	if (dcf_array [41] == 1) { tag += 20;  parity++; }
	
	//Monate
	if (dcf_array [45] == 1) { monat = 1;   parity++; }
	if (dcf_array [46] == 1) { monat += 2;	parity++; }
	if (dcf_array [47] == 1) { monat += 4;  parity++; }
	if (dcf_array [48] == 1) { monat += 8;  parity++; }
	if (dcf_array [49] == 1) { monat += 10; parity++; }

	//Jahr
	if (dcf_array [50] == 1) { jahr = 1;    parity++; }
	if (dcf_array [51] == 1) { jahr += 2;	parity++; }
	if (dcf_array [52] == 1) { jahr += 4;   parity++; }
	if (dcf_array [53] == 1) { jahr += 8;   parity++; }
	if (dcf_array [54] == 1) { jahr += 10;  parity++; }
	if (dcf_array [55] == 1) { jahr += 20;  parity++; }
	if (dcf_array [56] == 1) { jahr += 40;  parity++; }
	if (dcf_array [57] == 1) { jahr += 80;  parity++; }

	if (dcf_array [58] != (parity % 2)) return;
	parity = 0;

	//Stunden
	if (dcf_array [29] == 1) { stunde = 1;    parity++; }
	if (dcf_array [30] == 1) { stunde += 2;	  parity++;	}
	if (dcf_array [31] == 1) { stunde += 4;   parity++; }
	if (dcf_array [32] == 1) { stunde += 8;   parity++;	}
	if (dcf_array [33] == 1) { stunde += 10;  parity++;	}
	if (dcf_array [34] == 1) { stunde += 20;  parity++;	}
	
	if (dcf_array [35] != (parity % 2)) return;
	parity = 0;
	
	//Minuten
	if (dcf_array [21] == 1) { minute = 1;    parity++; }
	if (dcf_array [22] == 1) { minute += 2;   parity++;	}
	if (dcf_array [23] == 1) { minute += 4;   parity++;	}
	if (dcf_array [24] == 1) { minute += 8;   parity++;	}
	if (dcf_array [25] == 1) { minute += 10;  parity++;	}
	if (dcf_array [26] == 1) { minute += 20;  parity++;	}
	if (dcf_array [27] == 1) { minute += 40;  parity++;	}

	if (dcf_array [28] != (parity % 2)) return;

	if ( plausibilitaetscheck (	stunde, dcf_std_old, minute, dcf_min_old ) ) {

		display_stunde = stunde;
		display_minute = minute;
		display_sekunde = sekunde;
		display_tag = tag;		
		display_monat = monat;
		display_jahr = jahr;
		valid_time = 1;
	
	}
	
//	dcf_min_old = minute;		//sichern der aktuellen Zeitdaten
//	dcf_std_old = stunde;

}

/**********************************************************/
ISR(INT0_vect) {				//Beginn eines Zeitsignals
/**********************************************************/
	TCNT0 = 0;					//Rücksetzen des "Auszähltimers"
	dcf_active = 1;
	GICR &= ~(1<<INT0);			//deaktivieren des externen Interrupts, wenn Signal komplett, wird er wieder aktiviert
}								

/**********************************************************/
ISR(TIMER0_OVF_vect) {
/**********************************************************/
	
	display_sekunde = sekunde;
	display_last_break_length = breakcount / 10;

	if (dcf_active) {
	
		if ((breakcount > 400) && (breakcount < 500)) { //Signalpause (58. Bit) ?
			if (array_count >= 58) dcf77_exec ();
			sekunde = 0;
			valid_secs = 1;
			TCNT1 = TIMER_PRESET;
			breakcount = 0;
			array_count = 0;
			GICR |= (1<<INT0);		//aktivieren des ext. Int.
			return;
		}	
	
		breakcount = 0;

		display_last_impulse_length = timecount;
		display_array_count = array_count;

		if (PIND & (1<<PD2)) {								//noch H-Pegel
			timecount++;									//auszählen des H-Pegels
		} else {											//jetzt L-Pegel			
			dcf_array [array_count++] = (timecount > 35);			
			dcf_active = 0;
			timecount = 0;
			GICR |= (1<<INT0);
		}
	}
	else breakcount++;				//gerade kein Signal, also auszählen der Pause

	check_gong();
}

/**********************************************************/
ISR (TIMER1_OVF_vect) {				
/**********************************************************/
	sekunde++;	
	check_time_flip();						
	TCNT1 = TIMER_PRESET; 		
}

/**********************************************************/
void check_time_flip() {
/**********************************************************/
	if (sekunde == 60) {
		sekunde = 0;
		minute++;
		display_sekunde = sekunde;
		if (minute == 60) {
			minute == 0;
			stunde++;
			display_minute = minute;
			if (stunde == 24) {
				stunde = 0;
				display_stunde = stunde;
			}
		}
	}
}

/**********************************************************/
void check_gong() {
/**********************************************************/

	return;
	// Untrigger Gong
	PORTA &= ~(1<<2);
	if (display_minute == 0) {

		char gongs = display_stunde;
		if (gongs > 12) gongs -= 12;

		if ((display_sekunde) < (gongs * GONG_INTERVAL)) {
			if (((display_sekunde) % GONG_INTERVAL) == 0) {
				// Trigger Gong
			//	PORTA |= (1<<2);
			}
		}
	}
}
