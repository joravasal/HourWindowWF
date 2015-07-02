var watch_version = -1;
var COLOR_STRING_LENGTH = 54;
var bluetooth_res = -1;

//Function to convert hex format to a rgb color
function rgb2hex(rgb){
 rgb = rgb.match(/^rgb\((\d+),\s*(\d+),\s*(\d+)\)$/);
 return ("#" +
  ("0" + parseInt(rgb[1],10).toString(16)).slice(-2) +
  ("0" + parseInt(rgb[2],10).toString(16)).slice(-2) +
  ("0" + parseInt(rgb[3],10).toString(16)).slice(-2)).toUpperCase();
}

$(function () {
  $('#show_date').change(function() {
        var show_date = parseInt($("input[name=show_date]:checked").val(), 10);
        if(show_date == 0) {
            $('#date_format').addClass("hidden");
        } else {
            $('#date_format').removeClass("hidden");
        }
  });
});

$(function () {
  $('#bluetooth_signal').change(function() {
        bluetooth_res = parseInt($("input[name=bluetooth_signal]:checked").val(), 10);
        $('#color_affects_aplite').addClass("hidden");
        $('#color_affects_basalt').addClass("hidden");
        $('#bt_color').addClass("hidden");
        if(watch_version >= 3) {
            if(bluetooth_res == 1) {
                $('#bt_color').removeClass("hidden");
                $('#color_affects_basalt').removeClass("hidden");
                bluetooth_res = parseInt($("#color_affects_basalt_select").val(), 10);
                //if the user is changing to this value, we give more relevance to the setting
                //  of the battery, as the color affected by bluetooth is appearing anew
            }
            compare_color_affected(parseInt($("#battery_mode_basalt_select").val(), 10), false);
        } else { //aplite
            if(bluetooth_res == 1) {
                $('#color_affects_aplite').removeClass("hidden");
                bluetooth_res = parseInt($("#color_affects_aplite_select").val(), 10);
            }
        }
  });
});

$(function () {
    $('#battery_mode_aplite_select').change(function() {
        change_battery_mode(parseInt($("#battery_mode_aplite_select").val(), 10));
    });
});
$(function () {
    $('#battery_mode_basalt_select').change(function() {
        change_battery_mode(parseInt($("#battery_mode_basalt_select").val(), 10));
    });
});

function change_battery_mode(mode) {
    $('#battery_show_after_level').addClass("hidden");
    $('#battery_color_type').addClass("hidden");
    if(mode == 1) {
        $('#battery_show_after_level').removeClass("hidden");
    } else if(mode >= 4) {
        $('#battery_color_type').removeClass("hidden");
    }
    compare_color_affected(mode, false);
}

//only called if basalt
function compare_color_affected(mode, is_bluetooth_changing) {
    var mode_aux = bluetooth_res;
    if(is_bluetooth_changing) {
        mode_aux = parseInt($("#battery_mode_basalt_select").val(), 10);
        if(mode == 1 && mode_aux == 8) {
            mode_aux = 4;
            $('#battery_mode_basalt_select').val(mode_aux).selectmenu("refresh");
        } else if ((mode + 2) == mode_aux) {
            mode_aux++;
            $('#battery_mode_basalt_select').val(mode_aux).selectmenu("refresh");
        }
        hide_colors(mode, mode_aux);
    } else {
        if(mode == 8 && mode_aux == 1) {
            $("#color_affects_basalt_select").val(2).selectmenu("refresh");
            bluetooth_res = 2;
        } else if (mode == (mode_aux + 2)) {
            if (mode_aux == 5) mode_aux = 0;
            bluetooth_res = mode_aux + 1;
            $("#color_affects_basalt_select").val(bluetooth_res).selectmenu("refresh");
        }
        hide_colors(bluetooth_res, mode);
    }
}

function hide_colors(bluetooth, battery) {
    $('#bg_color').removeClass("hidden");
    $('#disk_fill_color').removeClass("hidden");
    $('#disk_stroke_color').removeClass("hidden");
    $('#min_fill_color').removeClass("hidden");
    $('#min_stroke_color').removeClass("hidden");
    if(bluetooth == 1) {
        $('#bg_color').addClass("hidden");
    } else if(bluetooth == 2) {
        $('#disk_fill_color').addClass("hidden");
    } else if(bluetooth == 3) {
        $('#disk_stroke_color').addClass("hidden");
    } else if(bluetooth == 4) {
        $('#min_fill_color').addClass("hidden");
    } else if(bluetooth == 5) {
        $('#min_stroke_color').addClass("hidden");
    }
    
    if(battery == 8) {
        $('#bg_color').addClass("hidden");
    } else if (battery == 4) {
        $('#disk_fill_color').addClass("hidden");
    } else if (battery == 5) {
        $('#disk_stroke_color').addClass("hidden");
    } else if (battery == 6) {
        $('#min_fill_color').addClass("hidden");
    } else if (battery == 7) {
        $('#min_stroke_color').addClass("hidden");
    }
}

$(function () {
  $('#color_affects_aplite_select').change(function() {
      bluetooth_res = parseInt($("#color_affects_aplite_select").val(), 10);
  });
});

$(function () {
  $('#color_affects_basalt_select').change(function() {
        bluetooth_res = parseInt($("#color_affects_basalt_select").val(), 10);
        compare_color_affected(bluetooth_res, true);
  });
});

$(function () {
  $('#min_battery_signal').on('click', function (event) {
	if(parseInt($("input[name=min_battery_signal]:checked").val(), 10) == 0)
            $('#min_color').removeClass("hidden");
        else
            $('#min_color').addClass("hidden");
  });
});

$(function () {
  $('#bg_color_box').on('click', function (event) {
	changeColorObject = 0;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#text_color_box').on('click', function (event) {
	changeColorObject = 1;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#disk_fill_color_box').on('click', function (event) {
	changeColorObject = 2;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#disk_stroke_color_box').on('click', function (event) {
	changeColorObject = 3;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#hour_window_color_box').on('click', function (event) {
	changeColorObject = 4;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#min_fill_color_box').on('click', function (event) {
	changeColorObject = 5;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#min_stroke_color_box').on('click', function (event) {
	changeColorObject = 6;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#bt_con_color_color_box').on('click', function (event) {
	changeColorObject = 7;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#bt_dis_color_color_box').on('click', function (event) {
	changeColorObject = 8;
	highlightColorBox($(this));
  });
});

function highlightColorBox($colorButton) {
	selectedColor = rgb2hex($colorButton.css("background-color"));
	
	var i;
	for (i = 1; i <= 13; i++)
	{
		$('#color-row'+i).children().removeClass("selected-color-box");
	}
	$('#color-box-'+selectedColor.substring(1).toUpperCase()).addClass("selected-color-box");
}

$(function () {
  $('#color-picker .color-box').on('click', function (event) {
	var selected_color = rgb2hex($(this).css("background-color"));

	if (changeColorObject == 0)
		$('#bg_color_box').css('background-color', selected_color);
        else if(changeColorObject == 1)
		$('#text_color_box').css('background-color', selected_color);
	else if(changeColorObject == 2)
		$('#disk_fill_color_box').css('background-color', selected_color);
        else if(changeColorObject == 3)
		$('#disk_stroke_color_box').css('background-color', selected_color);
        else if(changeColorObject == 4)
		$('#hour_window_color_box').css('background-color', selected_color);
        else if(changeColorObject == 5)
		$('#min_fill_color_box').css('background-color', selected_color);
        else if(changeColorObject == 6)
                $('#min_stroke_color_box').css('background-color', selected_color);
        else if(changeColorObject == 7)
                $('#bt_con_color_color_box').css('background-color', selected_color);
        else if(changeColorObject == 8)
                $('#bt_dis_color_color_box').css('background-color', selected_color);
	$( "#color-picker" ).popup( "close" );
  });
});

$(function () {
	$("#cancel").click(function() {
            console.log("Cancelling");
            document.location = "pebblejs://close";
	});
});

$(function () {
	$("#save").click(function() {
            console.log("Submit");

            var options = saveOptions();

            var return_to = getQueryParam('return_to', 'pebblejs://close#');
            var location = return_to + encodeURIComponent(JSON.stringify(options));
            console.log("Warping to:");
            console.log(location);
            document.location = location;
	});
});

function queryObj() {
	var result = {}, keyValuePairs = location.search.slice(1).split('&');

	keyValuePairs.forEach(function(keyValuePair) {
		keyValuePair = keyValuePair.split('=');
		result[keyValuePair[0]] = keyValuePair[1] || '';
	});

	return result;
}

function getQueryParam(variable, default_) {
    var query = location.search.substring(1);
    var vars = query.split('&');
    for (var i = 0; i < vars.length; i++) {
        var pair = vars[i].split('=');
        if (pair[0] == variable)
            return decodeURIComponent(pair[1]);
    }
    return default_ || false;
}

function saveOptions() {

	var options = {};
	
        options.KEY_SHOW_MINUTE_MARKS = parseInt($("input[name=show_minute_marks]:checked").val(), 10);
        options.KEY_SHOW_DATE = parseInt($("input[name=show_date]:checked").val(), 10);
        options.KEY_DATE_FORMAT = $("#date_format_input").val();
        options.KEY_DATE_POS = parseInt($("input[name=date_pos]:checked").val(), 10);
        
        options.KEY_BT_SIGNAL = bluetooth_res;
        options.KEY_BT_VIBRATE = parseInt($("input[name=bluetooth_vibrate]:checked").val(), 10);
        if (watch_version < 3){
            options.KEY_THEME_APLITE = parseInt($("#theme_aplite_select").val(), 10);
            options.KEY_HOUR_COLOR_APLITE = parseInt($("input[name=hour_window_color_aplite]:checked").val(), 10);
            options.KEY_MIN_COLOR_APLITE = parseInt($("input[name=minute_hand_color_aplite]:checked").val(), 10);
            options.KEY_BATTERY_SIGNAL = parseInt($("#battery_mode_aplite_select").val(), 10);
        } else {
            var colors = rgb2hex($('#bg_color_box').css("background-color")).replace("#", '');
            colors = colors + rgb2hex($('#text_color_box').css("background-color")).replace("#", '');
            
            colors = colors + rgb2hex($('#disk_fill_color_box').css("background-color")).replace("#", '');
            colors = colors + rgb2hex($('#disk_stroke_color_box').css("background-color")).replace("#", '');
            colors = colors + rgb2hex($('#hour_window_color_box').css("background-color")).replace("#", '');
            
            colors = colors + rgb2hex($('#min_fill_color_box').css("background-color")).replace("#", '');
            colors = colors + rgb2hex($('#min_stroke_color_box').css("background-color")).replace("#", '');
            
            colors = colors + rgb2hex($('#bt_con_color_color_box').css("background-color")).replace("#", '');
            colors = colors + rgb2hex($('#bt_dis_color_color_box').css("background-color")).replace("#", '');
            
            options.KEY_COLORS_BASALT = colors;
            
            options.KEY_BATTERY_SIGNAL = parseInt($("#battery_mode_basalt_select").val(), 10);
        }
        if (options.KEY_BATTERY_SIGNAL == 1) {
            var level = parseInt($("#battery_show_after_level_input").val(), 10);
            if(level > 100) level = 100;
            if(level < 0) level = 0;
            options.KEY_BATTERY_BELOW_LEVEL = level;
        } else if (options.KEY_BATTERY_SIGNAL >= 4) {
            options.KEY_BATTERY_COLOR_SCHEME = parseInt($("#battery_color_select").val(), 10);
        }
	return options;
}

$("document").ready(function() {
	watch_version = getQueryParam("watch_version", 1);
        console.log("ready - watch_version: "+watch_version);
        
        var vibrate = getQueryParam("btVibrate", 2);
        $('#bluetooth_vibrate_'+vibrate).prop( "checked", true ).checkboxradio("refresh");
        
        var show_minute_marks = getQueryParam("show_min_marks", -1);
        if(show_minute_marks < 0) show_minute_marks = 0; //solves a problem getQueryParam has with default value 0
        $('#show_minute_marks_'+show_minute_marks).prop( "checked", true ).checkboxradio("refresh");
        
        var show_date = getQueryParam("show_date", -1);
        if(show_date < 0) show_date = 0; //solves a problem getQueryParam has with default value 0
        $('#show_date_'+show_date).prop( "checked", true ).checkboxradio("refresh");
        var date_format = getQueryParam("date_format", "d, D.m (W), Y");
        $('#date_format_input').val(date_format);
        if(show_date == 1) {
            $('#date_format').removeClass("hidden");
        }
        var date_pos = getQueryParam("date_pos", 1);
        $('#date_pos_'+date_pos).prop( "checked", true ).checkboxradio("refresh");
        
        var def_bt_mode = 2;
        if(watch_version >= 3) def_bt_mode = 3;
        bluetooth_res = getQueryParam("btSignal", def_bt_mode);
        bluetooth_res = parseInt(isNumber(bluetooth_res) && bluetooth_res >= 0 && bluetooth_res <= 7 ? bluetooth_res : def_bt_mode);
        var btSignal = bluetooth_res;
        if(bluetooth_res >= 1 && bluetooth_res <= 5) {
            $('#bluetooth_signal_1').prop( "checked", true ).checkboxradio("refresh");
            btSignal = 1;
        } else {
            $('#bluetooth_signal_'+bluetooth_res).prop( "checked", true ).checkboxradio("refresh");
            $("#color_affects_basalt_select").val(1).selectmenu("refresh");
            $("#color_affects_aplite_select").val(1).selectmenu("refresh");
        }
        
        var def_batt_mode = 1;
        if(watch_version >= 3) def_batt_mode = 6;
        var batt_mode = getQueryParam("bat_mode", def_batt_mode);
        batt_mode = parseInt(isNumber(batt_mode) && batt_mode >= 0 && batt_mode <= 8 ? batt_mode : def_batt_mode);
        var batt_below_lvl = getQueryParam("bat_below_level", 40);
        batt_below_lvl = parseInt(isNumber(batt_below_lvl) && batt_below_lvl >= 0 && batt_below_lvl <= 100 ? batt_below_lvl : 40);
        $('#battery_show_after_level_input').val(batt_below_lvl);
        var batt_color = getQueryParam("bat_color_scheme", 3);
        batt_color = parseInt(isNumber(batt_color) && batt_color >= 0 && batt_color <= 3 ? batt_color : 3);
        $('#battery_color_select').val(batt_color).selectmenu("refresh");
        if(batt_mode == 1) {
            $('#battery_show_after_level').removeClass("hidden");
        } else if(batt_mode >= 4) {
            $('#battery_color_type').removeClass("hidden");
        }
	
	if (watch_version >= 3) {
            $('#battery_mode_basalt').removeClass("hidden");
            $('#battery_mode_basalt_select').val(batt_mode).selectmenu("refresh");
            
            var basalt_colors = getQueryParam("basalt_colors", 0);
            basalt_colors = basalt_colors.length == COLOR_STRING_LENGTH ? basalt_colors : "000000FFFFFF00550055AA00FFFFFFFFFFFFAAAAAA55AA00555500";
            $('#bg_color_box').css('background-color', "#"+basalt_colors.substring(0,6));
            $('#text_color_box').css('background-color', "#"+basalt_colors.substring(6,12));
            $('#disk_fill_color_box').css('background-color', "#"+basalt_colors.substring(12,18));
            $('#disk_stroke_color_box').css('background-color', "#"+basalt_colors.substring(18,24));
            $('#hour_window_color_box').css('background-color', "#"+basalt_colors.substring(24,30));
            $('#min_fill_color_box').css('background-color', "#"+basalt_colors.substring(30,36));
            $('#min_stroke_color_box').css('background-color', "#"+basalt_colors.substring(36,42));
            $('#bt_con_color_color_box').css('background-color', "#"+basalt_colors.substring(42,48));
            $('#bt_dis_color_color_box').css('background-color', "#"+basalt_colors.substring(48,54));
            
            $('#text_color').removeClass("hidden");
            $('#hour_window_color').removeClass("hidden");
            
            if(btSignal == 1) {
                $("#color_affects_basalt_select").val(bluetooth_res).selectmenu("refresh");
                $('#bt_color').removeClass("hidden");
                $('#color_affects_basalt').removeClass("hidden");
            }
            compare_color_affected(bluetooth_res, true);
            
	} else { //aplite
            $('#battery_mode_aplite').removeClass("hidden");
            $('#battery_mode_aplite_select').val(batt_mode).selectmenu("refresh");
            
            if(btSignal == 1) {
                $("#color_affects_aplite_select").val(bluetooth_res).selectmenu("refresh");
                $('#color_affects_aplite').removeClass("hidden");
            }
            
            var min_color_aplite = getQueryParam("min_color_aplite", 1);
            min_color_aplite = parseInt(isNumber(min_color_aplite) && min_color_aplite <= 1 ? min_color_aplite : 1);
            $('#minute_hand_color_aplite').removeClass("hidden");
            $('#minute_hand_color_aplite_'+min_color_aplite).prop( "checked", true ).checkboxradio("refresh");
            
            var hour_color_aplite = getQueryParam("hour_color_aplite", 1);
            hour_color_aplite = parseInt(isNumber(hour_color_aplite) && hour_color_aplite <= 1 ? hour_color_aplite : 1);
            $('#hour_window_color_aplite').removeClass("hidden");
            $('#hour_window_color_aplite_'+hour_color_aplite).prop( "checked", true ).checkboxradio("refresh");
            
            var theme = getQueryParam("theme_aplite", 0);
            theme = parseInt(isNumber(theme) && theme <= 5 ? theme : 0);
            $('#theme_aplite').removeClass("hidden");
            $("#theme_aplite_select").val(theme).selectmenu("refresh");
	}
	
});

function isNumber(n) {
	return !isNaN(parseFloat(n)) && isFinite(n);
}
