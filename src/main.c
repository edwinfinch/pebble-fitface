#include <pebble.h>
#include "main.h"
#include "strap/strap.h"
void loading_animation(bool start);
	
char* floatToString(char* buffer, int bufferSize, double number){
	char decimalBuffer[5];

	snprintf(buffer, bufferSize, "%d", (int)number);
	strcat(buffer, ".");

	snprintf(decimalBuffer, 5, "%02d", (int)((double)(number - (int)number) * (double)100));
	strcat(buffer, decimalBuffer);

	return buffer;
}

void refresh_bar(int bar){
	float pre_data[2] = {
		0, 0
	};
	pre_data[0] = (float)main_data.current_values[bar];
	pre_data[1] = (float)main_data.goals[bar];

	bool distance = false;
	if(main_data.imperial && bar == 3){
		distance = true;
		pre_data[0] = (float)(main_data.distanceMI[0])/100;
		pre_data[1] = (float)(main_data.distanceMI[1])/100;
	}
	else if(!main_data.imperial && bar == 3){
		pre_data[0] = (float)(main_data.current_values[bar])/100;
		pre_data[1] = (float)(main_data.goals[bar])/100;
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
			floatToString(buffer[bar], 12, (double)pre_data[0]);
			//snprintf(buffer[bar], sizeof(buffer[bar]), "~%s mi", );
		}
		else{
			floatToString(buffer[bar], 12, (double)pre_data[0]);
			//snprintf(buffer[bar], sizeof(buffer[bar]), "~%s km", floatToString(buffer[bar], 12, (double)pre_data[0]));
		}
	}
	
	switch(bar){
		case 0:
			current_rect = layer_get_frame(inverter_layer_get_layer(calories_b));
			text_layer_set_text(calories_v, buffer[0]);
			animate_layer(inverter_layer_get_layer(calories_b), &current_rect, &togo_rect, 1000, 0);
			break;
		case 1:
			current_rect = layer_get_frame(inverter_layer_get_layer(active_b));
			text_layer_set_text(active_v, buffer[1]);
			animate_layer(inverter_layer_get_layer(active_b), &current_rect, &togo_rect, 1000, 0);
			break;
		case 2:
			current_rect = layer_get_frame(inverter_layer_get_layer(steps_b));
			text_layer_set_text(steps_v, buffer[2]);
			animate_layer(inverter_layer_get_layer(steps_b), &current_rect, &togo_rect, 1000, 0);
			break;
		case 3:
			current_rect = layer_get_frame(inverter_layer_get_layer(distance_b));
			text_layer_set_text(distance_v, buffer[3]);
			animate_layer(inverter_layer_get_layer(distance_b), &current_rect, &togo_rect, 1000, 0);
			break;
		case 4:
			current_rect = layer_get_frame(inverter_layer_get_layer(calories_b));
			text_layer_set_text(calories_v, buffer[4]);
			animate_layer(inverter_layer_get_layer(calories_b), &current_rect, &togo_rect, 1000, 0);
			break;
		case 5:
			current_rect = layer_get_frame(inverter_layer_get_layer(distance_b));
			text_layer_set_text(distance_v, buffer[5]);
			animate_layer(inverter_layer_get_layer(distance_b), &current_rect, &togo_rect, 1000, 0);
			break;
	}
}

void refresh_callback(){
	for(int i = 0; i < 4; i++){
		refresh_bar(i);
	}
}

void bt_handler(bool connected){
	if(connected){
		bitmap_layer_set_bitmap(bt_icon_layer, bt_icon);
	}
	else{
		bitmap_layer_set_bitmap(bt_icon_layer, NULL);
	}
	layer_mark_dirty(bitmap_layer_get_layer(bt_icon_layer));
}

void battery_handler(BatteryChargeState state){
	charge_percent = state.charge_percent;
	layer_mark_dirty(battery_layer);
}

void settings_loaded(){
	alt_data_showing = false;
	for(int i = 0; i < 4; i++){
		refresh_bar(i);
	}
	if(alt_data_showing){
		refresh_bar(4);
		refresh_bar(5);
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
	APP_LOG(APP_LOG_LEVEL_INFO, "key: %d, data %d", key, value);
	switch(key){
		case 0:
			main_data.current_values[2] = value;
			strap_log_event("/settings_save");
			loading_animation(false);
			break;
		case 1:
			main_data.current_values[1] = value;
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
			main_data.goals[2] = value;
			break;
		case 7:
			main_data.goals[1] = value;
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
			strap_log_event(buffer);
			break;
		case 13:
			main_data.imperial = value;
			if(value){
				strap_log_event("/uses_imperial");
			}
			else{
				strap_log_event("/uses_metric");
			}
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
	}
}

void rec_handler(DictionaryIterator *iter, void *context){
	Tuple *t = dict_read_first(iter);
	refresh_timer = app_timer_register(500, settings_loaded, NULL);
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
	static char d_buffer[] = "Fri. xx.xx.'xx...";
	strftime(d_buffer, sizeof(d_buffer), "%a %d.%m.'%y", t);
	
	text_layer_set_text(time_layer, buffer);
	text_layer_set_text(date_layer, d_buffer);
	if(t->tm_min % 5 == 0){
		strap_log_event("/get_data_5m");
		get_data();
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
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_fill_color(ctx, GColorWhite);
	
	graphics_draw_rect(ctx, GRect(6, 67, 25, 36));
	graphics_draw_rect(ctx, GRect(12, 63, 13, 5));
	
	float fill_fix = ((float)charge_percent/100)*30;
	float pos_fix = (30-fill_fix)+70;
	//APP_LOG(APP_LOG_LEVEL_INFO, "%d", (int)fill_fix);
	graphics_fill_rect(ctx, GRect(9, pos_fix, 19, fill_fix), 0, GCornerNone);
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
		GRect current_f1 = layer_get_frame(inverter_layer_get_layer(active_b));
		animate_layer(inverter_layer_get_layer(active_b), &current_f1, &GRect(0, bar_heights[1], 0, BAR_WIDTH), 1000, 0);
		GRect current_f2 = layer_get_frame(inverter_layer_get_layer(steps_b));
		animate_layer(inverter_layer_get_layer(steps_b), &current_f2, &GRect(0, bar_heights[2], 0, BAR_WIDTH), 1000, 0);
		bitmap_layer_set_bitmap(iconlayer_1, cals_eaten);
		bitmap_layer_set_bitmap(iconlayer_2, battery_icon);
		bitmap_layer_set_bitmap(iconlayer_4, cals_left_i);
		layer_set_hidden(bitmap_layer_get_layer(iconlayer_3), true);
		static char buffer[] = "Heavy/Medium (battery)";
		snprintf(buffer, sizeof(buffer), "%s", main_data.battery[0]);
		text_layer_set_text(active_v, buffer);
		text_layer_set_text(steps_v, "Cals Left");
		refresh_bar(4);
		refresh_bar(5);
	}
	else{
		layer_set_hidden(bitmap_layer_get_layer(iconlayer_3), false);
		bitmap_layer_set_bitmap(iconlayer_1, cals_i);
		bitmap_layer_set_bitmap(iconlayer_2, time_i);
		bitmap_layer_set_bitmap(iconlayer_4, distance_i);
		refresh_bar(0);
		refresh_bar(1);
		refresh_bar(2);
		refresh_bar(3);
	}
}
	
void window_load(Window *w){
	Layer *window_layer = window_get_root_layer(w);
	time_layer = text_layer_init(GRect(33, 58, 80, 38), false);
	text_layer_set_text(time_layer, "00 00");
	
	date_layer = text_layer_init(GRect(33, 89, 80, 38), true);
	text_layer_set_text(date_layer, "Fri, 22.08.'14");
	
	dividing_layer = layer_create(GRect(0, 1, 144, 168));
	layer_set_update_proc(dividing_layer, dividing_proc);
	layer_add_child(window_layer, dividing_layer);
	
	battery_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(battery_layer, battery_proc);
	layer_add_child(window_layer, battery_layer);

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
	bt_icon = gbitmap_create_with_resource(RESOURCE_ID_BT_ICON);
	battery_icon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATTERY_ICON);
	
	iconlayer_1 = bitmap_layer_create(GRect(0, bar_heights[0], 24, 24));
	bitmap_layer_set_bitmap(iconlayer_1, cals_i);
	layer_add_child(window_layer, bitmap_layer_get_layer(iconlayer_1));
	
	iconlayer_2 = bitmap_layer_create(GRect(0, bar_heights[1]+2, 24, 24));
	bitmap_layer_set_bitmap(iconlayer_2, time_i);
	layer_add_child(window_layer, bitmap_layer_get_layer(iconlayer_2));
	
	iconlayer_3 = bitmap_layer_create(GRect(0, bar_heights[2]+2, 24, 24));
	bitmap_layer_set_bitmap(iconlayer_3, shoe_i);
	layer_add_child(window_layer, bitmap_layer_get_layer(iconlayer_3));

	iconlayer_4 = bitmap_layer_create(GRect(0, bar_heights[3]+2, 24, 24));
	bitmap_layer_set_bitmap(iconlayer_4, distance_i);
	layer_add_child(window_layer, bitmap_layer_get_layer(iconlayer_4));
	
	bt_icon_layer = bitmap_layer_create(GRect(100, 60, 50, 50));
	bitmap_layer_set_bitmap(bt_icon_layer, bt_icon);
	layer_add_child(window_layer, bitmap_layer_get_layer(bt_icon_layer));
	layer_add_child(window_layer, text_layer_get_layer(date_layer));
	layer_add_child(window_layer, text_layer_get_layer(time_layer));
	
	calories_v = text_layer_init(GRect(26, bar_heights[0]-4, 118, 38), true);
	text_layer_set_font(calories_v, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(calories_v, GTextAlignmentLeft);
	text_layer_set_text(calories_v, "Calories");
	layer_add_child(window_layer, text_layer_get_layer(calories_v));
	
	active_v = text_layer_init(GRect(26, bar_heights[1]-4, 118, 38), true);
	text_layer_set_font(active_v, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(active_v, GTextAlignmentLeft);
	text_layer_set_text(active_v, "Minutes");
	layer_add_child(window_layer, text_layer_get_layer(active_v));
	
	steps_v = text_layer_init(GRect(26, bar_heights[2]-4, 118, 38), true);
	text_layer_set_font(steps_v, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(steps_v, GTextAlignmentLeft);
	text_layer_set_text(steps_v, "Steps");
	layer_add_child(window_layer, text_layer_get_layer(steps_v));
	
	distance_v = text_layer_init(GRect(26, bar_heights[3]-4, 118, 38), true);
	text_layer_set_font(distance_v, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text_alignment(distance_v, GTextAlignmentLeft);
	text_layer_set_text(distance_v, "Distance");
	layer_add_child(window_layer, text_layer_get_layer(distance_v));
	
	calories_b = inverter_layer_create(GRect(0, bar_heights[0], 0, BAR_WIDTH));
	layer_add_child(window_layer, inverter_layer_get_layer(calories_b));
	
	active_b = inverter_layer_create(GRect(0, bar_heights[1], 0, BAR_WIDTH));
	layer_add_child(window_layer, inverter_layer_get_layer(active_b));
	
	steps_b = inverter_layer_create(GRect(0, bar_heights[2], 0, BAR_WIDTH));
	layer_add_child(window_layer, inverter_layer_get_layer(steps_b));
	
	distance_b = inverter_layer_create(GRect(0, bar_heights[3], 0, BAR_WIDTH));
	layer_add_child(window_layer, inverter_layer_get_layer(distance_b));
	
	struct tm *t;
  	time_t temp;        
  	temp = time(NULL);        
  	t = localtime(&temp);
	tick_handler(t, MINUTE_UNIT);
	
	refresh_timer = app_timer_register(1000, refresh_callback, NULL);
	
	bool con = bluetooth_connection_service_peek();
	bt_handler(con);
	
	BatteryChargeState state = battery_state_service_peek();
	battery_handler(state);
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

	strap_init();

	int value = persist_read_data(0, &main_data, sizeof(main_data));
	APP_LOG(APP_LOG_LEVEL_INFO, "FitFace: %d read", value);
	
	window_stack_push(window, true);
}

void deinit(){
	strap_deinit();
	int value = persist_write_data(0, &main_data, sizeof(main_data));
	APP_LOG(APP_LOG_LEVEL_INFO, "FitFace: %d written", value);
	tick_timer_service_unsubscribe();
	accel_tap_service_unsubscribe();
	window_destroy(window);
}
	
int main(){
	init();
	app_event_loop();
	deinit();
}