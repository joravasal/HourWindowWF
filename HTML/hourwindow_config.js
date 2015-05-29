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
  $('#bluetooth_signal').change(function() {
        bluetooth_res = parseInt($("input[name=bluetooth_signal]:checked").val(), 10);
        $('#color_affects_aplite').addClass("hidden");
        $('#color_affects_basalt').addClass("hidden");
        $('#bt_color').addClass("hidden");
        if(watch_version >= 3) {
            $('#bg_color').removeClass("hidden");
            $('#disk_fill_color').removeClass("hidden");
            $('#disk_stroke_color').removeClass("hidden");
            $('#min_fill_color').removeClass("hidden");
            $('#min_stroke_color').removeClass("hidden");
            if(bluetooth_res == 1) {
                $('#bt_color').removeClass("hidden");
                $('#color_affects_basalt').removeClass("hidden");
                bluetooth_res = parseInt($("#color_affects_basalt_select").val(), 10);
                if(bluetooth_res == 1) {
                    $('#bg_color').addClass("hidden");
                } else if(bluetooth_res == 2) {
                    $('#disk_fill_color').addClass("hidden");
                } else if(bluetooth_res == 3) {
                    $('#disk_stroke_color').addClass("hidden");
                } else if(bluetooth_res == 4) {
                    $('#min_fill_color').addClass("hidden");
                } else if(bluetooth_res == 5) {
                    $('#min_stroke_color').addClass("hidden");
                }
            }
            
        } else {
            if(bluetooth_res == 1) {
                $('#color_affects_aplite').removeClass("hidden");
                bluetooth_res = parseInt($("#color_affects_aplite_select").val(), 10);
            }
        }
  });
});

$(function () {
  $('#color_affects_aplite_select').change(function() {
      bluetooth_res = parseInt($("#color_affects_aplite_select").val(), 10);
  });
});

$(function () {
  $('#color_affects_basalt_select').change(function() {
        bluetooth_res = parseInt($("#color_affects_basalt_select").val(), 10);
        $('#bg_color').removeClass("hidden");
        $('#disk_fill_color').removeClass("hidden");
        $('#disk_stroke_color').removeClass("hidden");
        $('#min_fill_color').removeClass("hidden");
        $('#min_stroke_color').removeClass("hidden");
        if(bluetooth_res == 1) {
            $('#bg_color').addClass("hidden");
        } else if(bluetooth_res == 2) {
            $('#disk_fill_color').addClass("hidden");
        } else if(bluetooth_res == 3) {
            $('#disk_stroke_color').addClass("hidden");
        } else if(bluetooth_res == 4) {
            $('#min_fill_color').addClass("hidden");
        } else if(bluetooth_res == 5) {
            $('#min_stroke_color').addClass("hidden");
        }
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
	
        options.KEY_BT_SIGNAL = bluetooth_res;
        options.KEY_BT_VIBRATE = parseInt($("input[name=bluetooth_vibrate]:checked").val(), 10);
        if (watch_version < 3){
            options.KEY_THEME_APLITE = parseInt($("#theme_aplite_select").val(), 10);
            options.KEY_HOUR_COLOR_APLITE = parseInt($("input[name=hour_window_color_aplite]:checked").val(), 10);
            options.KEY_MIN_COLOR_APLITE = parseInt($("input[name=minute_hand_color_aplite]:checked").val(), 10);
        } else {
            options.KEY_BATTERY_SIGNAL = parseInt($("input[name=min_battery_signal]:checked").val(), 10);
            
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

        }
	return options;
}

$("document").ready(function() {
	watch_version = getQueryParam("watch_version", 1);
        console.log("ready - watch_version: "+watch_version);
        
        var vibrate = getQueryParam("btVibrate", 2);
        $('#bluetooth_vibrate_'+vibrate).prop( "checked", true ).checkboxradio("refresh");
        
        var bluetooth_res = getQueryParam("btSignal", 1);
        bluetooth_res = parseInt(isNumber(bluetooth_res) && bluetooth_res <= 7 ? bluetooth_res : 1);
        var btSignal = bluetooth_res;
        if(bluetooth_res >= 1 && bluetooth_res <= 5) {
            $('#bluetooth_signal_1').prop( "checked", true ).checkboxradio("refresh");
            btSignal = 1;
        } else {
            $('#bluetooth_signal_'+bluetooth_res).prop( "checked", true ).checkboxradio("refresh");
            $("#color_affects_basalt_select").val(1).selectmenu("refresh");
            $("#color_affects_aplite_select").val(1).selectmenu("refresh");
        }
	
	if (watch_version >= 3) {
            var minBatterySignal = getQueryParam("bgBT", 0);
            minBatterySignal = parseInt(isNumber(minBatterySignal) && minBatterySignal <= 1 ? minBatterySignal : 0);
            //$('#min_battery_signal').removeClass("hidden");
            $('#min_battery_signal_'+minBatterySignal).prop( "checked", true ).checkboxradio("refresh");
            
            var basalt_colors = getQueryParam("basalt_colors", 0);
            basalt_colors = basalt_colors.length == COLOR_STRING_LENGTH ? basalt_colors : "FFFFFF000000555555AAAAAAFFFFFFFF0000FF000055FF55FF5555";
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
            $('#bg_color').removeClass("hidden");
            $('#disk_fill_color').removeClass("hidden");
            $('#disk_stroke_color').removeClass("hidden");
            $('#min_fill_color').removeClass("hidden");
            $('#min_stroke_color').removeClass("hidden");
            if(btSignal == 1) {
                $("#color_affects_basalt_select").val(bluetooth_res).selectmenu("refresh");
                $('#bt_color').removeClass("hidden");
                $('#color_affects_basalt').removeClass("hidden");
                if(bluetooth_res == 1) {
                    $('#bg_color').addClass("hidden");
                } else if(bluetooth_res == 2) {
                    $('#disk_fill_color').addClass("hidden");
                } else if(bluetooth_res == 3) {
                    $('#disk_stroke_color').addClass("hidden");
                } else if(bluetooth_res == 4) {
                    $('#min_fill_color').addClass("hidden");
                } else if(bluetooth_res == 5) {
                    $('#min_stroke_color').addClass("hidden");
                }
            }
            
	} else {
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
