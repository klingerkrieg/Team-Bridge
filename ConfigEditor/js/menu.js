$(function(){

    $('#addCommand').click(function(){
        addCommandToMapView($('#command').val());
        enableSave();
    });

    $("#open").click(function(){
        loadFiles();
    });

    $("#openFile").click(function(){
        clear();
        var json = JSON.parse(fs.readFileSync($("#files").val(), 'utf8'));
        jsonToForm(json);
        actualName = $("#files").val();
        $('title').html(programName + ":" + actualName);
        $('#openModal').modal("hide");
        enableSave();
    });

    $('#new').click(function(){
        clear();
    });

    $("#openSave").click(function(){
        if (actualName == ""){
            $('#fileNameModal').modal("show");
        } else {
            saveFile();
        }
    });

    $("#openSaveAs").click(function(){
        $('#fileNameModal').modal("show");
        
    });

    $('#fileName').keypress(function(evt){
        if (evt.which == 13){
            actualName = $('#fileName').val() + ".json";
            saveFile();
        }
    });

    $('#save').click(function(){
        actualName = $('#fileName').val() + ".json";
        saveFile();
    });

    $('#fileNameModal').on('shown.bs.modal', function () {
        $('#fileName').trigger('focus');
    });
});


function clear(){
    $("title").html(programName);
    $('#mapView').html("");
    enableSave();
    actualName = "";
    $('#commonInfo')[0].reset();
    $('#msgOk').remove();
}


function saveFile(){
    var jsonConfig = {};
    jsonConfig.common = {};
    jsonConfig.keys = [];


    $('#commonInfo .toJSON').each(function(i,el){
        el = $(el);
        jsonConfig.common[el.attr('id')] = el.val();
        
        div = el.attr("divideby");
        if (div != undefined){
            jsonConfig.common[el.attr('id')] = parseInt(jsonConfig.common[el.attr('id')])/parseInt(div);
        }
    });


    $('#mapView').children().each(function(i,item){
        item = $(item);
        jsonConfig.keys.push(formToJSON(item));

    });
    
    
    jsonStr = JSON.stringify(jsonConfig,null,2);
    
    fs.writeFile(actualName, jsonStr, 'utf8', function(msg){
        
        if (msg != null){

            objMsg = $('<div id="msgFalha" class="alert alert-danger alert-dismissible fade show" role="alert">'
                    + msg +'<button type="button" class="close" data-dismiss="alert" aria-label="Close"><span aria-hidden="true">&times;</span></button></div>');
            $('#fileNameModal').find('#msgFalha').remove();
            $('#fileNameModal').find('.modal-body').append(objMsg);
            actualName = "";
        } else {
            $('#msgFalha').remove();
            $('#fileNameModal').modal("hide");
            $('title').html(programName + ":" + actualName);

            objMsg = '<div id="msgOk" class="alert alert-success alert-dismissible fade show" role="alert">'
            + "Arquivo salvo:" + actualName +'<button type="button" class="close" data-dismiss="alert" aria-label="Close"><span aria-hidden="true">&times;</span></button></div>';
            $('#msgOk').remove();
            $('#msgs').after(objMsg);
            enableSave();
        }
    });
    
}

function enableSave(){
    if ($('#mapView').children().length > 0){
        $('#openSave').attr("disabled",false);
        if (actualName != ""){
            $("#openSaveAs").attr("disabled",false);
        }
    } else {
        $('#openSave, #openSaveAs').attr("disabled",true);
    }
}

function loadFiles(){
    fs.readdir(".", function(err, file){
        files = $('#files');
        files.html("");
        var qtd = 0;
        for (var i = 0; i < file.length; i++){
            if (file[i].search(".json") > -1 && file[i] != "package.json"){
                files.append("<option>"+file[i]+"</option>");
                qtd++;
            }
        }
        if (qtd == 0){
            $('#openFile').attr("disabled",true);
        } else {
            $('#openFile').attr("disabled",false);
        }
    });
}

function addCommandToMapView(modelClass){
    clone =  $('#models').find("."+modelClass).clone();
    clone.append($('#actionModel #act').clone());
    clone.append('<img class="close" src="./img/close.png" onclick="removeMap(this)"/>');
    clone.find("#toKey option:eq(1)").attr("selected","");
    $('#mapView').append(clone);
    $('[data-toggle="tooltip"]').tooltip({animated: 'fade',html: true});
    
    
    return clone;
}


function removeMap(el){
    $(el).parent().remove();
    $('.tooltip').remove();
    enableSave();
}
