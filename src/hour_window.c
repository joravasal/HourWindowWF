#include <pebble.h>
#include "gpath_builder.h"
#include "hour_window.h"
#include <ctype.h>

Window *window;
static Layer *minute_hand_layer;
static Layer *background_layer;
TextLayer *hour_text_layer;
//TextLayer *last_hour_text_layer;

#ifdef PBL_COLOR
  char basalt_colors[EDITABLE_COLORS_LENGTH];
  int battery_mode = 0;
  int bluetooth_mode = 0;
#endif
static GColor minuteHandFill;
static GColor minuteHandStroke;
static GColor middleDotColor;
static GColor bgCircleFill;
static GColor bgCircleStroke;
static GColor bgColor;
static GColor textColor;
static GColor windowFill;
static GColor windowStroke;

AppTimer *timer;
const int timer_delay = 40;
int current_angle = 0;
int timer_angle = 360;

static GPath *minute_arrow;
static GPath *window_path;
static GPoint center;

static void minute_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(minute_hand_layer);
}

void timer_callback(void *data) {
  layer_mark_dirty(background_layer);
}

static void update_window_proc(Layer *layer, GContext *ctx) {
  APP_LOG(APP_LOG_LEVEL_DEBUG_VERBOSE, "updating window layer");
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  static char buffer[] = "00";
  static char buffer_prev[] = "00";
  
  int hour = t->tm_hour;
  //int prevHour = hour - 1;
  //Update hour text
  if(clock_is_24h_style() == true) {
    strftime(buffer, sizeof(buffer), "%H", t);
    text_layer_set_text(hour_text_layer, buffer);
    //if (prevHour < 0) prevHour = 23;
  } else {
    strftime(buffer, sizeof(buffer), "%I", t);
    text_layer_set_text(hour_text_layer, buffer);
    //if (prevHour < 0) prevHour = 11;
  }
  
  //text_layer_set_text_color(last_hour_text_layer, textColor);
  text_layer_set_text_color(hour_text_layer, textColor);
  
  //snprintf(buffer_prev, sizeof(buffer_prev), "%d", prevHour);
  //text_layer_set_text(last_hour_text_layer, buffer_prev);
  
  GPoint auxPoint = ANALOG_BG_HOUR_POINTS[hour % 12];
  layer_set_frame(text_layer_get_layer(hour_text_layer), GRect(auxPoint.x - (TEXT_WIDTH / 2), auxPoint.y - (TEXT_HEIGHT / 2) + TEXT_Y_OFFSET, TEXT_WIDTH, TEXT_HEIGHT));
  //auxPoint = ANALOG_BG_HOUR_POINTS[prevHour % 12];
  //layer_set_frame(text_layer_get_layer(last_hour_text_layer), GRect(auxPoint.x - (TEXT_WIDTH / 2), auxPoint.y - (TEXT_HEIGHT / 2) + TEXT_Y_OFFSET, TEXT_WIDTH, TEXT_HEIGHT));  
  
  //Upate background
  graphics_context_set_fill_color(ctx, bgCircleFill);
  graphics_context_set_stroke_color(ctx, bgCircleStroke);
  #ifdef PBL_COLOR
    graphics_context_set_stroke_width(ctx, 4);
  #endif
  graphics_fill_circle(ctx, center, CIRCLE_RADIUS);
  graphics_draw_circle(ctx, center, CIRCLE_RADIUS);
  #ifdef PBL_COLOR
    graphics_context_set_stroke_width(ctx, 1);
  #endif
  
  current_angle = 30 * ((hour) % 12);
  
  if(current_angle > timer_angle) {
    timer_angle = (timer_angle + 2) % 360;
    gpath_rotate_to(window_path, (timer_angle * TRIG_MAX_ANGLE) / 360);
    graphics_context_set_fill_color(ctx, windowFill);
    graphics_context_set_stroke_color(ctx, windowStroke);
    gpath_draw_filled(ctx, window_path);
    gpath_draw_outline(ctx, window_path);
    //Register next execution
    timer = app_timer_register(timer_delay, (AppTimerCallback) timer_callback, NULL);
  } else {
    timer_angle = current_angle;
    gpath_rotate_to(window_path, (current_angle * TRIG_MAX_ANGLE) / 360);
    graphics_context_set_fill_color(ctx, windowFill);
    graphics_context_set_stroke_color(ctx, windowStroke);
    gpath_draw_filled(ctx, window_path);
    gpath_draw_outline(ctx, window_path);
  }
  
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
    char str_color1[7];
		char str_color2[7];
    char str_color3[7];

		if (strlen(basalt_colors) >= EDITABLE_COLORS_LENGTH - 1) {
			memcpy(str_color1, &basalt_colors[0], 6);
			memcpy(str_color2, &basalt_colors[6], 6);
      memcpy(str_color3, &basalt_colors[12], 6);
			str_color1[6] = '\0';
			str_color2[6] = '\0';
      str_color3[6] = '\0';
		}
		else return;

		int bgColorInt = HexStringToUInt(str_color1);
		int diskColorInt = HexStringToUInt(str_color2);
    int minColorInt = HexStringToUInt(str_color3);
  
    APP_LOG(APP_LOG_LEVEL_DEBUG, "background color \"%d\"", bgColorInt);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "disk color \"%d\"", diskColorInt);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "min color \"%d\"", minColorInt);
    
    minuteHandFill = GColorFromHEX(minColorInt);
    minuteHandStroke = GColorFromHEX(minColorInt);
    middleDotColor = GColorFromHEX(diskColorInt);
    bgCircleFill = GColorFromHEX(diskColorInt);
    bgCircleStroke = GColorFromHEX(diskColorInt);
    bgColor = GColorFromHEX(bgColorInt);
    textColor = GColorFromHEX(diskColorInt);
    windowFill = GColorFromHEX(bgColorInt);
    windowStroke = GColorLightGray;
    
    window_set_background_color(window, bgColor);
  }
#else
  static void apply_colors(int theme, int min_color, int hour_color) {
    if(min_color == 0) { // Black
      minuteHandFill = GColorBlack;
      minuteHandStroke = GColorWhite;
      middleDotColor = GColorWhite;
    } else if (min_color == 1) { // White
      minuteHandFill = GColorWhite;
      minuteHandStroke = GColorBlack;
      middleDotColor = GColorBlack;
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
      bgCircleFill = GColorBlack;
      bgCircleStroke = GColorClear;
      bgColor = GColorWhite;
    } else if(theme == INVERTED_COLORS) {
      bgCircleFill = GColorWhite;
      bgCircleStroke = GColorClear;
      bgColor = GColorBlack;
    } else if(theme == SAME_COLORS_BLACK_CIRCLE) {
      bgCircleFill = GColorBlack;
      bgCircleStroke = GColorWhite;
      bgColor = GColorBlack;
    } else if(theme == SAME_COLORS_WHITE_CIRCLE) {
      bgCircleFill = GColorWhite;
      bgCircleStroke = GColorBlack;
      bgColor = GColorWhite;
    } else if(theme == SAME_COLORS_BLACK) {
      bgCircleFill = GColorBlack;
      bgCircleStroke = GColorClear;
      bgColor = GColorBlack;
    } else if(theme == SAME_COLORS_WHITE) {
      bgCircleFill = GColorWhite;
      bgCircleStroke = GColorClear;
      bgColor = GColorWhite;
    }
  
    window_set_background_color(window, bgColor);
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
    case KEY_BT_SIGNAL:
      persist_write_int(KEY_BT_SIGNAL, t->value->int16);
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
  layer_mark_dirty(minute_hand_layer);
  layer_mark_dirty(background_layer);
}

static void window_load(Window *window) {
  #ifdef PBL_COLOR
		if (persist_exists(KEY_COLORS_BASALT))
			persist_read_string(KEY_COLORS_BASALT, basalt_colors, EDITABLE_COLORS_LENGTH);
		else
			memcpy(basalt_colors, "FFFFFF555555AA0000"+'\0', EDITABLE_COLORS_LENGTH);
    apply_colors();
	#else
		int aplite_theme = persist_exists(KEY_THEME_APLITE) ? persist_read_int(KEY_THEME_APLITE) : ORIGINAL_COLORS;
    int min_color_aplite = persist_exists(KEY_MIN_COLOR_APLITE) ? persist_read_int(KEY_MIN_COLOR_APLITE) : 1;
    int hour_color_aplite = persist_exists(KEY_HOUR_COLOR_APLITE) ? persist_read_int(KEY_HOUR_COLOR_APLITE) : 1;
    apply_colors(aplite_theme, min_color_aplite, hour_color_aplite);
	#endif
    
  Layer *window_layer = window_get_root_layer(window);
  window_set_background_color(window, bgColor);
  GRect bounds = layer_get_bounds(window_layer);
  center = grect_center_point(&bounds);
  
  gpath_move_to(window_path, center);
  gpath_move_to(minute_arrow, center);

  background_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(background_layer, update_window_proc);
  layer_add_child(window_layer, background_layer);
  
  hour_text_layer = text_layer_create(GRect(0, 0, TEXT_WIDTH, TEXT_HEIGHT));
  text_layer_set_background_color(hour_text_layer, GColorClear);
  text_layer_set_text_color(hour_text_layer, textColor);
  text_layer_set_text_alignment(hour_text_layer, GTextAlignmentCenter);
  text_layer_set_font(hour_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  
  //Needed if at some point the movement of the window is animated and both hours are shown at the same time
  /*last_hour_text_layer = text_layer_create(GRect(0, 0, TEXT_WIDTH, TEXT_HEIGHT));
  text_layer_set_background_color(last_hour_text_layer, GColorClear);
  text_layer_set_text_color(last_hour_text_layer, textColor);
  text_layer_set_text_alignment(last_hour_text_layer, GTextAlignmentCenter);
  text_layer_set_font(last_hour_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  */
  layer_add_child(window_layer, text_layer_get_layer(hour_text_layer));
  //layer_add_child(window_layer, text_layer_get_layer(last_hour_text_layer));
  
  minute_hand_layer = layer_create(bounds);
  layer_set_update_proc(minute_hand_layer, update_minute_hand_proc);
  layer_add_child(window_layer, minute_hand_layer);
}

static void window_unload(Window *window) {
  layer_destroy(minute_hand_layer);
  layer_destroy(background_layer);
  
  text_layer_destroy(hour_text_layer);
  //text_layer_destroy(last_hour_text_layer);
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
  
  GPathBuilder *builder = gpath_builder_create(MAX_POINTS);
  gpath_builder_move_to_point(builder, WINDOW_POINTS[0]);
  gpath_builder_line_to_point(builder, WINDOW_POINTS[1]);
  gpath_builder_curve_to_point(builder, WINDOW_POINTS[4], WINDOW_POINTS[2], WINDOW_POINTS[3]);
  gpath_builder_line_to_point(builder, WINDOW_POINTS[0]);
  window_path = gpath_builder_create_path(builder);
  gpath_builder_destroy(builder);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, minute_tick_handler);
}

void handle_deinit(void) {
  window_destroy(window);
  gpath_destroy(minute_arrow);
  gpath_destroy(window_path);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}
