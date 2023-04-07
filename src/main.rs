#[macro_use]
extern crate glium;

pub mod cameras;
pub mod models;
pub mod props;
pub mod shaders;
pub mod textures;
pub mod dynamic_uniform;
pub mod physic_props;
pub mod js_land;

use std::borrow::BorrowMut;
use std::collections::HashMap;
use std::fs;
use std::ops::Mul;
use std::rc::Rc;
use std::sync::{mpsc, Arc, RwLock};
use std::sync::mpsc::Receiver;
use std::time::Instant;

#[macro_use]
extern crate lazy_static;
use cameras::Camera;
use deno_core::v8::{self, Local, Value};
use deno_core::{anyhow, resolve_path, FsModuleLoader, JsRuntime, RuntimeOptions, op};
use dynamic_uniform::DynamicUniforms;
use glium::draw_parameters::DepthTest;
use glium::glutin::event_loop::{ControlFlow, EventLoop};
use glium::glutin::window::WindowBuilder;
use glium::glutin::ContextBuilder;
use glium::index::{NoIndices, PrimitiveType};
use glium::texture::SrgbTexture2d;
use glium::uniforms::{UniformsStorage, AsUniformValue};
use glium::{Depth, Display, DrawParameters, Surface, VertexBuffer, Program};
use js_land::{KE_THREAD_INFORMER, KE_THREAD_WIN, propi, MAKE_REQUEST, REQUEST_TYPE, model_amount, RequestNewObj, shader_amount, shaderi, Vec2, create_js_thread, mouse_pos};
use models::Model;
use nalgebra::{Matrix4, Rotation3, Vector2, Vector3};
use props::Prop;
use shaders::{Shader, craft, shadvType};
use textures::Texture;
use winit::event::{Event, StartCause, WindowEvent};
use winit::platform::run_return::EventLoopExtRunReturn;

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
    let mut shaderz = HashMap::new();

    modelz.insert(0, models::load_obj("./pig.obj", &display));

    texturez.insert(
        0,
        Texture::craft("./engine_dependent/ellie_def/PiggoTexture.png", &display),
    );
    texturez.insert(
        1,
        Texture::craft("./engine_dependent/ellie_def/PiggoTexture.png", &display),
    );

    shaderz.insert(
        0,
        shaders::craft("./shaders/base", &display)
    );
    shaderi.write().expect("RwLock poisoned").insert(0, HashMap::new());

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
    create_js_thread(receiver, senderb, propz.clone());

    let mut js_ready = false;

    event_loop.borrow_mut().run_return(|event, _, control_flow| {
        *control_flow = ControlFlow::Poll;
        match event {
            Event::WindowEvent { event, .. } => {
                match event {
                    WindowEvent::CursorMoved { device_id, position, modifiers } => {
                        //sender.send(KE_THREAD_INFORMER::MouseMoved(Vec2{x: position.x as f32, y: position.y as f32})).unwrap();
                        match mouse_pos.try_write() {
                            Ok(mut n) => {
                                *n = Vec2{x: position.x as f32, y: position.y as f32};
                                drop(n);
                            },
                            Err(_) => (),
                        }
                    }
                    // WindowEvent::KeyboardInput { device_id, input, is_synthetic } => {
                    //     //sender.send(KE_THREAD_INFORMER::MouseMoved(Vec2{x: position.x as f32, y: position.y as f32})).unwrap();
                    //     match mouse_pos.write() {
                    //         Ok(mut n) => {
                    //             *n = Vec2{x: position.x as f32, y: position.y as f32};
                    //             drop(n);
                    //         },
                    //         Err(_) => (),
                    //     }
                    // }
                    WindowEvent::CloseRequested { .. } => {
                        *control_flow = ControlFlow::Exit;
                    },
                    _ => {

                    }
                }
            }
            Event::NewEvents(cause) => match cause {
                StartCause::ResumeTimeReached { .. } => (),
                StartCause::Init => (),
                _ => {}
            },
            _ => ()
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
        }else{
            return
        }

        // this would send a message to tick to those threads but yeah
        // game.tick()
        // js.tick()

        // propz.get_mut(&0).unwrap().set_rotation(Vector3::new(az.sin()/2.0, 0.0, 0.0));
        // propz.get_mut(&0).unwrap().position = Vector3::new(3.14 / 9.0 * az.sin(), 0.0, 0.0);

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

        #[cfg(debug_assertions)]
        {
            for (index, sh) in &shaderz {
                let name = sh.url.clone();
                let metadataF = fs::metadata(format!("{name}.frag")).expect("failed to check shader file");
                let metadataV = fs::metadata(format!("{name}.vert")).expect("failed to check shader file");

                if(metadataF.modified().unwrap() != sh.timeChangedF || metadataV.modified().unwrap() != sh.timeChangedV) {
                    let mut modelreq = MAKE_REQUEST.write().expect("RwLock poisoned");
                    let mut wopper = shader_amount.write().expect("RwLock poisoned");
                    *wopper+=1;

                    wopper; //DO NOT REMOVE THIS WOPPER, for some reason it works with it here

                    let wopper = *shader_amount.read().unwrap();

                    let new_req = RequestNewObj {
                        rType: REQUEST_TYPE::Shader,
                        url: String::from(&sh.url),
                        ID: *index
                    };

                    let woppera = modelreq.len() as i32;
                    modelreq.insert(woppera, new_req);
                    println!("{} has been updated", name);
                }
            }
        }

        match MAKE_REQUEST.try_write() {
            Ok(mut n) => {
                if !n.is_empty() {
                    n.iter().for_each(|(_index, newer)| {
                        match newer.rType {
                            REQUEST_TYPE::Shader => {
                                let url = newer.url.as_str();
                                shaderz.insert(newer.ID, shaders::craft(url, &display));
                            },
                            REQUEST_TYPE::Model => {
                                let url = newer.url.as_str();
                                modelz.insert(newer.ID, models::load_obj(url, &display));
                            },
                            REQUEST_TYPE::Texture => {
                                let url = newer.url.as_str();
                                texturez.insert(newer.ID, Texture::craft(url, &display));
                            },
                        }
                    });

                    n.clear();
                }

                drop(n);
            },
            Err(_) => (),
        }

        match propi.try_read() {
            Ok(n) => {

                let mut target = display.draw();
                target.clear_color_and_depth((0.2, 0.3, 0.3, 1.0), 1.0);
        
                let t = (Instant::now() - start).as_secs_f32() * 2.0;
        
                let (width, height) = target.get_dimensions();

                let mut main_cam = Camera::craft(Vector2::new(width as f32, height as f32));


                main_cam.set_rotation(Vector3::new(0.0, 0.0, 0.0));
                main_cam.position = Vector3::new(0.0, 0.0, -6.0);

                // dbg!(5.0*t.sin());

                main_cam.refresh();

                n.iter().for_each(|(_index, prop)| {

                    let model = &mut prop.rotation.matrix().to_homogeneous();
                    
                    model.append_nonuniform_scaling_mut(&prop.scale);

                    let model = model.append_translation(&prop.position);
        
                    let binding = *model.as_ref();
        
                    let mut uniform = dynamic_uniform::DynamicUniforms::new();
                    let view = main_cam.view_drop();
                    uniform.add("view", &view);
                    let perspective = main_cam.project_drop();
                    uniform.add("perspective", &perspective);
                    uniform.add("model", &binding);
                    uniform.add("u_light", &light);
                    uniform.add("diffuse_tex", is_texture_real(&texturez, prop));
                    uniform.add("normal_tex", is_texture2_real(&texturez, prop));

                    //shader globle vars (TODO)


                    //prop spesific shader vars
                    for (name, value) in &prop.shader_vars {
                        match value {
                            shadvType::bool(value) => {
                                uniform.add(&name, value);
                            },
                            shadvType::integer(value) => {
                                uniform.add(&name, value);
                            },
                            shadvType::float(value) => {
                                uniform.add(&name, value);
                            },
                            shadvType::vec2(value) => {
                                uniform.add(&name, value.as_ref());
                            },
                            shadvType::vec3(value) => {
                                uniform.add(&name, value.as_ref());
                            },
                            _ => println!("how"),

                        }
                    }

                    target
                        .draw(
                            is_model_real(&modelz, prop),
                            NoIndices(PrimitiveType::TrianglesList),
                            is_shader_real(&shaderz, prop),
                            &uniform,
                            &params,
                        )
                        .unwrap();
                });

                drop(n);
                target.finish().unwrap();
            },
            Err(_) => (),
        };

    });

}

fn is_shader_real<'a>(shadersz: &'a HashMap<i32, Shader>, prop: &Prop) -> &'a Program {
    //dbg!(&prop.model);
    let m = shadersz.get(&prop.shader);
    let real = match m {
        Some(a) => true,
        None => false
    };
    if real == false {return &shadersz.get(&0).unwrap().program};
    &m.unwrap().program
}

fn is_model_real<'a>(modelz: &'a HashMap<i32, Model>, prop: &Prop) -> &'a VertexBuffer<models::Vertex> {
    //dbg!(&prop.model);
    let m = modelz.get(&prop.model);
    let real = match m {
        Some(a) => true,
        None => false
    };
    if real == false {return &modelz.get(&0).unwrap().verts};
    &m.unwrap().verts
}

fn is_texture_real<'a>(texturez: &'a HashMap<i32, Texture>, prop: &Prop) -> &'a SrgbTexture2d {
    let m = texturez.get(&prop.texture1);
    let real = match m {
        Some(a) => true,
        None => false
    };
    if real == false {return &texturez.get(&0).unwrap().texture};
    &m.unwrap().texture
}

fn is_texture2_real<'a>(texturez: &'a HashMap<i32, Texture>, prop: &Prop) -> &'a SrgbTexture2d {
    let m = texturez.get(&prop.texture2);
    let real = match m {
        Some(a) => true,
        None => false
    };
    if real == false {return &texturez.get(&0).unwrap().texture};
    &m.unwrap().texture
}
