
var hack = new Font();
hack.GetFontFile("./fonts/consola.ttf");

var fps = new element();
fps.text = "Kitsune Demo :3";
fps.position = new Vector2(25.0, 70.0);
fps.scale = 0.5;
fps.text_font = hack;

Win_title("Fall elevator intro test"); //names title

var bbbb = new Prop();
var room = new Prop();
var room_model = new Model("./wall.obj");
var room_texture = new Texture("./textures/house.png");

room.model = room_model;
room.texture = room_texture;

let cube_model = new Model("./cube.obj");
var player = new Texture("./engine_dependent/missing.png");
var albido_create = new Texture("./create/container.png");
var specular_create = new Texture("./create/container_specular.png");

function radians(dgr){return dgr*(Math.PI/180);}
function degrees(rag){return rag*180/Math.PI;}

var awaowo = new Camera();
awaowo.position = new Vector3(0,1,0);
awaowo.rotation = new Vector3(0,0,0);
SetMainCam(awaowo);

var height = 435;
var max_time = 1000;
var time = 0;
lt=true;

bbbb.model = cube_model;
bbbb.texture = player;
bbbb.specular = specular_create;
print(cube_model._id);
bbbb.position = new Vector3(1.5,5,0);

let elevator_model = new Model("./whail.obj");
async function idk(spaceing, heidth){
    for(var i = 0; i <= heidth; i++){
        let create = new Prop();
        create.model = elevator_model;
        create.texture = albido_create;
        create.specular = specular_create;
        create.position = await new Vector3(0,i*spaceing,0);
        print(create.position.y);
    }
}

idk(4, 100)

function tick(delta){ //called every frame (TODO: multi thread)
    time = time+0.5;
    
    fps.text = "Level | y: "+awaowo.position.y;
    
    if(lt == true){
        if(awaowo.position.y <= 0){
            awaowo.position = new Vector3(0,0,0)
            lt=false;
        }else
            awaowo.position = new Vector3(0, height-(height*(time/max_time)), 0);
    }
}
