#include "settings.h"

uint8_t eeSettings[] EEMEM = { 0,1,2,1,8 };

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

		case SETTING_WELCOMEGONG:
			if (settings[setting] >= 2) settings[setting] = 0;
			break;

		case SETTING_GONG_ENABLE:
			if (settings[setting] >= 2) settings[setting] = 0;
			break;


		case SETTING_GONG_INTERVAL:
			if (settings[setting] >= 5) settings[setting] = 2;
			break;

		case SETTING_DISPLAY_MODE:
			if (settings[setting] >= 3) settings[setting] = 0;
			break;

	}

	eeprom_write_block(settings,eeSettings,NUM_SETTINGS);

}
