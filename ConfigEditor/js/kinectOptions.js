
function getHandTopJSON(el){
    json = {};
    json.type = "key";
 
    if (el.find("#hand").val() == "1"){
        json.key = "KINECT_LEFT_HAND_TOP";
    } else {
        json.key = "KINECT_RIGHT_HAND_TOP";
    }
 
    json.x = el.find("#x").val();
    json.yMod = el.find("#yMod").val();
    json.y = el.find("#y").val();
 
    json.toKey = el.find("#action").val();
 
    return json;
 }
 
 
 function getKinectWalkingJSON(el){
     json = {};
     json.type = "key";
  
     json.key = "KINECT_WALK";
     json.delay = el.find("#delay").val();
     json.height = parseInt(el.find("#height").val())/100;
     json.toKey = el.find("#action").val();
  
     return json;
  }
 
  function getKinectStepJSON(el){
     json = {};
     json.type = "key";
  
     if (el.find("#mode").val() == 1){
         json.key = "KINECT_STEP_UP";
     } else
     if (el.find("#mode").val() == 2){
         json.key = "KINECT_STEP_NORMAL";
     } else
     if (el.find("#mode").val() == 3){
         json.key = "KINECT_STEP_DOWN";
     }
     
     json.height = parseInt(el.find("#height").val())/100;
     json.toKey = el.find("#action").val();
  
     return json;
  }
 
 
  function getKinectBalanceJSON(el){
     json = {};
     json.type = "key";
  
     json.key = "KINECT_BALANCE";
     json.mod = el.find("#mod").val();
     json.angle = el.find("#angle").val();
  
     return json;
  }
 
  function getKinectFastHandJSON(el){
     json = {};
     json.type = "key";
     if (el.find("#hand").val() == 1){
         json.key = "KINECT_LEFT_HAND_FAST";
     } else {
         json.key = "KINECT_RIGHT_HAND_FAST";
     }
     json.mod = el.find("#velocity").val();
  
     return json;
  }

function getKinectFist(){
    json = {};
    json.type = "key";
    if (el.find("#hand").val() == 1){
        json.key = "KINECT_LEFT_FIST";
    } else {
        json.key = "KINECT_RIGHT_FIST";
    }
    if (el.find("#direction").val() == 1){
        json.key += "_UP";
    } else {
        json.key += "_DOWN";
    }
    
    return json;
}

function getKinectTurn(){
    json = {};
    json.type = "key";
    if (el.find("#direction").val() == 1){
        json.key = "KINECT_TURN_LEFT";
    } else {
        json.key = "KINECT_TURN_RIGHT";
    }
    
    return json;
}

function getKinectBody(){
    json = {};
    json.type = "key";

    switch (el.find("#direction").val()) {
        case 1:
            json.key = "KINECT_BODY_FRONT";
            break;
        case 2:
            json.key = "KINECT_BODY_RIGHT";
            break;
        case 3:
            json.key = "KINECT_BODY_BACK";
            break;
        case 4:
            json.key = "KINECT_BODY_LEFT";
            break;
    }
    
    
    return json;
}



var kinectActions = [{class:"handTop",func:getHandTopJSON, title:"[Kinect] Altura da mão"},
  {class:"kinectWalking",func:getKinectWalkingJSON, title:"[Kinect] Marcha estacionária"},
  {class:"kinectStep",func:getKinectStepJSON, title:"[Kinect] Step"},
  {class:"kinectBalance",func:getKinectStepJSON, title:"[Kinect] Equilíbrio"},
  {class:"kinectFist",func:getKinectFist, title:"[Kinect] Flexão de punho"},
  {class:"kinectTurn", func:getKinectTurn, title:"[Kinect] Girar corpo"},
  {class:"kinectBody", func:getKinectBody, title:"[Kinect] Inclinar corpo"}
];