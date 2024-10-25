use std::{time::{Instant, Duration}, collections::HashMap, borrow::BorrowMut, sync::{RwLock, mpsc::Sender}, future::Future};

use boa_engine::{Context, JsResult, JsValue, JsArgs, Source, property::{Attribute, PropertyKey}, NativeFunction, value::TryFromJs, JsNativeError, builtins::{function, string, error, json}, JsString};
use boa_runtime::Console;
use nalgebra::{Vector2, Vector3};
use serde_json::{json, Value};
use rust_socketio::{ClientBuilder, Payload, RawClient, client::Client};
use smol::future::FutureExt;

use crate::{cameras::Camera, fs_system::grab, menu_system::{menuimage, menutext, KEmenuTypes}, props::Prop, PhysWorld, PointLight, ALLOWFILEGRAB, CAMERAS, LIGHTS, MAIN_CAM, MENUS, MODEL_COUNT, PROPS, PW, REQUESTS, SCREEN_SIZE, SHADER_COUNT, TEXTURE_COUNT, WORLDS};

pub struct ScriptSpace<'a> {
    pub world: i16,
    context: boa_engine::Context<'a>,
    scripts: Vec<(String,String)> //actuall code, then it's file location
}

#[derive(Debug, TryFromJs)]
#[allow(dead_code)]
struct TestStruct {
    inner: bool,
    hello: String,
    // You can override the conversion of an attribute.
    #[boa(from_js_with = "lossy_conversion")]
    my_float: i16,
}

fn awawawa(_: &JsValue, args: &[JsValue], ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let arg = args.get(0).unwrap();

    // BUG: Dev branch seems to be passing string arguments along with quotes
    let libfile = arg
        .to_string(ctx)
        .expect("Failed to convert to string")
        .to_std_string_escaped();

        println!(
            "{}",
            libfile
        );

        Ok(JsValue::String("meow :3".into()))
}

fn delay(
    _this: &JsValue,
    args: &[JsValue],
    context: &mut Context<'_>,
) -> impl Future<Output = JsResult<JsValue>> {
    let millis = args.get_or_undefined(0).to_u32(context);

    async move {
        let millis = millis?;
        println!("Delaying for {millis} milliseconds ...");
        let now = Instant::now();
        smol::Timer::after(Duration::from_millis(millis as u64)).await;
        let elapsed = now.elapsed().as_secs_f64();
        Ok(elapsed.into())
    }
}


/// Converts the value lossly
fn lossy_conversion(value: &JsValue, _context: &mut Context) -> JsResult<i16> {
    match value {
        JsValue::Rational(r) => Ok(r.round() as i16),
        JsValue::Integer(i) => Ok(*i as i16),
        _ => Err(JsNativeError::typ()
            .with_message("cannot convert value to an i16")
            .into()),
    }
}

fn mod_prop_pos(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    let stz = st.get("z").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.position = Vector3::new(stx, sty, stz);
    
    Ok(JsValue::Undefined)

}

fn get_prop_pos(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    match PROPS.try_read() {
        Ok(propz)=>{
            let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

            let w = propz.get(&propid).unwrap();

            let json = json!({
                "x": w.position.x,
                "y": w.position.y,
                "z": w.position.z
            });

            let fvalue = JsValue::from_json(&json, _ctx).unwrap();
            
            return Ok(fvalue);
        }
        Err(_) => todo!(),
    }

}

fn mod_prop_scale(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    let stz = st.get("z").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.scale = Vector3::new(stx, sty, stz);
    
    Ok(JsValue::Undefined)

}

fn get_prop_scale(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get(&propid).unwrap();

    let json = json!({
        "x": w.scale.x,
        "y": w.scale.y,
        "z": w.scale.z
    });

    let fvalue = JsValue::from_json(&json, _ctx).unwrap();
    
    Ok(fvalue)

}

fn mod_prop_rot(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    let stz = st.get("z").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.set_rotation(Vector3::new(stx, sty, stz));
    
    Ok(JsValue::Undefined)

}

fn get_prop_rot(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get(&propid).unwrap();

    let json = json!({
        "x": w.rotation.x,
        "y": w.rotation.y,
        "z": w.rotation.z
    });

    let fvalue = JsValue::from_json(&json, _ctx).unwrap();
    
    Ok(fvalue)

}

fn mod_prop_shader(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_i32(_ctx).unwrap();
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.shader = st;
    
    Ok(JsValue::Undefined)

}

fn get_prop_shader(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();
    let st = _nargs.get_or_undefined(1).to_i32(_ctx).unwrap();

    let w = propz.get(&propid).unwrap();

    
    
    Ok(JsValue::new(w.shader))

}


fn mod_prop_model(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_i32(_ctx).unwrap();
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.model = st;
    
    Ok(JsValue::Undefined)

}

fn get_prop_model(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();
    let st = _nargs.get_or_undefined(1).to_i32(_ctx).unwrap();

    let w = propz.get(&propid).unwrap();

    
    
    Ok(JsValue::new(w.model))

}

fn mod_prop_texture(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_i32(_ctx).unwrap();
    let plat = _nargs.get_or_undefined(2).to_i32(_ctx).unwrap();
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.textures[st as usize] = plat;
    
    Ok(JsValue::Undefined)

}

fn get_prop_texture(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();
    let st = _nargs.get_or_undefined(1).to_i32(_ctx).unwrap();

    let w = propz.get(&propid).unwrap();

    
    
    Ok(JsValue::new(w.textures[st as usize]))

}

fn lookat_prop(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    let stz = st.get("z").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.look_at(Vector3::new(stx, sty, stz));
    
    Ok(JsValue::Undefined)

}

fn mod_prop_vel(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = REQUESTS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    let stz = st.get("z").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    propz.push(crate::KERequest::Phys_Prop_Push(propid, Vector3::new(stx, sty, stz)));
    drop(propz);
    
    Ok(JsValue::Undefined)

}

fn get_prop_vel(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = PROPS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get(&propid).unwrap();

    let json = json!({
        "x": w.velocity.x,
        "y": w.velocity.y,
        "z": w.velocity.z
    });

    let fvalue = JsValue::from_json(&json, _ctx).unwrap();
    
    Ok(fvalue)

}

fn mod_prop_vel_onlyside(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = REQUESTS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    propz.push(crate::KERequest::Phys_Prop_Push_SideOnly(propid, Vector2::new(stx, sty)));
    drop(propz);
    
    Ok(JsValue::Undefined)

}

fn mod_prop_copy_phys(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = REQUESTS.write().unwrap();
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    propz.push(crate::KERequest::copy_prop_phys_pose(propid));
    drop(propz);
    
    Ok(JsValue::Undefined)

}

fn create_prop(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let world = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let mut womp = Prop::new("nya :3".to_owned());
    womp.model = 0;
    womp.shader = 1;
    womp.textures = vec![0,0];
    let mut propz = PROPS.write().unwrap();
    let mut binding = WORLDS.write().unwrap();
    let mut worldz = binding.get_mut(&(world as u32)).unwrap();
    let i = propz.len() as i32;
    womp.selfid = i;
    propz.insert(i, womp);
    worldz.1.push(i);
    
    Ok(JsValue::Integer(i))

}

fn get_all_props_in_world(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut womp = Prop::new("nya :3".to_owned());
    womp.model = 0;
    womp.shader = 1;
    womp.textures = vec![0,0];
    let mut propz = PROPS.write().unwrap();
    let mut binding = WORLDS.write().unwrap();
    let mut worldz = binding.get_mut(&0).unwrap();
    let i = propz.len() as i32;
    womp.selfid = i;
    propz.insert(i, womp);
    worldz.1.push(i);
    
    Ok(JsValue::Integer(i))

}

fn delete_prop(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut reqs = REQUESTS.write().unwrap();

    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    reqs.push(crate::KERequest::Delete_Prop(propid));
    
    Ok(JsValue::null())

}

//gets prop by name
fn get_existing_prop_by_name(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let sat = _nargs.get_or_undefined(0).as_string().unwrap().as_ref();

    let propname = match std::string::String::from_utf16(sat) {
        Ok(x) => x,
        Err(_) => "".to_owned()
    };

    let propz = PROPS.read().unwrap();
    let w = propz.clone().into_iter();

    for (i, prop) in w{
        if(prop.name == propname){
            return Ok(JsValue::Integer(i))
        }
    }
    
    Ok(JsValue::Integer(-1))

}

//
// cameras
//

fn create_camera(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut camera_map = CAMERAS.write().unwrap();
    let mut screen = SCREEN_SIZE.read().unwrap();

    let mut womp = Camera::craft(*screen);
    womp.position=Vector3::zeros();
    womp.refresh();
    let i = camera_map.len() as i32;
    camera_map.insert(i, womp);
    
    Ok(JsValue::Integer(i))
}

fn mod_camera_pos(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut camera_map = CAMERAS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    let stz = st.get("z").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let mut w = camera_map.get_mut(&propid).unwrap();

    w.position = Vector3::new(stx, sty, stz);
    w.refresh();

    drop(camera_map);
    
    Ok(JsValue::Undefined)

}

fn get_camera_pos(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut camera_map = CAMERAS.write().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = camera_map.get(&propid).unwrap();

    let json = json!({
        "x": w.position.x,
        "y": w.position.y,
        "z": w.position.z
    });

    let fvalue = JsValue::from_json(&json, _ctx).unwrap();
    
    Ok(fvalue)

}

fn mod_camera_rot(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = CAMERAS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    let stz = st.get("z").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.set_rotation(Vector3::new(stx, sty, stz));
    
    Ok(JsValue::Undefined)

}

fn get_camera_rot(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = CAMERAS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get(&propid).unwrap();


    let gewru = w.rotation.euler_angles();
    

    let json = json!({
        "x": gewru.0,
        "y": gewru.1,
        "z": gewru.2
    });

    let fvalue = JsValue::from_json(&json, _ctx).unwrap();
    
    Ok(fvalue)

}

fn mod_camera_dis(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = CAMERAS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_boolean();
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.disabled=st;
    
    Ok(JsValue::Undefined)

}

fn get_camera_dis(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = CAMERAS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get(&propid).unwrap();
    
    Ok(JsValue::Boolean(w.disabled))

}

fn mod_camera_bw2(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = CAMERAS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_i32(_ctx).unwrap();
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.draw_buffer_to=st;
    
    Ok(JsValue::Undefined)

}

fn get_camera_bw2(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = CAMERAS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get(&propid).unwrap();
    
    Ok(JsValue::Integer(w.draw_buffer_to))

}

fn lookat_camera(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = CAMERAS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    let stz = st.get("z").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get_mut(&propid).unwrap();

    w.look_at(Vector3::new(stx, sty, stz));
    w.refresh();
    
    Ok(JsValue::Undefined)

}

fn set_main_camera(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = CAMERAS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let mut nya = MAIN_CAM.write().unwrap();

    *nya = propid;
    
    Ok(JsValue::Undefined)

}

//gets cam by name
fn get_existing_cam_by_name(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let sat = _nargs.get_or_undefined(0).as_string().unwrap().as_ref();

    let propname = match std::string::String::from_utf16(sat) {
        Ok(x) => x,
        Err(_) => "".to_owned()
    };

    let propz = CAMERAS.read().unwrap();
    let w = propz.clone().into_iter();

    for (i, prop) in w{
        if(prop.name == propname){
            return Ok(JsValue::Integer(i))
        }
    }
    
    Ok(JsValue::Integer(-1))

}

//
// texture
//

fn create_texture(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut texturecount = TEXTURE_COUNT.write().unwrap();
    let mut propz = REQUESTS.write().unwrap();
    
    let sat = _nargs.get_or_undefined(0).as_string().unwrap().as_ref();

    let locat = match std::string::String::from_utf16(sat) {
        Ok(x) => x,
        Err(_) => "".to_owned()
    };

    propz.push(crate::KERequest::Create_Texture(*texturecount, locat, 0,0));
    drop(propz);

    *texturecount = *texturecount + 1;
    
    Ok(JsValue::Integer(*texturecount-1))

}

//
// shader
//

fn create_shader(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut modelcount = SHADER_COUNT.write().unwrap();
    let mut propz = REQUESTS.write().unwrap();
    
    let sat = _nargs.get_or_undefined(0).as_string().unwrap().as_ref();

    let locat = match std::string::String::from_utf16(sat) {
        Ok(x) => x,
        Err(_) => "".to_owned()
    };

    propz.push(crate::KERequest::Create_Shader(*modelcount, locat));
    drop(propz);

    *modelcount = *modelcount + 1;
    
    Ok(JsValue::Integer(*modelcount-1))

}

//
// model
//

fn create_model(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut modelcount = MODEL_COUNT.write().unwrap();
    let mut propz = REQUESTS.write().unwrap();
    
    let sat = _nargs.get_or_undefined(0).as_string().unwrap().as_ref();

    let locat = match std::string::String::from_utf16(sat) {
        Ok(x) => x,
        Err(_) => "".to_owned()
    };

    propz.push(crate::KERequest::Create_Model(*modelcount, locat));
    drop(propz);

    *modelcount = *modelcount + 1;
    
    Ok(JsValue::Integer(*modelcount-1))

}

//
// lights
//

fn create_light(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut womp = PointLight::new();

    //maybe position

    let mut lightz = LIGHTS.write().unwrap();
    let i = lightz.len();
    lightz.insert(i, womp);
    
    Ok(JsValue::Integer(i as i32))

}

fn mod_light_pos(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut lightz = LIGHTS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    let stz = st.get("z").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = lightz.get_mut(propid as usize).unwrap();

    w.position = Vector3::new(stx, sty, stz);
    
    Ok(JsValue::Undefined)

}

fn get_light_pos(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut lightz = LIGHTS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = lightz.get(propid as usize).unwrap();

    let json = json!({
        "x": w.position.x,
        "y": w.position.y,
        "z": w.position.z
    });

    let fvalue = JsValue::from_json(&json, _ctx).unwrap();
    
    Ok(fvalue)

}

//
//  menu image
//

fn create_menu_image(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {

    //maybe position

    let mut menuz = MENUS.try_write().unwrap();

    let i = menuz.len() as i32;

    menuz.insert(i, KEmenuTypes::image(menuimage{
        position: Vector2::new(0.0, 0.0),
        size: Vector2::new(0.1, 0.1),
        shader: 3,
        texture: 1,
        uv: Vector2::new(0.0,0.0),
        render: true,
    }));
    
    Ok(JsValue::Integer(i as i32))

}

fn mod_menu_texture(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut menuz = MENUS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_i32(_ctx).unwrap();
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = menuz.get_mut(&propid).unwrap();

    match w {
        KEmenuTypes::image(awa) => {
            awa.texture = st;
        },
        _ => ()
    }
    
    Ok(JsValue::Undefined)

}

//
//  menu text
//

fn create_menu_text(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {

    //maybe position

    let mut menuz = MENUS.try_write().unwrap();

    let i = menuz.len() as i32;

    menuz.insert(i, KEmenuTypes::text(menutext{
        position: Vector2::new(-0.5, 0.5),
        size: Vector2::new(0.2, 0.2),
        shader: 4,
        font: 0,
        text: "awawawa".to_string(),
        render: true,
    }));
    
    Ok(JsValue::Integer(i as i32))

}

fn mod_menu_text_text(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();
    let sat = _nargs.get_or_undefined(1).as_string().unwrap().as_ref();

    let rawtext = match std::string::String::from_utf16(sat) {
        Ok(x) => x,
        Err(_) => "".to_owned()
    };

    let mut menuz = MENUS.write().unwrap();
    let w = menuz.get_mut(&propid).unwrap();

    match w {
        KEmenuTypes::text(awa) => {
            awa.text = rawtext
        },
        _ => ()
    }
    
    Ok(JsValue::Integer(-1))

}

fn get_menu_text_text(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let mut menuz = MENUS.read().unwrap();
    let w = menuz.get(&propid).unwrap();

    let finalz = match w {
        KEmenuTypes::text(awa) => {
            &awa.text
        },
        _ => ""
    };
    
    Ok(JsValue::String(finalz.into()))

}

//  TODO: fix typo
//
//  genaric menu functions
//

fn mod_menu_pos(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut menuz = MENUS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = menuz.get_mut(&propid).unwrap();

    match w {
        KEmenuTypes::text(awa) => {
            awa.position = Vector2::new(stx, sty);
        },
        KEmenuTypes::image(awa) => {
            awa.position = Vector2::new(stx, sty);
        },
    }
    
    Ok(JsValue::Undefined)

}

fn mod_menu_scale(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut menuz = MENUS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let stx = st.get("x").unwrap().as_f64().unwrap() as f32;
    let sty = st.get("y").unwrap().as_f64().unwrap() as f32;
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = menuz.get_mut(&propid).unwrap();

    match w {
        KEmenuTypes::text(awa) => {
            awa.size = Vector2::new(stx, sty);
        },
        KEmenuTypes::image(awa) => {
            awa.size = Vector2::new(stx, sty);
        },
    }
    
    Ok(JsValue::Undefined)

}

fn mod_menu_shader(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut menuz = MENUS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_i32(_ctx).unwrap();
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = menuz.get_mut(&propid).unwrap();

    match w {
        KEmenuTypes::text(awa) => {
            awa.shader = st;
        },
        KEmenuTypes::image(awa) => {
            awa.shader = st;
        },
    }
    
    Ok(JsValue::Undefined)

}

fn mod_menu_render(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut menuz = MENUS.write().unwrap();
    let st = _nargs.get_or_undefined(1).to_boolean();
    
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = menuz.get_mut(&propid).unwrap();

    match w {
        KEmenuTypes::text(awa) => {
            awa.render = st;
        },
        KEmenuTypes::image(awa) => {
            awa.render = st;
        },
    }
    
    Ok(JsValue::Undefined)

}

fn raycast_fire(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut phys_world = PW.write().unwrap();

    let st = _nargs.get_or_undefined(0).to_json(_ctx)?;
    let pos1x = st.get("x").unwrap().as_f64().unwrap() as f32;
    let pos1y = st.get("y").unwrap().as_f64().unwrap() as f32;
    let pos1z = st.get("z").unwrap().as_f64().unwrap() as f32;
    let pos1 = Vector3::new(pos1x, pos1y, pos1z);

    let st = _nargs.get_or_undefined(1).to_json(_ctx)?;
    let pos2x = st.get("x").unwrap().as_f64().unwrap() as f32;
    let pos2y = st.get("y").unwrap().as_f64().unwrap() as f32;
    let pos2z = st.get("z").unwrap().as_f64().unwrap() as f32;
    let pos2 = Vector3::new(pos2x, pos2y, pos2z);

    let rayreturn = phys_world.raycast_fire_simple(pos1,pos2);

    let json = json!({
        "hit": rayreturn.hit,
        "pos": {"x":rayreturn.point.x,"y":rayreturn.point.y, "z":rayreturn.point.z},
        "nor" : {"x":rayreturn.normal.x,"y":rayreturn.normal.y, "z":rayreturn.normal.z},
        "whats": rayreturn.whats
    });

    let fvalue = JsValue::from_json(&json, _ctx).unwrap();
    
    Ok(fvalue)

}

fn get_file(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    if !*ALLOWFILEGRAB.read().unwrap() {
        return Ok(JsValue::String("no file grab allowed".into()));
    }
    let sat = _nargs.get_or_undefined(0).as_string().unwrap().as_ref();

    let rawtext = match std::string::String::from_utf16(sat) {
        Ok(x) => x,
        Err(_) => "".to_owned()
    };
    
    Ok(JsValue::String(grab(&rawtext).into()))

}

fn engineexit(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = REQUESTS.write().unwrap();

    propz.push(crate::KERequest::exit);
    drop(propz);
    
    Ok(JsValue::Undefined)

}

fn tepter(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = REQUESTS.write().unwrap();

    let textureid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();
    let newbufferid = _nargs.get_or_undefined(1).to_i32(_ctx).unwrap();

    propz.push(crate::KERequest::Pin_Texture_Buffer(textureid, newbufferid));
    drop(propz);
    
    Ok(JsValue::Undefined)

}

fn window_cursor_lock(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut propz = REQUESTS.write().unwrap();
    let st = _nargs.get_or_undefined(0).to_boolean();

    propz.push(crate::KERequest::window_cursor_lock(st));
    drop(propz);
    
    Ok(JsValue::Undefined)
}

impl ScriptSpace<'_> {

    pub fn new() -> ScriptSpace<'static>{

        // Instantiate the execution context
        let mut context: Context<'_> = Context::default();

        // Initialize the Console object.
        let console = Console::init(&mut context);

        // Register the console as a global property to the context.
        context
            .register_global_property(Console::NAME, console, Attribute::all())
            .expect("the console object shouldn't exist yet");

        context.register_global_callable("awawawa", 0, NativeFunction::from_fn_ptr(awawawa))
            .unwrap();

        match context.eval(Source::from_bytes(include_str!("KE/ke_api.js"))) {
            Ok(res) => {
                
            }
            Err(e) => {
                // Pretty print the error
                eprintln!("Uncaught {e}");
            }
        };

        ScriptSpace {
            world: 0,
            context,
            scripts: Vec::new(),
        }
    }


    //note to self, do some magic with macros to make this nightmare less nightmareish
    pub fn pinpropz(&mut self){

        self.context.register_global_builtin_callable("get_existing_prop_by_name", 1, NativeFunction::from_fn_ptr(get_existing_prop_by_name));

        self.context.register_global_builtin_callable("create_prop", 1, NativeFunction::from_fn_ptr(create_prop));
        self.context.register_global_builtin_callable("delete_prop", 1, NativeFunction::from_fn_ptr(delete_prop));
        self.context.register_global_builtin_callable("mod_prop_pos", 1, NativeFunction::from_fn_ptr(mod_prop_pos));
        self.context.register_global_builtin_callable("get_prop_pos", 1, NativeFunction::from_fn_ptr(get_prop_pos));
        self.context.register_global_builtin_callable("mod_prop_scale", 1, NativeFunction::from_fn_ptr(mod_prop_scale));
        self.context.register_global_builtin_callable("get_prop_scale", 1, NativeFunction::from_fn_ptr(get_prop_scale));
        self.context.register_global_builtin_callable("mod_prop_shader", 1, NativeFunction::from_fn_ptr(mod_prop_shader));
        self.context.register_global_builtin_callable("get_prop_shader", 1, NativeFunction::from_fn_ptr(get_prop_shader));
        self.context.register_global_builtin_callable("mod_prop_model", 1, NativeFunction::from_fn_ptr(mod_prop_model));
        self.context.register_global_builtin_callable("get_prop_model", 1, NativeFunction::from_fn_ptr(get_prop_model));
        self.context.register_global_builtin_callable("mod_prop_texture", 1, NativeFunction::from_fn_ptr(mod_prop_texture));
        self.context.register_global_builtin_callable("get_prop_texture", 1, NativeFunction::from_fn_ptr(get_prop_texture));
        self.context.register_global_builtin_callable("mod_prop_rot", 1, NativeFunction::from_fn_ptr(mod_prop_rot));
        self.context.register_global_builtin_callable("get_prop_rot", 1, NativeFunction::from_fn_ptr(get_prop_rot));
        self.context.register_global_builtin_callable("lookat_prop", 1, NativeFunction::from_fn_ptr(lookat_prop));
        self.context.register_global_builtin_callable("mod_prop_vel", 1, NativeFunction::from_fn_ptr(mod_prop_vel));
        self.context.register_global_builtin_callable("get_prop_vel", 1, NativeFunction::from_fn_ptr(get_prop_vel));
        self.context.register_global_builtin_callable("mod_prop_vel_onlyside", 1, NativeFunction::from_fn_ptr(mod_prop_vel_onlyside));
        self.context.register_global_builtin_callable("mod_prop_copy_phys", 1, NativeFunction::from_fn_ptr(mod_prop_copy_phys));
        self.context.register_global_builtin_callable("get_all_props_in_world", 1, NativeFunction::from_fn_ptr(get_all_props_in_world));

        self.context.register_global_builtin_callable("get_existing_cam_by_name", 1, NativeFunction::from_fn_ptr(get_existing_cam_by_name));
        
        self.context.register_global_builtin_callable("create_camera", 1, NativeFunction::from_fn_ptr(create_camera));
        self.context.register_global_builtin_callable("mod_camera_pos", 1, NativeFunction::from_fn_ptr(mod_camera_pos));
        self.context.register_global_builtin_callable("get_camera_pos", 1, NativeFunction::from_fn_ptr(get_camera_pos));
        self.context.register_global_builtin_callable("mod_camera_rot", 1, NativeFunction::from_fn_ptr(mod_camera_rot));
        self.context.register_global_builtin_callable("get_camera_rot", 1, NativeFunction::from_fn_ptr(get_camera_rot));
        self.context.register_global_builtin_callable("mod_camera_dis", 1, NativeFunction::from_fn_ptr(mod_camera_dis));
        self.context.register_global_builtin_callable("get_camera_dis", 1, NativeFunction::from_fn_ptr(get_camera_dis));
        self.context.register_global_builtin_callable("mod_camera_bw2", 1, NativeFunction::from_fn_ptr(mod_camera_bw2));
        self.context.register_global_builtin_callable("get_camera_bw2", 1, NativeFunction::from_fn_ptr(get_camera_bw2));
        self.context.register_global_builtin_callable("lookat_camera", 1, NativeFunction::from_fn_ptr(lookat_camera));
        self.context.register_global_builtin_callable("set_main_camera", 1, NativeFunction::from_fn_ptr(set_main_camera));

        self.context.register_global_builtin_callable("create_light", 1, NativeFunction::from_fn_ptr(create_light));
        self.context.register_global_builtin_callable("mod_light_pos", 1, NativeFunction::from_fn_ptr(mod_light_pos));
        self.context.register_global_builtin_callable("get_light_pos", 1, NativeFunction::from_fn_ptr(get_light_pos));

        self.context.register_global_builtin_callable("create_menu_image", 1, NativeFunction::from_fn_ptr(create_menu_image));
        self.context.register_global_builtin_callable("create_menu_image_texture", 1, NativeFunction::from_fn_ptr(mod_menu_texture));

        self.context.register_global_builtin_callable("create_menu_text", 1, NativeFunction::from_fn_ptr(create_menu_text));
        self.context.register_global_builtin_callable("mod_menu_text_text", 1, NativeFunction::from_fn_ptr(mod_menu_text_text));
        self.context.register_global_builtin_callable("get_menu_text_text", 1, NativeFunction::from_fn_ptr(get_menu_text_text));

        //TODO: make gets
        self.context.register_global_builtin_callable("mod_menu_pos", 1, NativeFunction::from_fn_ptr(mod_menu_pos));
        self.context.register_global_builtin_callable("mod_menu_scale", 1, NativeFunction::from_fn_ptr(mod_menu_scale));
        self.context.register_global_builtin_callable("mod_menu_shader", 1, NativeFunction::from_fn_ptr(mod_menu_shader));
        self.context.register_global_builtin_callable("mod_menu_render", 1, NativeFunction::from_fn_ptr(mod_menu_render));

        self.context.register_global_builtin_callable("create_texture", 1, NativeFunction::from_fn_ptr(create_texture));

        self.context.register_global_builtin_callable("create_shader", 1, NativeFunction::from_fn_ptr(create_shader));

        self.context.register_global_builtin_callable("create_model", 1, NativeFunction::from_fn_ptr(create_model));

        self.context.register_global_builtin_callable("exit", 1, NativeFunction::from_fn_ptr(engineexit));
        //self.context.register_global_builtin_callable("quit", 1, NativeFunction::from_fn_ptr(engineexit));
        self.context.register_global_builtin_callable("tepter", 1, NativeFunction::from_fn_ptr(tepter));
        self.context.register_global_builtin_callable("window_cursor_lock", 1, NativeFunction::from_fn_ptr(window_cursor_lock));

        self.context.register_global_builtin_callable("raycast_fire", 1, NativeFunction::from_fn_ptr(raycast_fire));

        self.context.register_global_builtin_callable("get_file", 1, NativeFunction::from_fn_ptr(get_file));
        
        
    }

    pub fn pinemit(&mut self, reseve: Sender<Value>){

        let wopper = move | _this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>| -> JsResult<JsValue> {
            let st = _nargs.get_or_undefined(0).to_json(_ctx)?;
            reseve.send(st);
            Ok(JsValue::Boolean(true))
        };
        self.context.register_global_builtin_callable("_emit", 1, unsafe { NativeFunction::from_closure(wopper) });
    }

    pub fn pindummyemit(&mut self){

        let wopper = move | _this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>| -> JsResult<JsValue> {
            Ok(JsValue::Boolean(false))
        };
        self.context.register_global_builtin_callable("_emit", 1, unsafe { NativeFunction::from_closure(wopper) });
    }

    pub fn add_script(&mut self, location: String){
        let js_code: String = grab(&location);

        self.scripts.push((js_code, location))
    }

    pub fn run(&mut self){

        for code in &self.scripts {
            // Parse the source code
            match self.context.eval(Source::from_bytes(&code.0)) {
                Ok(res) => {
                    
                }
                Err(e) => {
                    // Pretty print the error
                    eprintln!("Uncaught {e}");
                }
            };
        }
    }

    //this inits server functions
    pub fn run_server(&mut self){

        for code in &self.scripts {
            match self.context.eval(Source::from_bytes(&("dispatchEvent(`server_start`,null)"))) {
                Ok(res) => {
                    // println!(
                    //     "{}",
                    //     res.to_string(&mut self.context).unwrap().to_std_string_escaped()
                    // );
                }
                Err(e) => {
                    // Pretty print the error
                    eprintln!("Uncaught {e}");
                }
            };
        }
    }

    pub fn triggerlis(&mut self, what:&String, data: &String){
        //match self.context.eval(Source::from_bytes(&("loop(".to_owned()+&delta.to_string().to_owned()+")"))) {

            // let globalThis = self.context.global_object();

            // let a = globalThis.get(PropertyKey::String("dispatchEvent".into()), &mut self.context);

            // a.unwrap().as_callable();

        match self.context.eval(Source::from_bytes(&("dispatchEvent(`".to_owned()+what+"`,"+data+")"))) {
            Ok(res) => {
                // println!(
                //     "{}",
                //     res.to_string(&mut self.context).unwrap().to_std_string_escaped()
                // );
            }
            Err(e) => {
                // Pretty print the error
                eprintln!("Uncaught {e}");
            }
        };
    }


    pub fn job_runs(&mut self, delta: f32){
        //match self.context.eval(Source::from_bytes(&("loop(".to_owned()+&delta.to_string().to_owned()+")"))) {
        match self.context.eval(Source::from_bytes(&("dispatchEvent(`tick`,".to_owned()+&delta.to_string().to_owned()+")"))) {
            Ok(res) => {
                // println!(
                //     "{}",
                //     res.to_string(&mut self.context).unwrap().to_std_string_escaped()
                // );
            }
            Err(e) => {
                // Pretty print the error
                eprintln!("Uncaught {e}");
            }
        };
    }

    pub fn job_server_runs(&mut self, delta: f32){
        //match self.context.eval(Source::from_bytes(&("loop(".to_owned()+&delta.to_string().to_owned()+")"))) {
        match self.context.eval(Source::from_bytes(&("dispatchEvent(`server_tick`,".to_owned()+&delta.to_string().to_owned()+")"))) {
            Ok(res) => {
                // println!(
                //     "{}",
                //     res.to_string(&mut self.context).unwrap().to_std_string_escaped()
                // );
            }
            Err(e) => {
                // Pretty print the error
                eprintln!("Uncaught {e}");
            }
        };
    }
}