#pragma once
#include <avr/eeprom.h>

// Settings file

#define SETTING_DISPLAY_MODE 0
#define SETTING_BRIGHTNESS 1
#define SETTING_GONG_MODE  2
#define SETTING_QUIET_START  3
#define SETTING_QUIET_END  4

#define NUM_SETTINGS 5

#define DM_DCF	 0
#define DM_UTC	 1
#define DM_EPOCH 2
#define DM_TEST  3

#define GM_OFF	 0
#define GM_OLD	 1
#define GM_BEN	 2
#define GM_CUCK  3

#define BRIGHTNESS_MAX 20
#define BRIGHTNESS_MIN 1000
#define BRIGHTNESS_STEPS 12
#define BRIGHTNESS_STEP ((BRIGHTNESS_MAX - BRIGHTNESS_MIN) / BRIGHTNESS_STEPS)
#define BRIGHTNESS_DEFAULT 10

#define GONG_OLD_INTERVAL 2
#define GONG_BEN_INTERVAL 3
#define GONG_BEN_CHIMELEN 12
#define GONG_CUCK_INTERVAL 1




extern uint8_t settings[NUM_SETTINGS];

void load_settings();
void change_setting(char setting);
