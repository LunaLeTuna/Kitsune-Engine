class Vector2 {
    constructor(x, y) {
        this.x = x;
        this.y = y;
    }
}

class Vector3 {
    constructor(x, y, z) {
        this.x = x;
        this.y = y;
        this.z = z;
    }
}

class Shader {
    constructor(url) {
        //this.ID = ops.create_shader(url);
    }
    shader_int(name, data) {
        //ops.mod_shader_var_int(this.ID, name, data);
    }

    shader_float(name, data) {
        //ops.mod_shader_var_float(this.ID, name, data);
    }

    shader_vec2(name, data) {
        //ops.mod_shader_var_vec2(this.ID, name, data);
    }

    shader_vec3(name, data) {
        //ops.mod_shader_var_vec3(this.ID, name, data);
    }
}

class Model {
    constructor(url) {
        //this.ID = ops.create_model(url);
    }
}

class Texture {
    constructor(url) {
        //this.ID = ops.create_texture(url);
    }
}

class Prop {
    set position(vec3i) {
        mod_prop_pos(this._KE_Prop, vec3i);
    }

    get position() {
        return get_prop_pos(this._KE_Prop);
    }

    set rotation(vec3i) {
        //ops.mod_prop_rot(this._KE_Prop, vec3i);
    }

    get rotation() {
        //return ops.get_prop_rot(this._KE_Prop);
    }

    set scale(vec3i) {
        //ops.mod_prop_scale(this._KE_Prop, vec3i);
    }

    get scale() {
        //return ops.get_prop_scale(this._KE_Prop);
    }

    set shader(modeli) {
        //ops.mod_prop_shader(this._KE_Prop, modeli.ID);
    }

    get shader() {
        //return ops.get_prop_shader(this._KE_Prop);
    }

    shader_int(name, data) {
        //ops.mod_prop_shader_var_int(this._KE_Prop, name, data);
    }

    shader_float(name, data) {
        //ops.mod_prop_shader_var_float(this._KE_Prop, name, data);
    }

    shader_vec2(name, data) {
        //ops.mod_prop_shader_var_vec2(this._KE_Prop, name, data);
    }

    shader_vec3(name, data) {
        //ops.mod_prop_shader_var_vec3(this._KE_Prop, name, data);
    }

    set model(modeli) {
        //ops.mod_prop_model(this._KE_Prop, modeli.ID);
    }

    get model() {
        //return ops.get_prop_model(this._KE_Prop);
    }

    set texture(modeli) {
        //ops.mod_prop_texture(this._KE_Prop, 0, modeli.ID);
    }

    get texture() {
        //return ops.get_prop_texture(this._KE_Prop, 0);
    }

    set texture2(modeli) {
        //ops.mod_prop_texture(this._KE_Prop, 1, modeli.ID);
    }

    get texture2() {
        //return ops.get_prop_texture(this._KE_Prop, 1);
    }

    constructor(l) {
        this._KE_Prop = l ?? create_prop();
    }
}

function getByName(name){
    var idd = get_existing_prop_by_name(name);
    if(idd == -1) {
        console.log(`Error: can't fine prop "${name}"`)
        return null;
    }
    return new Prop(idd);
}

class Camera {
    set position(vec3i) {
        //ops.mod_cam_pos(this._KE_Prop, vec3i);
    }

    get position() {
        //return ops.get_cam_pos(this._KE_Prop);
    }

    set rotation(vec3i) {
        //ops.mod_cam_rot(this._KE_Prop, vec3i);
    }

    get rotation() {
        //return ops.get_cam_rot(this._KE_Prop);
    }

    constructor() {
        //this._KE_Prop = ops.create_camera();
    }
}

function SetMainCam(av) {
    //ops.set_main_camera(av._KE_Prop);
}

function print(a) {
    //Deno.core.print(a);
}




// var firstID = "";
// var __gotFirstYet = false;
// function _first_player(delta){
//     // if(delta.type == "join" && !__gotFirstYet){ //for server
//     //     //firstID = delta.id
//     // }
// }
// addEventListener("client_update", _first_player)

function serverEmit(data){
    dispatchEvent("client_update", {"type":"update","fromid":"server","ishost":true,"data":data});
    _emit(data);
}

function emit(data){
    _emit(data);
}




var _KE_EVENT_LIST = {};

function addEventListener(name, fn) {
    if (_KE_EVENT_LIST.hasOwnProperty(name)) {
        _KE_EVENT_LIST[name].push(fn);
    } else {
        _KE_EVENT_LIST[name] = [];
        _KE_EVENT_LIST[name].push(fn);
    }
}

function dispatchEvent(name, data) {
    if (!_KE_EVENT_LIST.hasOwnProperty(name)) return;
    _KE_EVENT_LIST[name].forEach(element => {
        element(data);
    });
}

var _Socket_EVENT_LIST = {};

var socket = {

    "emit": function (name, data) {
    },

    "addEventListener": function (name, fn) {
        if (_Socket_EVENT_LIST.hasOwnProperty(name)) {
            _Socket_EVENT_LIST[name].push(fn);
        } else {
            _KE_EVENT_LIST[name] = [];
            _KE_EVENT_LIST[name].push(fn);
        }
    },

    "dispatchEvent": function (name, data) {
        if (!_Socket_EVENT_LIST.hasOwnProperty(name)) return;
        _Socket_EVENT_LIST[name].forEach(element => {
            element(data);
        });
    }

}
