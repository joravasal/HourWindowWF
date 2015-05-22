var aplite_theme = 0;
var aplite_hour_color = 1;
var aplite_min_color = 1;

var basalt_colors = -1;
var basalt_bgBTSignal = 0;
var basalt_minBatterySignal = 0;

function getItem(reference) {
	var item = localStorage.getItem(reference);
	return item;
}

function setItem(reference, item) {
	localStorage.setItem(reference ,item);
}

function loadLocalVariables() {
	aplite_theme = parseInt(getItem("theme_aplite"));
  aplite_hour_color = parseInt(getItem("hour_color_aplite"));
  aplite_min_color = parseInt(getItem("min_color_aplite"));
  
	basalt_colors = getItem("basalt_colors");
  basalt_bgBTSignal = parseInt(getItem("basalt_bgBTsignal"));
  basalt_minBatterySignal = parseInt(getItem("basalt_minBatterySignal"));
	
	aplite_theme = !aplite_theme ? 0 : aplite_theme;
  aplite_hour_color = !aplite_hour_color ? 1 : aplite_hour_color;
  aplite_min_color = !aplite_min_color ? 1 : aplite_min_color;
  
	basalt_colors = !basalt_colors ? "FFFFFF000000555555AAAAAAFFFFFFAA0000FF0000" : basalt_colors;
  basalt_bgBTSignal = !basalt_bgBTSignal ? 1 : basalt_bgBTSignal;
  basalt_minBatterySignal = !basalt_minBatterySignal ? 1 : basalt_minBatterySignal;
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
    var url = 'https://dl.dropboxusercontent.com/u/3223915/Pebble_config_pages/hourwindow_config_test.html'+
		'?watch_version='+getWatchVersion();
    if (getWatchVersion() < 3) {
      url = url + '&theme_aplite=' + aplite_theme +
          '&hour_color_aplite=' + aplite_hour_color +
          '&min_color_aplite=' + aplite_min_color;
    } else {
      url = url +	'&basalt_colors=' + basalt_colors +
          '&basalt_bgBTSignal=' + basalt_bgBTSignal +
          '&basalt_minBatterySignal=' + basalt_minBatterySignal;
    }
		
    Pebble.openURL(url);
  }
); 

Pebble.addEventListener("webviewclosed",
  function(e) {
    //Get JSON dictionary
    var configuration = JSON.parse(decodeURIComponent(e.response));
    console.log("Configuration window returned: " + JSON.stringify(configuration));
 
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
    
    if(configuration.KEY_COLORS_BASALT && configuration.KEY_COLORS_BASALT.length == 42 && configuration.KEY_COLORS_BASALT.localeCompare(basalt_colors) !== 0) {
      basalt_colors = configuration.KEY_COLORS_BASALT;
      setItem("basalt_colors", basalt_colors);
    }
    if(!isNaN(configuration.KEY_BT_SIGNAL) && configuration.KEY_BT_SIGNAL != basalt_bgBTSignal) {
      basalt_bgBTSignal = configuration.KEY_BT_SIGNAL;
      setItem("basalt_bgBTsignal", basalt_bgBTSignal);
    }
    if(!isNaN(configuration.KEY_BATTERY_SIGNAL) && configuration.KEY_BATTERY_SIGNAL != basalt_minBatterySignal) {
      basalt_minBatterySignal = configuration.KEY_BATTERY_SIGNAL;
      setItem("basalt_minBatterySignal", basalt_minBatterySignal);
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
		var watch_name = Pebble.getActiveWatchInfo().model;

		if (watch_name.indexOf("pebble_time_steel") >= 0) {
			watch_version = 4;
		} else if (watch_name.indexOf("pebble_time") >= 0) {
			watch_version = 3;
		} else if (watch_name.indexOf("qemu_platform_basalt") >= 0) {
			watch_version = 3;
		} else if (watch_name.indexOf("pebble_steel") >= 0) {
			watch_version = 2;
		}
	}
	
	return watch_version;
}