#pragma once
#include <pebble.h>

// Javascript and local storage keys
#define KEY_THEME_APLITE 0
#define KEY_MIN_COLOR_APLITE 1
#define KEY_HOUR_COLOR_APLITE 2
#define KEY_COLORS_BASALT 3
#define KEY_BT_SIGNAL 4
#define KEY_BATTERY_SIGNAL 5
  
// Each color has a length of 6 plus the ending char
#define EDITABLE_COLORS_LENGTH 43

// Styles for aplite watches
static const int ORIGINAL_COLORS = 0;
static const int INVERTED_COLORS = 1;
static const int SAME_COLORS_BLACK_CIRCLE = 2;
static const int SAME_COLORS_WHITE_CIRCLE = 3;
static const int SAME_COLORS_BLACK = 4;
static const int SAME_COLORS_WHITE = 5;

// Some generic constants for the UI
static const int MAX_POINTS = 256;
static const int TEXT_WIDTH = 22;
static const int TEXT_HEIGHT = 22;
static const int TEXT_Y_OFFSET = 12;
#ifdef PBL_COLOR
static const int CIRCLE_RADIUS = 68;
#else
static const int CIRCLE_RADIUS = 70;
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

