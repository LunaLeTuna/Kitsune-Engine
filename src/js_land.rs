use std::collections::HashMap;
use std::rc::Rc;
use std::sync::mpsc::{Receiver, Sender};
use std::sync::{Arc, RwLock};

use deno_core::serde::Serialize;
use deno_core::v8::{self, Local, Object, Value};
use deno_core::{
    anyhow, include_js_files, op, resolve_path, Extension, FsModuleLoader, JsRuntime, ResourceId, RuntimeOptions,
};
use nalgebra::{Vector2, Vector3};
use serde::Deserialize;
use winit::event::{DeviceId, KeyboardInput};

use crate::cameras::Camera;
use crate::props::Prop;
use crate::shaders::{ShaderVar, ShadvType};

// basically this enum allows the js thread to have access to the diffrent prop
// and resource has maps
pub enum KeThreadInformer {
    Swag(String),
    Awa,
}

pub enum KeThreadWin {
    JsReady,
    Swag(String),
    Based(u32),
    Awa,
}

pub enum RequestType {
    Model,
    Texture,
    Shader,
}

pub struct RequestNewObj {
    pub r_type: RequestType,
    pub url: String,
    pub id: i32,
}

pub struct KeyEvnt {
    pub device_id: DeviceId,
    pub input: KeyboardInput,
    pub is_synthetic: bool,
}

pub struct ScreenSize {
    pub x: u32,
    pub y: u32,
}

lazy_static! {
    //asset management
    pub static ref PROP_MAP: Arc<RwLock<HashMap<i32, Prop>>> = Arc::new(RwLock::new(HashMap::new()));
    pub static ref SHADER_MAP: Arc<RwLock<HashMap<i32, HashMap<String, ShaderVar>>>> = Arc::new(RwLock::new(HashMap::new()));
    pub static ref MAKE_REQUEST_MAP: Arc<RwLock<HashMap<i32, RequestNewObj>>> = Arc::new(RwLock::new(HashMap::new()));
    pub static ref SHADER_AMOUNT: RwLock<i32> = RwLock::new(0);
    pub static ref MODEL_AMOUNT: RwLock<i32> = RwLock::new(0);
    pub static ref TEXTURE_AMOUNT: Arc<RwLock<i32>> = Arc::new(RwLock::new(0));

    pub static ref CAMERA_MAP: Arc<RwLock<HashMap<i32, Camera>>> = Arc::new(RwLock::new(HashMap::new()));
    pub static ref MAIN_CAMERA: RwLock<i32> = RwLock::new(0);

    //input stuff
    pub static ref MOUSE_POS: Arc<RwLock<Vec2>> = Arc::new(RwLock::new(Vec2 { x: 0.0, y: 0.0 })); //some day add DeviceID for multiplayer with multimouses on x11 :3
    pub static ref KEY_EVENTS_MAP: Arc<RwLock<HashMap<u32, KeyEvnt>>> = Arc::new(RwLock::new(HashMap::new()));

    pub static ref SCREEN_SIZE: Arc<RwLock<Vec2>> = Arc::new(RwLock::new(Vec2 { x: 0.0, y: 0.0 }));
}

#[op]
fn op_sum(nums: Vec<f64>) -> Result<f64, deno_core::error::AnyError> {
    // Sum inputs
    let sum = nums.iter().fold(0.0, |a, v| a + v);
    // return as a Result<f64, AnyError>
    Ok(sum)
}

#[derive(Deserialize, Default, Debug, Clone, Copy, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Vec2 {
    pub x: f32,
    pub y: f32,
}

#[derive(Deserialize, Default, Debug, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct Vec3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

#[op]
fn create_vec3(x: f32, y: f32, z: f32) -> Result<Vec3, deno_core::error::AnyError> { Ok(Vec3 { x, y, z }) }

#[derive(Deserialize, Default, Debug, Serialize)]
#[serde(rename_all = "camelCase")]
pub struct PropV8 {
    rid: ResourceId,
}

// I feel like there is some way to auto make these mod_props with macros, but
// idk lol
#[op]
fn mod_prop_rot(prop_id: i32, vec3: Vec3) -> Result<i32, deno_core::error::AnyError> {
    if let Ok(mut prop_map) = PROP_MAP.write() {
        if let Some(prop) = prop_map.get_mut(&prop_id) {
            prop.set_rotation(Vector3::new(vec3.x, vec3.y, vec3.z));
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(prop_id)
}

#[op]
fn get_prop_rot(prop_id: i32) -> Result<Vec3, deno_core::error::AnyError> {
    let propz = PROP_MAP.read().expect("RwLock poisoned");

    let pos = propz.get(&prop_id).unwrap().rotation.euler_angles();

    Ok(Vec3 {
        x: pos.0,
        y: pos.1,
        z: pos.2,
    })
}

#[op]
fn mod_prop_pos(prop_id: i32, vec3: Vec3) -> Result<i32, deno_core::error::AnyError> {
    if let Ok(mut prop_map) = PROP_MAP.write() {
        if let Some(prop) = prop_map.get_mut(&prop_id) {
            prop.position = Vector3::new(vec3.x, vec3.y, vec3.z);
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(prop_id)
}

#[op]
fn get_prop_pos(prop_id: i32) -> Result<Vec3, deno_core::error::AnyError> {
    let propz = PROP_MAP.read().expect("RwLock poisoned");

    let pos = propz.get(&prop_id).unwrap().position;

    Ok(Vec3 {
        x: pos.x,
        y: pos.y,
        z: pos.z,
    })
}

#[op]
fn mod_prop_scale(prop_id: i32, vec3: Vec3) -> Result<i32, deno_core::error::AnyError> {
    if let Ok(mut prop_map) = PROP_MAP.write() {
        if let Some(prop) = prop_map.get_mut(&prop_id) {
            prop.scale = Vector3::new(vec3.x, vec3.y, vec3.z);
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(prop_id)
}

#[op]
fn get_prop_scale(prop_id: i32) -> Result<Vec3, deno_core::error::AnyError> {
    let propz = PROP_MAP.read().expect("RwLock poisoned");

    let pos = propz.get(&prop_id).unwrap().scale;

    Ok(Vec3 {
        x: pos.x,
        y: pos.y,
        z: pos.z,
    })
}

#[op]
fn mod_prop_shader(prop_id: i32, shaderc: i32) -> Result<i32, deno_core::error::AnyError> {
    if let Ok(mut prop_map) = PROP_MAP.write() {
        if let Some(prop) = prop_map.get_mut(&prop_id) {
            prop.shader = shaderc;
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(prop_id)
}

#[op]
fn get_prop_shader(prop_id: i32, _which: i32) -> Result<i32, deno_core::error::AnyError> {
    let propz = PROP_MAP.read().expect("RwLock poisoned");

    Ok(propz.get(&prop_id).unwrap().shader)
}

#[op]
fn mod_prop_shader_var_int(prop_id: i32, name: String, data: i32) -> Result<i32, deno_core::error::AnyError> {
    if let Ok(mut prop_map) = PROP_MAP.write() {
        if let Some(prop) = prop_map.get_mut(&prop_id) {
            prop.shader_vars.insert(name, ShadvType::Integer(data));
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(prop_id)
}

#[op]
fn mod_prop_shader_var_float(prop_id: i32, name: String, data: f32) -> Result<i32, deno_core::error::AnyError> {
    if let Ok(mut n) = PROP_MAP.write() {
        if let Some(prop) = n.get_mut(&prop_id) {
            prop.shader_vars.insert(name, ShadvType::Float(data));
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(prop_id)
}

#[op]
fn mod_prop_shader_var_vec2(prop_id: i32, name: String, data: Vec2) -> Result<i32, deno_core::error::AnyError> {
    let data = Vector2::new(data.x, data.y);

    if let Ok(mut prop_map) = PROP_MAP.write() {
        if let Some(prop) = prop_map.get_mut(&prop_id) {
            prop.shader_vars.insert(name, ShadvType::Vec2(data));
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(prop_id)
}

#[op]
fn mod_prop_shader_var_vec3(prop_id: i32, name: String, data: Vec3) -> Result<i32, deno_core::error::AnyError> {
    let data = Vector3::new(data.x, data.y, data.z);
    if let Ok(mut prop_map) = PROP_MAP.write() {
        if let Some(prop) = prop_map.get_mut(&prop_id) {
            prop.shader_vars.insert(name, ShadvType::Vec3(data));
        }
    };

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(prop_id)
}

#[op]
fn mod_prop_model(prop_id: i32, modelc: i32) -> Result<i32, deno_core::error::AnyError> {
    if let Ok(mut prop_map) = PROP_MAP.write() {
        if let Some(prop) = prop_map.get_mut(&prop_id) {
            prop.model = modelc;
        }
    };

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(prop_id)
}

#[op]
fn get_prop_model(prop_id: i32, _which: i32) -> Result<i32, deno_core::error::AnyError> {
    let propz = PROP_MAP.read().expect("RwLock poisoned");

    Ok(propz.get(&prop_id).unwrap().model)
}

#[op]
fn mod_prop_texture(prop_id: i32, which: i32, modelc: i32) -> Result<i32, deno_core::error::AnyError> {
    if let Ok(mut prop_map) = PROP_MAP.write() {
        if let Some(prop) = prop_map.get_mut(&prop_id) {
            match which {
                0 => prop.texture1 = modelc,
                1 => prop.texture2 = modelc,
                _ => (),
            }
        }
    };

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(prop_id)
}

#[op]
fn get_prop_texture(prop_id: i32) -> Result<i32, deno_core::error::AnyError> {
    let propz = PROP_MAP.read().expect("RwLock poisoned");

    Ok(propz.get(&prop_id).unwrap().texture1)
}

#[op]
fn create_prop() -> Result<i32, deno_core::error::AnyError> {
    let mut propz = PROP_MAP.write().expect("RwLock poisoned");
    let new_prop = Prop::new("nya".into());

    let wopper = propz.len() as i32;
    propz.insert(wopper, new_prop);

    Ok(wopper)
}

#[op]
fn mod_shader_var_int(shader_id: i32, name: String, data: i32) -> Result<i32, deno_core::error::AnyError> {
    if let Ok(mut shader_map) = SHADER_MAP.write() {
        if let Some(shader) = shader_map.get_mut(&shader_id) {
            shader.insert(
                name,
                ShaderVar {
                    data: ShadvType::Integer(data),
                },
            );
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(shader_id)
}

#[op]
fn mod_shader_var_float(shader_id: i32, name: String, data: f32) -> Result<i32, deno_core::error::AnyError> {
    if let Ok(mut shader_map) = SHADER_MAP.write() {
        if let Some(shader) = shader_map.get_mut(&shader_id) {
            shader.insert(
                name,
                ShaderVar {
                    data: ShadvType::Float(data),
                },
            );
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(shader_id)
}

#[op]
fn mod_shader_var_vec2(shader_id: i32, name: String, data: Vec2) -> Result<i32, deno_core::error::AnyError> {
    let data = Vector2::new(data.x, data.y);

    if let Ok(mut shader_map) = SHADER_MAP.write() {
        if let Some(shader) = shader_map.get_mut(&shader_id) {
            shader.insert(
                name,
                ShaderVar {
                    data: ShadvType::Vec2(data),
                },
            );
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(shader_id)
}

#[op]
fn mod_shader_var_vec3(shader_id: i32, name: String, data: Vec3) -> Result<i32, deno_core::error::AnyError> {
    let data = Vector3::new(data.x, data.y, data.z);
    if let Ok(mut shader_map) = SHADER_MAP.write() {
        if let Some(shader) = shader_map.get_mut(&shader_id) {
            shader.insert(
                name,
                ShaderVar {
                    data: ShadvType::Vec3(data),
                },
            );
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(shader_id)
}

#[op]
fn create_shader(url: String) -> Result<i32, deno_core::error::AnyError> {
    let mut modelreq = MAKE_REQUEST_MAP.write().expect("RwLock poisoned");
    let mut wopper = SHADER_AMOUNT.write().expect("RwLock poisoned");
    *wopper += 1;

    drop(wopper); // DO NOT REMOVE THIS WOPPER, for some reason it works with it here

    let wopper = *SHADER_AMOUNT.read().unwrap();

    let new_req = RequestNewObj {
        r_type: RequestType::Shader,
        url,
        id: wopper,
    };

    SHADER_MAP
        .write()
        .expect("RwLock poisoned")
        .insert(wopper, HashMap::new());

    let woppera = modelreq.len() as i32;
    modelreq.insert(woppera, new_req);

    Ok(wopper)
}

#[op]
fn create_model(url: String) -> Result<i32, deno_core::error::AnyError> {
    let mut modelreq = MAKE_REQUEST_MAP.write().expect("RwLock poisoned");
    let mut wopper = MODEL_AMOUNT.write().expect("RwLock poisoned");
    *wopper += 1;

    drop(wopper); // DO NOT REMOVE THIS WOPPER, for some reason it works with it here

    let wopper = *MODEL_AMOUNT.read().unwrap();

    let new_req = RequestNewObj {
        r_type: RequestType::Model,
        url,
        id: wopper,
    };

    let woppera = modelreq.len() as i32;
    modelreq.insert(woppera, new_req);

    Ok(wopper)
}

#[op]
fn create_texture(url: String) -> Result<i32, deno_core::error::AnyError> {
    let mut modelreq = MAKE_REQUEST_MAP.write().expect("RwLock poisoned");
    let mut wopper = MODEL_AMOUNT.write().expect("RwLock poisoned");
    *wopper += 1;

    drop(wopper); // NORE CAN YOU REMOVE THIS WOPPER, I have no idea why it works but it does

    let wopper = *MODEL_AMOUNT.read().unwrap();

    let new_req = RequestNewObj {
        r_type: RequestType::Texture,
        url,
        id: wopper,
    };

    let woppera = modelreq.len() as i32;
    modelreq.insert(woppera, new_req);

    Ok(wopper)
}

#[op]
fn mod_cam_rot(cam: i32, vec3: Vec3) -> Result<(), deno_core::error::AnyError> {
    if let Ok(mut camera_map) = CAMERA_MAP.write() {
        let a = camera_map.get_mut(&cam).unwrap();

        a.set_rotation(Vector3::new(vec3.x, vec3.y, vec3.z));

        a.refresh();
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(())
}

#[op]
fn get_cam_rot(camera_id: i32) -> Result<Vec3, deno_core::error::AnyError> {
    let propz = CAMERA_MAP.read().expect("RwLock poisoned");

    let pos = propz.get(&camera_id).unwrap().rotation.euler_angles();

    Ok(Vec3 {
        x: pos.0,
        y: pos.1,
        z: pos.2,
    })
}

#[op]
fn mod_cam_pos(camera_id: i32, vec3: Vec3) -> Result<(), deno_core::error::AnyError> {
    if let Ok(mut camera_map) = CAMERA_MAP.write() {
        if let Some(camera) = camera_map.get_mut(&camera_id) {
            camera.position = Vector3::new(-vec3.x, -vec3.y, -vec3.z);

            camera.refresh();
        }
    }

    // propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y,
    // vec3.z); drop(propz);
    Ok(())
}

#[op]
fn get_cam_pos(camera_id: i32) -> Result<Vec3, deno_core::error::AnyError> {
    let propz = CAMERA_MAP.read().expect("RwLock poisoned");

    let pos = propz.get(&camera_id).unwrap().position;

    Ok(Vec3 {
        x: pos.x,
        y: pos.y,
        z: pos.z,
    })
}

#[op]
fn create_camera() -> Result<i32, deno_core::error::AnyError> {
    if let Ok(screen_size) = SCREEN_SIZE.try_read() {
        let mut cameraz = CAMERA_MAP.write().expect("RwLock poisoned");
        let new_cam = Camera::craft(*screen_size);

        let wopper = cameraz.len() as i32;
        cameraz.insert(wopper, new_cam);

        Ok(wopper)
    } else {
        Ok(0)
    }
}

#[op]
fn set_main_camera(camera_id: i32) -> Result<(), deno_core::error::AnyError> {
    if SCREEN_SIZE.try_read().is_ok() {
        let mut camz = MAIN_CAMERA.write().expect("RwLock poisoned");

        *camz = camera_id;
    }

    Ok(())
}

// #[op]
// fn add_event_listener<FP, 'scope>(
//     state: &mut OpState,
//     scope: &mut v8::HandleScope<'scope>,
//     cb: serde_v8::Value<'scope>,) -> Result<(), deno_core::error::AnyError> {
//     let awa = state.borrow::<String>();
//     println!("{}", awa);
//     Ok({})
// }

async fn async_js_loop(
    file_path: &str,
    receiver: Receiver<KeThreadInformer>,
    sender: Sender<KeThreadWin>,
    _propz: Arc<RwLock<HashMap<i32, Prop>>>,
) -> anyhow::Result<()> {
    let ext = Extension::builder("KE_OBjects")
        .ops(vec![
            mod_cam_pos::decl(),
            get_cam_pos::decl(),
            mod_cam_rot::decl(),
            get_cam_rot::decl(),
            create_camera::decl(),
            set_main_camera::decl(),
            mod_prop_rot::decl(),
            get_prop_rot::decl(),
            mod_prop_pos::decl(),
            get_prop_pos::decl(),
            mod_prop_scale::decl(),
            get_prop_scale::decl(),
            mod_prop_shader::decl(),
            get_prop_shader::decl(),
            mod_prop_shader_var_int::decl(),
            mod_prop_shader_var_float::decl(),
            mod_prop_shader_var_vec2::decl(),
            mod_prop_shader_var_vec3::decl(),
            mod_prop_model::decl(),
            get_prop_model::decl(),
            mod_prop_texture::decl(),
            get_prop_texture::decl(),
            create_prop::decl(),
            mod_shader_var_int::decl(),
            mod_shader_var_float::decl(),
            mod_shader_var_vec2::decl(),
            mod_shader_var_vec3::decl(),
            create_shader::decl(),
            create_model::decl(),
            create_texture::decl(),
            op_sum::decl(),
        ])
        .js(include_js_files!(dir "KE", "ke_wrap.js",))
        .build();

    let mut js_runtime = JsRuntime::new(RuntimeOptions {
        extensions_with_js: vec![ext],
        module_loader: Some(Rc::new(FsModuleLoader)),
        ..Default::default()
    });

    let main_module = resolve_path(file_path)?;

    let module_id = js_runtime.load_main_module(&main_module, None).await?;
    let result = js_runtime.mod_evaluate(module_id);

    js_runtime.run_event_loop(false).await?;

    println!("loaded result {:?}", result.await);

    let namespace = js_runtime.get_module_namespace(module_id)?;

    let scope = &mut js_runtime.handle_scope();

    let module_object = v8::Local::<v8::Object>::new(scope, namespace);

    let module_properties = module_object.get_property_names(scope, Default::default()).unwrap();

    println!("module properties: {}", module_properties.to_rust_string_lossy(scope));

    // get tick function
    let export_fn_name = v8::String::new(scope, "tick").unwrap();

    dbg!(export_fn_name.to_rust_string_lossy(scope));

    let export_fn = module_object
        .get(scope, export_fn_name.into())
        .expect("couldnt find fn");

    let function = v8::Local::<v8::Function>::try_from(export_fn)?;

    let empty: &[Local<Value>] = &[];
    let recv: Local<Value> = module_object.into();

    sender.send(KeThreadWin::JsReady).unwrap();

    let mut az: f32 = 0.0;

    let mouse_last_pos = Vec2 { x: 0.0, y: 0.0 };

    let mouseenv = v8::ObjectTemplate::new(scope);

    let xname = v8::String::new(scope, "x").unwrap();
    mouseenv.set(xname.into(), v8::Number::new(scope, 0.into()).into());

    let yname = v8::String::new(scope, "y").unwrap();
    mouseenv.set(yname.into(), v8::Number::new(scope, 0.into()).into());

    let mouseenv = mouseenv.new_instance(scope).unwrap();

    let keyenv = v8::ObjectTemplate::new(scope);
    let codename = v8::String::new(scope, "keycode").unwrap();
    keyenv.set(xname.into(), v8::Number::new(scope, 0.into()).into());

    let keyenv = keyenv.new_instance(scope).unwrap();

    loop {
        az += 0.0001;
        // if no message just dont until there is

        let thread_mail = receiver.try_recv();

        if thread_mail.is_ok() {
            continue;
        } else {
            // match thread_mail.unwrap() {
            //     KE_THREAD_INFORMER::Awa => (),
            //     KE_THREAD_INFORMER::Swag(_) => todo!()
            // }

            // not 100% sure there may be a mem leak here, keep 85 eyes open at all times
            // :::::::::::;::::::::::::::::::::::::::::::3

            if let Ok(n) = MOUSE_POS.try_read() {
                if n.x != mouse_last_pos.x || n.y != mouse_last_pos.y {
                    let scope = &mut v8::HandleScope::new(scope);
                    // println!("nya {} {}", n.x, n.y);
                    // mouse_last_pos = Vec2{x:n.x,y:n.y};

                    let x = v8::Number::new(scope, n.x as f64).into();
                    mouseenv.set(scope, xname.into(), x);

                    let y = v8::Number::new(scope, n.y as f64).into();
                    mouseenv.set(scope, yname.into(), y);

                    // get input function
                    fun_name(scope, module_object, recv, "mousemove".to_owned(), mouseenv.into())?;
                }
            }

            if let Ok(mut n) = KEY_EVENTS_MAP.try_write() {
                if !n.is_empty() {
                    n.iter().for_each(|(_index, evnt)| {
                        let scope = &mut v8::HandleScope::new(scope);
                        // println!("nya {} {}", n.x, n.y);
                        // mouse_last_pos = Vec2{x:n.x,y:n.y};

                        let x = v8::Number::new(scope, evnt.input.scancode as f64).into();
                        keyenv.set(scope, codename.into(), x);

                        // get input function
                        match evnt.input.state {
                            winit::event::ElementState::Pressed => {
                                fun_name(scope, module_object, recv, "keydown".to_owned(), keyenv.into()).unwrap()
                            }
                            winit::event::ElementState::Released => {
                                fun_name(scope, module_object, recv, "keyup".to_owned(), keyenv.into()).unwrap()
                            }
                        };
                    });
                }

                n.drain();
            }

            // let mut propz = propz.write().expect("RwLock poisoned");
            // propz.get_mut(&0).unwrap().set_rotation(Vector3::new(az.sin()/2.0, 0.0,
            // 0.0)); propz.get_mut(&0).unwrap().position = Vector3::new(3.14 /
            // 9.0 * az.sin(), 0.0, 0.0);

            let scope = &mut v8::HandleScope::new(scope);

            if az > 1000.0 {
                az = 0.0
            }

            // println!("meow {}", az);

            function.call(scope, recv, empty);
            // drop(scope);
        }

        sender.send(KeThreadWin::Awa).unwrap();
    }
}

fn fun_name(
    scope: &mut v8::HandleScope,
    module_object: Local<Object>,
    recv: Local<Value>,
    name: String,
    data: Local<Value>,
) -> Result<(), anyhow::Error> {
    let export_fn_name = v8::String::new(scope, "_KE_EVENT_PUSH").unwrap();
    let export_fn = module_object
        .get(scope, export_fn_name.into())
        .expect("couldnt find fn");

    let function = v8::Local::<v8::Function>::try_from(export_fn)?;
    let awa = v8::String::new(scope, &name).unwrap();

    function.call(scope, recv, &[awa.into(), data]);

    Ok(())
}

pub fn js_thread(
    receiver: Receiver<KeThreadInformer>,
    sender: Sender<KeThreadWin>,
    propz: Arc<RwLock<HashMap<i32, Prop>>>,
) {
    let tokio_thread = tokio::runtime::Builder::new_current_thread()
        .enable_all()
        .build()
        .unwrap();

    tokio_thread
        .block_on(async_js_loop("./index.js", receiver, sender, propz))
        .unwrap();
}

pub fn create_js_thread(
    receiver: Receiver<KeThreadInformer>,
    sender: Sender<KeThreadWin>,
    propz: Arc<RwLock<HashMap<i32, Prop>>>,
) {
    let _ = std::thread::spawn(move || js_thread(receiver, sender, propz));
}
