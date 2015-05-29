#include <pebble.h>
#include <ctype.h>
#include "gpath_builder.h"
#include "hour_window.h"
#include "pebble-assist.h"

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

int bluetooth_vibrate = 2;
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

static GBitmap *bt_disc_bm;
static GBitmap *bt_conn_bm;
static BitmapLayer *bt_status_conn_layer;
static BitmapLayer *bt_status_disc_layer;

AppTimer *timer;
const int timer_delay = 40;
int current_angle = 0;
int timer_angle = 360;

static void minute_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  LOG("Tick handler: minute");
  layer_mark_dirty(minute_hand_layer);
}

void timer_callback(void *data) {
  layer_mark_dirty(background_layer);
  layer_mark_dirty(blocker_layer);
}

static void bt_handler(bool connected) {
  LOG("BlueTooth service changed, connected: %d", connected);
  DEBUG("BlueTooth mode: %d", bluetooth_mode);
  if(connected == true) {
    if(bluetooth_vibrate == BT_VIBRATE_ON){
      vibes_short_pulse();
    }
    
    switch(bluetooth_mode) {
      case BT_CHANGE_BG:
        bgColor = bluetoothConnectedColor;
        #ifdef PBL_COLOR
          windowFill = bluetoothConnectedColor;
        #endif
        window_set_background_color(window, bgColor);
        layer_mark_dirty(background_layer);
        break;
      case BT_CHANGE_DISC_FILL:
        discFill = bluetoothConnectedColor;
        layer_mark_dirty(background_layer);
        break;
      case BT_CHANGE_DISC_STROKE:
        discStroke = bluetoothConnectedColor;
        layer_mark_dirty(background_layer);
        break;
      case BT_CHANGE_MIN_HANDLE_FILL:
        minuteHandFill = bluetoothConnectedColor;
        #ifdef PBL_BW
          minuteHandStroke = bluetoothDisconnectedColor;
          middleDotColor = bluetoothDisconnectedColor;
        #endif
        layer_mark_dirty(minute_hand_layer);
        break;
      case BT_CHANGE_MIN_HANDLE_STROKE:
        minuteHandStroke = bluetoothConnectedColor;
        #ifdef PBL_BW
          minuteHandFill = bluetoothDisconnectedColor;
          middleDotColor = bluetoothDisconnectedColor;
        #endif
        layer_mark_dirty(minute_hand_layer);
        break;
      case BT_ICON:
        layer_show(bt_status_conn_layer);
        layer_hide(bt_status_disc_layer);
        break;
      case BT_ICON_ONLY_DISC:
        layer_hide(bt_status_conn_layer);
        layer_hide(bt_status_disc_layer);
        break;
      default:
        //Nothing to do, no notification
        break;
    }
  }
  else {
    if(bluetooth_vibrate == BT_VIBRATE_ON || bluetooth_vibrate == BT_VIBRATE_ON_DISC){
      vibes_double_pulse();
    }
    
    switch(bluetooth_mode) {
      case BT_CHANGE_BG:
        bgColor = bluetoothDisconnectedColor;
        #ifdef PBL_COLOR
          windowFill = bluetoothDisconnectedColor;
        #endif
        window_set_background_color(window, bgColor);
        layer_mark_dirty(background_layer);
        break;
      case BT_CHANGE_DISC_FILL:
        discFill = bluetoothDisconnectedColor;
        layer_mark_dirty(background_layer);
        break;
      case BT_CHANGE_DISC_STROKE:
        discStroke = bluetoothDisconnectedColor;
        layer_mark_dirty(background_layer);
        break;
      case BT_CHANGE_MIN_HANDLE_FILL:
        minuteHandFill = bluetoothDisconnectedColor;
        #ifdef PBL_BW
          minuteHandStroke = bluetoothConnectedColor;
          middleDotColor = bluetoothConnectedColor;
        #endif
        layer_mark_dirty(minute_hand_layer);
        break;
      case BT_CHANGE_MIN_HANDLE_STROKE:
        minuteHandStroke = bluetoothDisconnectedColor;
        #ifdef PBL_BW
          minuteHandFill = bluetoothConnectedColor;
          middleDotColor = bluetoothConnectedColor;
        #endif
        layer_mark_dirty(minute_hand_layer);
        break;
      case BT_ICON:
      case BT_ICON_ONLY_DISC:
        layer_show(bt_status_disc_layer);
        layer_hide(bt_status_conn_layer);
        break;
      default:
        //Nothing to do, no notification
        break;
    }
  }
}

static void update_window_proc(Layer *layer, GContext *ctx) {
  LOG("Updating Window layer");
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  static char buffer[] = "00";
  static char buffer_prev[] = "00";
  
  int hour = t->tm_hour;
  int prevHour = hour - 1;
  //Update hour text
  if(clock_is_24h_style() == true) {
    print_time(buffer, "%H", t);
    text_layer_set_text(hour_text_layer, buffer);
    if (prevHour < 0) prevHour = 23;
  } else {
    print_time(buffer, "%I", t);
    text_layer_set_text(hour_text_layer, buffer);
    if (prevHour < 0) prevHour = 11;
  }
  
  print_int(buffer_prev, "%d", prevHour);
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
  LOG("Updating Blockers layer");
  gpath_rotate_to(blocker_path, (((timer_angle + 30) % 360) * TRIG_MAX_ANGLE) / 360);
  graphics_context_set_fill_color(ctx, discFill);
  gpath_draw_filled(ctx, blocker_path);

  gpath_rotate_to(blocker_path, (((timer_angle - 30) % 360) * TRIG_MAX_ANGLE) / 360);
  graphics_context_set_fill_color(ctx, discFill);
  gpath_draw_filled(ctx, blocker_path);
}

static void update_minute_hand_proc(Layer *layer, GContext *ctx) {
  LOG("Updating minute hand layer");
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
    LOG("Applying change in colors (TIME)");
    int string_num = (EDITABLE_COLORS_LENGTH - 1) / 6;
    char color_strings[string_num][7];
    int colorInts[string_num];

		if (strlen(basalt_colors) >= EDITABLE_COLORS_LENGTH - 1) {
      for (int i = 0; i < string_num; i++) {
        memcpy(color_strings[i], &basalt_colors[6*i], 6);
        color_strings[i][6] = '\0';
        colorInts[i] = HexStringToUInt(color_strings[i]);
      }
		}
		else return;
    
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
    
  }
#else
  static void apply_colors(int theme, int min_color, int hour_color) {
    LOG("Applying change in colors (OG)");
    if(min_color == 0) { // Black
      minuteHandFill = GColorBlack;
      minuteHandStroke = GColorWhite;
      middleDotColor = GColorWhite;
      if (bluetooth_mode == BT_CHANGE_MIN_HANDLE_FILL) {
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      }
    } else if (min_color == 1) { // White
      minuteHandFill = GColorWhite;
      minuteHandStroke = GColorBlack;
      middleDotColor = GColorBlack;
      if (bluetooth_mode == BT_CHANGE_MIN_HANDLE_FILL) {
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
      } else if (bluetooth_mode == BT_CHANGE_DISC_FILL) {
        discStroke = GColorBlack;
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      } else if (bluetooth_mode == BT_ICON || bluetooth_mode == BT_ICON_ONLY_DISC) {
        bitmap_layer_set_compositing_mode(bt_status_conn_layer, GCompOpAssignInverted);
        bitmap_layer_set_compositing_mode(bt_status_disc_layer, GCompOpAssignInverted);
      }
    } else if(theme == INVERTED_COLORS) {
      discFill = GColorWhite;
      discStroke = GColorWhite;
      bgColor = GColorBlack;
      if (bluetooth_mode == BT_CHANGE_BG) {
        discStroke = GColorBlack;
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      } else if (bluetooth_mode == BT_CHANGE_DISC_FILL) {
        discStroke = GColorWhite;
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      } else if (bluetooth_mode == BT_ICON || bluetooth_mode == BT_ICON_ONLY_DISC) {
        bitmap_layer_set_compositing_mode(bt_status_conn_layer, GCompOpAssign);
        bitmap_layer_set_compositing_mode(bt_status_disc_layer, GCompOpAssign);
      }
    } else if(theme == SAME_COLORS_BLACK_CIRCLE) {
      discFill = GColorBlack;
      discStroke = GColorWhite;
      bgColor = GColorBlack;
      if (bluetooth_mode == BT_CHANGE_BG) {
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      } else if (bluetooth_mode == BT_CHANGE_DISC_FILL) {
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      } else if (bluetooth_mode == BT_ICON || bluetooth_mode == BT_ICON_ONLY_DISC) {
        bitmap_layer_set_compositing_mode(bt_status_conn_layer, GCompOpAssign);
        bitmap_layer_set_compositing_mode(bt_status_disc_layer, GCompOpAssign);
      }
    } else if(theme == SAME_COLORS_WHITE_CIRCLE) {
      discFill = GColorWhite;
      discStroke = GColorBlack;
      bgColor = GColorWhite;
      if (bluetooth_mode == BT_CHANGE_BG) {
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      } else if (bluetooth_mode == BT_CHANGE_DISC_FILL) {
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      } else if (bluetooth_mode == BT_ICON || bluetooth_mode == BT_ICON_ONLY_DISC) {
        bitmap_layer_set_compositing_mode(bt_status_conn_layer, GCompOpAssignInverted);
        bitmap_layer_set_compositing_mode(bt_status_disc_layer, GCompOpAssignInverted);
      }
    } else if(theme == SAME_COLORS_BLACK) {
      discFill = GColorBlack;
      discStroke = GColorBlack;
      bgColor = GColorBlack;
      if (bluetooth_mode == BT_CHANGE_BG) {
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      } else if (bluetooth_mode == BT_CHANGE_DISC_FILL) {
        bluetoothConnectedColor = GColorBlack;
        bluetoothDisconnectedColor = GColorWhite;
      } else if (bluetooth_mode == BT_ICON || bluetooth_mode == BT_ICON_ONLY_DISC) {
        bitmap_layer_set_compositing_mode(bt_status_conn_layer, GCompOpAssign);
        bitmap_layer_set_compositing_mode(bt_status_disc_layer, GCompOpAssign);
      }
    } else if(theme == SAME_COLORS_WHITE) {
      discFill = GColorWhite;
      discStroke = GColorWhite;
      bgColor = GColorWhite;
      if (bluetooth_mode == BT_CHANGE_BG) {
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      } else if (bluetooth_mode == BT_CHANGE_DISC_FILL) {
        bluetoothConnectedColor = GColorWhite;
        bluetoothDisconnectedColor = GColorBlack;
      } else if (bluetooth_mode == BT_ICON || bluetooth_mode == BT_ICON_ONLY_DISC) {
        bitmap_layer_set_compositing_mode(bt_status_conn_layer, GCompOpAssignInverted);
        bitmap_layer_set_compositing_mode(bt_status_disc_layer, GCompOpAssignInverted);
      }
    }
  }
#endif

static void in_recv_handler(DictionaryIterator *iterator, void *context) {
  LOG("JS message received");
  Tuple *t = dict_read_first(iterator);
  int new_bt_mode = -1;
  int new_bt_vibrate = -1;
  #ifdef PBL_COLOR
    char new_colors[EDITABLE_COLORS_LENGTH];
  #else
    int new_theme = -1;
    int new_min_hand_color = -1;
    int new_hour_color = -1;
  #endif
  while(t) {
    switch (t->key) {
    
    case KEY_BT_VIBRATE:
      new_bt_vibrate = t->value->int16;
      if (new_bt_vibrate > -1 && new_bt_vibrate <= 2 &&
          (new_bt_vibrate != bluetooth_vibrate || !persist_exists(KEY_BT_VIBRATE))) {
        bluetooth_vibrate = new_bt_vibrate;
        persist_write_int(KEY_BT_VIBRATE, new_bt_vibrate);
      }
      break;
    case KEY_BT_SIGNAL:
      new_bt_mode = t->value->int16;
      if (new_bt_mode > -1 && new_bt_mode <= 7 &&
          (new_bt_mode != bluetooth_mode || !persist_exists(KEY_BT_SIGNAL))) {
        bluetooth_mode = new_bt_mode;
        persist_write_int(KEY_BT_SIGNAL, new_bt_mode);
      }
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
  LOG("Window loading");
  bluetooth_vibrate = persist_read_int_safe(KEY_BT_VIBRATE, BT_VIBRATE_ON_DISC);
  bluetooth_mode = persist_read_int_safe(KEY_BT_SIGNAL, BT_CHANGE_BG);
  #ifdef PBL_COLOR
    memcpy(basalt_colors, "FFFFFF000000555555AAAAAAFFFFFFFF0000FF000055FF55FF5555"+'\0', EDITABLE_COLORS_LENGTH);
		persist_read_string(KEY_COLORS_BASALT, basalt_colors, EDITABLE_COLORS_LENGTH);
    apply_colors();
	#else
		int aplite_theme = persist_read_int_safe(KEY_THEME_APLITE, ORIGINAL_COLORS);
    int min_color_aplite = persist_read_int_safe(KEY_MIN_COLOR_APLITE, 1);
    int hour_color_aplite = persist_read_int_safe(KEY_HOUR_COLOR_APLITE, 1);
    apply_colors(aplite_theme, min_color_aplite, hour_color_aplite);
	#endif
  
  window_set_background_color(window, bgColor);
  GRect bounds = window_get_bounds(window);
  center = grect_center_point(&bounds);
  
  gpath_move_to(window_path, center);
  gpath_move_to(blocker_path, center);
  gpath_move_to(minute_arrow, center);

  background_layer = layer_create(bounds);
  layer_set_update_proc(background_layer, update_window_proc);
  layer_add_to_window(background_layer, window);
  
  bt_disc_bm = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_DISCONNECTED_ICON);
  bt_conn_bm = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_CONNECTED_ICON);
  bt_status_conn_layer = bitmap_layer_create(GRect(2, 2, BT_ICON_WIDTH, BT_ICON_HEIGHT));
  bt_status_disc_layer = bitmap_layer_create(GRect(2, 2, BT_ICON_WIDTH, BT_ICON_HEIGHT));
#ifdef PBL_COLOR
  bitmap_layer_set_transparent(bt_status_conn_layer);
  bitmap_layer_set_transparent(bt_status_disc_layer);
#endif
  bitmap_layer_set_bitmap(bt_status_conn_layer, bt_conn_bm);
  bitmap_layer_set_bitmap(bt_status_disc_layer, bt_disc_bm);
  bitmap_layer_add_to_layer(bt_status_conn_layer, background_layer);
  bitmap_layer_add_to_layer(bt_status_disc_layer, background_layer);
  layer_hide(bt_status_disc_layer);
  layer_hide(bt_status_conn_layer);
  
  hour_text_layer = text_layer_create(GRect(0, 0, TEXT_WIDTH, TEXT_HEIGHT));
  text_layer_set_colors(hour_text_layer, textColor, GColorClear);
  text_layer_set_text_alignment(hour_text_layer, GTextAlignmentCenter);
  text_layer_set_system_font(hour_text_layer, FONT_KEY_GOTHIC_18);
  
  last_hour_text_layer = text_layer_create(GRect(0, 0, TEXT_WIDTH, TEXT_HEIGHT));
  text_layer_set_colors(last_hour_text_layer, textColor, GColorClear);
  text_layer_set_text_alignment(last_hour_text_layer, GTextAlignmentCenter);
  text_layer_set_system_font(last_hour_text_layer, FONT_KEY_GOTHIC_18);
  
  text_layer_add_to_window(hour_text_layer, window);
  text_layer_add_to_window(last_hour_text_layer, window);
  
  blocker_layer = layer_create(bounds);
  layer_set_update_proc(blocker_layer, update_blockers_proc);
  layer_add_to_window(blocker_layer, window);
  
  minute_hand_layer = layer_create(bounds);
  layer_set_update_proc(minute_hand_layer, update_minute_hand_proc);
  layer_add_to_window(minute_hand_layer, window);
  
  bt_handler(bluetooth_connection_service_peek());
}

static void window_unload(Window *window) {
  LOG("Window unloading");
  layer_destroy_safe(minute_hand_layer);
  layer_destroy_safe(background_layer);
  layer_destroy_safe(blocker_layer);
  
  bitmap_layer_destroy_safe(bt_status_conn_layer);
  bitmap_layer_destroy_safe(bt_status_disc_layer);
  gbitmap_destroy_safe(bt_disc_bm);
  gbitmap_destroy_safe(bt_conn_bm);
  
  text_layer_destroy_safe(hour_text_layer);
  text_layer_destroy_safe(last_hour_text_layer);
}

void handle_init(void) {
  LOG("INIT");
  window = window_create();
  if(NULL == window) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Window creation failed!! :O");
    return;
  }
  window_handlers(window, window_load, window_unload);
  
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open_max();
  
  minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  
  //Create the opening for the hour
  GPathBuilder *builder = gpath_builder_create(MAX_POINTS);
  gpath_builder_move_to_point(builder, WINDOW_POINTS[0]);
  gpath_builder_line_to_point(builder, WINDOW_POINTS[1]);
  gpath_builder_curve_to_point(builder, WINDOW_POINTS[4], WINDOW_POINTS[2], WINDOW_POINTS[3]);
  gpath_builder_line_to_point(builder, WINDOW_POINTS[0]);
  window_path = gpath_builder_create_path(builder);
  
  //Create the blocker for the extra text in the animations
  gpath_builder_move_to_point(builder, HOUR_BLOCKER[0]);
  gpath_builder_line_to_point(builder, HOUR_BLOCKER[1]);
  gpath_builder_curve_to_point(builder, HOUR_BLOCKER[4], HOUR_BLOCKER[2], HOUR_BLOCKER[3]);
  gpath_builder_line_to_point(builder, HOUR_BLOCKER[0]);
  blocker_path = gpath_builder_create_path(builder);
  gpath_builder_destroy(builder);
  
  // Bluetooth subscription
  bluetooth_connection_service_subscribe(bt_handler);
  
  // Show the Window on the watch, with animated=true
  window_stack_push(window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, minute_tick_handler);
}

void handle_deinit(void) {
  LOG("DEINIT");
  window_destroy_safe(window);
  gpath_destroy_safe(minute_arrow);
  gpath_destroy_safe(window_path);
  gpath_destroy_safe(blocker_path);
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}