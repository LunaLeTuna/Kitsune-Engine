//TODO: make obj reader take in faces with <=4 vectors
var room = new Prop();
var ellie = new Prop();
ellie.scale = new Vector3(0.1,0.1,0.1);
ellie.position = new Vector3(0,0.35,0);


var create = new Prop();
create.position = new Vector3(3,1.2,3);

var bbbb = new Prop();

var hack = new Font();
hack.GetFontFile("./fonts/SF.ttf");

var fps = new element();
fps.text = "Kitsune Demo :3";
fps.position = new Vector2(25.0, 70.0);
fps.scale = 0.5;
fps.text_font = hack;
print("THE GAME STARTED!!!!"); //prints in console/terminal
Win_title("woppa simulator"); //names title

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
print(cube_model._id);

bbbb.model = cube_model;
bbbb.texture = albido_create;
bbbb.specular = specular_create;
print(cube_model._id);
bbbb.position = new Vector3(1.5,5,0);
bbbb.AddForce(new Vector3(0,20,0));
bbbb.create_physbody();

function buttonpushedpog(event){
    var keyCode = event;

    let cam_rot = awaowo.rotation.y;

    let current = {x:awaowo.position.x,y:awaowo.position.z};

    let temp = {x:awaowo.position.x,y:awaowo.position.z};

    if(keyCode == 259){
        bbbb.AddForce(new Vector3(0,20,0));
        print("F");
    }

    if (keyCode == 87) {
        temp.y-=0.1;
        var new_place = pivot_point(temp, current, cam_rot);
        awaowo.position = new Vector3(new_place.x, 2, new_place.y);
        CursorPin(true);
    } else if (keyCode == 83) {
        temp.y+=0.1;
        var new_place = pivot_point(temp, current, cam_rot);
        awaowo.position = new Vector3(new_place.x, 2, new_place.y);
        CursorPin(false);
    } else if (keyCode == 65){
        temp.x-=0.1;
        var new_place = pivot_point(temp, current, cam_rot);
        awaowo.position = new Vector3(new_place.x, 2, new_place.y);
    } else if (keyCode == 68){
        temp.x+=0.1;
        var new_place = pivot_point(temp, current, cam_rot);
        awaowo.position = new Vector3(new_place.x, 2, new_place.y);
    }
    //awaowo.position = new Vector3(0, 2, wobbo);
}

Input.addEventListener("keypress", buttonpushedpog);
Input.addEventListener("key", buttonpushedpog);

function pivot_point(place, center, rot){
  var resultx = Math.cos(rot)*(place.x-center.x)-Math.sin(rot)*(place.y-center.y)+center.x;
  var resulty = Math.sin(rot)*(place.x-center.x)+Math.cos(rot)*(place.y-center.y)+center.y;
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

    awaowo.rotation = new Vector3(pitch,yaw,0);
}

Input.addEventListener("mousemove", mousty);

var bza = false;
var time = 0;
var walled = false; //tells us if wall has already exists
function tick(delta){ //called every frame (TODO: multi thread)
    time = time+0.01;

    create.position = new Vector3(0+Math.cos(time/2)*6, 1.56, 0+Math.sin(time/2)*6);

    fps.text = "screen | width: "+innerWidth+" | height: "+innerHeight+"\ncurser | x: "+xpos+" | y: "+ypos+"\nbox | y: "+bbbb.position.y;
    fps.position = new Vector2(xpos, innerHeight/2);

    if(time>8&&bza==false){
        //
        bza=true;
                print("awa ^^"+cube_model._id);
        var bbbbb = new Prop();
        bbbbb.model = cube_model;
        bbbbb.texture = albido_create;
        bbbbb.specular = specular_create;
        bbbbb.position = new Vector3(0,6,0);
        bbbbb.AddForce(new Vector3(-3,0,0));


        bbbbb.create_physbody();

    }

    return;
}
