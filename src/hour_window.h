#pragma once
#include <pebble.h>

// Javascript and local storage keys
#define KEY_THEME_APLITE 0
#define KEY_MIN_COLOR_APLITE 1
#define KEY_HOUR_COLOR_APLITE 2
#define KEY_COLORS_BASALT 3
#define KEY_BT_SIGNAL 4
#define KEY_BATTERY_SIGNAL 5
#define KEY_BT_VIBRATE 6
#define KEY_BATTERY_BELOW_LEVEL 7
#define KEY_BATTERY_COLOR_SCHEME 8
#define KEY_SHOW_DATE 9
#define KEY_SHOW_MINUTE_MARKS 10
#define KEY_DATE_FORMAT 11
#define KEY_DATE_POS 12

// Default duration of tap_timer
#define TAP_TIMER_LENGTH_DEF 5000
  
// Max. length of the date format string
#define DATE_FORMAT_MAX_LENGTH 16
  
// Each color has a length of 6 plus the ending char
#define EDITABLE_COLORS_LENGTH 55

// Bluetooth vibration notification
#define BT_VIBRATE_OFF 0
#define BT_VIBRATE_ON 1
#define BT_VIBRATE_ON_DISC 2
  
// Bluetooth modes
#define BT_OFF  0 //no status visible
#define BT_CHANGE_BG 1 //BG color changes depending on the BT status
#define BT_CHANGE_DISC_FILL 2 //The disc fill color changes depending on the BT status
#define BT_CHANGE_DISC_STROKE 3 //The disc stroke color changes depending on the BT status
#define BT_CHANGE_MIN_HANDLE_FILL 4 //The minute hand fill color changes depending on the BT status
#define BT_CHANGE_MIN_HANDLE_STROKE 5 //The minute hand stroke color changes depending on the BT status
#define BT_ICON 6 //An icon shows the BT status
#define BT_ICON_ONLY_DISC 7 //An icon shows the BT status only if it is disconnected
  
// Battery modes
#define BATT_OFF 0 //no battery status visible
#define BATT_NUMBER 1 //Battery level shown with a number on a corner
#define BATT_BAR_TOP 2 //Battery shown with size/color of a bar on top
#define BATT_BAR_BOTTOM 3 //Battery shown with size/color of a bar on bottom
#define BATT_CHANGE_DISC_FILL 4
#define BATT_CHANGE_DISC_STROKE 5
#define BATT_CHANGE_MIN_HANDLE_FILL 6
#define BATT_CHANGE_MIN_HANDLE_SROKE 7
#define BATT_CHANGE_BG 8

// Styles for aplite watches
#define ORIGINAL_COLORS 0 //White background with black disc
#define INVERTED_COLORS 1 //Black background with white disc
#define SAME_COLORS_BLACK_CIRCLE 2 //BLack background with black disc and white circle
#define SAME_COLORS_WHITE_CIRCLE 3 //White background with white disc and black circle
#define SAME_COLORS_BLACK 4 //All black
#define SAME_COLORS_WHITE 5 //All white

// Some generic constants for the UI
#define MAX_POINTS 256
#define TEXT_WIDTH 22
#define TEXT_HEIGHT 22
#define TEXT_Y_OFFSET 12

#define CIRCLE_INNER_RADIUS 67
#define CIRCLE_OUTER_RADIUS 70

#define BT_ICON_WIDTH 24
#define BT_ICON_HEIGHT 32
  
#define BATT_CHARGING_ICON_WIDTH 12
#define BATT_CHARGING_ICON_HEIGHT 16
  
#define BATT_TEXT_WIDTH 38
#define BATT_TEXT_HEIGHT 22

#ifdef PBL_COLOR
  // Colors for battery levels
  static const uint8_t BATTERY_LEVEL_COLORS_CLEAR[11] = {
    GColorRedARGB8, //level 0
    GColorFollyARGB8, //level 10
    GColorSunsetOrangeARGB8, //level 20
    GColorOrangeARGB8, //level 30
    GColorRajahARGB8, //level 40
    GColorIcterineARGB8, //level 50
    GColorYellowARGB8, //level 60
    GColorSpringBudARGB8, //level 70
    GColorBrightGreenARGB8, //level 80
    GColorGreenARGB8, //level 90-100
    GColorBlueARGB8, //charging
  };
  // Colors for battery levels
  static const uint8_t BATTERY_LEVEL_COLORS_DARK[11] = {
    GColorBulgarianRoseARGB8, //level 0
    GColorDarkCandyAppleRedARGB8, //level 10
    GColorRoseValeARGB8, //level 20
    GColorWindsorTanARGB8, //level 30
    GColorWindsorTanARGB8, //level 40
    GColorChromeYellowARGB8, //level 50
    GColorChromeYellowARGB8, //level 60
    GColorLimerickARGB8, //level 70
    GColorArmyGreenARGB8, //level 80
    GColorDarkGreenARGB8, //level 90-100
    GColorOxfordBlueARGB8, //charging
  };
#endif

// Hour text center positions
static const struct GPoint ANALOG_BG_HOUR_POINTS[] = {
  {72, 18},
  {98, 27},
  {116, 47},
  {123, 71},
  {116, 96},
  {98, 115},
  {72, 126},
  {46, 115},
  {28, 96},
  {21, 71},
  {28, 47},
  {46, 27},
};

// Points to create the path for the opening to show the hour
  // Remember to center and rotate
static const struct GPoint WINDOW_POINTS[] = {
  {0, -10}, //Starting point
  {-15, -60}, //First top point
  {-7, -65}, //Auxiliary point for the bezier curve
  {7, -65}, //Auxiliary point for the bezier curve
  {15, -60}, //Second top point
};

// Points to create the path to block from view the auxiliar text
  // Remember to center and rotate
static const struct GPoint HOUR_BLOCKER[] = {
  {0, 10}, //Starting point
  {-18, -62}, //First top point
  {-15, -67}, //Auxiliary point for the bezier curve
  {15, -67}, //Auxiliary point for the bezier curve
  {18, -62}, //Second top point
};

// Points to create the hand for the minutes
  // Remember to center and rotate
static const GPathInfo MINUTE_HAND_POINTS = {
  4,
  (GPoint []) {
    {-5, 0},
    {0, 15},
    {5, 0},
    {0, -55}
  }
};

