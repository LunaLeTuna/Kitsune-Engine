#[macro_use]
extern crate glium;

pub mod cameras;
pub mod models;
pub mod props;
pub mod shaders;
pub mod textures;

use std::borrow::BorrowMut;
use std::collections::HashMap;
use std::rc::Rc;
use std::sync::{mpsc, Arc, RwLock};
use std::sync::mpsc::Receiver;
use std::time::Instant;

#[macro_use]
extern crate lazy_static;
use cameras::Camera;
use deno_core::v8::{self, Local, Value};
use deno_core::{anyhow, resolve_path, FsModuleLoader, JsRuntime, RuntimeOptions, op};
use glium::draw_parameters::DepthTest;
use glium::glutin::event_loop::{ControlFlow, EventLoop};
use glium::glutin::window::WindowBuilder;
use glium::glutin::ContextBuilder;
use glium::index::{NoIndices, PrimitiveType};
use glium::{Depth, Display, DrawParameters, Surface};
use js_land::{KE_THREAD_INFORMER, KE_THREAD_WIN, propi};
use nalgebra::{Matrix4, Rotation3, Vector2, Vector3};
use props::Prop;
use textures::Texture;
use winit::event::{Event, StartCause, WindowEvent};
use winit::platform::run_return::EventLoopExtRunReturn;

mod js_land{
    use std::borrow::BorrowMut;
    use std::collections::HashMap;
    use std::rc::Rc;
    use std::sync::{Arc, RwLock};
    use std::sync::mpsc::{self, Sender};
    use std::sync::mpsc::Receiver;
    use std::time::Instant;


    use deno_core::serde::Serialize;
    use deno_core::v8::{self, Local, Value};
    use deno_core::{anyhow, resolve_path, FsModuleLoader, JsRuntime, RuntimeOptions, op, Extension, ResourceId, include_js_files};
    use nalgebra::{Vector3, Rotation3};
    use serde::Deserialize;

    use crate::models::Model;
    use crate::props::Prop;

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

    lazy_static! {
        pub static ref propi: Arc<RwLock<HashMap<i32, Prop<'static>>>> = Arc::new(RwLock::new(HashMap::new()));
    }

    #[op]
    fn op_sum(nums: Vec<f64>) -> Result<f64, deno_core::error::AnyError> {
    // Sum inputs
    let sum = nums.iter().fold(0.0, |a, v| a + v);
    // return as a Result<f64, AnyError>
    Ok(sum)
    }


    #[derive(Deserialize, Default, Debug)]
    #[derive(Serialize)]
    #[serde(rename_all = "camelCase")]
    pub struct Vec3 {
    x: f32,
    y: f32,
    z: f32,
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

    #[op]
    fn mod_prop_pos(prop: PropV8, vec3: Vec3) -> Result<(), deno_core::error::AnyError> {
        let mut propz = propi.write().expect("RwLock poisoned");

        let prop_id = prop.rid as i32;

        //dbg!("awa", prop_id);

        propz.get_mut(&prop_id).unwrap().position = Vector3::new(vec3.x, vec3.y, vec3.z);
        
        Ok(())
    }

    #[op]
    fn get_prop_pos(prop: PropV8) -> Result<Vec3, deno_core::error::AnyError> {
        let mut propz = propi.read().expect("RwLock poisoned");

        let prop_id = prop.rid as i32;
        let pos = propz.get(&prop_id).unwrap().position;
        
        Ok(Vec3{
            x:pos.x,
            y:pos.y,
            z:pos.z
        })
    }

    #[op]
    fn create_prop() -> Result<PropV8, deno_core::error::AnyError> {
        let mut propz = propi.write().expect("RwLock poisoned");
        let new_prop = Prop {
            name: "nya",
            model: 0,
            position: Vector3::new(0.0, 0.0, 0.0),
            scale: Vector3::new(1.0, 1.0, 1.0),
            texture1: 0,
            texture2: 1,
            rotation: Rotation3::new(Vector3::zeros()),
            render: true
        };

        let wopper = propz.len() as i32;
        propz.insert(wopper, new_prop);

        let wopper = wopper as u32;
        
        Ok(PropV8{
            rid: wopper
        })
    }


    async fn async_js_loop(file_path: &str, receiver: Receiver<KE_THREAD_INFORMER>, sender: Sender<KE_THREAD_WIN>, propz: Arc<RwLock<HashMap<i32, Prop<'_>>>>) -> anyhow::Result<()> {
        
        let ext = Extension::builder("KE_OBjects")
        .ops(vec![
          // An op for summing an array of numbers
          // The op-layer automatically deserializes inputs
          // and serializes the returned Result & value
          create_vec3::decl(),
          mod_prop_pos::decl(),
          get_prop_pos::decl(),
          create_prop::decl(),
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

        loop {
            az=az+0.0001;
            // if no message just dont until there is

            let thread_mail = receiver.try_recv();

            if thread_mail.is_err() {
                continue;
            }else{
                match thread_mail.unwrap() {
                    KE_THREAD_INFORMER::Awa => (),
                    KE_THREAD_INFORMER::Swag(_) => todo!(),
                }
            }

            // let mut propz = propz.write().expect("RwLock poisoned");
            // propz.get_mut(&0).unwrap().set_rotation(Vector3::new(az.sin()/2.0, 0.0, 0.0));
            // propz.get_mut(&0).unwrap().position = Vector3::new(3.14 / 9.0 * az.sin(), 0.0, 0.0);

            let scope = &mut v8::HandleScope::new(scope);

            if az > 1000.0 {az = 0.0}

            //println!("meow {}", az);

            function.call(scope, recv, empty);
        }
    }

    fn js_thread(receiver: Receiver<KE_THREAD_INFORMER>, sender: Sender<KE_THREAD_WIN>, propz: Arc<RwLock<HashMap<i32, Prop<'_>>>>) {
        let tokio_thread = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap();

        tokio_thread.block_on(async_js_loop("./index.js", receiver, sender, propz)).unwrap();
    }

    pub fn create_js_thread(receiver: Receiver<KE_THREAD_INFORMER>, sender: Sender<KE_THREAD_WIN>, propz: Arc<RwLock<HashMap<i32, Prop<'static>>>>) { let _ = std::thread::spawn(move || js_thread(receiver, sender, propz)); }
}

fn main() {
    // init gl and window
    let mut event_loop = EventLoop::new();
    let wb = WindowBuilder::new();
    let cb = ContextBuilder::new().with_depth_buffer(24);
    let display = Display::new(wb, cb, &event_loop).unwrap();

    // creates things
    let mut propz = propi.clone();
    let mut modelz = HashMap::new();
    let mut texturez = HashMap::new();

    modelz.insert(0, models::load_obj("./pig.obj", &display));

    texturez.insert(
        0,
        Texture::craft("./engine_dependent/ellie_def/PiggoTexture.png", &display),
    );
    texturez.insert(
        1,
        Texture::craft("./engine_dependent/ellie_def/PiggoTexture.png", &display),
    );


    // {
    //     let mut propz = propz.write().expect("RwLock poisoned");
    //     let pig = Prop {
    //         name: "nya",
    //         model: 0,
    //         position: Vector3::new(0.0, 0.0, 0.0),
    //         scale: Vector3::new(1.0, 1.0, 1.0),
    //         texture1: 0,
    //         texture2: 1,
    //         rotation: Rotation3::new(Vector3::zeros()),
    //         render: false
    //     };
        
    //     propz.insert(0, pig);
    // }

    let program = shaders::craft("./shaders/base", &display);

    let start = Instant::now();

    // ok so for the message sending probably want uhhhh the thing

    // ok channel lets you send data
    // do you have any data you want to send to js

    // i think that might be wicked for having like
    // function you call from js to rs to fetch that stuff
    let (sender, receiver) = mpsc::channel::<KE_THREAD_INFORMER>();
    let (senderb, receiverb) = mpsc::channel::<KE_THREAD_WIN>();

    // init v8/deno
    // ok so this insta starts the tick loop which you probably dont want until the
    // rest initializes also you probaly want to tie the tick to the render
    // updates so youll need a way to like send messages tot htat thread
    js_land::create_js_thread(receiver, senderb, propz.clone());

    let mut js_ready = false;

    event_loop.borrow_mut().run_return(|event, _, control_flow| {
        *control_flow = ControlFlow::Poll;
        match event {
            Event::WindowEvent { event, .. } => {
                if event == WindowEvent::CloseRequested {
                    *control_flow = ControlFlow::Exit;
                }
            }
            Event::NewEvents(cause) => match cause {
                StartCause::ResumeTimeReached { .. } => (),
                StartCause::Init => (),
                _ => {}
            },
            _ => {}
        }

        let thread_mail = receiverb.try_recv();

        if !thread_mail.is_err() {
            match thread_mail.unwrap() {
                KE_THREAD_WIN::js_ready => {
                    js_ready = true;
                },
                KE_THREAD_WIN::Swag(_) => (),
                KE_THREAD_WIN::Based(_) => (),
                KE_THREAD_WIN::Awa => (),
            }
        }

        let propz = propz.read().expect("RwLock poisoned");

        // this would send a message to tick to those threads but yeah
        // game.tick()
        // js.tick()

        let mut target = display.draw();
        target.clear_color_and_depth((0.2, 0.3, 0.3, 1.0), 1.0);

        let t = (Instant::now() - start).as_secs_f32() * 2.0;

        let (width, height) = target.get_dimensions();

        let mut main_cam = Camera::craft(Vector2::new(width as f32, height as f32));

        // propz.get_mut(&0).unwrap().set_rotation(Vector3::new(az.sin()/2.0, 0.0, 0.0));
        // propz.get_mut(&0).unwrap().position = Vector3::new(3.14 / 9.0 * az.sin(), 0.0, 0.0);

        main_cam.set_rotation(Vector3::new(0.0, 0.0, 0.0));
        main_cam.position = Vector3::new(0.0, 0.0, -6.0);

        // dbg!(5.0*t.sin());

        main_cam.refresh();

        let light = [1.4, 0.4, 0.7f32];

        let params = DrawParameters {
            depth: Depth {
                test: DepthTest::IfLess,
                write: true,
                ..Default::default()
            },
            ..Default::default()
        };

        // send message :)

        sender.send(KE_THREAD_INFORMER::Awa).unwrap();

        propz.iter().for_each(|(_index, prop)| {

            let model = &prop.rotation.matrix().to_homogeneous().append_translation(&prop.position);

            let binding = *model.as_ref();

            let uniform = uniform! {
                model: binding,
                view: main_cam.view_drop(),
                perspective: main_cam.project_drop(),
                u_light: light,
                //not sure how I feel about getting over and over again
                diffuse_tex: &texturez.get(&prop.texture1).unwrap().texture,
                normal_tex: &texturez.get(&prop.texture2).unwrap().texture
            };

            target
                .draw(
                    &modelz.get(&prop.model).unwrap().verts,
                    NoIndices(PrimitiveType::TrianglesList),
                    &program.program,
                    &uniform,
                    &params,
                )
                .unwrap();
        });

        target.finish().unwrap();
    });

    // this here does not return, borrowing variables not returning... I think
    // event_loop.run(move |event, _, control_flow| {
    // });
}
