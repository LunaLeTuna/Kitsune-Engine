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
use std::sync::mpsc;
use std::sync::mpsc::Receiver;
use std::time::Instant;

use cameras::Camera;
use deno_core::v8::{self, Local, Value};
use deno_core::{anyhow, resolve_path, FsModuleLoader, JsRuntime, RuntimeOptions};
use glium::draw_parameters::DepthTest;
use glium::glutin::event_loop::{ControlFlow, EventLoop};
use glium::glutin::window::WindowBuilder;
use glium::glutin::ContextBuilder;
use glium::index::{NoIndices, PrimitiveType};
use glium::{Depth, Display, DrawParameters, Surface};
use js_land::{KE_THREAD_INFORMER, KE_THREAD_WIN};
use nalgebra::{Matrix4, Rotation3, Vector2, Vector3};
use props::Prop;
use textures::Texture;
use winit::event::{Event, StartCause, WindowEvent};
use winit::platform::run_return::EventLoopExtRunReturn;

mod js_land{
    use std::borrow::BorrowMut;
    use std::collections::HashMap;
    use std::rc::Rc;
    use std::sync::mpsc::{self, Sender};
    use std::sync::mpsc::Receiver;
    use std::time::Instant;


    use deno_core::v8::{self, Local, Value};
    use deno_core::{anyhow, resolve_path, FsModuleLoader, JsRuntime, RuntimeOptions};

    //basically this enum allows the js thread to have access to the diffrent prop and resource has maps
    pub enum KE_THREAD_INFORMER {
        Swag(String),
        Based(u32),
        Awa,
    }

    pub enum KE_THREAD_WIN {
        js_ready,
        Swag(String),
        Based(u32),
        Awa,
    }

    async fn async_js_loop(file_path: &str, receiver: Receiver<KE_THREAD_INFORMER>, sender: Sender<KE_THREAD_WIN>) -> anyhow::Result<()> {
        let mut js_runtime = JsRuntime::new(RuntimeOptions {
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

        loop {
            // if no message just dont until there is
            if receiver.try_recv().is_err() {
                continue;
            }

            let scope = &mut v8::HandleScope::new(scope);

            function.call(scope, recv, empty);
        }
    }

    fn js_thread(receiver: Receiver<KE_THREAD_INFORMER>, sender: Sender<KE_THREAD_WIN>) {
        let tokio_thread = tokio::runtime::Builder::new_current_thread()
            .enable_all()
            .build()
            .unwrap();

        tokio_thread.block_on(async_js_loop("./index.js", receiver, sender)).unwrap();
    }

    pub fn create_js_thread(receiver: Receiver<KE_THREAD_INFORMER>, sender: Sender<KE_THREAD_WIN>) { let _ = std::thread::spawn(move || js_thread(receiver, sender)); }
}

fn main() {
    // init gl and window
    let mut event_loop = EventLoop::new();
    let wb = WindowBuilder::new();
    let cb = ContextBuilder::new().with_depth_buffer(24);
    let display = Display::new(wb, cb, &event_loop).unwrap();

    // creates things
    let mut propz = HashMap::new();
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


    let mut pig = Prop {
        name: "nya",
        model: 0,
        position: Vector3::new(0.0, 0.0, 0.0),
        scale: Vector3::new(1.0, 1.0, 1.0),
        texture1: 0,
        texture2: 1,
        rotation: Rotation3::new(Vector3::zeros()),
    };
    
    propz.insert(0, pig);

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
    js_land::create_js_thread(receiver, senderb);

    let mut az: f32 = 0.0;

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

        az=az+1f32;

        if !js_ready {
            if receiverb.try_recv().is_err() {
                return;
            }else{
                js_ready = true;
            }
        }

        // this would send a message to tick to those threads but yeah
        // game.tick()
        // js.tick()

        let mut target = display.draw();
        target.clear_color_and_depth((0.2, 0.3, 0.3, 1.0), 1.0);

        let t = (Instant::now() - start).as_secs_f32() * 2.0;

        let (width, height) = target.get_dimensions();

        let mut main_cam = Camera::craft(Vector2::new(width as f32, height as f32));

        propz.get_mut(&0).unwrap().set_rotation(Vector3::new(az.sin()/2.0, 0.0, 0.0));
        propz.get_mut(&0).unwrap().position = Vector3::new(3.14 / 9.0 * az.sin(), 0.0, 0.0);

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

        for (_index, prop) in &propz {

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
        }

        target.finish().unwrap();
    });

    // this here does not return, borrowing variables not returning... I think
    // event_loop.run(move |event, _, control_flow| {
    // });
}
