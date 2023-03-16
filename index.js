let tether = 0;

var pig = new Deno.core.ops.create_prop();

var nya = new Deno.core.ops.create_vec3(1,0,-6);

export function tick() {
  tether++;
  nya.x=Math.sin(tether*0.001)*2;
  nya.y=Math.cos(tether*0.001);

  Deno.core.ops.mod_prop_pos(pig, nya);

  // return {
  //   swag: "wicked",
  //   tether,
  // };
  Deno.core.print(`${nya.x} ${nya.y} ${nya.z}\n`);
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
