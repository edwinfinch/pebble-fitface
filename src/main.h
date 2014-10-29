#pragma once
#define BAR_WIDTH 28
	
Window *window;
TextLayer *date_layer, *time_layer;
TextLayer *calories_v, *active_v, *steps_v, *distance_v;
GBitmap *shoe_i, *distance_i, *cals_i, *time_i, *cals_left_i, *cals_eaten, *bt_icon, *battery_icon;
BitmapLayer *iconlayer_1, *iconlayer_2, *iconlayer_3, *iconlayer_4, *bt_icon_layer;
Layer *dividing_layer, *battery_layer, *loading_layer;
InverterLayer *calories_b, *active_b, *steps_b, *distance_b;
bool alt_data_showing = true;
AppTimer *refresh_timer, *loading_timer;
int charge_percent = 0;

typedef struct FitbitData {
	int current_values[6];
	int distanceMI[2];
	int goals[6];
	char battery[0][12];
	bool imperial;
}FitbitData;

const int bar_heights[6] = {
	0, 29, 112, 141, 0, 141
};

FitbitData main_data;

static char buffer[6][12] = {
	"1337.......", "1337.......", "1337.......", "1337.......", "1337.......", "1337.......", 
};

static TextLayer* text_layer_init(GRect location, bool is_date)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, GColorWhite);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_text_alignment(layer, GTextAlignmentCenter);
	text_layer_set_font(layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_32)));
	if(is_date){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	}
	return layer;
}

void stopped(Animation *anim, bool finished, void *context){
    property_animation_destroy((PropertyAnimation*) anim);
}
 
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay){
    PropertyAnimation *anim = property_animation_create_layer_frame(layer, start, finish);
     
    animation_set_duration((Animation*) anim, duration);
    animation_set_delay((Animation*) anim, delay);
     
    AnimationHandlers handlers = {
        .stopped = (AnimationStoppedHandler) stopped
    };
    animation_set_handlers((Animation*) anim, handlers, NULL);
     
    animation_schedule((Animation*) anim);
}