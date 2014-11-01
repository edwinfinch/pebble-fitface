#include <pebble.h>
#include "extras.h"
#include "notification.h"

NotificationLayer notif_layer;
	
void root_proc(Layer *l, GContext *ctx){
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_rect(ctx, GRect(0, 0, 144, 168), 1, GCornersAll);
}

void notif_layer_init(Window *w){
	notif_layer.root_layer = layer_create(GRect(0, 168, 144, 168));
	layer_set_update_proc(notif_layer.root_layer, root_proc);
	layer_add_child(window_get_root_layer(w), notif_layer.root_layer);
	layer_mark_dirty(notif_layer.root_layer);
	
	notif_layer.title = text_layer_create(GRect(4, 0, 136, 168));
	text_layer_set_font(notif_layer.title, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_background_color(notif_layer.title, GColorClear);
	layer_add_child(notif_layer.root_layer, text_layer_get_layer(notif_layer.title));
	
	notif_layer.content = text_layer_create(GRect(4, 30, 136, 168));
	text_layer_set_font(notif_layer.content, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_background_color(notif_layer.content, GColorClear);
	layer_add_child(notif_layer.root_layer, text_layer_get_layer(notif_layer.content));
}

void notif_layer_deinit(){
	layer_destroy(notif_layer.root_layer);
	text_layer_destroy(notif_layer.title);
	text_layer_destroy(notif_layer.content);
}

void notif_layer_push_notif(const char *title, const char *content, int vibrate, int length){
	switch(vibrate){
		case 1:
			vibes_short_pulse();
			break;
		case 2:
			vibes_double_pulse();
			break;
		case 3:
			vibes_long_pulse();
			break;
	}
	
	text_layer_set_text(notif_layer.title, title);
	text_layer_set_text(notif_layer.content, content);
	
	animate_layer(notif_layer.root_layer, &GRect(0, 170, 144, 168), &GRect(0, 0, 144, 168), 600, 0);
	animate_layer(notif_layer.root_layer, &GRect(0, 0, 144, 168), &GRect(0, 170, 144, 168), 600, length);
}