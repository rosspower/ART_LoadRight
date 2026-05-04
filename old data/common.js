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

function nav_marker(){
  var path = window.location.pathname;
  var page = path.split("/").pop();
  switch(page) {
      case "index.html":
          document.getElementById('nav_home').style.backgroundColor = "crimson";
      break;
      case "presets.html":
      document.getElementById('nav_presets').style.backgroundColor = "crimson";
      break;
      case "schedule.html":
      document.getElementById('nav_schedule').style.backgroundColor = "crimson";
      break;
      case "settings.html":
          document.getElementById('nav_settings').style.backgroundColor = "crimson";
      break;
      case "service.html":
        document.getElementById('nav_service').style.backgroundColor = "crimson";
        
      break;       
      default:
        document.getElementById('nav_home').style.backgroundColor = "crimson";
  }
}

function logout(){
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/logout", true);
  xhr.send();
  setTimeout(function(){ window.open("/logged-out","_self"); }, 1000);
}

!function(e,t){"use strict";var n=null,a="PointerEvent"in e||e.navigator&&"msPointerEnabled"in e.navigator,i="ontouchstart"in e||navigator.MaxTouchPoints>0||navigator.msMaxTouchPoints>0,o=a?"pointerdown":i?"touchstart":"mousedown",r=a?"pointerup":i?"touchend":"mouseup",m=a?"pointermove":i?"touchmove":"mousemove",u=a?"pointerleave":i?"touchleave":"mouseleave",s=0,c=0,l=10,v=10;function f(e){p(),e=function(e){if(void 0!==e.changedTouches)return e.changedTouches[0];return e}(e),this.dispatchEvent(new CustomEvent("long-press",{bubbles:!0,cancelable:!0,detail:{clientX:e.clientX,clientY:e.clientY,offsetX:e.offsetX,offsetY:e.offsetY,pageX:e.pageX,pageY:e.pageY},clientX:e.clientX,clientY:e.clientY,offsetX:e.offsetX,offsetY:e.offsetY,pageX:e.pageX,pageY:e.pageY,screenX:e.screenX,screenY:e.screenY}))||t.addEventListener("click",function e(n){t.removeEventListener("click",e,!0),function(e){e.stopImmediatePropagation(),e.preventDefault(),e.stopPropagation()}(n)},!0)}function d(a){p(a);var i=a.target,o=parseInt(function(e,n,a){for(;e&&e!==t.documentElement;){var i=e.getAttribute(n);if(i)return i;e=e.parentNode}return a}(i,"data-long-press-delay","1500"),10);n=function(t,n){if(!(e.requestAnimationFrame||e.webkitRequestAnimationFrame||e.mozRequestAnimationFrame&&e.mozCancelRequestAnimationFrame||e.oRequestAnimationFrame||e.msRequestAnimationFrame))return e.setTimeout(t,n);var a=(new Date).getTime(),i={},o=function(){(new Date).getTime()-a>=n?t.call():i.value=requestAnimFrame(o)};return i.value=requestAnimFrame(o),i}(f.bind(i,a),o)}function p(t){var a;(a=n)&&(e.cancelAnimationFrame?e.cancelAnimationFrame(a.value):e.webkitCancelAnimationFrame?e.webkitCancelAnimationFrame(a.value):e.webkitCancelRequestAnimationFrame?e.webkitCancelRequestAnimationFrame(a.value):e.mozCancelRequestAnimationFrame?e.mozCancelRequestAnimationFrame(a.value):e.oCancelRequestAnimationFrame?e.oCancelRequestAnimationFrame(a.value):e.msCancelRequestAnimationFrame?e.msCancelRequestAnimationFrame(a.value):clearTimeout(a)),n=null}"function"!=typeof e.CustomEvent&&(e.CustomEvent=function(e,n){n=n||{bubbles:!1,cancelable:!1,detail:void 0};var a=t.createEvent("CustomEvent");return a.initCustomEvent(e,n.bubbles,n.cancelable,n.detail),a},e.CustomEvent.prototype=e.Event.prototype),e.requestAnimFrame=e.requestAnimationFrame||e.webkitRequestAnimationFrame||e.mozRequestAnimationFrame||e.oRequestAnimationFrame||e.msRequestAnimationFrame||function(t){e.setTimeout(t,1e3/60)},t.addEventListener(r,p,!0),t.addEventListener(u,p,!0),t.addEventListener(m,function(e){var t=Math.abs(s-e.clientX),n=Math.abs(c-e.clientY);(t>=l||n>=v)&&p()},!0),t.addEventListener("wheel",p,!0),t.addEventListener("scroll",p,!0),t.addEventListener(o,function(e){s=e.clientX,c=e.clientY,d(e)},!0)}(window,document);