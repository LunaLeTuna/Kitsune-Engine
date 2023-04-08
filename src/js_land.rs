    use std::borrow::BorrowMut;
    use std::collections::HashMap;
    use std::hash::Hash;
    use std::rc::Rc;
    use std::string;
    use std::sync::{Arc, RwLock};
    use std::sync::mpsc::{self, Sender};
    use std::sync::mpsc::Receiver;
    use std::time::Instant;


    use deno_ast::swc::ast::Function;
    use deno_core::serde::Serialize;
    use deno_core::v8::{self, Local, Value, Isolate, Object};
    use deno_core::{anyhow, resolve_path, FsModuleLoader, JsRuntime, RuntimeOptions, op, Extension, ResourceId, include_js_files, ZeroCopyBuf, OpState, serde_v8};
    use glium::program::ShaderType;
    use nalgebra::{Vector3, Rotation3, Vector2};
    use serde::Deserialize;
    use tokio::runtime;
    use winit::event::{DeviceEvent, KeyboardInput, DeviceId, VirtualKeyCode};

    use crate::cameras::Camera;
    use crate::models::Model;
    use crate::props::Prop;
    use crate::shaders::{shadvType, shader_var};

    //basically this enum allows the js thread to have access to the diffrent prop and resource has maps
    pub enum KE_THREAD_INFORMER {
        Swag(String),
        Awa,
    }

    pub enum KE_THREAD_WIN {
        js_ready,
        Swag(String),
        Based(u32),
        Awa,
    }

    pub enum REQUEST_TYPE {
        Model,
        Texture,
        Shader,
    }

    pub struct RequestNewObj {
        pub rType: REQUEST_TYPE,
        pub url: String,
        pub ID: i32
    }

    pub struct KeyEvnt {
        pub device_id: DeviceId,
        pub input: KeyboardInput,
        pub is_synthetic: bool
    }

    pub struct ScreenSize {
        pub x: u32,
        pub y: u32
    }

    lazy_static! {
        //asset management
        pub static ref propi: Arc<RwLock<HashMap<i32, Prop>>> = Arc::new(RwLock::new(HashMap::new()));
        pub static ref shaderi: Arc<RwLock<HashMap<i32, HashMap<String, shader_var>>>> = Arc::new(RwLock::new(HashMap::new()));
        pub static ref MAKE_REQUEST: Arc<RwLock<HashMap<i32, RequestNewObj>>> = Arc::new(RwLock::new(HashMap::new()));
        pub static ref shader_amount: RwLock<i32> = RwLock::new(0);
        pub static ref model_amount: RwLock<i32> = RwLock::new(0);
        pub static ref texture_amount: Arc<RwLock<i32>> = Arc::new(RwLock::new(0));

        pub static ref camerai: Arc<RwLock<HashMap<i32, Camera>>> = Arc::new(RwLock::new(HashMap::new()));
        pub static ref MAIN_CAMERA: RwLock<i32> = RwLock::new(0);

        //input stuff
        pub static ref mouse_pos: Arc<RwLock<Vec2>> = Arc::new(RwLock::new(Vec2 { x: 0.0, y: 0.0 })); //some day add DeviceID for multiplayer with multimouses on x11 :3
        pub static ref key_events: Arc<RwLock<HashMap<u32, KeyEvnt>>> = Arc::new(RwLock::new(HashMap::new()));

        pub static ref SCREEN_SIZE: Arc<RwLock<Vec2>> = Arc::new(RwLock::new(Vec2 { x: 0.0, y: 0.0 }));
    }

    #[op]
    fn op_sum(nums: Vec<f64>) -> Result<f64, deno_core::error::AnyError> {
    // Sum inputs
    let sum = nums.iter().fold(0.0, |a, v| a + v);
    // return as a Result<f64, AnyError>
    Ok(sum)
    }

    #[derive(Deserialize, Default, Debug, Clone, Copy)]
    #[derive(Serialize)]
    #[serde(rename_all = "camelCase")]
    pub struct Vec2 {
        pub x: f32,
        pub y: f32,
    }

    #[derive(Deserialize, Default, Debug)]
    #[derive(Serialize)]
    #[serde(rename_all = "camelCase")]
    pub struct Vec3 {
        pub x: f32,
        pub y: f32,
        pub z: f32,
    }

    #[op]
    fn create_vec3(x: f32, y: f32, z: f32) -> Result<Vec3, deno_core::error::AnyError> {
        
        Ok(Vec3{
            x:x,
            y:y,
            z:z
        })
    }

    #[derive(Deserialize, Default, Debug)]
    #[derive(Serialize)]
    #[serde(rename_all = "camelCase")]
    
    pub struct PropV8 {
        rid: ResourceId
    }


    //I feel like there is some way to auto make these mod_props with macros, but idk lol
    #[op]
    fn mod_prop_rot(prop: i32, vec3: Vec3) -> Result<i32, deno_core::error::AnyError>{

        match propi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                let b = Vector3::new(vec3.x, vec3.y, vec3.z);
                a.unwrap().set_rotation(b);

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn get_prop_rot(prop: i32) -> Result<Vec3, deno_core::error::AnyError> {
        let propz = propi.read().expect("RwLock poisoned");

        let pos = propz.get(&prop).unwrap().rotation.euler_angles();
        
        Ok(Vec3{
            x:pos.0,
            y:pos.1,
            z:pos.2
        })
    }

    #[op]
    fn mod_prop_pos(prop: i32, vec3: Vec3) -> Result<i32, deno_core::error::AnyError>{

        match propi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                let b = Vector3::new(vec3.x, vec3.y, vec3.z);
                a.unwrap().position = b;

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn get_prop_pos(prop: i32) -> Result<Vec3, deno_core::error::AnyError> {
        let propz = propi.read().expect("RwLock poisoned");

        let pos = propz.get(&prop).unwrap().position;
        
        Ok(Vec3{
            x:pos.x,
            y:pos.y,
            z:pos.z
        })
    }

    #[op]
    fn mod_prop_scale(prop: i32, vec3: Vec3) -> Result<i32, deno_core::error::AnyError>{

        match propi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                let b = Vector3::new(vec3.x, vec3.y, vec3.z);
                let c = a.unwrap().scale = b;

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn get_prop_scale(prop: i32) -> Result<Vec3, deno_core::error::AnyError> {
        let propz = propi.read().expect("RwLock poisoned");

        let pos = propz.get(&prop).unwrap().scale;
        
        Ok(Vec3{
            x:pos.x,
            y:pos.y,
            z:pos.z
        })
    }

    #[op]
    fn mod_prop_shader(prop: i32, shaderc: i32) -> Result<i32, deno_core::error::AnyError>{
        match propi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                a.unwrap().shader = shaderc;

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn get_prop_shader(prop: i32, which: i32) -> Result<i32, deno_core::error::AnyError> {
        let propz = propi.read().expect("RwLock poisoned");
        
        Ok(propz.get(&prop).unwrap().shader)
    }

    #[op]
    fn mod_prop_shader_var_int(prop: i32, name: String, data: i32) -> Result<i32, deno_core::error::AnyError>{
        match propi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                a.unwrap().shader_vars.insert(name, shadvType::integer(data));

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn mod_prop_shader_var_float(prop: i32, name: String, data: f32) -> Result<i32, deno_core::error::AnyError>{
        match propi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                a.unwrap().shader_vars.insert(name, shadvType::float(data));

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn mod_prop_shader_var_vec2(prop: i32, name: String, data: Vec2) -> Result<i32, deno_core::error::AnyError>{
        let data = Vector2::new(data.x, data.y);
        match propi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                a.unwrap().shader_vars.insert(name, shadvType::vec2(data));

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn mod_prop_shader_var_vec3(prop: i32, name: String, data: Vec3) -> Result<i32, deno_core::error::AnyError>{
        let data = Vector3::new(data.x, data.y, data.z);
        match propi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                a.unwrap().shader_vars.insert(name, shadvType::vec3(data));

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn mod_prop_model(prop: i32, modelc: i32) -> Result<i32, deno_core::error::AnyError>{
        match propi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                a.unwrap().model = modelc;

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn get_prop_model(prop: i32, which: i32) -> Result<i32, deno_core::error::AnyError> {
        let propz = propi.read().expect("RwLock poisoned");
        
        Ok(propz.get(&prop).unwrap().model)
    }

    #[op]
    fn mod_prop_texture(prop: i32, which: i32, modelc: i32) -> Result<i32, deno_core::error::AnyError>{
        match propi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                match which {
                    0 => a.unwrap().texture1 = modelc,
                    1 => a.unwrap().texture2 = modelc,
                    _ => ()
                }

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn get_prop_texture(prop: i32) -> Result<i32, deno_core::error::AnyError> {
        let propz = propi.read().expect("RwLock poisoned");
        
        Ok(propz.get(&prop).unwrap().texture1)
    }

    #[op]
    fn create_prop() -> Result<i32, deno_core::error::AnyError> {
        let mut propz = propi.write().expect("RwLock poisoned");
        let new_prop = Prop::new("nya".into());

        let wopper = propz.len() as i32;
        propz.insert(wopper, new_prop);
        
        Ok(wopper)
    }

    #[op]
    fn mod_shader_var_int(prop: i32, name: String, data: i32) -> Result<i32, deno_core::error::AnyError>{
        match shaderi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                a.unwrap().insert(name, shader_var{data:shadvType::integer(data)});

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn mod_shader_var_float(prop: i32, name: String, data: f32) -> Result<i32, deno_core::error::AnyError>{
        match shaderi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                a.unwrap().insert(name, shader_var{data:shadvType::float(data)});

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn mod_shader_var_vec2(prop: i32, name: String, data: Vec2) -> Result<i32, deno_core::error::AnyError>{
        let data = Vector2::new(data.x, data.y);
        match shaderi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                a.unwrap().insert(name, shader_var{data:shadvType::vec2(data)});

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn mod_shader_var_vec3(prop: i32, name: String, data: Vec3) -> Result<i32, deno_core::error::AnyError>{
        let data = Vector3::new(data.x, data.y, data.z);
        match shaderi.write() {
            Ok(mut n) => {
                let a = n.get_mut(&prop);
                a.unwrap().insert(name, shader_var{data:shadvType::vec3(data)});

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(prop)
    }

    #[op]
    fn create_shader(url: String) -> Result<i32, deno_core::error::AnyError> {
        let mut modelreq = MAKE_REQUEST.write().expect("RwLock poisoned");
        let mut wopper = shader_amount.write().expect("RwLock poisoned");
        *wopper+=1;

        wopper; //DO NOT REMOVE THIS WOPPER, for some reason it works with it here

        let wopper = *shader_amount.read().unwrap();

        let new_req = RequestNewObj {
            rType: REQUEST_TYPE::Shader,
            url: String::from(url),
            ID: wopper
        };
        shaderi.write().expect("RwLock poisoned").insert(wopper, HashMap::new());

        let woppera = modelreq.len() as i32;
        modelreq.insert(woppera, new_req);
        
        Ok(wopper)
    }

    #[op]
    fn create_model(url: String) -> Result<i32, deno_core::error::AnyError> {
        let mut modelreq = MAKE_REQUEST.write().expect("RwLock poisoned");
        let mut wopper = model_amount.write().expect("RwLock poisoned");
        *wopper+=1;

        wopper; //DO NOT REMOVE THIS WOPPER, for some reason it works with it here

        let wopper = *model_amount.read().unwrap();

        let new_req = RequestNewObj {
            rType: REQUEST_TYPE::Model,
            url: String::from(url),
            ID: wopper
        };

        let woppera = modelreq.len() as i32;
        modelreq.insert(woppera, new_req);
        
        Ok(wopper)
    }

    #[op]
    fn create_texture(url: String) -> Result<i32, deno_core::error::AnyError> {
        let mut modelreq = MAKE_REQUEST.write().expect("RwLock poisoned");
        let mut wopper = model_amount.write().expect("RwLock poisoned");
        *wopper+=1;

        wopper; //NORE CAN YOU REMOVE THIS WOPPER, I have no idea why it works but it does

        let wopper = *model_amount.read().unwrap();

        let new_req = RequestNewObj {
            rType: REQUEST_TYPE::Texture,
            url: String::from(url),
            ID: wopper
        };

        let woppera = modelreq.len() as i32;
        modelreq.insert(woppera, new_req);
        
        Ok(wopper)
    }

    #[op]
    fn mod_cam_rot(cam: i32, vec3: Vec3) -> Result<(), deno_core::error::AnyError>{

        match camerai.write() {
            Ok(mut n) => {
                let a = n.get_mut(&cam).unwrap();
                let b = Vector3::new(vec3.x, vec3.y, vec3.z);
                a.set_rotation(b);
                a.refresh();

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(())
    }

    #[op]
    fn get_cam_rot(cam: i32) -> Result<Vec3, deno_core::error::AnyError> {
        let propz = camerai.read().expect("RwLock poisoned");

        let pos = propz.get(&cam).unwrap().rotation.euler_angles();
        
        Ok(Vec3{
            x:pos.0,
            y:pos.1,
            z:pos.2
        })
    }

    #[op]
    fn mod_cam_pos(cam: i32, vec3: Vec3) -> Result<(), deno_core::error::AnyError>{

        match camerai.write() {
            Ok(mut n) => {
                let a = n.get_mut(&cam).unwrap();
                let b = Vector3::new(-vec3.x, -vec3.y, -vec3.z);
                a.position = b;
                a.refresh();

                drop(n);
            },
            Err(_) => (),
        };

        //propz.get_mut(&prop).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        //drop(propz);
        Ok(())
    }

    #[op]
    fn get_cam_pos(cam: i32) -> Result<Vec3, deno_core::error::AnyError> {
        let propz = camerai.read().expect("RwLock poisoned");

        let pos = propz.get(&cam).unwrap().position;
        
        Ok(Vec3{
            x:pos.x,
            y:pos.y,
            z:pos.z
        })
    }

    #[op]
    fn create_camera() -> Result<i32, deno_core::error::AnyError> {
        match SCREEN_SIZE.try_read() {
            Ok(n) => {
                let mut cameraz = camerai.write().expect("RwLock poisoned");
                let new_cam = Camera::craft(*n);

                let wopper = cameraz.len() as i32;
                cameraz.insert(wopper, new_cam);

                drop(n);
                return Ok(wopper);
            },
            Err(_) => (),
        }
        Ok(0)
    }

    #[op]
    fn set_main_camera(cam: i32) -> Result<(), deno_core::error::AnyError> {
        match SCREEN_SIZE.try_read() {
            Ok(n) => {
                let mut camz = MAIN_CAMERA.write().expect("RwLock poisoned");

                *camz = cam;

                drop(n);
            },
            Err(_) => (),
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

    async fn async_js_loop(file_path: &str, receiver: Receiver<KE_THREAD_INFORMER>, sender: Sender<KE_THREAD_WIN>, propz: Arc<RwLock<HashMap<i32, Prop>>>) -> anyhow::Result<()> {
        
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



        //get tick function
        let export_fn_name = v8::String::new(scope, "tick").unwrap();

        dbg!(export_fn_name.to_rust_string_lossy(scope));

        let export_fn = module_object
            .get(scope, export_fn_name.into())
            .expect("couldnt find fn");

        let function = v8::Local::<v8::Function>::try_from(export_fn)?;




        let empty: &[Local<Value>] = &[];
        let recv: Local<Value> = module_object.into();

        sender.send(KE_THREAD_WIN::js_ready).unwrap();

        let mut az: f32 = 0.0;

        let mut mouse_last_pos = Vec2{x:0.0,y:0.0};

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
            az=az+0.0001;
            // if no message just dont until there is
            
            let thread_mail = receiver.try_recv();

            if thread_mail.is_err() {
                continue;
            }else{
                match thread_mail.unwrap() {
                    KE_THREAD_INFORMER::Awa => (),
                    KE_THREAD_INFORMER::Swag(_) => todo!()
                }
            }

            // not 100% sure there may be a mem leak here, keep 85 eyes open at all times  :::::::::::;::::::::::::::::::::::::::::::3

            match mouse_pos.try_read() {
                Ok(mut n) => {
                    if n.x!=mouse_last_pos.x||n.y!=mouse_last_pos.y {
                        let scope = &mut v8::HandleScope::new(scope);
                        //println!("nya {} {}", n.x, n.y);
                        //mouse_last_pos = Vec2{x:n.x,y:n.y};

                        let x = v8::Number::new(scope, n.x as f64).into();
                        mouseenv.set(scope, xname.into(), x);
                        let y = v8::Number::new(scope, n.y as f64).into();
                        mouseenv.set(scope, yname.into(), y);

                        //get input function
                        fun_name(scope, module_object, recv, "mousemove".to_owned(), mouseenv.into())?;
                        
                    }
                    drop(n);
                },
                Err(_) => (),
            }

            // match key_events.try_read() {
            //     Ok(mut n) => {
            //         if n.len()!=0 {
            //             n.iter().for_each(|evnt| {
            //                 let scope = &mut v8::HandleScope::new(scope);
            //                 //println!("nya {} {}", n.x, n.y);
            //                 //mouse_last_pos = Vec2{x:n.x,y:n.y};

            //                 let mole = v8::ObjectTemplate::new(scope);
            //                 let lename = v8::String::new(scope, "keyID").unwrap(); //.name().into()
            //                 let key = v8::Number::new(scope, evnt.input.scancode.into());
            //                 mole.set(lename.into(), key.into());
                            
            //                 let real = mole.new_instance(scope).unwrap().into();

            //                 //get input function
            //                 fun_name(scope, module_object, recv, "keydown".to_owned(), real);
            //                 ()
            //             });
            //         }
            //         drop(n);
            //     },
            //     Err(_) => (),
            // }

            match key_events.try_write() {
                Ok(mut n) => {
                    if n.len()!=0 {
                        n.iter().for_each(|(_index, evnt)| {
                            let scope = &mut v8::HandleScope::new(scope);
                            //println!("nya {} {}", n.x, n.y);
                            //mouse_last_pos = Vec2{x:n.x,y:n.y};

                            let x = v8::Number::new(scope, evnt.input.scancode as f64).into();
                            keyenv.set(scope, codename.into(), x);

                            //get input function
                            match evnt.input.state {
                                winit::event::ElementState::Pressed => fun_name(scope, module_object, recv, "keydown".to_owned(), keyenv.into()),
                                winit::event::ElementState::Released => fun_name(scope, module_object, recv, "keyup".to_owned(), keyenv.into()),
                            };

                            ()
                        });
                    }
                    n.drain();
                    drop(n);
                },
                Err(_) => (),
            }

            // let mut propz = propz.write().expect("RwLock poisoned");
            // propz.get_mut(&0).unwrap().set_rotation(Vector3::new(az.sin()/2.0, 0.0, 0.0));
            // propz.get_mut(&0).unwrap().position = Vector3::new(3.14 / 9.0 * az.sin(), 0.0, 0.0);


            let scope = &mut v8::HandleScope::new(scope);
            

            if az > 1000.0 {az = 0.0}

            //println!("meow {}", az);



            function.call(scope, recv, empty);
            //drop(scope);
            sender.send(KE_THREAD_WIN::Awa).unwrap();
        }
    }

    fn fun_name(scope: &mut v8::HandleScope, module_object: Local<Object>, recv: Local<Value>, name: String, data: Local<Value>) -> Result<(), anyhow::Error> {
        let export_fn_name = v8::String::new(scope, "_KE_EVENT_PUSH").unwrap();
        let export_fn = module_object
            .get(scope, export_fn_name.into())
            .expect("couldnt find fn");
        let function = v8::Local::<v8::Function>::try_from(export_fn)?;
        let awa = v8::String::new(scope, &name).unwrap();
        function.call(scope, recv, &[awa.into(), data]);
        Ok(())
    }

    pub fn js_thread(receiver: Receiver<KE_THREAD_INFORMER>, sender: Sender<KE_THREAD_WIN>, propz: Arc<RwLock<HashMap<i32, Prop>>>) {
        let tokio_thread = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap();

        tokio_thread.block_on(async_js_loop("./index.js", receiver, sender, propz)).unwrap();
    }

    pub fn create_js_thread(receiver: Receiver<KE_THREAD_INFORMER>, sender: Sender<KE_THREAD_WIN>, propz: Arc<RwLock<HashMap<i32, Prop>>>) { let _ = std::thread::spawn(move || js_thread(receiver, sender, propz)); }