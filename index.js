Win_title("Blåhaj's Berry Collection"); //names title

var blahaj_tex_1 = new Texture("./bbc_textures/blahaj/b1.png");
var blahaj_tex_2 = new Texture("./bbc_textures/blahaj/p1.png");
var blahaj_tex_3 = new Texture("./bbc_textures/blahaj/p2.png");

var heart_im = new Texture("./bbc_textures/UI/heart.png");
var darke_im = new Texture("./bbc_textures/UI/darxk.png");

var enemy_textures = [];
var i = 1;
while(i !== 10){
    var tt = new Texture("./bbc_textures/enemies/L"+i+".png");
    enemy_textures.push(tt);
    i++;
}

var shader_frames = [];
var i = 0;
while(i !== 52){
    let awanya = i.toString().padStart(4, "0");
    var tt = new Texture("./bbc_textures/shatter/"+awanya+".png");
    shader_frames.push(tt);
    i++;
}

var bbc_shaders = new Shader("./bbc_shaders/normal_animate");

var numa = new ImageElement();
numa.position = new Vector2(0, 0);
numa.scale = new Vector2(0, 0);
numa.shader = bbc_shaders;
numa.texture = blahaj_tex_1;
numa.texture1 = shader_frames[0];
var on_shatter_frame = 0;
var frame_wait = 0;
var max_frame_wait = 5;
var is_shatter_done = false;

var is_dead = false;
var waiting_start = true;

  var w = false;
  var s = false;
  var a = false;
  var d = false;
  var space = false;
  
  function buttonpushedpog(event){
    var keyCode = event;

    if (keyCode == 87) 
        w=true;
    
    if (keyCode == 83) 
        s=true;

    if (keyCode == 65)
        a=true;

    if (keyCode == 68)
        d=true;

    if (keyCode == 32)
        on_space();

}

function buttonthedpog(event){
    var keyCode = event;

    if (keyCode == 87) 
        w=false;

    if (keyCode == 83) 
        s=false;

    if (keyCode == 65)
        a=false;

    if (keyCode == 68)
        d=false;

}

Input.addEventListener("keypress", buttonpushedpog);
Input.addEventListener("keyrelease", buttonthedpog);

function radians(dgr){return dgr*(Math.PI/180);}
function degrees(rag){return rag*180/Math.PI;}


CursorPin(false);

let speed = 10; //how fast side to side movement is
let jump = 3.5; //how much space will add to gravity
let gravity = 0; //current effect on blahaj +up -down
let grav_pull = -0.05; //how much gravity is added to gravity every tick
let min_gravity = 5; //caps jump
let max_gravity = -5; //termenal volocity down

let player_max_health = 10;
let player_health = 10;

let hack = new Font();
hack.GetFontFile("./fonts/SF.ttf");

let sans = new Font();
sans.GetFontFile("./fonts/ComicSans.ttf");

var blaheal = 5;
var blahealmax = 5;

let health = new TextElement();
health.position = new Vector2(100.0, 80.0);
health.scale = 0.5;
health.font = hack;
health.text = "Health: "+blaheal+"/"+blahealmax;

let nyanometer = new ImageElement();
nyanometer.position = new Vector2(100.0, 80.0);
nyanometer.scale = new Vector2(100.0, 100.0);
nyanometer.texture = heart_im;

var blahaj = new ImageElement();
blahaj.position = new Vector2(innerWidth/2, 70.0);
blahaj.scale = new Vector2(150.0, 100.0);
blahaj.texture = blahaj_tex_1;


let Death_sign = new TextElement();
Death_sign.position = new Vector2(-100.0, -100.0);
Death_sign.scale = 0;
Death_sign.font = sans;
Death_sign.text = "YOU DIED";
Death_sign.color = new Vector3(1,0,0);

var Death_size = 0;
var Death_tick = 0.005;
var Death_target = 3;

let start_test = new TextElement();
start_test.position = new Vector2(innerWidth/2, innerHeight/2-140);
start_test.scale = 1;
start_test.font = hack;
start_test.text = "Space To Start";
start_test.color = new Vector3(1,1,1);

var dark = new ImageElement();
dark.position = new Vector2(0, 0);
dark.scale = new Vector2(0, 0);
dark.texture = darke_im;

var monark = false;

var sxsss = 0;

var Spawn_Timer = 0;
var max_Spawn_Timer = 66;

var ents = [];
var enemy_max = 30;


async function Deathamon(){
    numa.position = blahaj.position;
    numa.scale = new Vector2(blahaj.scale.x*2, blahaj.scale.y*2);
    numa.flipped_y = blahaj.flipped_y;
    health.text = "health 0/"+blahealmax;
    return is_dead=true;
}

var time = 0;
function tick(delta){

    if(waiting_start){
        blahaj.position = new Vector2(innerWidth/2, innerHeight/2);
        start_test.position = new Vector2(innerWidth/2, innerHeight/2-140);
        return;
    }

    if(is_dead){
        Death_sign.position = new Vector2(innerWidth/2, innerHeight/2);
        if(Death_size < Death_target)
        Death_size+=Death_tick;
        else
        Death_size = Death_target
        Death_sign.scale = Death_size

        dark.position = new Vector2(innerWidth/2, innerHeight/2);
        dark.scale = new Vector2(innerWidth, innerHeight);

        if(!is_shatter_done)
        if(frame_wait >= max_frame_wait){
            blahaj.scale = new Vector2(0,0);
            frame_wait=0;
            if(on_shatter_frame >= shader_frames.length-1){
                is_shatter_done=true;
            }else{
                on_shatter_frame++
                numa.texture1 = shader_frames[on_shatter_frame];
            }
            
        }else
        frame_wait++

        return;
    }

    health.text = "health "+blaheal+"/"+blahealmax;

    ents.forEach((ent, nya) => {
        if(ent.element.position.x<-5||ent.element.position.x>innerWidth+5){
            ent.element.Delete();
            ents.splice(nya, 1); 
            return;
        }

        var a = blahaj.position.x - ent.element.position.x;
        var b = blahaj.position.y - ent.element.position.y;

        if(Math.sqrt( a*a + b*b )  < 30){
            blaheal--;
            ent.element.Delete();
            ents.splice(nya, 1); 
            return;
        }

        if(blaheal == 0){ 
            Deathamon();
        }

        if(ent.going == "left"){
            ent.element.position = new Vector2(ent.element.position.x-ent.speed,ent.element.position.y);
        }else if(ent.going == "right"){
            ent.element.position = new Vector2(ent.element.position.x+ent.speed,ent.element.position.y);
        }
        // print(ent.element._id)
    });

    if(Spawn_Timer >= max_Spawn_Timer && ents.length <= enemy_max){
        Spawn_Timer=0;
        var temp_emy = {
            going: "", // string as left or right
            speed: 0.5,
            type: "enemy",
            element: null
        };
        temp_emy.element = new ImageElement();
        let min = Math.ceil(0);
        let max = Math.floor(8);
        var randomx = Math.floor(Math.random()*(max-min+1))+min;
        temp_emy.element.texture = enemy_textures[randomx];
        temp_emy.element.scale = new Vector2(60, 60);
        if(Math.random()>0.5){
            temp_emy.going = "left";
            temp_emy.element.position = new Vector2(innerWidth,Math.random()*innerHeight);
        }else{
            temp_emy.going = "right"
            temp_emy.element.position = new Vector2(0,Math.random()*innerHeight);
            temp_emy.element.flipped_y = true;
        }
        // temp_emy.element.texture = enemy_textures[Math.round(Math.random())*enemy_textures.length];
        ents.push(temp_emy);
    }else{
        Spawn_Timer++;
    }

    if(blahaj.position.y < 0) Deathamon();

    time = time+0.01;

    if(blahaj.position.x < 0-5) blahaj.position = new Vector2(innerWidth,blahaj.position.y);
    if(blahaj.position.x > innerWidth+5) blahaj.position = new Vector2(0,blahaj.position.y);

    if(min_gravity <= gravity) gravity = min_gravity;
    if(max_gravity >= gravity) gravity = max_gravity;
    
    gravity+=grav_pull;

    var beet = (Math.sin(time*3)*30)+120;
    // nyanometer.position = new Vector2(100.0-(beet/2), 100.0-(beet/2));
    nyanometer.scale = new Vector2(beet, beet);

    // blahaj.position = new Vector2(innerWidth/2, innerHeight/2);

    let current = {x:0,y:0};

    let temp = {x:0,y:0};

    if (w) {
        null
    } 
    if (s) {
        temp.y-=speed;
    }
    if (a){
        temp.x-=speed;
        blahaj.flipped_y = true;
    }
    if (d){
        temp.x+=speed;
        blahaj.flipped_y = false;
    }

    blahaj.position = new Vector2(blahaj.position.x+temp.x,blahaj.position.y+gravity);
}

function on_space(){
    if(waiting_start){
        waiting_start=false;
        start_test.position = new Vector2(0,0);
        start_test.scale=0;
    }
    if(!is_dead)
    gravity=jump;
}