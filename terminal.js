
function mapKeyPressToActualCharacter(isShiftKey, characterCode) {
    if (typeof isShiftKey != "boolean" || typeof characterCode != "number") {
        return false;
    }
    var characterMap = [];
    characterMap[192] = "~";
    characterMap[49] = "!";
    characterMap[50] = "@";
    characterMap[51] = "#";
    characterMap[52] = "$";
    characterMap[53] = "%";
    characterMap[54] = "^";
    characterMap[55] = "&";
    characterMap[56] = "*";
    characterMap[57] = "(";
    characterMap[48] = ")";
    characterMap[45] = "_";
    characterMap[61] = "+";
    characterMap[91] = "{";
    characterMap[93] = "}";
    characterMap[92] = "|";
    characterMap[59] = ":";
    characterMap[39] = "\"";
    characterMap[44] = "<";
    characterMap[46] = ">";
    characterMap[47] = "?";
    characterMap[32] = " ";
    var character = "";
    if (isShiftKey) {
        if ( characterCode >= 65 && characterCode <= 90 ) {
            character = String.fromCharCode(characterCode);
        } else {
            character = characterMap[characterCode];
        }
    } else {
        if ( characterCode >= 65 && characterCode <= 90 ) {
            character = String.fromCharCode(characterCode).toLowerCase();
        } else {
            character = String.fromCharCode(characterCode);
        }
    }
    return character;
}



var room = new Prop();
var ellie = new Prop();
ellie.scale = new Vector3(0.1,0.1,0.1);
ellie.position = new Vector3(0,0.35,0);


var create = new Prop();
create.position = new Vector3(3,1.2,3);

var hack = new Font();
hack.GetFontFile("./fonts/consola.ttf");

var fps = new element();
fps.position = new Vector2(5, innerHeight-100);
fps.text_font = hack;
fps.text_color = new Vector3(1,1,0);

function init(){ //calls when game starts
    print("terminal started!!!!"); //prints in console/terminal
    Win_title("Kit Tab"); //names title

    var room_model = new Model("./wall.obj");
    var room_texture = new Texture("./textures/house.png");

    room.model = room_model;
    room.texture = room_texture;

    var cube_model = new Model("./cube.obj");
    var albido_create = new Texture("./create/container.png");
    var specular_create = new Texture("./create/container_specular.png");

    create.model = cube_model;
    create.texture = albido_create;
    create.specular = specular_create;

    return;
}

var log = "";
var chapper = "";

var if_shift = 0;

function letgoofkeyt(event){
    var keyCode = event;

    if(if_shift && keyCode == 340) //shift
        if_shift = false;
}

function buttonpushedpog(event){
    var keyCode = event;

    chard = String.fromCharCode(keyCode);

    if(!if_shift) //shift
        chard = chard.toLowerCase();

    if(if_shift)
        chard = mapKeyPressToActualCharacter(true, keyCode);

    if(!if_shift && keyCode == 340){
        if_shift = true;
        return;
    }

    if (keyCode == 259) { //backspace
        chapper = chapper.slice(0, -1);
        return;
    }
    if (keyCode == 257) { //enter
        if(chapper == "clear"){
            log = "";
            chapper = "";
        }

        if(chapper != ""){
            log += cmd(chapper);
            chapper = "";
        }
        return;
    }

    chapper += chard;
    //print(event);
}

Input.addEventListener("keypress", buttonpushedpog);
Input.addEventListener("keyrelease", letgoofkeyt);
Input.addEventListener("keyhold", buttonpushedpog);

function pivot_point(place, center, rot){
  var resultx = Math.cos(rot)*(place.x-center.x)-Math.sin(rot)*(place.y-center.y)+center.x
  var resulty = Math.sin(rot)*(place.x-center.x)+Math.cos(rot)*(place.y-center.y)+center.y
  return {"x": resultx, "y": resulty};
}

var firstMouse = true;
var yaw   = -90.0;
var pitch =  0.0;
var lastX =  800.0 / 2.0;
var lastY =  600.0 / 2.0;
var fov   =  45.0;

var deltaTime = 0.0;
var lastFrame = 0.0;

function radians(dgr){return dgr*(Math.PI/180);}
function degrees(rag){return rag*180/Math.PI;}

var awaowo = new Camera();
awaowo.position = new Vector3(0,2,0);
awaowo.rotation = new Vector3(0,0,0);
SetMainCam(awaowo);

var time = 0;
var walled = false; //tells us if wall has already exists
function tick(delta){ //called every frame (TODO: multi thread)
    time = time+0.01;

    create.position = new Vector3(0+Math.cos(time/2)*6, 1.56, 0+Math.sin(time/2)*6);

    fps.position = new Vector2(10, innerHeight-20);

    if(Math.cos(time*10)>=0.5)
    fps.text = log+chapper+"_";
    else
    fps.text = log+chapper

    return;
}
