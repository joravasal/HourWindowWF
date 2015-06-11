var aplite_theme = 0;
var aplite_hour_color = 1;
var aplite_min_color = 1;

var basalt_colors = -1;

var bt_signal = 2;
var bt_vibrate = 2;

var battery_mode = 1;
var battery_below_level = 40;
var battery_color_scheme = 3;

function getItem(reference) {
	var item = localStorage.getItem(reference);
	return item;
}

function setItem(reference, item) {
	localStorage.setItem(reference ,item);
}

function loadLocalVariables() {
  bt_signal = parseInt(getItem("bt_signal"));
  var def_bt_signal = 2;
  if(getWatchVersion() >= 3) {
    def_bt_signal = 3;
  }
  bt_signal = !bt_signal ? def_bt_signal : bt_signal;
  
  bt_vibrate = parseInt(getItem("bt_vibrate"));
  bt_vibrate = !bt_vibrate ? 2 : bt_vibrate;
  
  battery_mode = parseInt(getItem("battery_mode"));
  var def_batt_signal = 1;
  if(getWatchVersion() >= 3) {
    def_batt_signal = 6;
  }
  battery_mode = !battery_mode ? def_batt_signal : battery_mode;
  
  battery_below_level = parseInt(getItem("battery_below_level"));
  battery_below_level = !battery_below_level ? 40 : battery_below_level;
  
  battery_color_scheme = parseInt(getItem("battery_color_scheme"));
  battery_color_scheme = !battery_color_scheme ? 3 : battery_color_scheme;
  
	aplite_theme = parseInt(getItem("theme_aplite"));
  aplite_hour_color = parseInt(getItem("hour_color_aplite"));
  aplite_min_color = parseInt(getItem("min_color_aplite"));
	
	aplite_theme = !aplite_theme ? 0 : aplite_theme;
  aplite_hour_color = !aplite_hour_color ? 1 : aplite_hour_color;
  aplite_min_color = !aplite_min_color ? 1 : aplite_min_color;
  
	basalt_colors = getItem("basalt_colors");
	basalt_colors = !basalt_colors ? "000000FFFFFF00550055AA00FFFFFFFFFFFFAAAAAA55AA00555500" : basalt_colors;
}

Pebble.addEventListener("ready",
  function(e) {
    console.log("PebbleKit JS ready!");
    loadLocalVariables();
  }
);

Pebble.addEventListener("showConfiguration",
  function(e) {
    //Load the remote config page
    var url = 'https://dl.dropboxusercontent.com/u/3223915/Pebble_config_pages/hourwindow_config_1.3.html' +
        '?watch_version=' + getWatchVersion() + '&btSignal=' + bt_signal + '&btVibrate=' + bt_vibrate +
        '&bat_mode=' + battery_mode + '&bat_below_level=' + battery_below_level;
    if (getWatchVersion() < 3) {
      url = url + '&theme_aplite=' + aplite_theme +
          '&hour_color_aplite=' + aplite_hour_color +
          '&min_color_aplite=' + aplite_min_color;
    } else {
      url = url  + '&bat_color_scheme=' + battery_color_scheme + 
          '&basalt_colors=' + basalt_colors;
    }
		
    Pebble.openURL(url);
  }
); 

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
    
    if(!isNaN(configuration.KEY_BT_SIGNAL) && configuration.KEY_BT_SIGNAL != bt_signal) {
      bt_signal = configuration.KEY_BT_SIGNAL;
      setItem("bt_signal", bt_signal);
    }
    
    if(!isNaN(configuration.KEY_BT_VIBRATE) && configuration.KEY_BT_VIBRATE != bt_vibrate) {
      bt_vibrate = configuration.KEY_BT_VIBRATE;
      setItem("bt_vibrate", bt_vibrate);
    }
    
    if(!isNaN(configuration.KEY_BATTERY_SIGNAL) && configuration.KEY_BATTERY_SIGNAL != battery_mode) {
      battery_mode = configuration.KEY_BATTERY_SIGNAL;
      setItem("battery_mode", battery_mode);
    }
    
    if(!isNaN(configuration.KEY_BATTERY_BELOW_LEVEL) && configuration.KEY_BATTERY_BELOW_LEVEL != battery_below_level) {
      battery_below_level = configuration.KEY_BATTERY_BELOW_LEVEL;
      setItem("battery_below_level", battery_below_level);
    }
    
    if(!isNaN(configuration.KEY_BATTERY_COLOR_SCHEME) && configuration.KEY_BATTERY_COLOR_SCHEME != battery_color_scheme) {
      battery_color_scheme = configuration.KEY_BATTERY_COLOR_SCHEME;
      setItem("battery_color_scheme", battery_color_scheme);
    }
 
    if(!isNaN(configuration.KEY_THEME_APLITE) && configuration.KEY_THEME_APLITE != aplite_theme) {
      aplite_theme = configuration.KEY_THEME_APLITE;
      setItem("theme_aplite", aplite_theme);
    }
    if(!isNaN(configuration.KEY_HOUR_COLOR_APLITE) && configuration.KEY_HOUR_COLOR_APLITE != aplite_hour_color) {
      aplite_hour_color = configuration.KEY_HOUR_COLOR_APLITE;
      setItem("hour_color_aplite", aplite_hour_color);
    }
    if(!isNaN(configuration.KEY_MIN_COLOR_APLITE) && configuration.KEY_MIN_COLOR_APLITE != aplite_min_color) {
      aplite_min_color = configuration.KEY_MIN_COLOR_APLITE;
      setItem("min_color_aplite", aplite_min_color);
    }
    
    if(configuration.KEY_COLORS_BASALT && configuration.KEY_COLORS_BASALT.length == 54 && configuration.KEY_COLORS_BASALT.localeCompare(basalt_colors) !== 0) {
      basalt_colors = configuration.KEY_COLORS_BASALT;
      setItem("basalt_colors", basalt_colors);
    }
    
    //Send to Pebble, persist there
    Pebble.sendAppMessage(
      configuration,
      function(e) {
        console.log("Sending settings data...");
      },
      function(e) {
        console.log("Settings feedback failed!");
      }
    );
  }
);

function getWatchVersion() {
	// 1 = Pebble OG
	// 2 = Pebble Steel
	// 3 = Pebble Time
	// 3 = Pebble Basalt Emulator (currently Pebble Time)
	// 4 = Pebble Time Steel
	
	var watch_version = 1;

	if(Pebble.getActiveWatchInfo) {
		// Available for use!
    try {
      var watch = Pebble.getActiveWatchInfo();
      var watch_name = watch.model;
    
      if (watch_name.indexOf("time_steel") >= 0) {
        watch_version = 4;
      } else if (watch_name.indexOf("time") >= 0) {
        watch_version = 3;
      } else if (watch_name.indexOf("qemu_basalt") >= 0) {
        watch_version = 3;
      } else if (watch_name.indexOf("steel") >= 0) {
        watch_version = 2;
      }
    } catch(err) {
      console.log("getActiveWatchInfo() FAILED!");
      watch_version = 3;
    }
	} else {
    console.log("getActiveWatchInfo is not available");
  }
	
	return watch_version;
}