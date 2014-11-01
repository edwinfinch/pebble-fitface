#pragma once
#include <pebble.h>
	
typedef struct NotificationLayer {
	Layer *root_layer;
	TextLayer *title;
	TextLayer *content;
}NotificationLayer;
	
void notif_layer_init(Window *w);
void notif_layer_deinit();
void notif_layer_push_notif(const char *title, const char *content, int vibrate, int length);