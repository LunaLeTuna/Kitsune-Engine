const core = globalThis.Deno.core;
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

// export {
//     Vector3,
//     Prop
// };