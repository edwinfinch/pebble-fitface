#pragma once
#define BAR_WIDTH 28
	
Window *window;
TextLayer *date_layer, *time_layer, *day_layer, *day_layer_num;
TextLayer *bar_0, *bar_1, *bar_2, *bar_3;
GBitmap *shoe_i, *distance_i, *cals_i, *time_i, *cals_left_i, *cals_eaten, *bt_icon, *battery_icon;
BitmapLayer *iconlayer_1, *iconlayer_2, *iconlayer_3, *iconlayer_4, *bt_icon_layer;
Layer *dividing_layer, *battery_layer, *loading_layer;
InverterLayer *bar_0_inv, *bar_1_inv, *bar_2_inv, *bar_3_inv, *theme;
bool alt_data_showing = true;
AppTimer *refresh_timer, *loading_timer, *new_data_timer, *settings_refresh;
int charge_percent = 0;

typedef struct FitbitData {
	int current_values[6];
	int distanceMI[2];
	int goals[6];
	char battery[0][12];
	bool imperial;
}FitbitData;

typedef struct Settings {
	bool btdisalert;
	bool btrealert;
	bool date;
	bool batterybar;
	bool theme;
	bool debug;
	uint8_t override;
}Settings;

const int bar_heights[6] = {
	0, 29, 112, 141, 0, 141
};

const char *bt_bools[2] = {
	"BT Disconnected", "BT Connected"
};

FitbitData main_data;
Settings settings;

static char buffer[6][12] = {
	"1337.......", "1337.......", "1337.......", "1337.......", "1337.......", "1337.......", 
};