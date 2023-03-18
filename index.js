let tether = 0;

var pig = new ops.create_prop();

export function tick() {
  tether+=0.001;

  let av ={x:Math.sin(tether), y:0, z:0};

  Deno.core.ops.mod_prop_pos(pig, av);
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
