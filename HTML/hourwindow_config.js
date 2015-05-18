var watch_version = -1;

//Function to convert hex format to a rgb color
function rgb2hex(rgb){
 rgb = rgb.match(/^rgb\((\d+),\s*(\d+),\s*(\d+)\)$/);
 return ("#" +
  ("0" + parseInt(rgb[1],10).toString(16)).slice(-2) +
  ("0" + parseInt(rgb[2],10).toString(16)).slice(-2) +
  ("0" + parseInt(rgb[3],10).toString(16)).slice(-2)).toUpperCase();
}

$(function () {
  $('#bg_bluetooth_signal').on('click', function (event) {
	if(parseInt($("input[name=bg_bluetooth_signal]:checked").val(), 10) == 0)
            $('#bg_color').removeClass("hidden");
        else
            $('#bg_color').addClass("hidden");
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
  $('#disk_color_box').on('click', function (event) {
	changeColorObject = 1;
	highlightColorBox($(this));
  });
});

$(function () {
  $('#min_color_box').on('click', function (event) {
	changeColorObject = 2;
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
		$('#disk_color_box').css('background-color', selected_color);
        else
                $('#min_color_box').css('background-color', selected_color);
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
	
        if (watch_version < 3){
            options.KEY_THEME_APLITE = parseInt($("#theme_aplite_select").val(), 10);
            options.KEY_HOUR_COLOR_APLITE = parseInt($("input[name=hour_window_color_aplite]:checked").val(), 10);
            options.KEY_MIN_COLOR_APLITE = parseInt($("input[name=minute_hand_color_aplite]:checked").val(), 10);
        } else {
            options.KEY_BT_SIGNAL = parseInt($("input[name=bg_bluetooth_signal]:checked").val(), 10);
            options.KEY_BATTERY_SIGNAL = parseInt($("input[name=min_battery_signal]:checked").val(), 10);
            
            var colors = rgb2hex($('#bg_color_box').css("background-color")).replace("#", '');
            colors = colors + rgb2hex($('#disk_color_box').css("background-color")).replace("#", '');
            colors = colors + rgb2hex($('#min_color_box').css("background-color")).replace("#", '');
            options.KEY_COLORS_BASALT = colors;
        }
	return options;
}

$("document").ready(function() {
	watch_version = getQueryParam("watch_version", 1);
	
	if (watch_version >= 3) {
            var bgBTSignal = getQueryParam("bgBT", 0);
            bgBTSignal = parseInt(isNumber(bgBTSignal) && bgBTSignal <= 1 ? bgBTSignal : 0);
            $('#bg_bluetooth_signal').removeClass("hidden");
            $('#bg_bluetooth_signal_'+bgBTSignal).prop( "checked", true ).checkboxradio("refresh");
            
            var minBatterySignal = getQueryParam("bgBT", 0);
            minBatterySignal = parseInt(isNumber(minBatterySignal) && minBatterySignal <= 1 ? minBatterySignal : 0);
            $('#min_battery_signal').removeClass("hidden");
            $('#min_battery_signal_'+minBatterySignal).prop( "checked", true ).checkboxradio("refresh");
            
            var basalt_colors = getQueryParam("basalt_colors", 0);
            basalt_colors = basalt_colors.length == 18 ? basalt_colors : "FFFFFF555555AA0000";
            $('#bg_color_box').css('background-color', "#"+basalt_colors.substring(0,6));
            $('#disk_color_box').css('background-color', "#"+basalt_colors.substring(6,12));
            $('#min_color_box').css('background-color', "#"+basalt_colors.substring(12,18));
            
            if(bgBTSignal == 0)
                $('#bg_color').removeClass("hidden");
            $('#disk_color').removeClass("hidden");
            if(minBatterySignal == 0)
                $('#min_color').removeClass("hidden");
            
	} else {
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
