var configUrl = "http://www.edwinfinch.com/fitface/fitpebble-php/?token=";

Pebble.addEventListener("showConfiguration", function(e) {
    Pebble.openURL(configUrl + getToken());
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

function getToken(){
    var accToken = Pebble.getAccountToken();
    var localToken = localStorage.getItem("custToken");
    var useGenToken = parseInt(localStorage.getItem("useGenToken"));
    var genToken = localStorage.getItem("genToken");
    if(accToken.length > 1 && useGenToken !== true){
        localStorage.setItem("custToken", accToken);
        console.log("Returning actual account token.");
        return accToken;
    }
    else if(localToken.length > 5){
        console.log("Returning stored account token.");
        return localToken;
    }
    else if(useGenToken){
        console.log("Returning stored generated token.");
        return genToken;
    }
    else{
        Pebble.showSimpleNotificationOnPebble("FitFace", "Please log in again through the settings page.");
        console.log("Creating generated token and storing it in 'genToken'.");
        localStorage.setItem("useGenToken", 1);
        var generated = Math.random() * 10000000000000;
        var generatedFix = Math.floor(generated);
        localStorage.setItem("genToken", generatedFix);
        return generatedFix;
    }
}

var publicData;

function sendFix(){
    var distanceFixMI_v = ((data.distance_v_mi)*100);
    var distanceFixKM_v = ((data.distance_v)*100);
    var distanceFixMI_g = ((data.distance_g_mi)*100);
    var distanceFixKM_g = ((data.distance_g)*100);

    console.log("Sending fixed distance data message.");

    Pebble.sendAppMessage({
        "distance_v_mi": distanceFixMI_v,
        "distance_g_mi": distanceFixMI_g,
        "distance_v": distanceFixKM_v,
        "distance_g": distanceFixKM_g
    });
}

function getData() {
    Pebble.sendAppMessage({
        "loading_data": 1
    });
    var response;
    var req = new XMLHttpRequest();
    req.open('GET', "http://edwinfinch.com/fitface/fitpebble-php/request.php?js=fitface&token=" + getToken(), true);
    console.log("http://edwinfinch.com/fitface/fitpebble-php/request.php?js=fitface&token=" + getToken());
    req.onload = function(e){
        if (req.readyState == 4) {
            if(req.status == 200) {
            console.log(req.responseText);

            if(req.responseText.indexOf("401") != -1){
                Pebble.sendAppMessage({"error":2});
                return;
            }
            else if(req.responseText.indexOf("Error") != -1){
                Pebble.sendAppMessage({"error":3});
                return;
            }

            data = JSON.parse(req.responseText);
            publicData = data;

            Pebble.sendAppMessage(data);

            setTimeout(sendFix, 400);
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
