#include <pebble.h>

TextLayer* text_layer_init(GRect location, bool is_date);
void animate_layer(Layer *layer, GRect *start, GRect *finish, int duration, int delay);