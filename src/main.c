#include <pebble.h>
#include "main.h"
#include "notification.h"
#include "extras.h"

void loading_animation(bool start);
void get_data();
void shake_handler(AccelAxisType axis, int32_t direction);

#define GetRekt GRect
	
char* floatToString(double number){
	static char buffer[] = "Hello world!";
	char decimalBuffer[5];

	snprintf(buffer, 15, "%d", (int)number);
	strcat(buffer, ".");

	snprintf(decimalBuffer, 5, "%02d", (int)((double)(number - (int)number) * (double)100));
	strcat(buffer, decimalBuffer);

	return buffer;
}

void refresh_bar(int bar){
	double pre_data[2] = {
		0, 0
	};
	pre_data[0] = (double)main_data.current_values[bar];
	pre_data[1] = (double)main_data.goals[bar];

	bool distance = false;
	if(main_data.imperial && bar == 3){
		distance = true;
		pre_data[0] = (double)(main_data.distanceMI[0])/100;
		pre_data[1] = (double)(main_data.distanceMI[1])/100;
	}
	else if(!main_data.imperial && bar == 3){
		pre_data[0] = (double)(main_data.current_values[bar])/100;
		pre_data[1] = (double)(main_data.goals[bar])/100;
		distance = true;
	}
	
	float fix = ((pre_data[0]/pre_data[1])*144);
	GRect togo_rect = GRect(0, bar_heights[bar], fix, BAR_WIDTH);
	GRect current_rect;
	
	//APP_LOG(APP_LOG_LEVEL_INFO, "%d bar, %d casted predata[0], %d casted predata[1]", bar, (int)pre_data[0], (int)pre_data[1]);

	if(!distance){
		snprintf(buffer[bar], sizeof(buffer[bar]), "%d", (int)pre_data[0]);
	}
	else{
		if(main_data.imperial){
			static char buffer_[] = "Hello world!";
			strcpy(buffer_, floatToString(pre_data[0]));
			snprintf(buffer[bar], sizeof(buffer[bar]), "%s mi", buffer_);
		}
		else{
			static char buffer_[] = "Hello world!";
			strcpy(buffer_, floatToString(pre_data[0]));
			snprintf(buffer[bar], sizeof(buffer[bar]), "%s km", buffer_);
		}
	}
	
	switch(bar){
		case 0:
			current_rect = layer_get_frame(inverter_layer_get_layer(bar_0_inv));
			text_layer_set_text(bar_0, buffer[0]);
			animate_layer(inverter_layer_get_layer(bar_0_inv), &current_rect, &togo_rect, 1000, 0);
			break;
		case 1:
			current_rect = layer_get_frame(inverter_layer_get_layer(bar_1_inv));
			text_layer_set_text(bar_1, buffer[1]);
			animate_layer(inverter_layer_get_layer(bar_1_inv), &current_rect, &togo_rect, 1000, 0);
			break;
		case 2:
			current_rect = layer_get_frame(inverter_layer_get_layer(bar_2_inv));
			text_layer_set_text(bar_2, buffer[2]);
			animate_layer(inverter_layer_get_layer(bar_2_inv), &current_rect, &togo_rect, 1000, 0);
			break;
		case 3:
			current_rect = layer_get_frame(inverter_layer_get_layer(bar_3_inv));
			text_layer_set_text(bar_3, buffer[3]);
			animate_layer(inverter_layer_get_layer(bar_3_inv), &current_rect, &togo_rect, 1000, 0);
			break;
		case 4:
			current_rect = layer_get_frame(inverter_layer_get_layer(bar_0_inv));
			text_layer_set_text(bar_0, buffer[4]);
			animate_layer(inverter_layer_get_layer(bar_0_inv), &current_rect, &togo_rect, 1000, 0);
			break;
		case 5:
			current_rect = layer_get_frame(inverter_layer_get_layer(bar_3_inv));
			text_layer_set_text(bar_1, buffer[5]);
			animate_layer(inverter_layer_get_layer(bar_3_inv), &current_rect, &togo_rect, 1000, 0);
			break;
	}
}

void refresh_callback(){
	for(int i = 0; i < 4; i++){
		refresh_bar(i);
	}
	size_t free_m = heap_bytes_free();
	if(settings.debug){APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes free", free_m);}
}

void refresh_settings(){
	layer_set_hidden(battery_layer, settings.batterybar);
	layer_set_hidden(inverter_layer_get_layer(theme), settings.theme);
	layer_set_hidden(text_layer_get_layer(day_layer), settings.date);
	layer_set_hidden(text_layer_get_layer(day_layer_num), settings.date);
}

void battery_handler(BatteryChargeState state){
	charge_percent = state.charge_percent;
	layer_mark_dirty(battery_layer);
}

void bt_handler(bool connected){
	if(connected && settings.btrealert){
		notif_layer_push_notif("Bluetooth", "Bluetooth connection reestablished with phone.", 2, 5000);
	}
	else if(!connected && settings.btdisalert){
		notif_layer_push_notif("Bluetooth", "Bluetooth disconnected from phone.", 1, 4000);
	}
}

void loading_timer_callback(){
	loading_animation(true);
}

void loading_animation(bool start){
	if(start){
		animate_layer(loading_layer, &GRect(0, 0, 144, 168), &GRect(144, 0, 144, 168), 700, 0);
		animate_layer(loading_layer, &GRect(144, 0, 144, 168), &GRect(0, 0, 144, 168), 700, 710);
		loading_timer = app_timer_register(1500, loading_timer_callback, NULL);
		layer_set_hidden(loading_layer, false);
	}
	else{
		app_timer_cancel(loading_timer);
		layer_set_hidden(loading_layer, true);
	}
}

void process_tuple(Tuple *t){
	int key = t->key;
	int value = t->value->int32;
	if(settings.debug){APP_LOG(APP_LOG_LEVEL_INFO, "key: %d, data %d", key, value);}
	switch(key){
		case 0:
			main_data.current_values[1] = value;
			//strap_log_event("/settings_save");
			loading_animation(false);
			app_timer_cancel(new_data_timer);
			new_data_timer = app_timer_register(3000000, get_data, NULL);
			break;
		case 1:
			main_data.current_values[2] = value;
			break;
		case 2:
			main_data.current_values[3] = value;
			break;
		case 3:
			main_data.current_values[0] = value;
			break;
		case 4:
			main_data.current_values[4] = value;
			break;
		case 5:
			main_data.current_values[5] = value;
			break;
		case 6:
			main_data.goals[1] = value;
			break;
		case 7:
			main_data.goals[2] = value;
			break;
		case 8:
			main_data.goals[3] = value;
			break;
		case 9:
			main_data.goals[0] = value;
			break;
		case 10:
			main_data.goals[4] = value;
			break;
		case 11:
			main_data.goals[5] = value;
			break;
		case 12:
			strcpy(main_data.battery[0], t->value->cstring);
			static char buffer[] = "Battery medium......s";
			snprintf(buffer, sizeof(buffer), "/battery_%s", t->value->cstring);
			//strap_log_event(buffer);
			break;
		case 13:
			main_data.imperial = value;
			/*
			if(value){
				strap_log_event("/uses_imperial");
			}
			else{
				strap_log_event("/uses_metric");
			}
			*/
			break;
		case 14:
			main_data.distanceMI[1] = value;
			break;
		case 15:
			main_data.distanceMI[0] = value;
			break;
		case 16:
			loading_animation(true);
			break;
		case 17:
			settings.btdisalert = value;
			settings_refresh = app_timer_register(500, refresh_settings, NULL);
			break;
		case 18:
			settings.btrealert = value;
			break;
		case 19:
			settings.batterybar = value;
			break;
		case 20:
			settings.date = value;
			break;
		case 21:
			settings.theme = value;
			break;
		case 22:
			settings.debug = value;
			break;
		case 23:
			app_timer_cancel(refresh_timer);
			switch(value){
				case 1:
					notif_layer_push_notif("FitFace Error", "Error 402: No token. I assume you're an iOS user. Email contact@edwinfinch.com.", 3, 15000);
					text_layer_set_text(bar_1, "402");
					break;
				case 2:
					notif_layer_push_notif("FitFace Error", "Error 401: No access. Please email contact@edwinfinch.com to get your account reset.", 3, 15000);
					text_layer_set_text(bar_1, "401");
					break;
				case 3:
					notif_layer_push_notif("FitFace Error", "Error 404: No reason provided. Make sure you're logged in!", 3, 15000);
					text_layer_set_text(bar_1, "404");
					break;
			}
			loading_animation(false);
			app_timer_cancel(new_data_timer);
			new_data_timer = app_timer_register(3000000, get_data, NULL);
			text_layer_set_text(bar_0, "Error");
			text_layer_set_text(bar_2, "Contact");
			text_layer_set_text(bar_3, "Edwin Finch");
			break;
	}
}

void rec_handler(DictionaryIterator *iter, void *context){
	Tuple *t = dict_read_first(iter);
	refresh_timer = app_timer_register(500, refresh_callback, NULL);
	if(t)
	{
		process_tuple(t);
	}
	while(t != NULL)
	{
		t = dict_read_next(iter);
		if(t)
		{
			process_tuple(t);
		}
	}
}

void get_data(){
	DictionaryIterator *iter;
	AppMessageResult result = app_message_outbox_begin(&iter);
	
	dict_write_cstring(iter, 20, "getData");
	
	app_message_outbox_send();
}

void tick_handler(struct tm *t, TimeUnits unit){
	static char buffer[] = "00 00";
	if(clock_is_24h_style()){
		strftime(buffer, sizeof(buffer), "%H:%M", t);
   	}
   	else{
		strftime(buffer,sizeof(buffer), "%I:%M", t);
	}
	text_layer_set_text(time_layer, buffer);

	static char d_buffer_tx[] = "Wed.";
	strftime(d_buffer_tx, sizeof(d_buffer_tx), "%a", t);
	text_layer_set_text(day_layer, d_buffer_tx);

	static char d_buffer_num[] = "25.";
	strftime(d_buffer_num, sizeof(d_buffer_num), "%d", t);
	text_layer_set_text(day_layer_num, d_buffer_num);

	if(t->tm_wday == 3){
		text_layer_set_text(day_layer, "W.");
	}
}

void dividing_proc(Layer *l, GContext *ctx){
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_line(ctx, GPoint(0, 57), GPoint(144, 57));
	graphics_draw_line(ctx, GPoint(0, 58), GPoint(144, 58));
	graphics_draw_line(ctx, GPoint(0, 108), GPoint(144, 108));
	graphics_draw_line(ctx, GPoint(0, 109), GPoint(144, 109));
}

void battery_proc(Layer *l, GContext *ctx){
	int fix = (charge_percent*144)/100;
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_line(ctx, GPoint(0, 166), GPoint(fix, 166));
	graphics_draw_line(ctx, GPoint(0, 167), GPoint(fix, 167));
}

void loading_proc(Layer *l, GContext *ctx){
	graphics_draw_line(ctx, GPoint(0, 57), GPoint(72, 57));
	graphics_draw_line(ctx, GPoint(0, 58), GPoint(72, 58));
	graphics_draw_line(ctx, GPoint(0, 108), GPoint(72, 108));
	graphics_draw_line(ctx, GPoint(0, 109), GPoint(72, 109));
}

void shake_handler(AccelAxisType axis, int32_t direction){
	alt_data_showing = !alt_data_showing;
	if(!alt_data_showing){
		GRect current_f1 = layer_get_frame(inverter_layer_get_layer(bar_1_inv));
		animate_layer(inverter_layer_get_layer(bar_1_inv), &current_f1, &GRect(0, bar_heights[1], 0, BAR_WIDTH), 1000, 0);
		GRect current_f2 = layer_get_frame(inverter_layer_get_layer(bar_2_inv));
		animate_layer(inverter_layer_get_layer(bar_2_inv), &current_f2, &GRect(0, bar_heights[2], 0, BAR_WIDTH), 1000, 0);

		Layer *layer_bar_2 = text_layer_get_layer(bar_2);
		Layer *layer_bar_3 = text_layer_get_layer(bar_3);
		GetRekt current_layer3 = layer_get_frame(layer_bar_2);
		GetRekt current_layer4 = layer_get_frame(layer_bar_3);

		animate_layer(layer_bar_2, &current_layer3, &GRect(16, bar_heights[2]-4, 118, 38), 700, 0);
		animate_layer(layer_bar_3, &current_layer4, &GRect(16, bar_heights[3]-4, 118, 38), 700, 0); 

		bitmap_layer_set_bitmap(iconlayer_1, cals_eaten);
		bitmap_layer_set_bitmap(iconlayer_2, cals_left_i);
		layer_set_hidden(bitmap_layer_get_layer(iconlayer_3), true);
		layer_set_hidden(bitmap_layer_get_layer(iconlayer_4), true);

		static char buffer[] = "Heavy/Medium (battery)";
		snprintf(buffer, sizeof(buffer), "Fitbit: %s", main_data.battery[0]);
		text_layer_set_text(bar_3, buffer);

		static char bt_buffer[] = "Disconnected.";
		snprintf(bt_buffer, sizeof(buffer), "%s.", bt_bools[bluetooth_connection_service_peek()]);
		text_layer_set_text(bar_2, bt_buffer);

		refresh_bar(4);
		refresh_bar(5);

		size_t free_m = heap_bytes_free();
		if(settings.debug){APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes free", free_m);}
	}
	else{
		bitmap_layer_set_bitmap(iconlayer_1, cals_i);
		bitmap_layer_set_bitmap(iconlayer_3, time_i);
		layer_set_hidden(bitmap_layer_get_layer(iconlayer_3), false);
		layer_set_hidden(bitmap_layer_get_layer(iconlayer_4), false);
		bitmap_layer_set_bitmap(iconlayer_2, shoe_i);
		bitmap_layer_set_bitmap(iconlayer_4, distance_i);
		refresh_bar(0);
		refresh_bar(1);
		refresh_bar(2);
		refresh_bar(3);

		Layer *layer_bar_2 = text_layer_get_layer(bar_2);
		Layer *layer_bar_3 = text_layer_get_layer(bar_3);
		GetRekt current_layer3 = layer_get_frame(layer_bar_2);
		GetRekt current_layer4 = layer_get_frame(layer_bar_3);

		animate_layer(layer_bar_2, &current_layer3, &GRect(26, bar_heights[2]-4, 118, 38), 700, 0);
		animate_layer(layer_bar_3, &current_layer4, &GRect(26, bar_heights[3]-4, 118, 38), 700, 0); 
		/*
		size_t free_m = heap_bytes_free();
		APP_LOG(APP_LOG_LEVEL_INFO, "%d bytes free", free_m);
		*/
	}
}
	
void window_load(Window *w){
	Layer *window_layer = window_get_root_layer(w);

	time_layer = text_layer_init(GRect(2, 60, 144, 50), false);
	text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
	text_layer_set_text(time_layer, "00 00");

	day_layer = text_layer_init(GRect(2, 75, 24, 38), true);
	text_layer_set_text_alignment(day_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(day_layer));
	
	day_layer_num = text_layer_init(GRect(118, 72, 24, 38), true);
	text_layer_set_text_alignment(day_layer_num, GTextAlignmentCenter);
	text_layer_set_font(day_layer_num, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(day_layer_num));

	dividing_layer = layer_create(GRect(0, 1, 144, 168));
	layer_set_update_proc(dividing_layer, dividing_proc);
	layer_add_child(window_layer, dividing_layer);
	
	battery_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(battery_layer, battery_proc);

	loading_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(loading_layer, loading_proc);
	layer_add_child(window_layer, loading_layer);
	layer_set_hidden(loading_layer, true);
	
	shoe_i = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SHOE_ICON);
	time_i = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TIME_ICON);
	cals_eaten = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CALS_ICON);
	cals_i = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FIRE_ICON);
	distance_i = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DISTANCE_ICON);
	cals_left_i = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LEFT_ICON);
	
	iconlayer_1 = bitmap_layer_create(GRect(0, bar_heights[0], 24, 24));
	bitmap_layer_set_bitmap(iconlayer_1, cals_i);
	layer_add_child(window_layer, bitmap_layer_get_layer(iconlayer_1));
	
	iconlayer_2 = bitmap_layer_create(GRect(0, bar_heights[1]+2, 24, 24));
	bitmap_layer_set_bitmap(iconlayer_2, shoe_i);
	layer_add_child(window_layer, bitmap_layer_get_layer(iconlayer_2));
	
	iconlayer_3 = bitmap_layer_create(GRect(0, bar_heights[2]+2, 24, 24));
	bitmap_layer_set_bitmap(iconlayer_3, time_i);
	layer_add_child(window_layer, bitmap_layer_get_layer(iconlayer_3));

	iconlayer_4 = bitmap_layer_create(GRect(0, bar_heights[3]+2, 24, 24));
	bitmap_layer_set_bitmap(iconlayer_4, distance_i);
	layer_add_child(window_layer, bitmap_layer_get_layer(iconlayer_4));
	
	layer_add_child(window_layer, text_layer_get_layer(time_layer));
	layer_add_child(window_layer, battery_layer);
	
	bar_0 = text_layer_init(GRect(26, bar_heights[0]-4, 118, 38), true);
	text_layer_set_font(bar_0, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(bar_0, GTextAlignmentLeft);
	text_layer_set_text(bar_0, "Calories");
	layer_add_child(window_layer, text_layer_get_layer(bar_0));
	
	bar_1 = text_layer_init(GRect(26, bar_heights[1]-4, 118, 38), true);
	text_layer_set_font(bar_1, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(bar_1, GTextAlignmentLeft);
	text_layer_set_text(bar_1, "Minutes");
	layer_add_child(window_layer, text_layer_get_layer(bar_1));
	
	bar_2 = text_layer_init(GRect(26, bar_heights[2]-4, 118, 38), true);
	text_layer_set_font(bar_2, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(bar_2, GTextAlignmentLeft);
	text_layer_set_text(bar_2, "Steps");
	layer_add_child(window_layer, text_layer_get_layer(bar_2));
	
	bar_3 = text_layer_init(GRect(26, bar_heights[3]-4, 118, 38), true);
	text_layer_set_font(bar_3, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(bar_3, GTextAlignmentLeft);
	text_layer_set_text(bar_3, "Distance");
	layer_add_child(window_layer, text_layer_get_layer(bar_3));
	
	bar_0_inv = inverter_layer_create(GRect(0, bar_heights[0], 0, BAR_WIDTH));
	layer_add_child(window_layer, inverter_layer_get_layer(bar_0_inv));
	
	bar_1_inv = inverter_layer_create(GRect(0, bar_heights[1], 0, BAR_WIDTH));
	layer_add_child(window_layer, inverter_layer_get_layer(bar_1_inv));
	
	bar_2_inv = inverter_layer_create(GRect(0, bar_heights[2], 0, BAR_WIDTH));
	layer_add_child(window_layer, inverter_layer_get_layer(bar_2_inv));
	
	bar_3_inv = inverter_layer_create(GRect(0, bar_heights[3], 0, BAR_WIDTH));
	layer_add_child(window_layer, inverter_layer_get_layer(bar_3_inv));

	theme = inverter_layer_create(GetRekt(0, 0, 144, 168));
	layer_add_child(window_layer, inverter_layer_get_layer(theme));

	notif_layer_init(window);
	
	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
	tick_handler(t, MINUTE_UNIT);
	
	refresh_timer = app_timer_register(1000, refresh_callback, NULL);
	
	BatteryChargeState state = battery_state_service_peek();
	battery_handler(state);

	refresh_settings();

	new_data_timer = app_timer_register(300000, get_data, NULL);
}

void window_unload(Window *w){
	
}

void init(){
	window = window_create();
	window_set_background_color(window, GColorBlack);
	window_set_window_handlers(window, (WindowHandlers){
		.load = window_load,
		.unload = window_unload,
	});
	
	accel_tap_service_subscribe(shake_handler);
	tick_timer_service_subscribe(MINUTE_UNIT, &tick_handler);
	
	battery_state_service_subscribe(battery_handler);
	bluetooth_connection_service_subscribe(bt_handler);
	
	app_message_register_inbox_received(rec_handler);
	app_message_open(512, 512);

	//strap_init();

	int value = persist_read_data(0, &main_data, sizeof(main_data));
	if(settings.debug){APP_LOG(APP_LOG_LEVEL_INFO, "FitFace: %d read from Fitbit data.", value);}
	int value2 = persist_read_data(1, &settings, sizeof(settings));
	if(settings.debug){APP_LOG(APP_LOG_LEVEL_INFO, "FitFace: %d read from settings struct.", value2);}
	
	window_stack_push(window, true);
}

void deinit(){
	//strap_deinit();
	int value = persist_write_data(0, &main_data, sizeof(main_data));
	if(settings.debug){APP_LOG(APP_LOG_LEVEL_INFO, "FitFace: %d written to Fitbit data.", value);}
	int value2 = persist_write_data(1, &settings, sizeof(settings));
	if(settings.debug){APP_LOG(APP_LOG_LEVEL_INFO, "FitFace: %d written to settings struct.", value2);}
	tick_timer_service_unsubscribe();
	accel_tap_service_unsubscribe();
	window_destroy(window);
}
	
int main(){
	init();
	app_event_loop();
	deinit();
}