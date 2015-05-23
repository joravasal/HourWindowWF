#include <pebble.h>
#include <ctype.h>
#include "gpath_builder.h"
#include "hour_window.h"

Window *window;
static Layer *minute_hand_layer;
static Layer *blocker_layer;
static Layer *background_layer;
TextLayer *hour_text_layer;
TextLayer *last_hour_text_layer;

static GPath *minute_arrow;
static GPath *window_path;
static GPath *blocker_path;
static GPoint center;

int bluetooth_mode = 0;
#ifdef PBL_COLOR
  char basalt_colors[EDITABLE_COLORS_LENGTH];
  int battery_mode = 0;
#endif
static GColor minuteHandFill;
static GColor minuteHandStroke;
static GColor middleDotColor;
static GColor discFill;
static GColor discStroke;
static GColor bgColor;
static GColor textColor;
static GColor windowFill;
static GColor windowStroke;
static GColor bluetoothDisconnectedColor;
static GColor bluetoothConnectedColor;

AppTimer *timer;
const int timer_delay = 40;
int current_angle = 0;
int timer_angle = 360;

static void minute_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(minute_hand_layer);
}

void timer_callback(void *data) {
  layer_mark_dirty(background_layer);
  layer_mark_dirty(blocker_layer);
}

static void bt_handler(bool connected) {
  if(connected == true) {
    switch(bluetooth_mode) {
      case BT_CHANGE_BG:
        bgColor = bluetoothConnectedColor;
        #ifdef PBL_COLOR
          windowFill = bluetoothConnectedColor;
        #endif
        window_set_background_color(window, bgColor);
        layer_mark_dirty(background_layer);
        break;
      case BT_CHANGE_DISC:
        discFill = bluetoothConnectedColor;
        layer_mark_dirty(background_layer);
        break;
      case BT_CHANGE_MIN_HANDLE:
        minuteHandFill = bluetoothConnectedColor;
        #ifdef PBL_BW
          minuteHandStroke = bluetoothDisconnectedColor;
          middleDotColor = bluetoothDisconnectedColor;
        #endif
        layer_mark_dirty(minute_hand_layer);
        break;
    }
  }
  else {
    switch(bluetooth_mode) {
      case BT_CHANGE_BG:
        bgColor = bluetoothDisconnectedColor;
        #ifdef PBL_COLOR
          windowFill = bluetoothDisconnectedColor;
        #endif
        window_set_background_color(window, bgColor);
        layer_mark_dirty(background_layer);
        break;
      case BT_CHANGE_DISC:
        discFill = bluetoothDisconnectedColor;
        layer_mark_dirty(background_layer);
        break;
      case BT_CHANGE_MIN_HANDLE:
        minuteHandFill = bluetoothDisconnectedColor;
        #ifdef PBL_BW
          minuteHandStroke = bluetoothConnectedColor;
          middleDotColor = bluetoothConnectedColor;
        #endif
        layer_mark_dirty(minute_hand_layer);
        break;
    }
  }
}

static void update_window_proc(Layer *layer, GContext *ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "updating window layer");
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  static char buffer[] = "00";
  static char buffer_prev[] = "00";
  
  int hour = t->tm_hour;
  int prevHour = hour - 1;
  //Update hour text
  if(clock_is_24h_style() == true) {
    strftime(buffer, sizeof(buffer), "%H", t);
    text_layer_set_text(hour_text_layer, buffer);
    if (prevHour < 0) prevHour = 23;
  } else {
    strftime(buffer, sizeof(buffer), "%I", t);
    text_layer_set_text(hour_text_layer, buffer);
    if (prevHour < 0) prevHour = 11;
  }
  
  snprintf(buffer_prev, sizeof(buffer_prev), "%d", prevHour);
  text_layer_set_text(last_hour_text_layer, buffer_prev);
  
  GPoint auxPoint = ANALOG_BG_HOUR_POINTS[hour % 12];
  layer_set_frame(text_layer_get_layer(hour_text_layer), GRect(auxPoint.x - (TEXT_WIDTH / 2), auxPoint.y - (TEXT_HEIGHT / 2) + TEXT_Y_OFFSET, TEXT_WIDTH, TEXT_HEIGHT));
  auxPoint = ANALOG_BG_HOUR_POINTS[prevHour % 12];
  layer_set_frame(text_layer_get_layer(last_hour_text_layer), GRect(auxPoint.x - (TEXT_WIDTH / 2), auxPoint.y - (TEXT_HEIGHT / 2) + TEXT_Y_OFFSET, TEXT_WIDTH, TEXT_HEIGHT));  
  
  //Upate background
  graphics_context_set_fill_color(ctx, discFill);
  graphics_context_set_stroke_color(ctx, discStroke);
  #ifdef PBL_COLOR
    graphics_context_set_stroke_width(ctx, 2);
  #endif
  graphics_fill_circle(ctx, center, CIRCLE_RADIUS);
  graphics_draw_circle(ctx, center, CIRCLE_RADIUS);
  #ifdef PBL_COLOR
    graphics_context_set_stroke_width(ctx, 1);
  #endif
  
  current_angle = 30 * ((hour) % 12);
  
  //Check if we have to start an animation
  if(current_angle > timer_angle || (current_angle == 0 && timer_angle < 360 && timer_angle != 0)) {
    timer_angle = (timer_angle + 1) % 360;    
    gpath_rotate_to(window_path, (timer_angle * TRIG_MAX_ANGLE) / 360);
    graphics_context_set_fill_color(ctx, windowFill);
    graphics_context_set_stroke_color(ctx, windowStroke);
    gpath_draw_filled(ctx, window_path);
    gpath_draw_outline(ctx, window_path);
    //Register next execution
    timer = app_timer_register(timer_delay, (AppTimerCallback) timer_callback, NULL);
  } else { //Static drawing
    timer_angle = current_angle;    
    gpath_rotate_to(window_path, (current_angle * TRIG_MAX_ANGLE) / 360);
    graphics_context_set_fill_color(ctx, windowFill);
    graphics_context_set_stroke_color(ctx, windowStroke);
    gpath_draw_filled(ctx, window_path);
    gpath_draw_outline(ctx, window_path);
  }
}

static void update_blockers_proc(Layer *layer, GContext *ctx) {
    gpath_rotate_to(blocker_path, (((timer_angle + 30) % 360) * TRIG_MAX_ANGLE) / 360);
    graphics_context_set_fill_color(ctx, discFill);
    gpath_draw_filled(ctx, blocker_path);
    
    gpath_rotate_to(blocker_path, (((timer_angle - 30) % 360) * TRIG_MAX_ANGLE) / 360);
    graphics_context_set_fill_color(ctx, discFill);
    gpath_draw_filled(ctx, blocker_path);
}

static void update_minute_hand_proc(Layer *layer, GContext *ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "updating minute hand layer");
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  graphics_context_set_fill_color(ctx, minuteHandFill);
  graphics_context_set_stroke_color(ctx, minuteHandStroke);

  gpath_rotate_to(minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_filled(ctx, minute_arrow);
  #ifdef PBL_COLOR
    graphics_context_set_stroke_width(ctx, 2);
  #endif
  gpath_draw_outline(ctx, minute_arrow);
  #ifdef PBL_COLOR
    graphics_context_set_stroke_width(ctx, 1);
  #endif
  
  // dot in the middle
  graphics_context_set_fill_color(ctx, middleDotColor);
  graphics_fill_rect(ctx, GRect(center.x - 1, center.y - 1, 3, 3), 0, GCornerNone);
}

#ifdef PBL_COLOR
  unsigned int HexStringToUInt(char const* hexstring) {
		unsigned int result = 0;
		char const *c = hexstring;
		unsigned char thisC;

		while( (thisC = *c) != 0 ) {
			thisC = toupper(thisC);
			result <<= 4;

			if( isdigit(thisC)) {
				result += thisC - '0';
      } else if(isxdigit(thisC)) {
				result += thisC - 'A' + 10;
      } else {
				APP_LOG(APP_LOG_LEVEL_WARNING, "ERROR: Unrecognised hex character \"%c\"", thisC);
				return 0;
			}
			++c;
		}
		return result;  
	}
  
  static void apply_colors() {
    int string_num = (EDITABLE_COLORS_LENGTH - 1) / 6;
    char color_strings[string_num][7];
    int colorInts[string_num];
    /*char str_color1[7];
		char str_color2[7];
    char str_color3[7];
    char str_color4[7];
    char str_color5[7];
    char str_color6[7];
    char str_color7[7];
    char str_color8[7];
    char str_color9[7];*/

		if (strlen(basalt_colors) >= EDITABLE_COLORS_LENGTH - 1) {
      for (int i = 0; i < string_num; i++) {
        memcpy(color_strings[i], &basalt_colors[6*i], 6);
        color_strings[i][6] = '\0';
        colorInts[i] = HexStringToUInt(color_strings[i]);
      }
			/*memcpy(str_color1, &basalt_colors[0], 6);
			memcpy(str_color2, &basalt_colors[6], 6);
      memcpy(str_color3, &basalt_colors[12], 6);
      memcpy(str_color4, &basalt_colors[18], 6);
      memcpy(str_color5, &basalt_colors[24], 6);
      memcpy(str_color6, &basalt_colors[30], 6);
      memcpy(str_color7, &basalt_colors[36], 6);
      memcpy(str_color8, &basalt_colors[42], 6);
      memcpy(str_color9, &basalt_colors[48], 6);
			str_color1[6] = '\0';
			str_color2[6] = '\0';
      str_color3[6] = '\0';
      str_color4[6] = '\0';
      str_color5[6] = '\0';
      str_color6[6] = '\0';
      str_color7[6] = '\0';
      str_color8[6] = '\0';
      str_color9[6] = '\0';*/
		}
		else return;

		/*int bgColorInt = HexStringToUInt(str_color1);
    int textColorInt = HexStringToUInt(str_color2);
		int diskFillColorInt = HexStringToUInt(str_color3);
    int diskStrokeColorInt = HexStringToUInt(str_color4);
    int hourWindowStrokeColorInt = HexStringToUInt(str_color5);
    int minFillColorInt = HexStringToUInt(str_color6);
    int minStrokeColorInt = HexStringToUInt(str_color7);
    int bluetoothConnectedColorInt = HexStringToUInt(str_color8);
    int bluetoothDisconnectedColorInt = HexStringToUInt(str_color9);*/
    
    bgColor = GColorFromHEX(colorInts[0]);
    textColor = GColorFromHEX(colorInts[1]);
    
    discFill = GColorFromHEX(colorInts[2]);
    discStroke = GColorFromHEX(colorInts[3]);
    
    windowStroke = GColorFromHEX(colorInts[4]);
    
    minuteHandFill = GColorFromHEX(colorInts[5]);
    minuteHandStroke = GColorFromHEX(colorInts[6]);
    
    bluetoothConnectedColor = GColorFromHEX(colorInts[7]);
    bluetoothDisconnectedColor = GColorFromHEX(colorInts[8]);
    
    middleDotColor = GColorFromHEX(colorInts[2]);
    windowFill = GColorFromHEX(colorInts[0]);
    /*bgColor = GColorFromHEX(bgColorInt);
    textColor = GColorFromHEX(textColorInt);
    
    discFill = GColorFromHEX(diskFillColorInt);
    discStroke = GColorFromHEX(diskStrokeColorInt);
    
    windowStroke = GColorFromHEX(hourWindowStrokeColorInt);
    
    minuteHandFill = GColorFromHEX(minFillColorInt);
    minuteHandStroke = GColorFromHEX(minStrokeColorInt);
    
    bluetoothConnectedColor = GColorFromHEX(bluetoothConnectedColorInt);
    bluetoothDisconnectedColor = GColorFromHEX(bluetoothDisconnectedColorInt);
    
    middleDotColor = GColorFromHEX(diskFillColorInt);
    windowFill = GColorFromHEX(bgColorInt);*/
    
  }
#else
  static void apply_colors(int theme, int min_color, int hour_color) {
    if(min_color == 0) { // Black
      minuteHandFill = GColorBlack;
      minuteHandStroke = GColorWhite;
      middleDotColor = GColorWhite;
      if (bluetooth_mode == BT_CHANGE_MIN_HANDLE) {
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      }
    } else if (min_color == 1) { // White
      minuteHandFill = GColorWhite;
      minuteHandStroke = GColorBlack;
      middleDotColor = GColorBlack;
      if (bluetooth_mode == BT_CHANGE_MIN_HANDLE) {
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      }
    }
  
    if(hour_color == 0) { // Black
      textColor = GColorWhite;
      windowFill = GColorBlack;
      windowStroke = GColorWhite;
    } else if (hour_color == 1) { // White
      textColor = GColorBlack;
      windowFill = GColorWhite;
      windowStroke = GColorBlack;
    }
    
    if(theme == ORIGINAL_COLORS) {
      discFill = GColorBlack;
      discStroke = GColorBlack;
      bgColor = GColorWhite;
      if (bluetooth_mode == BT_CHANGE_BG) {
        discStroke = GColorWhite;
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      } else if (bluetooth_mode == BT_CHANGE_DISC) {
        discStroke = GColorBlack;
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      }
    } else if(theme == INVERTED_COLORS) {
      discFill = GColorWhite;
      discStroke = GColorWhite;
      bgColor = GColorBlack;
      if (bluetooth_mode == BT_CHANGE_BG) {
        discStroke = GColorBlack;
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      } else if (bluetooth_mode == BT_CHANGE_DISC) {
        discStroke = GColorWhite;
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      }
    } else if(theme == SAME_COLORS_BLACK_CIRCLE) {
      discFill = GColorBlack;
      discStroke = GColorWhite;
      bgColor = GColorBlack;
      if (bluetooth_mode == BT_CHANGE_BG) {
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      } else if (bluetooth_mode == BT_CHANGE_DISC) {
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      }
    } else if(theme == SAME_COLORS_WHITE_CIRCLE) {
      discFill = GColorWhite;
      discStroke = GColorBlack;
      bgColor = GColorWhite;
      if (bluetooth_mode == BT_CHANGE_BG) {
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      } else if (bluetooth_mode == BT_CHANGE_DISC) {
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      }
    } else if(theme == SAME_COLORS_BLACK) {
      discFill = GColorBlack;
      discStroke = GColorBlack;
      bgColor = GColorBlack;
      if (bluetooth_mode == BT_CHANGE_BG) {
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      } else if (bluetooth_mode == BT_CHANGE_DISC) {
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      }
    } else if(theme == SAME_COLORS_WHITE) {
      discFill = GColorWhite;
      discStroke = GColorWhite;
      bgColor = GColorWhite;
      if (bluetooth_mode == BT_CHANGE_BG) {
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      } else if (bluetooth_mode == BT_CHANGE_DISC) {
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      }
    }
  }
#endif

static void in_recv_handler(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "JS message received");
  Tuple *t = dict_read_first(iterator);
  #ifdef PBL_COLOR
    char new_colors[EDITABLE_COLORS_LENGTH];
  #else
    int new_theme = -1;
    int new_min_hand_color = -1;
    int new_hour_color = -1;
  #endif
  while(t) {
    switch (t->key) {
    
    case KEY_BT_SIGNAL:
      bluetooth_mode = t->value->int16;
      persist_write_int(KEY_BT_SIGNAL, t->value->int16);
      break;
      
    #ifdef PBL_COLOR
    case KEY_COLORS_BASALT:
      if (persist_exists(KEY_COLORS_BASALT)) {
        memcpy(new_colors, t->value->cstring, t->length);
        if (strcmp(basalt_colors, new_colors) != 0) {
          persist_write_string(KEY_COLORS_BASALT, new_colors);
          memcpy(basalt_colors, new_colors, strlen(new_colors));
        }
      } else {
        memcpy(basalt_colors, t->value->cstring, t->length);
        persist_write_string(KEY_COLORS_BASALT, basalt_colors);
      }
      break;
    case KEY_BATTERY_SIGNAL:
      persist_write_int(KEY_BATTERY_SIGNAL, t->value->int16);
      break;
      
    #else
    case KEY_THEME_APLITE:
      if (!persist_exists(KEY_THEME_APLITE) ||
          (persist_exists(KEY_THEME_APLITE) &&
            persist_read_int(KEY_THEME_APLITE) != t->value->int16)) {
        persist_write_int(KEY_THEME_APLITE, t->value->int16);
        new_theme = t->value->int16;
      }
      break;
    case KEY_HOUR_COLOR_APLITE:
      if (!persist_exists(KEY_HOUR_COLOR_APLITE) ||
          (persist_exists(KEY_HOUR_COLOR_APLITE) &&
            persist_read_int(KEY_HOUR_COLOR_APLITE) != t->value->int16)) {
        persist_write_int(KEY_HOUR_COLOR_APLITE, t->value->int16);
        new_hour_color = t->value->int16;
      }
      break;
    case KEY_MIN_COLOR_APLITE:
      if (!persist_exists(KEY_MIN_COLOR_APLITE) ||
          (persist_exists(KEY_MIN_COLOR_APLITE) &&
            persist_read_int(KEY_MIN_COLOR_APLITE) != t->value->int16)) {
        persist_write_int(KEY_MIN_COLOR_APLITE, t->value->int16);
        new_min_hand_color = t->value->int16;
      }
      break;
    #endif
    }
    
    t = dict_read_next(iterator);
  }
  #ifdef PBL_COLOR
    apply_colors();
  #else
    apply_colors(new_theme, new_min_hand_color, new_hour_color);
  #endif
  
  text_layer_set_text_color(last_hour_text_layer, textColor);
  text_layer_set_text_color(hour_text_layer, textColor);
  window_set_background_color(window, bgColor);
  bt_handler(bluetooth_connection_service_peek());
  layer_mark_dirty(minute_hand_layer);
  layer_mark_dirty(background_layer);
  layer_mark_dirty(blocker_layer);
}

static void window_load(Window *window) {
  bluetooth_mode = persist_exists(KEY_BT_SIGNAL) ? persist_read_int(KEY_BT_SIGNAL) : BT_CHANGE_BG;
  #ifdef PBL_COLOR
		if (persist_exists(KEY_COLORS_BASALT))
			persist_read_string(KEY_COLORS_BASALT, basalt_colors, EDITABLE_COLORS_LENGTH);
		else
			memcpy(basalt_colors, "FFFFFF000000555555AAAAAAFFFFFFFF0000FF000055FF55FF5555"+'\0', EDITABLE_COLORS_LENGTH);
    apply_colors();
	#else
		int aplite_theme = persist_exists(KEY_THEME_APLITE) ? persist_read_int(KEY_THEME_APLITE) : ORIGINAL_COLORS;
    int min_color_aplite = persist_exists(KEY_MIN_COLOR_APLITE) ? persist_read_int(KEY_MIN_COLOR_APLITE) : 1;
    int hour_color_aplite = persist_exists(KEY_HOUR_COLOR_APLITE) ? persist_read_int(KEY_HOUR_COLOR_APLITE) : 1;
    apply_colors(aplite_theme, min_color_aplite, hour_color_aplite);
	#endif
  bt_handler(bluetooth_connection_service_peek());
    
  Layer *window_layer = window_get_root_layer(window);
  window_set_background_color(window, bgColor);
  GRect bounds = layer_get_bounds(window_layer);
  center = grect_center_point(&bounds);
  
  gpath_move_to(window_path, center);
  gpath_move_to(blocker_path, center);
  gpath_move_to(minute_arrow, center);

  background_layer = layer_create(bounds);
  layer_set_update_proc(background_layer, update_window_proc);
  layer_add_child(window_layer, background_layer);
  
  hour_text_layer = text_layer_create(GRect(0, 0, TEXT_WIDTH, TEXT_HEIGHT));
  text_layer_set_background_color(hour_text_layer, GColorClear);
  text_layer_set_text_color(hour_text_layer, textColor);
  text_layer_set_text_alignment(hour_text_layer, GTextAlignmentCenter);
  text_layer_set_font(hour_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  
  last_hour_text_layer = text_layer_create(GRect(0, 0, TEXT_WIDTH, TEXT_HEIGHT));
  text_layer_set_background_color(last_hour_text_layer, GColorClear);
  text_layer_set_text_color(last_hour_text_layer, textColor);
  text_layer_set_text_alignment(last_hour_text_layer, GTextAlignmentCenter);
  text_layer_set_font(last_hour_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  
  layer_add_child(window_layer, text_layer_get_layer(hour_text_layer));
  layer_add_child(window_layer, text_layer_get_layer(last_hour_text_layer));
  
  blocker_layer = layer_create(bounds);
  layer_set_update_proc(blocker_layer, update_blockers_proc);
  layer_add_child(window_layer, blocker_layer);
  
  minute_hand_layer = layer_create(bounds);
  layer_set_update_proc(minute_hand_layer, update_minute_hand_proc);
  layer_add_child(window_layer, minute_hand_layer);
}

static void window_unload(Window *window) {
  layer_destroy(minute_hand_layer);
  layer_destroy(background_layer);
  
  text_layer_destroy(hour_text_layer);
  text_layer_destroy(last_hour_text_layer);
}

void handle_init(void) {
  window = window_create();
  Layer *window_layer = window_get_root_layer(window);
  
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  
  //Create the opening for the hour
  GPathBuilder *builder = gpath_builder_create(MAX_POINTS);
  gpath_builder_move_to_point(builder, WINDOW_POINTS[0]);
  gpath_builder_line_to_point(builder, WINDOW_POINTS[1]);
  gpath_builder_curve_to_point(builder, WINDOW_POINTS[4], WINDOW_POINTS[2], WINDOW_POINTS[3]);
  gpath_builder_line_to_point(builder, WINDOW_POINTS[0]);
  window_path = gpath_builder_create_path(builder);
  gpath_builder_destroy(builder);
  
  //Create the blocker for the extra text in the animations
  builder = gpath_builder_create(MAX_POINTS);
  gpath_builder_move_to_point(builder, HOUR_BLOCKER[0]);
  gpath_builder_line_to_point(builder, HOUR_BLOCKER[1]);
  gpath_builder_curve_to_point(builder, HOUR_BLOCKER[4], HOUR_BLOCKER[2], HOUR_BLOCKER[3]);
  gpath_builder_line_to_point(builder, HOUR_BLOCKER[0]);
  blocker_path = gpath_builder_create_path(builder);
  gpath_builder_destroy(builder);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, minute_tick_handler);
  
  // Bluetooth subscription
  bluetooth_connection_service_subscribe(bt_handler);
}

void handle_deinit(void) {
  window_destroy(window);
  gpath_destroy(minute_arrow);
  gpath_destroy(window_path);
  gpath_destroy(blocker_path);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}