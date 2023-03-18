let tether = 0;

var pig = new Prop();

export function tick() {
  tether+=0.001;


  pig.position = new Vector3(Math.sin(tether), Math.cos(tether), 0);
  
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
