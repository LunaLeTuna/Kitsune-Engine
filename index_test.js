//TODO: make obj reader take in faces with <=4 vectors

function init(){ //calls when game starts
    print("THE GAME STARTED!!!!"); //prints in console/terminal
    Win_title("ellie simulator"); //names title

    //kinda a bug rn, but models and textures must be defined in init or I guess tick
    var room_model = new Model("./wall.obj");
    var room_texture = new Texture("./textures/house.png");
    var room = new Prop();
    room.model = room_model;
    room.texture = room_texture;

    return;
}

var dog = new Prop(); // we create a ellie named dog
var cat = new Prop(); // we create a ellie named cat

var time = 0;
var walled = false; //tells us if wall has already exists
function tick(delta){ //called every frame (TODO: multi thread)
    time = time+.1;

    dog.position = new Vector3(-5, 3+Math.cos(time), 0+Math.sin(time)); //make dog do the mario

    cat.position = new Vector3(0+Math.cos(time/2)*6, 1.56, 0+Math.sin(time/2)*6); //we spin cat
    cat.rotation = new Vector3(0, -time/2, 0); // we make cat look in the derection they are moving

    if(time >= 50&&walled==false){ //waits a few seconds and triggers; walled is to make sure it does not trigger again
        var wall = new Prop(); //creates a ellie named wall
        wall.position = new Vector3(0, 4, 0); //moves them middle of room, out of the floor
        print("zam"); //prints in console/terminal
        walled=true;
    }

    return;
}
