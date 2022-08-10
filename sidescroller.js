// //TODO: make obj reader take in faces with <=4 vectors
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
var player = new Texture("./engine_dependent/missing.png");
var albido_create = new Texture("./create/container.png");
var specular_create = new Texture("./create/container_specular.png");

create.model = cube_model;
create.texture = albido_create;
create.specular = specular_create;
print(cube_model._id);
create.mass = 5;
create.create_physbody();

bbbb.model = cube_model;
bbbb.texture = player;
bbbb.specular = specular_create;
print(cube_model._id);
bbbb.position = new Vector3(1.5,5,0);
bbbb.AddForce(new Vector3(0,5,0));
bbbb.mass = 1;
bbbb.create_physbody();

var w = false;
var s = false;
var a = false;
var d = false;

var speed = 0.5;
var maxspeed = 13;

function buttonpushedpog(event){
    var keyCode = event;

    if (keyCode == 87) {
        bbbb.AddForce(new Vector3(0,10,0));
    } else if (keyCode == 65){
        a=true;
    } else if (keyCode == 68){
        d=true;
    }
    //awaowo.position = new Vector3(0, 2, wobbo);
}

function buttonthedpog(event){
    var keyCode = event;

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
awaowo.position = new Vector3(0,0,0);
awaowo.rotation = new Vector3(0,0,0);
SetMainCam(awaowo);

var xpos;
var ypos;

var bza = false;
var time = 0;
var walled = false; //tells us if wall has already exists
function tick(delta){ //called every frame (TODO: multi thread)
    time = time+0.01;
    
    create.position = new Vector3(0+Math.cos(time/2)*6, 1.56, 0+Math.sin(time/2)*6);

    awaowo.position = new Vector3(bbbb.position.x, bbbb.position.y, 10);

    fps.text = "volocity | x: "+bbbb.LinearVelocity.x+" | y: "+bbbb.position.y+"\nbox | x: "+bbbb.position.x+" | y: "+bbbb.position.y;

    if(bbbb.LinearVelocity.x > maxspeed){
        bbbb.LinearVelocity = new Vector3(maxspeed, bbbb.LinearVelocity.y, 0);
    }

    if(bbbb.LinearVelocity.x < -maxspeed){
        bbbb.LinearVelocity = new Vector3(-maxspeed, bbbb.LinearVelocity.y, 0);
    }

    if(bbbb.position.y > maxspeed){
        bbbb.LinearVelocity = new Vector3(bbbb.LinearVelocity.x, -maxspeed, 0);
    }

    if(bbbb.position.y < -maxspeed){
        bbbb.LinearVelocity = new Vector3(bbbb.LinearVelocity.x, maxspeed, 0);
    }
    if(bbbb.position.z !== 0){
         bbbb.position = new Vector3(bbbb.position.x, bbbb.position.y, 0);
            
    }

//     var cc = RayCast(new Vector3(0,5,0),new Vector3(0,0,0));
//     if(cc.hasHit){
//         print(cc.position.y);
//     }

    var xsa = 0;
    var ysa = 0;

//     if (s) {
//         ysa=speed;
//     }
    if (a){
        xsa=-speed;
    }
    if (d){
        xsa=speed;
    }
    bbbb.AddForce(new Vector3(xsa,ysa,0));

    if(time>8&&bza==false){
        //
        bza=true;
        print("awa ^^"+cube_model._id);
        var bbbbb = new Prop();
        bbbbb.model = cube_model;
        bbbbb.texture = albido_create;
        bbbbb.specular = specular_create;
        bbbbb.position = new Vector3(0,0,6);
        bbbbb.mass = 1;

        bbbbb.create_physbody();

    }

    return;
}
