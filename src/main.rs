#[macro_use]
extern crate glium;

pub mod cameras;
pub mod shaders;
pub mod models;
pub mod textures;
pub mod props;

use std::collections::HashMap;
use std::fs;
use std::time::{Duration, Instant};

use cameras::Camera;
use glium::draw_parameters::DepthTest;
use glium::glutin::event_loop::{ControlFlow, EventLoop};
use glium::glutin::window::WindowBuilder;
use glium::glutin::ContextBuilder;
use glium::index::{NoIndices, PrimitiveType};
use glium::{Depth, Display, DrawParameters, Surface};
use nalgebra::{Matrix4, Vector2, Vector3, Rotation3};
use textures::Texture;
use winit::event::{Event, StartCause, WindowEvent};

use v8::{self, Local};

fn main() {
    //init gl and window
    let event_loop = EventLoop::new();
    let wb = WindowBuilder::new();
    let cb = ContextBuilder::new().with_depth_buffer(24);
    let display = Display::new(wb, cb, &event_loop).unwrap();

    //init v8
    let platform = v8::new_default_platform(0, false).make_shared();
    v8::V8::initialize_platform(platform);
    v8::V8::initialize();

    let isolate = &mut v8::Isolate::new(v8::CreateParams::default());
    let handle_scope = &mut v8::HandleScope::new(isolate);
    let context = v8::Context::new(handle_scope);
    let context_scope = &mut v8::ContextScope::new(handle_scope, context);

    fun_name(context_scope);

    //creates things
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

    let mut pig = props::Prop {
        name: "nya".into(),
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

    /*
    let mut i=0;
    while 100>=i {
        fun_name1(context_scope, context);
        i+=1;
    }
    */

    fun_name1(context_scope, context); // I want this in the event loop

    //this here does not return, borrowing variables not returning... I think
    event_loop.run(move |event, _, control_flow| {
        let next_frame_time = Instant::now() + Duration::from_nanos(16_666_667);

        *control_flow = ControlFlow::WaitUntil(next_frame_time);

        match event {
            Event::WindowEvent { event, .. } => {
                if event == WindowEvent::CloseRequested {
                    *control_flow = ControlFlow::Exit;

                    //close v8
                    unsafe {
                      v8::V8::dispose();
                    }
                    v8::V8::dispose_platform();
                }
            }
            Event::NewEvents(cause) => match cause {
                StartCause::ResumeTimeReached { .. } => (),
                StartCause::Init => (),
                _ => {}
            },
            _ => {}
        }

        let mut target = display.draw();
        target.clear_color_and_depth((0.2, 0.3, 0.3, 1.0), 1.0);

        let t = (Instant::now() - start).as_secs_f32() * 2.0;

        let (width, height) = target.get_dimensions();

        let mut main_cam = Camera::craft(Vector2::new(width as f32, height as f32));

        pig.set_rotation(Vector3::new(3.14/2.0*t.sin(),0.0,0.0));

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

        for (index, prop) in &propz {
            let mut model = Matrix4::new_scaling(1.0);

            model = model.append_translation(&prop.position);

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
}

fn fun_name1(context_scope: &mut v8::ContextScope<v8::HandleScope>, context: Local<v8::Context>) {
    let mut scope = v8::TryCatch::new(context_scope);
    let key = v8::String::new(&mut scope, "tick").unwrap();
    let tick_funk = context.global(&mut scope).get(&mut scope, key.into()).unwrap();
    if tick_funk.is_function() {
    let globe = context.global(&mut scope);

    let targ: v8::Local<v8::Value> = v8::Number::new(&mut scope, 0.0).into();

    let tick_object: v8::Local<v8::Object> = tick_funk.to_object(&mut scope).unwrap();
    let ftick_result = v8::Local::<v8::Function>::try_from(tick_object).unwrap().call(&mut scope, globe.into(), &[targ]).unwrap();
    dbg!(ftick_result.to_rust_string_lossy(&mut scope));
    }
}

fn fun_name(context_scope: &mut v8::ContextScope<v8::HandleScope>) {
    let mut scope = v8::TryCatch::new(context_scope);
    let js = fs::read_to_string(format!("./index.js")).unwrap();
    let rogger = v8::String::new(&mut scope, &js).unwrap();
    let script = v8::Script::compile(&mut scope, rogger, None).unwrap();
    let result = script.run(&mut scope).unwrap();
    let result = result.to_string(&mut scope).unwrap();
    println!("{}", result.to_rust_string_lossy(&mut scope));
}
