var fs = require('fs');



$(function(){
    
    $('#addCommand').click(function(){
        if ($('#command').val() == 1){
            clone = $('#handTopModel').clone();
        }

        $('#mapView').append(clone);
    });


    $("#save").click(function(){
        var jsonConfig = [];
        $('#mapView').children().each(function(i,item){
            item = $(item);
            if (item.hasClass("handTop")){
                jsonConfig.push(getHandTopJSON(item));
            }
        });
        

        jsonStr = JSON.stringify(jsonConfig);

        fs.writeFile('config.json', jsonStr, 'utf8', function(msg){
            console.log(msg);
        });
        
    });

});


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


function removeMap(el){
    $(el).parent().remove();
}