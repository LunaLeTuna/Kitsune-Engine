const core = Deno.core;
const ops = core.ops;

class Vector3 {
    constructor(x,y,z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }
}

class Model {
    constructor(url) {
        this.ID = ops.create_model(url);
    }
}

class Prop {
    set position(vec3i){
        ops.mod_prop_pos(this._KE_Prop, vec3i);
    }

    get position(){
        return ops.get_prop_pos(this._KE_Prop);
    }

    set rotation(vec3i){
        ops.mod_prop_rot(this._KE_Prop, vec3i);
    }

    get rotation(){
        return ops.get_prop_rot(this._KE_Prop);
    }

    set model(modeli){
        ops.mod_prop_model(this._KE_Prop, modeli.ID);
    }

    get model(){
        return ops.get_prop_model(this._KE_Prop);
    }

    constructor() {
        this._KE_Prop = new ops.create_prop();
    }
}

// export {
//     Vector3,
//     Prop
// };