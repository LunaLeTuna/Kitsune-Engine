var room_size = 0;
var room = [];

//side
var wall1 = new Model("./map_objects/wall1.obj");
var room_texture = new Texture("./dev.png");

//corner
var corner1 = new Model("./map_objects/corner1.obj");
var room_texture = new Texture("./dev.png");


//door
var door1 = new Model("./map_objects/the_hole.obj");
var room_texture = new Texture("./dev.png");

//floor
var floor1 = new Model("./map_objects/floor1.obj");
var room_texture = new Texture("./dev.png");

function create_map_element(type, position, r) {
    if(!type) type = "floor";
    if(!position) position = new Vector3(0,0,0);
    let rotationc;
    if(r==0) rotationc = new Vector3(0,0,0);
    else if(r==1) rotationc = new Vector3(0,1.57,0);
    else if(r==2) rotationc = new Vector3(0,3.14,0);
    else if(r==3) rotationc = new Vector3(0,4.71,0);
    else rotationc = new Vector3(0,0,0);

    if(type=="wall"){
        room[room_size] = new Prop();
        room[room_size].model = wall1;
        room[room_size].texture = room_texture;
        room[room_size].scale = new Vector3(10,10,10);
        room[room_size].position = position;
        room[room_size].rotation = rotationc;
        room_size++;
        return;
    }
    if(type=="corner"){
        room[room_size] = new Prop();
        room[room_size].model = corner1;
        room[room_size].texture = room_texture;
        room[room_size].scale = new Vector3(10,10,10);
        room[room_size].position = position;
        room[room_size].rotation = rotationc;
        room_size++;
        return;
    }
    if(type=="door"){
        room[room_size] = new Prop();
        room[room_size].model = door1;
        room[room_size].texture = room_texture;
        room[room_size].scale = new Vector3(10,10,10);
        room[room_size].position = position;
        room[room_size].rotation = rotationc;
        room_size++;
        return;
    }
    if(type=="floor"){
        room[room_size] = new Prop();
        room[room_size].model = floor1;
        room[room_size].texture = room_texture;
        room[room_size].scale = new Vector3(10,10,10);
        room[room_size].position = position;
        room[room_size].rotation = rotationc;
        room_size++;
        return;
    }
}


// var map = [[["wall",0]]];

var map = [
    [["corner",3],["n",0],["door",0],["wall",0],["wall",0],["corner",0]],
    [["wall",3],["floor",0],["floor",0],["floor",0],["floor",0],["wall",1]],
    [["wall",3],["floor",0],["floor",0],["floor",0],["floor",0],["wall",1]],
    [["wall",3],["floor",0],["floor",0],["floor",0],["floor",0],["wall",1]],
    [["corner",2],["door",2],["n",2],["wall",2],["wall",2],["corner",1]]
]

var map_scale = 20;

function genmap(){
    var maxX = map[0].length-1;
    var maxY = map.length-1;

    for(let iy = 0; iy <= maxY; iy++){
        for(let ix = 0; ix <= maxX; ix++){
            create_map_element(map[iy][ix][0],new Vector3(iy*map_scale,0,ix*map_scale),map[iy][ix][1]);
        }
    }
}

//basic create objects
var cube_model = new Model("./cube.obj");
var albido_create = new Texture("./create/container.png");
var specular_create = new Texture("./create/container_specular.png");

//player
var player = new Prop();
player.model = cube_model;
player.texture = albido_create;
player.specular = specular_create;
print(cube_model._id);
player.position = new Vector3(1.5,5,0);
player.mass = 1;
player.create_physbody();

//player camera
var p_cam = new Camera();
p_cam.position = new Vector3(0,2,0);
p_cam.rotation = new Vector3(0,0,0);
SetMainCam(p_cam);

//player light
var p_torch = new PointLight();


//
//player movement
//
 function pivot_point(place, center, rot){
    var resultx = Math.cos(rot)*(place.x-center.x)-Math.sin(rot)*(place.y-center.y)+center.x
    var resulty = Math.sin(rot)*(place.x-center.x)+Math.cos(rot)*(place.y-center.y)+center.y
    return {"x": resultx, "y": resulty};
 } 

  var w = false;
  var s = false;
  var a = false;
  var d = false;
  
  function buttonpushedpog(event){
    var keyCode = event;

    if (keyCode == 87) {
        w=true;
    } else if (keyCode == 83) {
        s=true;
    } else if (keyCode == 65){
        a=true;
    } else if (keyCode == 68){
        d=true;
    }
    //awaowo.position = new Vector3(0, 2, wobbo);
}

function buttonthedpog(event){
    var keyCode = event;

    if (keyCode == 87) {
        w=false;
    }

    if (keyCode == 83) {
        s=false;
    }

    if (keyCode == 65){
        a=false;
    }

    if (keyCode == 68){
        d=false;
    }
    //awaowo.position = new Vector3(0, 2, wobbo);
}

Input.addEventListener("keypress", buttonpushedpog);
Input.addEventListener("keyrelease", buttonthedpog);

var firstMouse = true;
var yaw   = -90.0;
var pitch =  0.0;
var lastX =  800.0 / 2.0;
var lastY =  600.0 / 2.0;

function radians(dgr){return dgr*(Math.PI/180);}
function degrees(rag){return rag*180/Math.PI;}

var xpos;
var ypos;

function mousty(event){

    //print("mouse x: " + event.xpos);
    //print("mouse y: " + event.ypos);
    xpos = event.xpos
    ypos = event.ypos

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    let xoffset = xpos - lastX;
    let yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    let sensitivity = 0.1; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch -= yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0)
        pitch = 89.0;
    if (pitch < -89.0)
        pitch = -89.0;

    yaw = yaw%360;
    pitch = pitch%360;

    p_cam.rotation = new Vector3(pitch,yaw,0);
}

Input.addEventListener("mousemove", mousty);


CursorPin(true);

var speed = 0.2;
var maxspeed = 4;

var monark = false;

var sxsss = 0;

function tick(delta){

    if(monark==false){
        genmap();
        monark=true;
    }

    p_cam.position = new Vector3(player.position.x, player.position.y+2, player.position.z);
    p_torch.position = new Vector3(player.position.x, player.position.y, player.position.z);

    let cam_rot = p_cam.rotation.y;

    let current = {x:0,y:0};

    let temp = {x:0,y:0};

    if (w) {
        temp.y-=speed;
    } 
    if (s) {
        temp.y+=speed;
    }
    if (a){
        temp.x-=speed;
    }
    if (d){
        temp.x+=speed;
    }

    if((player.LinearVelocity.x < maxspeed || player.LinearVelocity.z < maxspeed))
    if((player.LinearVelocity.x > -maxspeed || player.LinearVelocity.z > -maxspeed))
    if(((w||s)||(a||d))){
        var new_place = pivot_point(temp, current, cam_rot);
        player.AddForce(new Vector3(new_place.x,0,new_place.y));
    }

    //print("x:"+player.position.x+" y:"+ player.position.y+" z:"+ player.position.z);
    //print("x:"+p_torch.position.x+" y:"+ p_torch.position.y+" z:"+ p_torch.position.z);
}