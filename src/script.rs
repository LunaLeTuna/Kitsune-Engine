use std::{time::{Instant, Duration}, collections::HashMap, borrow::BorrowMut, sync::{RwLock, mpsc::Sender}, future::Future};

use boa_engine::{Context, JsResult, JsValue, JsArgs, Source, property::{Attribute, PropertyKey}, NativeFunction, value::TryFromJs, JsNativeError, builtins::{function, string, error, json}, JsString};
use boa_runtime::Console;
use nalgebra::Vector3;
use serde_json::{json, Value};
use rust_socketio::{ClientBuilder, Payload, RawClient, client::Client};
use smol::future::FutureExt;

use crate::{fs_system::grab, props::Prop, PointLight, LIGHTS, PROPS};

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
    let mut propz = PROPS.read().unwrap();
    let propid = _nargs.get_or_undefined(0).to_i32(_ctx).unwrap();

    let w = propz.get(&propid).unwrap();

    let json = json!({
        "x": w.position.x,
        "y": w.position.y,
        "z": w.position.z
    });

    let fvalue = JsValue::from_json(&json, _ctx).unwrap();
    
    Ok(fvalue)

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

fn create_prop(_this: &JsValue, _nargs: &[JsValue], _ctx: &mut Context<'_>) -> JsResult<JsValue> {
    let mut womp = Prop::new("nya :3".to_owned());
    womp.model = 0;
    womp.textures = vec![0,0];
    let mut propz = PROPS.write().unwrap();
    let i = propz.len() as i32;
    propz.insert(i, womp);
    
    Ok(JsValue::Integer(i))

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

    pub fn pinpropz(&mut self){

        self.context.register_global_builtin_callable("get_existing_prop_by_name", 1, NativeFunction::from_fn_ptr(get_existing_prop_by_name));

        self.context.register_global_builtin_callable("create_prop", 1, NativeFunction::from_fn_ptr(create_prop));
        self.context.register_global_builtin_callable("mod_prop_pos", 1, NativeFunction::from_fn_ptr(mod_prop_pos));
        self.context.register_global_builtin_callable("get_prop_pos", 1, NativeFunction::from_fn_ptr(get_prop_pos));
        self.context.register_global_builtin_callable("mod_prop_scale", 1, NativeFunction::from_fn_ptr(mod_prop_scale));
        self.context.register_global_builtin_callable("get_prop_scale", 1, NativeFunction::from_fn_ptr(get_prop_scale));
        self.context.register_global_builtin_callable("mod_prop_rot", 1, NativeFunction::from_fn_ptr(mod_prop_rot));
        self.context.register_global_builtin_callable("get_prop_rot", 1, NativeFunction::from_fn_ptr(get_prop_rot));

        self.context.register_global_builtin_callable("create_light", 1, NativeFunction::from_fn_ptr(create_light));
        self.context.register_global_builtin_callable("mod_light_pos", 1, NativeFunction::from_fn_ptr(mod_light_pos));
        self.context.register_global_builtin_callable("get_light_pos", 1, NativeFunction::from_fn_ptr(get_light_pos));
        
        
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