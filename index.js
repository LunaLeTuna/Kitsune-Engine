const core = Deno.core;
const ops = core.ops;

class Vector3 {
  constructor(x,y,z) {
      this.x = x;
      this.y = y;
      this.z = z;
  }
}

class Prop {
  set position(vec3i){
      ops.mod_prop_pos(this._KE_Prop, vec3i);
  }

  get position(){
      return ops.get_prop_pos(this._KE_Prop);
  }

  constructor() {
      this._KE_Prop = new ops.create_prop();
  }
}

let tether = 0;

var pig = new Prop();

export function tick() {
  tether+=0.001;

  //pig.position = new Vector3(Math.sin(tether), Math.cos(tether), 0);
  
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
