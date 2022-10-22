Win_title("Blåhaj's Berry Collection"); //names title

var blahaj_tex_1 = new Texture("./bbc_textures/blahaj/b1.png");
var blahaj_tex_2 = new Texture("./bbc_textures/blahaj/p1.png");

var heart_im = new Texture("./bbc_textures/UI/heart.png");
var darke_im = new Texture("./bbc_textures/UI/darxk.png");

var is_dead = false;

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

let speed = 20; //how fast side to side movement is
let jump = 5; //how much space will add to gravity
let gravity = 0; //current effect on blahaj +up -down
let grav_pull = -0.07; //how much gravity is added to gravity every tick
let min_gravity = 5; //caps jump
let max_gravity = -5; //termenal volocity down

let player_max_health = 10;
let player_health = 10;

let hack = new Font();
hack.GetFontFile("./fonts/SF.ttf");

let sans = new Font();
sans.GetFontFile("./fonts/ComicSans.ttf");

let health = new TextElement();
health.position = new Vector2(100.0, 80.0);
health.scale = 0.5;
health.font = hack;
health.text = "Health: 10/10";

let nyanometer = new ImageElement();
nyanometer.position = new Vector2(100.0, 80.0);
nyanometer.scale = new Vector2(100.0, 100.0);
nyanometer.texture = heart_im;

var blahaj = new ImageElement();
blahaj.position = new Vector2(innerWidth/2, 70.0);
blahaj.scale = new Vector2(100.0, 50.0);
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

var dark = new ImageElement();
dark.position = new Vector2(0, 0);
dark.scale = new Vector2(0, 0);
dark.texture = darke_im;

var monark = false;

var sxsss = 0;

var Spawn_Timer = 0;
var max_Spawn_Timer = 60;

var enemyies = [];
var enemy_max = 10;

var time = 0;
function tick(delta){
    if(is_dead){
        Death_sign.position = new Vector2(innerWidth/2, innerHeight/2);
        if(Death_size < Death_target)
        Death_size+=Death_tick;
        else
        Death_size = Death_target
        Death_sign.scale = Death_size

        dark.position = new Vector2(innerWidth/2, innerHeight/2);
        dark.scale = new Vector2(innerWidth, innerHeight);
        return;
    }

    enemyies.forEach((enemy, nya) => {
        if(enemy.element.position.x<-5||enemy.element.position.x>innerWidth+5){
            enemy.element.Delete();
            enemyies.splice(nya, 1); 
            return;
        }
        if(enemy.going == "left"){
            enemy.element.position = new Vector2(enemy.element.position.x-enemy.speed,enemy.element.position.y);
        }else if(enemy.going == "right"){
            enemy.element.position = new Vector2(enemy.element.position.x+enemy.speed,enemy.element.position.y);
        }
        // print(enemy.element._id)
    });

    if(Spawn_Timer >= max_Spawn_Timer && enemyies.length <= enemy_max){
        Spawn_Timer=0;
        var temp_emy = {
            going: "", // string as left or right
            speed: 0.5,
            element: null
        };
        temp_emy.element = new ImageElement();
        if(Math.random()>0.5){
            temp_emy.going = "left";
            temp_emy.element.position = new Vector2(innerWidth,Math.random()*innerHeight);
            temp_emy.element.flipped_y = true;
        }else{
            temp_emy.going = "right"
            temp_emy.element.position = new Vector2(0,Math.random()*innerHeight);
        }
        temp_emy.element.texture = blahaj_tex_2;
        enemyies.push(temp_emy);
    }else{
        Spawn_Timer++;
    }

    if(blahaj.position.y < 0) is_dead = true;

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
    gravity=jump;
}