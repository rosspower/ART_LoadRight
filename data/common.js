var isFault = false;
let errorMsg = "";
let faultArray = [0,0,0,0,0,0,0,0,0,0,0];



function motorReset(){
    console.log("sending reset Command: " );
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "/motor_reset", true);
        xhr.send(); 
  }
  
  function addSpacer(msg){
    if (msg.length > 0){
      msg += "; ";
    } 
    return msg;
  }
  
  

   // get signal strength
 function getRssi() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var y = this.responseText;
      const signalBox = document.getElementById('rssi-graphic');
      var classes = "signal-icon ";
      var signal = parseInt(y);
      if (signal < -90 ) classes +="";
      if (signal > -90 && signal <= -80) classes +="poor";
      if (signal > - 80 && signal <= -70 ) classes += "weak";
      if (signal > - 70 && signal <= -60 ) classes += "medium";
      if (signal > -60 ) classes += "strong";
      signalBox.className=classes;
      try {
        document.getElementById("rssi-label").innerHTML = this.responseText + "db";
      } catch (error){
       //console.log(error);
      }
     }
  };
  xhttp.open("GET", "/rssi", true);
  xhttp.send();
}

function logout(){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout", true);
  xhr.send();
  setTimeout(function(){ window.open("/logged-out","_self"); }, 1000);
}

function menuFunction() {
  var x = document.getElementById("myLinks");
  if (x.style.display === "block") {
    x.style.display = "none";
  } else {
    x.style.display = "block";
  }
}
function nospace(originalString){
  let newString = originalString.replaceAll(" ", "_");
  return newString;
}



