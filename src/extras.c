#include <pebble.h>
#include "extras.h"

TextLayer* text_layer_init(GRect location, bool is_date){
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, GColorWhite);
	text_layer_set_background_color(layer, GColorClear);
	text_layer_set_text_alignment(layer, GTextAlignmentCenter);
	text_layer_set_font(layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_IMPACT_40)));
	if(is_date){
		text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
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