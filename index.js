//TODO: make obj reader take in faces with <=4 vectors
var room = new Prop();
function init(){ //calls when game starts
    print("THE GAME STARTED!!!!"); //prints in console/terminal
    Win_title("woppa simulator"); //names title

    var room_model = new Model("./wall.obj");
    var room_texture = new Texture("./textures/house.png");

    room.model = room_model;
    room.texture = room_texture;

    return;
}

// var dog = new Prop(); // we create a ellie named dog
// var cat = new Prop(); // we create a ellie named cat

var wobbo = 0;
function buttonpushedpog(event){
    var keyCode = event;

    if (keyCode == 87) {
        wobbo++;
    } else if (keyCode == 83) {
        wobbo--;
    }
}

Input.addEventListener("key", buttonpushedpog);

function radians(dgr){return dgr*(Math.PI/180);}
function degrees(rag){return rag*180/Math.PI;}

var awaowo = new Camera();
awaowo.position = new Vector3(0,2,0);
SetMainCam(awaowo);

var time = 0;
var walled = false; //tells us if wall has already exists
function tick(delta){ //called every frame (TODO: multi thread)
    time = time+1;

        awaowo.position = new Vector3(wobbo, 2, 0);
        //var poc = new Vector3(0+Math.cos(time/2)*6, 2, 0+Math.sin(time/2)*6);
        //awaowo.position = poc;

        awaowo.rotation = new Vector3(0,0,0);
        //awaowo.LookAt(new Vector3(0,2,0));
        //room.rotation = new Vector3(0, -time/4, 0);

//     cat.position = new Vector3(0+Math.cos(time/2)*6, 1.56, 0+Math.sin(time/2)*6); //we spin cat
//     cat.rotation = new Vector3(0, -time/2, 0); // we make cat look in the derection they are moving

    return;
}
