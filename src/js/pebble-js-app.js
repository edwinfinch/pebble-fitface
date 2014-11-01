var configUrl = "http://www.edwinfinch.com/fitface/fitpebble-php/?token=";

Pebble.addEventListener("showConfiguration", function(e) {
    Pebble.openURL(configUrl + Pebble.getAccountToken());
});

Pebble.addEventListener("webviewclosed", function(e){
    console.log("Configuration closed");
    console.log("Response = " + e.response.length + "   " + e.response);
    if (e.response) { // user clicked Save/Submit, not Cancel/Done
        console.log("User hit save");
        values = JSON.parse(decodeURIComponent(e.response));
        console.log("stringified options: " + JSON.stringify((values)));
        for(var key in values) {
            localStorage.setItem(key, values[key]);
        }    
        Pebble.sendAppMessage(
            values
        );
    }
});

function getData() {
    Pebble.sendAppMessage({
        "loading_data": 1
    });
    var response;
    var req = new XMLHttpRequest();
    req.open('GET', "http://edwinfinch.com/fitface/fitpebble-php/request.php?js=fitface&token=" + Pebble.getAccountToken(), true);
    console.log("http://edwinfinch.com/fitface/fitpebble-php/request.php?js=fitface&token=" + Pebble.getAccountToken());
    req.onload = function(e){
        if (req.readyState == 4) {
            if(req.status == 200) {
            console.log(req.responseText);
            data = JSON.parse(req.responseText);
            console.log("Response: " + data);

            Pebble.sendAppMessage(data);

            //Either I am retarded or not because I don't believe pebble supports sending floats so I use this cheap trick
            var distanceFixMI_v = ((data.distance_v_mi)*100);
            var distanceFixKM_v = ((data.distance_v)*100);
            var distanceFixMI_g = ((data.distance_g_mi)*100);
            var distanceFixKM_g = ((data.distance_g)*100);
            Pebble.sendAppMessage({
                "distance_v_mi": distanceFixMI_v,
                "distance_g_mi": distanceFixMI_g,
                "distance_v": distanceFixKM_v,
                "distance_g": distanceFixKM_g
            });
            //}
            } else {
            console.log("Error: could not connect! (is internet connection kill?)");
            }
        }
    };
    req.send(null);
}

Pebble.addEventListener("appmessage", function(e) {
    getData();
});

Pebble.addEventListener("ready", function(e) {
    getData();
});
