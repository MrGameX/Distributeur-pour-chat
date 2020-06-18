var content = document.querySelector('#menu');
var sidebarBody = document.querySelector('#hamburger-sidebar-body');
var button = document.querySelector('.icon');
var overlay = document.querySelector('#hamburger-overlay')
var activatedClass = 'hamburger-activated';

sidebarBody.innerHTML = content.innerHTML;

button.addEventListener('click', function() {
	sidebarBody.parentNode.classList.add(activatedClass);
	overlay.classList.add(activatedClass);

});

overlay.addEventListener('click', function(){
	sidebarBody.parentNode.classList.remove(activatedClass);
	overlay.classList.remove(activatedClass);
});

document.addEventListener('touchstart', handleTouchStart, false);        
document.addEventListener('touchmove', handleTouchMove, false);

var xDown = null;                                                        
var yDown = null;

function getTouches(evt) {
  return evt.touches ||             // browser API
         evt.originalEvent.touches; // jQuery
}                                                     

function handleTouchStart(evt) {
    const firstTouch = getTouches(evt)[0];                                      
    xDown = firstTouch.clientX;                                      
    yDown = firstTouch.clientY;                                      
};                                                

function handleTouchMove(evt) {
    if ( ! xDown || ! yDown ) {
        return;
    }

    var xUp = evt.touches[0].clientX;                                    
    var yUp = evt.touches[0].clientY;

    var xDiff = xDown - xUp;
    var yDiff = yDown - yUp;

    if ( Math.abs( xDiff ) > Math.abs( yDiff ) ) {/*most significant*/
        if ( xDiff > 0 ) {
            /* left swipe */ 
			sidebarBody.parentNode.classList.remove(activatedClass);
			overlay.classList.remove(activatedClass);
        } else {
            /* right swipe */
            sidebarBody.parentNode.classList.add(activatedClass);
			overlay.classList.add(activatedClass);
        }                       
    } else {
        if ( yDiff > 0 ) {
            /* up swipe */ 
        } else { 
            /* down swipe */
        }                                                                 
    }
    /* reset values */
    xDown = null;
    yDown = null;                                             
};

//setInterval(loadDoc, 10000);
function loadDoc() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function(){
        if (this.readyState == 4 && this.status == 200) {
            document.body.innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/", true);
    xhttp.send();
}