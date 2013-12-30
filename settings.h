#pragma once
#include <avr/eeprom.h>

// Settings file

#define SETTING_DISPLAY_MODE 0
#define SETTING_GONG_ENABLE  1
#define SETTING_GONG_INTERVAL 2
#define SETTING_WELCOMEGONG  3
#define SETTING_BRIGHTNESS 4

#define NUM_SETTINGS 5

#define DM_DCF	 0
#define DM_UTC	 1
#define DM_EPOCH 2

#define BRIGHTNESS_MAX 100
#define BRIGHTNESS_MIN 1500
#define BRIGHTNESS_STEPS 16
#define BRIGHTNESS_STEP ((BRIGHTNESS_MAX - BRIGHTNESS_MIN) / BRIGHTNESS_STEPS)
#define BRIGHTNESS_DEFAULT 10

extern uint8_t settings[NUM_SETTINGS];

void load_settings();
void change_setting(char setting);
