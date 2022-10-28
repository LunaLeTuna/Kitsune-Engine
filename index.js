Win_title("Blåhaj's Berry Collection"); //names title

var blahaj_tex_1 = new Texture("./bbc_textures/blahaj/b1.png");
var blahaj_tex_2 = new Texture("./bbc_textures/blahaj/p1.png");

var heart_im = new Texture("./bbc_textures/UI/heart.png");
var darke_im = new Texture("./bbc_textures/UI/darxk.png");

var enemy_textures = [];
var i = 1;
while(i !== 10){
    var tt = new Texture("./bbc_textures/enemies/L"+i+".png");
    enemy_textures.push(tt);
    i++;
}

var berry_textures = [];
var i = 1;
while(i !== 9){
    var tt = new Texture("./bbc_textures/berries/B"+i+".png");
    berry_textures.push(tt);
    i++;
}

var powerup_textures = [];
var i = 1;
while(i !== 3){
    var tt = new Texture("./bbc_textures/powerup/po"+i+".png");
    powerup_textures.push(tt);
    i++;
}

var shader_frames = [];
var i = 0;
while(i !== 29){
    let awanya = i.toString().padStart(4, "0");
    var tt = new Texture("./bbc_textures/shatter/"+awanya+".png");
    shader_frames.push(tt);
    i++;
}

var cub_frames = [];
var i = 0;
while(i !== 53){
    let awanya = i.toString().padStart(4, "0");
    var tt = new Texture("./bbc_textures/cube/"+awanya+".png");
    cub_frames.push(tt);
    i++;
}

var bbc_shaders = new Shader("./bbc_shaders/normal_animate");

var numa = new ImageElement();
numa.position = new Vector2(0, 0);
numa.scale = new Vector2(0, 0);
numa.shader = bbc_shaders;
numa.texture = blahaj_tex_1;
numa.texture1 = shader_frames[0];
var on_animation_frame = 0;
var frame_wait = 0;
var max_frame_wait = 5;
var is_animation_done = false;

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

    if (keyCode == 340)
        use_powerup();

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

var blascore = 0;

let score = new TextElement();
score.position = new Vector2(5.0, innerHeight-25);
score.scale = 0.5;
score.font = hack;
score.text = "score: "+blascore;
score.alignX = "left";
score.alignY = "top";

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

/*
0 = non
1 = nya star
2 = cube
*/
var power_up = 0;
var last_power_up = 0;
var power_up_timer = 0;
var powerup_setups = [
function() {

    blahaj.shaderDefault();
    blahaj.texture = blahaj_tex_1;
    power_up = 0;
    power_up_timer=0;
},

function() {
    blahaj.shaderDefault();
    blahaj.texture = blahaj_tex_2;
    power_up = 1;
    power_up_timer=500;
},

function() {
    blahaj.shader = bbc_shaders;
    blahaj.texture1 = cub_frames[0];
    power_up = 2;
    power_up_timer=500;
}

]



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
    on_animation_frame=0;
    frame_wait=0;
    max_frame_wait=5;
    numa.position = blahaj.position;
    numa.scale = new Vector2(blahaj.scale.x*2, blahaj.scale.y*2);
    numa.flipped_y = blahaj.flipped_y;
    health.text = "health 0/"+blahealmax;
    return is_dead=true;
}

var time = 0;
function tick(delta){

    score.position = new Vector2(5.0, innerHeight-25);

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

        if(!is_animation_done)
        if(frame_wait >= max_frame_wait){
            blahaj.scale = new Vector2(0,0);
            frame_wait=0;
            if(on_animation_frame >= shader_frames.length-1){
                is_animation_done=true;
            }else{
                on_animation_frame++
                numa.texture1 = shader_frames[on_animation_frame];
            }
            
        }else
        frame_wait++

        return;
    }

    if(power_up !== last_power_up){
        print("owo");
        powerup_setups[power_up]();
        last_power_up = power_up;
    }

    if(power_up !== 0){
        if(power_up==2){
            if(frame_wait >= max_frame_wait){
                frame_wait=0;
                if(on_animation_frame >= cub_frames.length-1){
                    on_animation_frame=1;
                    blahaj.texture1 = cub_frames[on_animation_frame];
                }else{
                    on_animation_frame++
                    blahaj.texture1 = cub_frames[on_animation_frame];
                }
                
            }else
            frame_wait++
        }

        if(power_up_timer > 0 && power_up !== 0){
            power_up_timer--;
        }else{
            power_up=0;
        }
    }

    score.text = "score: "+blascore;
    health.text = "health "+blaheal+"/"+blahealmax;

    ents.forEach((ent, nya) => {
        if(ent.element.position.x<-5||ent.element.position.x>innerWidth+5){
            ent.element.Delete();
            ents.splice(nya, 1); 
            return;
        }

        if(ent.element.position.y<-5||ent.element.position.y>innerHeight+5){
            ent.element.Delete();
            ents.splice(nya, 1); 
            return;
        }

        var a = blahaj.position.x - ent.element.position.x;
        var b = blahaj.position.y - ent.element.position.y;

        if(Math.sqrt( a*a + b*b )  < 30 && ent.type == "enemy"){
            if(power_up !== 0) blaheal--;
            ent.element.Delete();
            ents.splice(nya, 1); 
            return;
        }

        if(Math.sqrt( a*a + b*b )  < 30 && ent.type == "fruit"){
            blascore++;
            if(blaheal<blahealmax && Math.random()<0.1)blaheal++;
            ent.element.Delete();
            ents.splice(nya, 1); 
            return;
        }

        if(Math.sqrt( a*a + b*b )  < 30 && ent.type == "powerup"){
            blascore++;
            power_up=ent.power_up;
            if(blaheal<blahealmax && Math.random()<0.33)blaheal++;
            ent.element.Delete();
            ents.splice(nya, 1); 
            return;
        }

        if(blaheal == 0){ 
            Deathamon();
        }

        if(ent.going == "left"){
            return ent.element.position = new Vector2(ent.element.position.x-ent.speed,ent.element.position.y);
        }
        if(ent.going == "right"){
            return ent.element.position = new Vector2(ent.element.position.x+ent.speed,ent.element.position.y);
        }
        if(ent.going == "up"){
            return ent.element.position = new Vector2(ent.element.position.x,ent.element.position.y+ent.speed);
        }
        if(ent.going == "down"){
            return ent.element.position = new Vector2(ent.element.position.x,ent.element.position.y-ent.speed);
        }
    });

    if(Spawn_Timer >= max_Spawn_Timer && ents.length <= enemy_max){
        Spawn_Timer=0;
        let temp_emy = {
            going: "", // string as left or right
            speed: 0.5,
            type: "enemy",
            element: null
        };
        temp_emy.element = new ImageElement();
        let min = Math.ceil(0);
        let max = Math.floor(enemy_textures.length-1);
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
    
    if(Spawn_Timer >= max_Spawn_Timer && ents.length <= enemy_max){
        let temp_emy = {
            going: "", // string as up or down
            speed: 0.5,
            type: "fruit",
            element: null
        };
        temp_emy.element = new ImageElement();
        let min = Math.ceil(0);
        let max = Math.floor(berry_textures.length-1);
        var randomx = Math.floor(Math.random()*(max-min+1))+min;
        temp_emy.element.texture = berry_textures[randomx];
        temp_emy.element.scale = new Vector2(60, 60);
        if(Math.random()>0.5){
            temp_emy.going = "down";
            temp_emy.element.position = new Vector2(Math.random()*innerWidth,innerHeight);
        }else{
            temp_emy.going = "up"
            temp_emy.element.position = new Vector2(Math.random()*innerWidth,0);
        }
        // temp_emy.element.texture = enemy_textures[Math.round(Math.random())*enemy_textures.length];
        ents.push(temp_emy);
    }

    if(Math.random() < 3 && power_up == 0)
    if(Spawn_Timer >= max_Spawn_Timer && ents.length <= enemy_max){
        let min = Math.ceil(0);
        let max = Math.floor(powerup_textures.length-1);
        var randomx = Math.floor(Math.random()*(max-min+1))+min;
        let temp_emy = {
            going: "", // string as up or down
            speed: 0.5,
            power_up: randomx+1,
            type: "powerup",
            element: null
        };
        temp_emy.element = new ImageElement();
        temp_emy.element.texture = powerup_textures[randomx];
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
        gravity=-4;
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

function use_powerup(){
    if(power_up==0) return;

    if(power_up==1){
        let temp_emy = {
            going: "", // string as up or down
            speed: 15,
            type: "bullet",
            element: null
        };
        temp_emy.element = new ImageElement();
        temp_emy.element.texture = heart_im;
        temp_emy.element.scale = new Vector2(60, 60);
        temp_emy.element.position = blahaj.position;
        if(blahaj.flipped_y){
            temp_emy.going = "left";
        }else{
            temp_emy.going = "right"
            temp_emy.element.flipped_y = true;
        }
        ents.push(temp_emy);
        return;
    }
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
