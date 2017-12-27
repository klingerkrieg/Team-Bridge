var fs = require('fs');
var exec = require('child_process');




/* Configurações gerais */
var programName = "ConfigEditor";
var actualName = "";


var modelIncludes = [{url:"kinectOptions.html",onLoad:function(){}},
                    {url:"leapMotionOptions.html",onLoad:function(){}},
                    {url:"keyboardMouseOptions.html",onLoad:loadKeyboardOptions},
                    {url:"teamOptions.html",onLoad:function(){}}];

var actions = [{class:"handTop", title:"[Kinect] Altura da mão"},
                    {class:"kinectWalking", title:"[Kinect] Marcha estacionária"},
                    {class:"kinectStep", title:"[Kinect] Step"},
                    {class:"kinectBalance", title:"[Kinect] Equilíbrio"},
                    {class:"kinectFastHand", title:"[Kinect] Velocidade do gesto"},
                    {class:"kinectWrist", title:"[Kinect] Flexão de punho"},
                    {class:"kinectTurn", title:"[Kinect] Girar corpo"},
                    {class:"kinectBody", title:"[Kinect] Inclinar corpo"},
                    {class:"kinectCloseHand", title:"[KinectV2] Fechar mão"},
                    {class:"kinectLasso", title:"[KinectV2] Laço"}
                  ];

var leapMotionActions = [{class:"leapCloseHand", title:"[LeapMotion] Fechar mão"},
                  {class:"leapPinch", title:"[LeapMotion] Movimento de pinça"},
                  {class:"leapWrist", title:"[LeapMotion] Flexionar punho"}
                ];

var keyboardMouseActions = [{class:"keyboard", title:"Teclado"},
                {class:"mouse", title:"Mouse"}
              ];

var teamActions = [{class:"nedGloveGrab", title:"[NEDGlove] Fechar mão"},
                    {class:"nedGlovePinch", title:"[NEDGlove] Movimento de pinça"}];

actions = actions.concat(leapMotionActions);
actions = actions.concat(keyboardMouseActions);
actions = actions.concat(teamActions);



var devicesFormClasses = ["leapMotion","kinect","keyboard","mouse","nedGlove"];



/* Fim Configurações gerais */



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


    //Ao fechar modal do ned ou leapmotion
    $('#nedGloveModal').on('hide.bs.modal', function () {
        nedGloveReturnOk = true;
        clearInterval(nedGloveInterval);
    });
    $('#leapMotionModal').on('hide.bs.modal', function () {
        leapReturnOk = true;
        clearInterval(leapInterval);
    });
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

    keys.push({t:"Mouse Botão esquerdo (Apertar e soltar)",code:"VK_LBUTTON"});
    keys.push({t:"Mouse Botão do meio (Apertar e soltar)",code:"VK_MBUTTON"});
    keys.push({t:"Mouse Botão direito (Apertar e soltar)",code:"VK_RBUTTON"});
    keys.push({t:"Definir posição",code:"KINECT_SET_CENTER_POS"});
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


function formToJSON(el){
    json = {};
    
    json.divClass = el.attr('class');
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
        json.mouseX = parseInt(el.find("#moveMouseSpace #x").val());
        console.log(json.mouseX);
        json.mouseY = parseInt(el.find("#moveMouseSpace #y").val());
    } else 
    if ( json.toKey == "ALERT" || json.toKey == "MESSAGE"){
        json.msg = el.find("#msg").val();
    }

    if (el.find("#toKeyWhile").prop("checked")){
        json.toKeyWhile = json.toKey+"";
    }
    if (el.find("#toKeyDown").prop("checked")){
        json.toKeyDown = json.toKey+"";
    }
    if (el.find("#toKeyUp").prop("checked")){
        json.toKeyUp = json.toKey+"";
    }

    if (el.find("#saveData").prop("checked")){
        json.saveData = el.find("#saveData").val();
    }

    if (json.strengthMin < 0){
        json.strengthMin = 0;
    }
    if (json.strengthMax < 0){
        json.strengthMax = 0;
    }

    delete json.toKey;
    
    return json;
}


function jsonToForm(json){
    
    $.each(json.common,function(key,val){
        el = $("#"+key);
        el.val(val);
        divBy = el.attr("divideby");
        if (divBy != undefined){
            el.val(parseFloat(val)*parseInt(divBy));
        }
    })


    for (var i = 0; i < json.keys.length; i++){
        option = json.keys[i];
        
        div = addCommandToMapView(option.divClass);
        div.find(".toJSON").each(function(y, el){
            el = $(el);
            
            //Para todos os outros campos, preenche normalmente
            el.val(option[el.attr('id')]);
            divBy = el.attr("divideby");
            if (divBy != undefined){
                el.val(parseFloat(option[el.attr('id')])*parseInt(divBy));
            }

        });

        if (option.toKeyWhile != undefined){
            div.find("#toKeyWhile").prop("checked",true);
            div.find("#toKey").val(option.toKeyWhile);
        } else {
            div.find("#toKeyWhile").prop("checked",false);
        }
        if (option.toKeyDown != undefined){
            div.find("#toKeyDown").prop("checked",true);
            div.find("#toKey").val(option.toKeyDown);
        } else {
            div.find("#toKeyDown").prop("checked",false);
        }
        if (option.toKeyUp != undefined){
            div.find("#toKeyUp").prop("checked",true);
            div.find("#toKey").val(option.toKeyUp);
        } else {
            div.find("#toKeyUp").prop("checked",false);
        }

        if (option.saveData != undefined){
            div.find("#saveData").prop("checked",true);
        } else {
            div.find("#saveData").prop("checked",false);
        }


        showExtraActions(div.find('#toKey'));
        
        if ( option.mouseX != undefined){
            div.find("#moveMouseSpace #x").val(option.mouseX);
            div.find("#moveMouseSpace #y").val(option.mouseY);
        } else 
        if ( option.msg != undefined ){
            div.find("#msg").val(option.msg);
        }
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
    act = $('.keyboard #key');
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

var nedGloveDivSelected;
var nedGloveInterval;
var nedGloveThumb;
var nedGloveIndex;
var nedGloveMode;
var nedGloveReturnOk = true;
function closeNedGloveModal(){
    $('#nedGloveModal').modal("hide");
}

function openNedGloveModal(btn, mode){
    nedGloveMode = mode;
    $('#nedGloveModal').modal("show");
    if (nedGloveMode == 'pinch'){
        $('#pinchModeDiv').show();
    } else {
        $('#pinchModeDiv').hide();
    }
    changeFingersSensors();
    nedGloveDivSelected = $(btn).parent();
    nedGloveInterval = setInterval(loadNedGlovesValuesFromSerial, 1000);
    $('#nedGloveModal #btnSaveStrDiv button').removeClass('btn-primary').addClass('btn-secondary').val(0);
    $('#nedGloveModal #btnSaveLeap button span').html("");
}

function saveStrToButton(btn){
    //Quando o botão for  clicado a classe secundaria é removida
    //A classe primária é adicionada
    //O valor da força fica fixo
    btn = $(btn);
    btn.addClass('btn-primary').removeClass('btn-secondary');
    btn.find('span').html($('#nedGloveModal #nedStr').html());
    btn.val(btn.find('span').html());
}

function saveNedGloveStr(){
    //Caso nenhum valor tenha sido escolhido o valor padrão é 0
    var minStr = $('#nedGloveModal #btnSaveStrDiv #min').val();
    var maxStr = $('#nedGloveModal #btnSaveStrDiv #max').val();
    nedGloveDivSelected.find("#strengthMin").val(minStr);
    nedGloveDivSelected.find("#strengthMax").val(maxStr);
    closeNedGloveModal();
}


function changeFingersSensors(){
    //Colore os dedos indicador e polegar
    $(".thumb").removeClass("thumb");
    $(".index").removeClass("index");

    if (nedGloveMode == 'pinch'){
        nedGloveThumb = $('#nedGloveModal #thumb').val();
        nedGloveIndex = $('#nedGloveModal #index').val();
        $('#nedGloveModal #sens'+nedGloveThumb).addClass("thumb");
        $('#nedGloveModal #sens'+nedGloveIndex).addClass("index");
        $('#nedGloveModal #thumb').addClass("thumb");
        $('#nedGloveModal #index').addClass("index");
    }
}

function loadNedGlovesValuesFromSerial(){
    //Evita várias chamadas ao software externo
    if (nedGloveReturnOk == false){
        return;
    }
    nedGloveReturnOk = false;
    exec.exec("SerialPortToString.exe -p "+$("#nedGloveModal #port").val()+" -b "+$("#nedGloveModal #bauds").val()+"", function(error, stdout, stderr) {
        nedGloveReturnOk = true;
        if (error) {
          console.log('exec error: '+error);
          return;
        }
        console.log('stdout: '+stdout);
        values = stdout.split(";");
        var strength = 0;
        //Limpa todos
        for (var i = 0; i < 5; i++){
            $("#nedGloveModal #sens"+i).html("");
        }
        for (var i = 0; i < values.length; i++){
            strength += parseInt(values[i]);
            $("#nedGloveModal #sens"+i).html(values[i]);
        }

        //Conta somente a força do indicador e polegar
        if (nedGloveMode == 'pinch'){
            strength = parseInt(values[nedGloveThumb]);
            strength += parseInt(values[nedGloveIndex]);
            strength = parseInt(250 - (strength / 2));
            $('#nedGloveModal #nedStr').html(strength);
        } else {
            strength = parseInt(250 - (strength / 5));
            $('#nedGloveModal #nedStr').html(strength);
        }
        //Aplica a força nas spans dentro dos botoes que ainda so possuem a classe secundaria
        $('#nedGloveModal #btnSaveStrDiv .btn-secondary').find('span').html(strength);
        
    });
}



/* LEAP MOTION */
var leapMotionMode;
var leapInterval;
var leapMotionDivSelected;
var leapReturnOk = true;
function openLeapMotionModal(btn, mode){
    leapMotionMode = mode;

    $('#leapMotionModal').modal("show");
    if (leapMotionMode == 'wrist'){
        $('#leapMotionModal .leapWrist').show();
        $('#leapMotionModal .leapPinch').hide();
        $('#leapMotionModal .leapCloseHand').hide();
        $('#leapMotionModal .leapCloseAndPinch').hide();
    } else
    if (leapMotionMode == 'pinch'){
        $('#leapMotionModal .leapWrist').hide();
        $('#leapMotionModal .leapPinch').show();
        $('#leapMotionModal .leapCloseHand').hide();
        $('#leapMotionModal .leapCloseAndPinch').show();
    } else {
        $('#leapMotionModal .leapWrist').hide();
        $('#leapMotionModal .leapPinch').hide();
        $('#leapMotionModal .leapCloseHand').show();
        $('#leapMotionModal .leapCloseAndPinch').show();
    }
    
    leapMotionDivSelected = $(btn).parent();
    leapInterval = setInterval(loadLeapValues, 1000);
    $('#leapMotionModal #btnSaveLeap button').removeClass('btn-primary').addClass('btn-secondary').val(0);
    $('#leapMotionModal #btnSaveLeap button span').html("");
}


function loadLeapValues(){
    if (leapReturnOk == false){
        return;
    }
    leapReturnOk = false;
    exec.exec("LeapToString.exe", function(error, stdout, stderr) {
        leapReturnOk = true;
        if (error) {
          console.log('exec error: '+error);
          return;
        }
        console.log('stdout: '+stdout);
        eval('var json = '+stdout);
        
        if (json.msg != undefined){

        } else
        if (leapMotionMode == 'wrist'){
            $('#leapMotionModal #leapWristAngle').html(json.wrist);
        } else
        if (leapMotionMode == 'pinch'){
            $('#leapMotionModal #leapPinchDistance').html(json.pinch);
        } else {
            $('#leapMotionModal #leapCloseHandAngle').html(json.close);
        }
        
    });
}

function closeLeapMotionModal(){
    $('#leapMotionModal').modal("hide");
}

function saveLeapMotionValues(){
    //Caso nenhum valor tenha sido escolhido o valor padrão é 0
    var minStr = $('#leapMotionModal #btnSaveLeap #min').val();
    var maxStr = $('#leapMotionModal #btnSaveLeap #max').val();
    leapMotionDivSelected.find("#angleMin").val(minStr);
    leapMotionDivSelected.find("#angleMax").val(maxStr);
    leapMotionDivSelected.find("#distanceMin").val(minStr);
    leapMotionDivSelected.find("#distanceMax").val(maxStr);

    var val = $('#leapMotionModal #btnSaveLeap #val').val();
    leapMotionDivSelected.find("#angle").val(val);
    closeLeapMotionModal();
}

function saveLeapValuesToButton(btn){
    //Quando o botão for  clicado a classe secundaria é removida
    //A classe primária é adicionada
    //O valor da força fica fixo
    btn = $(btn);
    btn.addClass('btn-primary').removeClass('btn-secondary');
    if (leapMotionMode == 'wrist'){
        btn.find('span').html($('#leapMotionModal #leapWristAngle').html());
    } else
    if (leapMotionMode == 'pinch'){
        btn.find('span').html($('#leapMotionModal #leapPinchDistance').html());
    } else {
        btn.find('span').html($('#leapMotionModal #leapCloseHandAngle').html());
    }
    btn.val(btn.find('span').html());
}