
function getKeyboardMouseJSON(el){
    json = {};
    json.type = "key";
 
    json.key = el.find("#from").val();
    json.toKey = el.find("#action").val();
 
    return json;
 }


var keyboardMouseActions = [{class:"keyboard",func:getKeyboardMouseJSON, title:"Teclado"},
  {class:"mouse",func:getKeyboardMouseJSON, title:"Mouse"}
]

function getKeyboardKeys(){
  keys = [{t:"ESC",code:"VK_ESCAPE"},
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
  {t:"PrtScreen",code:"VK_SNAPSHOT"},        
  {t:"Seta cima",code:"VK_UP"},        
  {t:"Seta direita",code:"VK_RIGHT"},        
  {t:"Seta baixo",code:"VK_DOWN"},        
  {t:"Seta esquerda",code:"VK_LEFT"}
];

  for (var i = 0; i <= 9; i++){
      keys.push({t:i,code:i});
  }
  for(var i = 9; i < 36 ;i++){
      chr = i.toString(36).toUpperCase();
      keys.push({t:chr,code:chr});
  }
  return keys;
}

function loadKeyboardOptions(){
  act = $('#keyboardModel #from');
  keys = getKeyboardKeys();
  for(var i = 0; i < keys.length ;i++){
      act.append("<option value='"+keys[i].code+"'>"+keys[i].t+"</option>");
  }
}