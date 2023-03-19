let tether = 0;

var pig = new Prop();

var sus = new Prop();

var box = new Model("./susan.obj");
sus.model = box;

export function tick() {
  tether+=0.001;

  sus.position = new Vector3(Math.sin(tether), Math.cos(tether), 0);
  sus.rotation = new Vector3(Math.sin(tether)/2, 0, 0);
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
