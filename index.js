let tether = 0;

var pig = new Prop();

var sus = new Prop();

var box = new Prop();

var susan = new Model("./susan.obj");
var red = new Shader("./shaders/av");
sus.model = susan;
sus.shader = red;

var cube = new Model("./box.obj");
var nya = new Texture("./wtf.png");
box.model = cube;
box.texture = nya;
box.texture2 = nya;

var wopes = [];

var axmax = 20;
var ax = 0;

var move_over = new Vector3(0,0,-7)

for (let i = 0; i < axmax; i++) {
  var yippe = new Prop();
  yippe.position = new Vector3(move_over.x+ax, move_over.y+ax, move_over.z+ax);
  yippe.scale = new Vector3(0.2, 0.2, 0.2);
  wopes.push(yippe);
  yippe.model = cube;
  ax++;
}

function move_pig(e){
  pig.position = new Vector3(e.x*0.001,e.y*-0.001,0)
}
addEventListener("mousemove", move_pig)

function kb(e){
  print("awawa "+e.keycode)
}
addEventListener("keydown", kb)

export function tick() {
  tether+=0.0006;
  red.shader_float("awa", tether);
  sus.shader_float("poggers", tether);

  sus.position = new Vector3(Math.sin(tether)*4, Math.cos(tether)*3, 0);
  sus.rotation = new Vector3(Math.sin(tether), 0, 0);

  box.position = new Vector3(Math.sin(tether+3)*4, Math.cos(tether+3)*3, 0);
  box.rotation = new Vector3(tether, tether*Math.sin(tether+3), tether*2);
  //pig.position = av;

  wopes.forEach((a, index) => {
    a.position = new Vector3(Math.sin(tether+index)*4, Math.cos(tether+index)*3, Math.tan(tether+index)*1+Math.sin(tether+index)*4);
    a.rotation = new Vector3(tether, tether*0.3, tether*2);
  });
  
  return tether;
}

Deno.core.print("swag\n\n");

//Deno.core.ops.add_event_listener("awa", 0);

// return "awa";


//now not 100% sure how to emplement this in deno properly
//so this is the next best thing I can think of lmao to add "global" events
export function _KE_EVENT_PUSH(name, data){
  dispatchEvent(name, data);
}