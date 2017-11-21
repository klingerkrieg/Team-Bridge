var fs = require('fs');

var modelIncludes = [{url:"kinectOptions.html",onLoad:function(){}},
                    {url:"leapMotionOptions.html",onLoad:function(){}},
                    {url:"keyboardMouseOptions.html",onLoad:loadKeyboardOptions}];

$(function(){

    for (var i = 0; i < modelIncludes.length; i++){
        $.ajax({url:modelIncludes[i].url,async: false}).done(function(res){
            content = $(res);
            $('#models').append(content);
            modelIncludes[i].onLoad();
        });
    }

    //Adiciona os possiveis comandos a serem utilizados
    addCommands();
    //Cria os options com as possíveis ações
    addActions();

    
    $('#addCommand').click(function(){
        clone = $('#'+$('#command').val()+'Model').clone();
        clone.attr('id',"");
        clone.append($('#actionModel #act').clone());
        clone.append('<img class="close" src="./img/close.png" onclick="removeMap(this)"/>');
        $('#mapView').append(clone);
    });


    $("#save").click(function(){
        var jsonConfig = [];
        $('#mapView').children().each(function(i,item){
            item = $(item);
            
            for (var y = 0; y < kinectActions.length; y++){
                if (item.hasClass(kinectActions[y].class)){
                    jsonConfig.push(kinectActions[y].func(item));
                }
            }

        });
        
        console.log(jsonConfig);
        jsonStr = JSON.stringify(jsonConfig);

        fs.writeFile('config.json', jsonStr, 'utf8', function(msg){
            //console.log(msg);
        });
        
    });

});


function removeMap(el){
    $(el).parent().remove();
}

function addCommands(){
    cmds = $('#command');
    
    actions = kinectActions;
    actions = actions.concat(leapMotionActions);
    actions = actions.concat(keyboardMouseActions);

    for (var y = 0; y < actions.length; y++){
        cmds.append("<option value='"+actions[y].class+"'>"+actions[y].title+"</option>");
    }
}

function addActions(){
    act = $('#actionModel #action');

    keys = getKeyboardKeys();
    keys.push({t:"Definir posição",code:"KINECT_DETERMINE_CENTER_POS"});
    keys.push({t:"Mover mouse",code:"VK_MOUSEMOVE"});
    keys.push({t:"Alerta (Vermelho)",code:"ALERT"});
    keys.push({t:"Mensagem (Verde)",code:"MESSAGE"});

    for(var i = 0; i < keys.length ;i++){
        act.append("<option value='"+keys[i].code+"'>"+keys[i].t+"</option>");
    }
}

function showExtraActions(el){
    el = $(el);
    console.log(el.val());

    parent = $(el).parent();
    if (el.val() == "VK_MOUSEMOVE"){
        parent.find("#moveMouseSpace").show();
        parent.find("#msgSpace").hide();
    } else 
    if (el.val() == "ALERT" || el.val() == "MESSAGE"){
        parent.find("#moveMouseSpace").hide();
        parent.find("#msgSpace").show();
    } else {
        parent.find("#moveMouseSpace").hide();
        parent.find("#msgSpace").hide();
    }
}