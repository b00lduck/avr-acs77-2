#include "settings.h"

uint8_t eePad[] EEMEM = { 0,0,0,0,0,0,0,0,0,0 };
uint8_t eeSettings[] EEMEM = { DM_DCF, 10, GM_CUCK, 0, 6};

uint8_t settings[NUM_SETTINGS];

/************************************************************************/
void load_settings() {
/************************************************************************/
	eeprom_read_block(settings,eeSettings,NUM_SETTINGS);
}


/************************************************************************/
void change_setting(char setting) {
/************************************************************************/

	settings[setting]++;

	switch (setting) {

		case SETTING_BRIGHTNESS:			
			if (settings[setting] >= (BRIGHTNESS_STEPS-1)) settings[setting] = 1;
			break;

		case SETTING_GONG_MODE:
			if (settings[setting] >= 4) settings[setting] = 0;
			break;

		case SETTING_DISPLAY_MODE:
			if (settings[setting] >= 3) settings[setting] = 0;
			break;
			
		case SETTING_QUIET_START:
			if (settings[setting] >= 24) settings[setting] = 0;
			break;
		
		case SETTING_QUIET_END:
			if (settings[setting] >= 24) settings[setting] = 0;
			break;					

	}

	eeprom_write_block(settings,eeSettings,NUM_SETTINGS);

}
