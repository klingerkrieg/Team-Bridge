
var fs = require('fs');

var programName = "ConfigEditor";
var actualName = "";

var modelIncludes = [{url:"kinectOptions.html",onLoad:function(){}},
                    {url:"leapMotionOptions.html",onLoad:function(){}},
                    {url:"keyboardMouseOptions.html",onLoad:loadKeyboardOptions}];

var actions = [{class:"handTop", title:"[Kinect] Altura da mão"},
                    {class:"kinectWalking", title:"[Kinect] Marcha estacionária"},
                    {class:"kinectStep", title:"[Kinect] Step"},
                    {class:"kinectBalance", title:"[Kinect] Equilíbrio"},
                    {class:"kinectFastHand", title:"[Kinect] Velocidade do gesto"},
                    {class:"kinectFist", title:"[Kinect] Flexão de punho"},
                    {class:"kinectTurn", title:"[Kinect] Girar corpo"},
                    {class:"kinectBody", title:"[Kinect] Inclinar corpo"}
                  ];

var leapMotionActions = [{class:"leapCloseHand", title:"[LeapMotion] Fechar mão"},
                  {class:"leapPinch", title:"[LeapMotion] Movimento de pinça"},
                  {class:"leapFist", title:"[LeapMotion] Flexionar punho"}
                ];

var keyboardMouseActions = [{class:"keyboard", title:"Teclado"},
                {class:"mouse", title:"Mouse"}
              ]

actions = actions.concat(leapMotionActions);
actions = actions.concat(keyboardMouseActions);

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


    $('[data-toggle="tooltip"]').tooltip({placement:"bottom"});
});

function addCommands(){
    cmds = $('#command');

    for (var y = 0; y < actions.length; y++){
        cmds.append("<option value='"+actions[y].class+"'>"+actions[y].title+"</option>");
    }
}

function addActions(){
    act = $('#actionModel #toKey');

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


function formToJSON(el,divClass){
    json = {};
    
    json.divClass = divClass;
    el.find(".toJSON").each(function(i,el){
        el = $(el);
        json[el.attr('id')] = el.val();

        if ($.isNumeric(json[el.attr('id')])){
            json[el.attr('id')] = parseFloat(json[el.attr('id')]);
        }

        div = el.attr("divideby");
        if (div != undefined){
            json[el.attr('id')] = parseInt(json[el.attr('id')])/parseInt(div);
        }
    });


    if ( json.toKey == "VK_MOUSEMOVE"){
        json.mouseX = parseInt(parent.find("#moveMouseSpace #x").val());
        json.mouseY = parseInt(parent.find("#moveMouseSpace #y").val());
    } else 
    if ( json.toKey == "ALERT" || json.toKey == "MESSAGE"){
        json.msg = parent.find("#msg").val();
    }

    if (el.find("#toKeyWhile").prop("checked")){
        json.toKeyWhile = json.toKey;
    }
    if (el.find("#toKeyDown").prop("checked")){
        json.toKeyDown = json.toKey;
    }
    if (el.find("#toKeyUp").prop("checked")){
        json.toKeyUp = json.toKey;
    }

    delete json.toKey;

    //Adiciona o endereço do dispositivo
    json.dev = $('#'+json.dev).val();

    return json;
}


function jsonToForm(json){
    console.log("aqui");
    $.each(json.common,function(key,val){
        el = $("#"+key);
        el.val(val);
        div = el.attr("divideby");
        if (div != undefined){
            el.val(parseFloat(val)*parseInt(div));
        }
    })


    for (var i = 0; i < json.keys.length; i++){
        option = json.keys[i];
        
        div = addCommandToMapView(option.divClass);
        div.find(".toJSON").each(function(y, el){
            el = $(el);
            el.val(option[el.attr('id')]);
            div = el.attr("divideby");
            if (div != undefined){
                el.val(parseFloat(option[el.attr('id')])*parseInt(div));
            }
        });
    }
}


function getKeyboardKeys(){

    keys = [];

    for(var i = 10; i < 36 ;i++){
        chr = i.toString(36).toUpperCase();
        keys.push({t:chr,code:chr});
    }

    keys = keys.concat([{t:"ESC",code:"VK_ESCAPE"},
    {t:"F1",code:"VK_F1"},
    {t:"F2",code:"VK_F2"},
    {t:"F3",code:"VK_F3"},
    {t:"F4",code:"VK_F4"},
    {t:"F5",code:"VK_F5"},
    {t:"F6",code:"VK_F6"},
    {t:"F7",code:"VK_F7"},
    {t:"F8",code:"VK_F8"},
    {t:"F9",code:"VK_F9"},
    {t:"F10",code:"VK_F10"},
    {t:"F11",code:"VK_F11"},
    {t:"F12",code:"VK_F12"},
    {t:"TAB",code:"VK_TAB"},
    {t:"CAPS LOCK",code:"VK_CAPITAL"},
    {t:"SHIFT (Esq)",code:"VK_LSHIFT"},
    {t:"SHIFT (Dir)",code:"VK_RSHIFT"},
    {t:"ENTER",code:"VK_RETURN"},
    {t:"CTRL",code:"VK_CONTROL"},
    {t:"ALT",code:"VK_MENU"},        
    {t:"ESPAÇO",code:"VK_SPACE"},        
    {t:"HOME",code:"VK_HOME"},        
    {t:"END",code:"VK_END"},        
    {t:"INSERT",code:"VK_INSERT"},        
    {t:"DELETE",code:"VK_DELETE"},
    {t:"Print Screen",code:"VK_SNAPSHOT"},        
    {t:"Seta cima",code:"VK_UP"},        
    {t:"Seta direita",code:"VK_RIGHT"},        
    {t:"Seta baixo",code:"VK_DOWN"},    
    {t:"Seta esquerda",code:"VK_LEFT"},    
    {t:"Mouse esquerdo (Apertar)",code:"VK_LBUTTON_DOWN"},
    {t:"Mouse esquerdo (Soltar)",code:"VK_LBUTTON_UP"},
    {t:"Mouse meio (Apertar)",code:"VK_MBUTTON_DOWN"},
    {t:"Mouse meio (Soltar)",code:"VK_MBUTTON_UP"},
    {t:"Mouse direito (Apertar)",code:"VK_RBUTTON_DOWN"},
    {t:"Mouse direito (Soltar)",code:"VK_RBUTTON_UP"}
  ]);

    for (var i = 0; i <= 9; i++){
        keys.push({t:i,code:i});
    }
    
    return keys;
  }
  
function loadKeyboardOptions(){
    act = $('#keyboardModel #key');
    keys = getKeyboardKeys();
    for(var i = 0; i < keys.length ;i++){
        act.append("<option value='"+keys[i].code+"'>"+keys[i].t+"</option>");
    }
}

function toKeyWhileVerify(el){
    el = $(el);
    if (el.attr("id") == "toKeyWhile" && el.prop("checked") ){
        el.parent().parent().find("#toKeyDown,#toKeyUp").prop("checked",false);
    } else
    if ( (el.attr("id") == "toKeyDown" || el.attr("id") == "toKeyUp") && el.prop("checked") ){
        el.parent().parent().find("#toKeyWhile").prop("checked",false);
    }
}