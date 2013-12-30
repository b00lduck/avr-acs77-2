#include "dcf77_new.h"

// Uhrzeit/Datum DCF77
volatile struct time dcf77;
volatile struct time utc;

volatile uint8_t dcf77_mesz;

volatile uint32_t y2k_timestamp = 0;  // Sekunden seit 1.1.2000 GMT

volatile uint8_t rx_bit_counter = 0;  //Bitzähler für RX Bit
unsigned long long dcf_rx_buffer = 0; //64 Bit für DCF77 benötigt werden 59 Bits

//Hilfs Sekunden Counter
unsigned int h_dcf77_ss = 0;
uint8_t	dcf_data_ready = 0;

/**********************************************************/
ISR(INT0_vect) {				
/**********************************************************/
	if (MCUCR == INT0_RISING_EDGE) {

		SBI(PORTA,3);

		//Sekunden Hilfs Counter berechnen
		h_dcf77_ss += TCNT1 - (65535 - (SYSCLK / 1024));
		
		TCNT1 = 65535 - (SYSCLK / 1024); //Zurücksetzen des Timers

		if (h_dcf77_ss > (SYSCLK / 1024 / 100 * 75)) { //ist eine Sekunde verstrichen?			
			add_one_sec(); 		//Addiere +1 zu Sekunden			
			h_dcf77_ss = 0;		//Zurücksetzen des Hilfs Counters
		};

		if (dcf_data_ready) do_dcf77();

		MCUCR = INT0_FALLING_EDGE;

	} else {

		CBI(PORTA,3);

		//Auslesen der Pulsweite von ansteigender Flanke zu abfallender Flanke
		uint16_t pulse_wide = TCNT1;
		//Zurücksetzen des Timers
		TCNT1 = 65535 - (SYSCLK / 1024);
		//Sekunden Hilfs Counter berechnen
		h_dcf77_ss = h_dcf77_ss + pulse_wide - (65535 - (SYSCLK / 1024));
		//Parity speichern
		//beginn von Bereich P1/P2/P3
		if (rx_bit_counter == 21 || rx_bit_counter ==  29 || rx_bit_counter ==  36) {
			flags.parity_err = 0;
		}
		if (rx_bit_counter == 28) {flags.parity_P1 = flags.parity_err;};
		if (rx_bit_counter == 35) {flags.parity_P2 = flags.parity_err;};
		if (rx_bit_counter == 58) {flags.parity_P3 = flags.parity_err;};

		//Überprüfen ob eine 0 oder eine 1 empfangen wurde
		//Abfrage größer als 150ms		
		if (pulse_wide > (65535 - (SYSCLK / 1024)/100*85)) {
			//Schreiben einer 1 im dcf_rx_buffer an der Bitstelle rx_bit_counter
			dcf_rx_buffer = dcf_rx_buffer | ((unsigned long long) 1 << rx_bit_counter);			
			flags.parity_err = flags.parity_err ^ 1; //Toggel Hilfs Parity
		}
		
		MCUCR = INT0_RISING_EDGE;	//Nächster Interrupt wird ausgelöst bei ansteigender Flanke
		
		rx_bit_counter++;	//RX Bit Counter wird um 1 incrementiert

	}
}								

/**********************************************************/
ISR (TIMER1_OVF_vect) {	 // Wird ausgelöst bei 59. sekunde			
/**********************************************************/

	valid_secs = 1;

	struct DCF77_Bits *rx_buffer = (struct DCF77_Bits *)(unsigned long long)&dcf_rx_buffer;
		
	//Zurücksetzen des Timers
	TCNT1 = 65535 - (SYSCLK / 1024);
	//wurden alle 59 Bits empfangen und sind die Paritys richtig?
	if (rx_bit_counter == 59 &&
	    flags.parity_P1 == rx_buffer->P1 &&
		flags.parity_P2 == rx_buffer->P2 &&
		flags.parity_P3 == rx_buffer->P3) { 
	
		//Alle 59Bits empfangen stellen der Uhr nach DCF77 Buffer		
		dcf_data_ready = 1;	
		add_one_sec(); 		//Addiere +1 zu Sekunden	

	} else {
		//nicht alle 59Bits empfangen bzw kein DCF77 Signal Uhr läuft auf quartz weiter
		bad_signal = 1;
		add_one_sec();
		dcf_rx_buffer = 0;	//Löschen des Rx Buffers
		dcf_data_ready = 0;	
	}

	rx_bit_counter = 0;	//zurücksetzen des RX Bit Counters	


}


/**********************************************************/
void init_dcf() {
/**********************************************************/

	//externer Interrupt
	MCUCR = INT0_RISING_EDGE;		
	GICR |= (1<<INT0);

	// Timer 1
	TIMSK |= (1<<TOIE1);				// Interrupt Overflow Enable
	TCCR1B |= (1<<CS10 | 1<<CS12); 		// Prescaler 1024
	TCNT1 = 65535 - (SYSCLK / 1024);		// Timer Preset

	sei();

}

/**********************************************************/
void welcome_gong() {
/**********************************************************/
	SBI(PORTA,2);
	delayloop16(3000);
	CBI(PORTA,2);
}

/**********************************************************/
void add_one_sec(void) {
/**********************************************************/
	dcf77.ss++;		//Addiere +1 zu Sekunden
	if (dcf77.ss == 60) {
		dcf77.ss = 0;
		dcf77.mm++;		//Addiere +1 zu Minuten
		if (dcf77.mm == 60) {
			dcf77.mm = 0;
			dcf77.hh++;		//Addiere +1 zu Stunden
			if (dcf77.hh == 24) {
				dcf77.hh = 0;
			}
		}
	}
	calculate_y2k_timestamp();
	calculate_utc();
}
			                  //	0,  1   2   3    4    5    6    7    8    9   10   11   12
uint16_t vergangene_tage[] =     {  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 }; 
uint16_t vergangene_tage_sj[] =  {  0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }; 

/*****************************************************/
void calculate_y2k_timestamp() {
/*****************************************************/
	// Sekunden seit Jahr 2000
	
	y2k_timestamp = dcf77.ss;

	uint16_t tage_seit_jahresanfang;

	if ((dcf77.year % 4) == 0) {
		tage_seit_jahresanfang = dcf77.day + vergangene_tage_sj[dcf77.mon-1];
	} else {
		tage_seit_jahresanfang = dcf77.day + vergangene_tage[dcf77.mon-1];
	}

	uint16_t tage_seit_y2k = (dcf77.year * 365) + ((dcf77.year-1) / 4) + tage_seit_jahresanfang;

	y2k_timestamp += (uint32_t)tage_seit_y2k * 86400;
	y2k_timestamp += (uint32_t)dcf77.hh * 3600;
	y2k_timestamp += (uint32_t)dcf77.mm * 60;	

	if (dcf77_mesz)	y2k_timestamp -= 7200;
	else y2k_timestamp -= 3600;
	
}


/*****************************************************/
void calculate_utc() {
/*****************************************************/

	uint32_t temp = y2k_timestamp;

	// JAHR/SCHALTJAHR
	char schaltjahr = 1;
	utc.year = 0;
	while (temp > ((365+schaltjahr) * 86400)) {		
		temp -= ((365+schaltjahr) * 86400);
		utc.year++;
		schaltjahr = (utc.year % 4) == 0;		
	}

	// MONAT
	uint16_t yeardays = (temp / 86400) + 1;
	uint16_t i = 0;
	if (schaltjahr) {
		while (yeardays > vergangene_tage_sj[i]) i++;
		utc.day = yeardays - vergangene_tage_sj[i-1];	
	} else {
		while (yeardays > vergangene_tage[i]) i++;
		utc.day = yeardays - vergangene_tage[i-1];
	}
	utc.mon = i;

	// Stunden, minuten und sekunden
	utc.ss = temp % 60;
	utc.mm = (temp % 3600) / 60;
	utc.hh = (temp % 86400) / 3600;

}



/**********************************************************/
void check_gong() {
/**********************************************************/

	struct time* gongtime;
	
	switch (settings[SETTING_DISPLAY_MODE]) {
	
		case DM_UTC:
			gongtime = &utc;
			break;
		case DM_DCF:
			gongtime = &dcf77;
			break;	
		case DM_EPOCH:	
		default:
			gongtime = 0;
	}

	if (gongtime) {
		if (gongtime->mm == 0) {
			char gong = gongtime->hh;
			if (gong > 12) gong -= 12;
			if (gong == 0) gong = 12;
			if (((gongtime->ss % settings[SETTING_GONG_INTERVAL]) == 0) && (gongtime->ss < (gong*settings[SETTING_GONG_INTERVAL]))) {
				SBI(PORTA,2);
			} else {
				CBI(PORTA,2);
			}
		} else {
		  CBI(PORTA,2);		
		}
	} else {
	  CBI(PORTA,2);		
	}
}

/***************************************************/
void do_dcf77() {
/***************************************************/
	dcf_data_ready = 0;

	struct DCF77_Bits *rx_buffer = (struct DCF77_Bits *)(unsigned long long)&dcf_rx_buffer;

	dcf77.mm = rx_buffer->Min-((rx_buffer->Min/16)*6);
	dcf77.hh = rx_buffer->Hour-((rx_buffer->Hour/16)*6);
	dcf77.day = rx_buffer->Day-((rx_buffer->Day/16)*6); 
	dcf77.mon = rx_buffer->Month-((rx_buffer->Month/16)*6);
	dcf77.year =  rx_buffer->Year-((rx_buffer->Year/16)*6);
	dcf77.ss = 0;	
	
	// MESZ / MES
	if ((rx_buffer->Z1 == 1) && (rx_buffer->Z2 == 0)) dcf77_mesz = 1;
	else dcf77_mesz = 0;

	dcf_rx_buffer = 0;	//Löschen des Rx Buffers


	valid_time = 1;
	bad_signal = 0;
	calculate_y2k_timestamp();
	calculate_utc();

}
