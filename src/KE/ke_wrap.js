const core = Deno.core;
const ops = core.ops;

class Vector3 {
    constructor(x,y,z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }
}

class Shader {
    constructor(url) {
        this.ID = ops.create_shader(url);
    }
}

class Model {
    constructor(url) {
        this.ID = ops.create_model(url);
    }
}

class Texture {
    constructor(url) {
        this.ID = ops.create_texture(url);
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

    set scale(vec3i){
        ops.mod_prop_scale(this._KE_Prop, vec3i);
    }

    get scale(){
        return ops.get_prop_scale(this._KE_Prop);
    }

    set shader(modeli){
        ops.mod_prop_shader(this._KE_Prop, modeli.ID);
    }

    get shader(){
        return ops.get_prop_shader(this._KE_Prop);
    }

    shader_int(name, data) {
        ops.mod_prop_shader_var_int(this._KE_Prop, name, data);
    }

    shader_float(name, data) {
        ops.mod_prop_shader_var_float(this._KE_Prop, name, data);
    }

    shader_vec2(name, data) {
        ops.mod_prop_shader_var_vec2(this._KE_Prop, name, data);
    }

    shader_vec3(name, data) {
        ops.mod_prop_shader_var_vec3(this._KE_Prop, name, data);
    }

    set model(modeli){
        ops.mod_prop_model(this._KE_Prop, modeli.ID);
    }

    get model(){
        return ops.get_prop_model(this._KE_Prop);
    }

    set texture(modeli){
        ops.mod_prop_texture(this._KE_Prop, 0, modeli.ID);
    }

    get texture(){
        return ops.get_prop_texture(this._KE_Prop, 0);
    }

    set texture2(modeli){
        ops.mod_prop_texture(this._KE_Prop, 1, modeli.ID);
    }

    get texture2(){
        return ops.get_prop_texture(this._KE_Prop, 1);
    }

    constructor() {
        this._KE_Prop = ops.create_prop();
    }
}

// export {
//     Vector3,
//     Prop
// };