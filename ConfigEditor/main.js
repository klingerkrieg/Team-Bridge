var fs = require('fs');



$(function(){

    $.ajax({url:'kinectOptions.html'}).done(function(res){
        content = $(res);
        $('#models').append(content);
    });

    //Adiciona os possiveis comandos a serem utilizados
    addCommands();

    addActions();

    
    $('#addCommand').click(function(){
        clone = $('#'+$('#command').val()+'Model').clone();
        clone.attr('id',"");
        clone.append($('#actionModel #act').clone());
        clone.append('<button onclick="removeMap(this)" type="button">Remove</button>');
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
    for (var y = 0; y < kinectActions.length; y++){
        cmds.append("<option value='"+kinectActions[y].class+"'>"+kinectActions[y].title+"</option>");
    }
}

function addActions(){
    act = $('#actionModel #action');

    keys = [{t:"ESC",code:"ESC"},
    {t:"F1",code:"ESC"},
    {t:"F2",code:"ESC"},
    {t:"F3",code:"ESC"},
    {t:"F4",code:"ESC"},
    {t:"F5",code:"ESC"},
    {t:"F6",code:"ESC"},
    {t:"F7",code:"ESC"},
    {t:"F8",code:"ESC"},
    {t:"F9",code:"ESC"},
    {t:"F10",code:"ESC"},
    {t:"F11",code:"ESC"},
    {t:"F12",code:"ESC"},
    {t:"TAB",code:"ESC"},
    {t:"CAPS LOCK",code:"ESC"},
    {t:"SHIFT (Esq)",code:"ESC"},
    {t:"SHIFT (Dir)",code:"ESC"},
    {t:"ENTER",code:"ESC"},
    {t:"CTRL",code:"ESC"},
    {t:"ALT",code:"ESC"},        
    {t:"ESPAÇO",code:"ESC"},        
    {t:"HOME",code:"ESC"},        
    {t:"END",code:"ESC"},        
    {t:"Seta cima",code:"ESC"},        
    {t:"Seta direita",code:"ESC"},        
    {t:"Seta baixo",code:"ESC"},        
    {t:"Seta esquerda",code:"ESC"},
    {t:"Definir posição",code:"ESC"}
];

    for (var i = 0; i <= 9; i++){
        act.append("<option value='"+i+"'>"+i+"</option>");
    }
    for(var i = 9; i < 36 ;i++){
        chr = i.toString(36).toUpperCase();
        act.append("<option value='"+chr+"'>"+chr+"</option>");
    }
    for(var i = 0; i < keys.length ;i++){
        act.append("<option value='"+keys[i].code+"'>"+keys[i].t+"</option>");
    }
}