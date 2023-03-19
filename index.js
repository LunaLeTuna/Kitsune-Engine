let tether = 0;

var pig = new Prop();

var sus = new Prop();

var box = new Prop();

var susan = new Model("./susan.obj");
sus.model = susan;

var cube = new Model("./box.obj");
var nya = new Texture("./wtf.png");
box.model = cube;
box.texture = nya;

var wopes = [];

var width = 5;
var length = 5;
var height = 5;

var ax = 0;
var bx = 0;
var cx = 0;

var move_over = new Vector3(0,0,-7)

for (let i = 0; i < width*length*height; i++) {
  var yippe = new Prop();
  if(ax >= width) ax = 0;
  if(bx >= length) {bx = 0; ax++}
  if(cx >= height) {cx = 0; bx ++}
  yippe.position = new Vector3(move_over.x+ax, move_over.y+bx, move_over.z+cx);
  yippe.scale = new Vector3(0.3, 0.3, 0.3);
  wopes.push(yippe);
  yippe.model = cube;
  cx++;
}

export function tick() {
  tether+=0.0001;

  sus.position = new Vector3(Math.sin(tether)*4, Math.cos(tether)*3, 0);
  sus.rotation = new Vector3(Math.sin(tether), 0, 0);

  box.position = new Vector3(Math.sin(tether+3)*4, Math.cos(tether+3)*3, 0);
  box.rotation = new Vector3(tether, tether*Math.sin(tether+3), tether*2);
  //pig.position = av;

  wopes.forEach(a => {
    a.rotation = new Vector3(tether, tether*Math.sin(tether+3), tether*2);
  });
  
  return tether;
}

export function tick2() {
  tether++;

  return `loop ${tether}`;
}

export function tick3() {
  tether++;

  return `loop ${tether}`;
}

Deno.core.print("swag\n\n");

// return "awa";
