
function getLeapMotionCloseHandJSON(el){
    json = {};
    json.type = "key";
 
    if (el.find("#hand").val() == "1"){
        json.key = "LEAP_LEFT_CLOSED";
    } else {
        json.key = "LEAP_RIGHT_CLOSED";
    }
 
    json.toKey = el.find("#action").val();
 
    return json;
 }

 function getLeapMotionPinchJSON(el){
    json = {};
    json.type = "key";
 
    if (el.find("#hand").val() == "1"){
        json.key = "LEAP_LEFT_PINCH";
    } else {
        json.key = "LEAP_RIGHT_PINCH";
    }
 
    json.toKey = el.find("#action").val();
 
    return json;
 }

 function getLeapMotionFistJSON(el){
    json = {};
    json.type = "key";
 
    if (el.find("#hand").val() == "1"){
        json.key = "LEAP_LEFT_FIST";
    } else {
        json.key = "LEAP_RIGHT_FIST";
    }
    if (el.find("#direction").val() == 1){
        json.key += "_UP";
    } else {
        json.key += "_DOWN";
    }
 
    json.toKey = el.find("#action").val();
 
    return json;
 }
 


var leapMotionActions = [{class:"leapCloseHand",func:getLeapMotionCloseHandJSON, title:"[LeapMotion] Fechar mão"},
  {class:"leapPinch",func:getLeapMotionPinchJSON, title:"[LeapMotion] Movimento de pinça"},
  {class:"leapFist",func:getLeapMotionFistJSON, title:"[LeapMotion] Flexionar punho"}
];