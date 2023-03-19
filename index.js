let tether = 0;

var pig = new Prop();

var sus = new Prop();

var box = new Model("./susan.obj");
var nya = new Texture("./wtf.png");
sus.model = box;
sus.texture = nya;

export function tick() {
  tether+=0.0001;

  sus.position = new Vector3(Math.sin(tether)*4, Math.cos(tether)*3, 0);
  sus.rotation = new Vector3(Math.sin(tether), 0, 0);
  //pig.position = av;
  
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
