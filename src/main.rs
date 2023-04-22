#[macro_use]
extern crate glium;

pub mod cameras;
pub mod dynamic_uniform;
pub mod js_land;
pub mod models;
pub mod physic_props;
pub mod props;
pub mod shaders;
pub mod textures;

use std::borrow::BorrowMut;
use std::collections::HashMap;
use std::fs;
use std::sync::mpsc;
use std::time::Instant;

#[macro_use]
extern crate lazy_static;
use cameras::Camera;
use glium::draw_parameters::DepthTest;
use glium::glutin::event_loop::{ControlFlow, EventLoop};
use glium::glutin::window::WindowBuilder;
use glium::glutin::ContextBuilder;
use glium::index::{NoIndices, PrimitiveType};
use glium::texture::SrgbTexture2d;
use glium::{Depth, Display, DrawParameters, Program, Surface, VertexBuffer};
use js_land::{
    create_js_thread, KeThreadInformer, KeThreadWin, KeyEvnt, RequestNewObj, RequestType, Vec2, CAMERA_MAP,
    KEY_EVENTS_MAP, MAIN_CAMERA, MAKE_REQUEST_MAP, MOUSE_POS, PROP_MAP, SCREEN_SIZE, SHADER_AMOUNT, SHADER_MAP,
};
use models::Model;
use nalgebra::Vector3;
use props::Prop;
use shaders::{Shader, ShadvType};
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
    let propz = PROP_MAP.clone();
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

    shaderz.insert(0, shaders::craft("./shaders/base", &display));
    SHADER_MAP.write().expect("RwLock poisoned").insert(0, HashMap::new());

    let start = Instant::now();

    let (sender, receiver) = mpsc::channel::<KeThreadInformer>();
    let (senderb, receiverb) = mpsc::channel::<KeThreadWin>();

    if let Ok(mut camera_map) = CAMERA_MAP.try_write() {
        let (width, height) = display.get_framebuffer_dimensions();
        let mut main_cam = Camera::craft(Vec2 {
            x: width as f32,
            y: height as f32,
        });

        main_cam.set_rotation(Vector3::new(0.0, 0.0, 0.0));
        main_cam.position = Vector3::new(0.0, 0.0, -6.0);

        main_cam.refresh();
        camera_map.insert(0, main_cam);
    }

    // init v8/deno
    // ok so this insta starts the tick loop which you probably dont want until the
    // rest initializes also you probaly want to tie the tick to the render
    // updates so youll need a way to like send messages tot htat thread
    create_js_thread(receiver, senderb, propz);

    let mut js_ready = false;

    event_loop.borrow_mut().run_return(|event, _, control_flow| {
        *control_flow = ControlFlow::Poll;

        match event {
            Event::WindowEvent { event, .. } => match event {
                WindowEvent::CursorMoved {
                    device_id: _, position, ..
                } => {
                    if let Ok(mut mouse_pos) = MOUSE_POS.try_write() {
                        *mouse_pos = Vec2 {
                            x: position.x as f32,
                            y: position.y as f32,
                        };
                    }
                }
                WindowEvent::KeyboardInput {
                    device_id,
                    input,
                    is_synthetic,
                } => {
                    if let Ok(mut key_events_map) = KEY_EVENTS_MAP.try_write() {
                        key_events_map.insert(
                            input.scancode,
                            KeyEvnt {
                                is_synthetic,
                                input,
                                device_id,
                            },
                        );
                    }
                }
                WindowEvent::CloseRequested { .. } => {
                    *control_flow = ControlFlow::Exit;
                }
                _ => {}
            },
            Event::NewEvents(cause) => match cause {
                StartCause::ResumeTimeReached { .. } => (),
                StartCause::Init => (),
                _ => {}
            },
            _ => (),
        }

        if let Ok(thread_mail) = receiverb.try_recv() {
            match thread_mail {
                KeThreadWin::JsReady => {
                    js_ready = true;
                }
                KeThreadWin::Swag(_) => (),
                KeThreadWin::Based(_) => (),
                KeThreadWin::Awa => (),
            }
        } else {
            return;
        }

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

        sender.send(KeThreadInformer::Awa).unwrap();

        #[cfg(debug_assertions)]
        {
            for (index, sh) in &shaderz {
                let name = sh.url.clone();
                let metadata_f = fs::metadata(format!("{name}.frag")).expect("failed to check shader file");
                let metadata_v = fs::metadata(format!("{name}.vert")).expect("failed to check shader file");

                if metadata_f.modified().unwrap() != sh.time_changed_f
                    || metadata_v.modified().unwrap() != sh.time_changed_v
                {
                    let mut modelreq = MAKE_REQUEST_MAP.write().expect("RwLock poisoned");
                    let mut wopper = SHADER_AMOUNT.write().expect("RwLock poisoned");
                    *wopper += 1;

                    drop(wopper); // DO NOT REMOVE THIS WOPPER, for some reason it works with it here

                    let _wopper = *SHADER_AMOUNT.read().unwrap();

                    let new_req = RequestNewObj {
                        r_type: RequestType::Shader,
                        url: String::from(&sh.url),
                        id: *index,
                    };

                    let woppera = modelreq.len() as i32;
                    modelreq.insert(woppera, new_req);
                    println!("{} has been updated", name);
                }
            }
        }

        if let Ok(mut make_request_map) = MAKE_REQUEST_MAP.try_write() {
            if !make_request_map.is_empty() {
                make_request_map.iter().for_each(|(_index, newer)| match newer.r_type {
                    RequestType::Shader => {
                        let url = newer.url.as_str();
                        shaderz.insert(newer.id, shaders::craft(url, &display));
                    }
                    RequestType::Model => {
                        let url = newer.url.as_str();
                        modelz.insert(newer.id, models::load_obj(url, &display));
                    }
                    RequestType::Texture => {
                        let url = newer.url.as_str();
                        texturez.insert(newer.id, Texture::craft(url, &display));
                    }
                });

                make_request_map.clear();
            }
        }

        if let Ok(main_camera_id) = MAIN_CAMERA.try_read() {
            if let Ok(mut camera_map) = CAMERA_MAP.try_write() {
                // maybe not do this every frame... later...
                let (width, height) = display.get_framebuffer_dimensions();

                let main_cam = camera_map.get_mut(&main_camera_id).unwrap();
                main_cam.reproject(Vec2 {
                    x: width as f32,
                    y: height as f32,
                });

                if let Ok(prop_map) = PROP_MAP.try_read() {
                    let mut target = display.draw();
                    target.clear_color_and_depth((0.2, 0.3, 0.3, 1.0), 1.0);

                    let _t = (Instant::now() - start).as_secs_f32() * 2.0;

                    let (width, height) = target.get_dimensions();

                    if let Ok(mut screen_size) = SCREEN_SIZE.write() {
                        *screen_size = Vec2 {
                            x: width as f32,
                            y: height as f32,
                        };
                    }

                    prop_map.iter().for_each(|(_index, prop)| {
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
                        uniform.add("diffuse_tex", get_texture(&texturez, prop));
                        uniform.add("normal_tex", get_texture2(&texturez, prop));

                        // shader globle vars (TODO)

                        // prop spesific shader vars
                        for (name, value) in &prop.shader_vars {
                            match value {
                                ShadvType::Bool(value) => {
                                    uniform.add(name, value);
                                }
                                ShadvType::Integer(value) => {
                                    uniform.add(name, value);
                                }
                                ShadvType::Float(value) => {
                                    uniform.add(name, value);
                                }
                                ShadvType::Vec2(value) => {
                                    uniform.add(name, value.as_ref());
                                }
                                ShadvType::Vec3(value) => {
                                    uniform.add(name, value.as_ref());
                                }
                            }
                        }

                        target
                            .draw(
                                get_model(&modelz, prop),
                                NoIndices(PrimitiveType::TrianglesList),
                                get_shader(&shaderz, prop),
                                &uniform,
                                &params,
                            )
                            .unwrap();
                    });

                    target.finish().unwrap();
                }
            }
        }

        sender.send(KeThreadInformer::Awa).unwrap();
    });
}

fn get_shader<'a>(shadersz: &'a HashMap<i32, Shader>, prop: &Prop) -> &'a Program {
    match shadersz.get(&prop.shader) {
        Some(shader) => &shader.program,
        None => &shadersz.get(&0).unwrap().program,
    }
}

fn get_model<'a>(modelz: &'a HashMap<i32, Model>, prop: &Prop) -> &'a VertexBuffer<models::Vertex> {
    match modelz.get(&prop.model) {
        Some(model) => &model.verts,
        None => &modelz.get(&0).unwrap().verts,
    }
}

fn get_texture<'a>(texturez: &'a HashMap<i32, Texture>, prop: &Prop) -> &'a SrgbTexture2d {
    match texturez.get(&prop.texture1) {
        Some(texture) => &texture.texture,
        None => &texturez.get(&0).unwrap().texture,
    }
}

fn get_texture2<'a>(texturez: &'a HashMap<i32, Texture>, prop: &Prop) -> &'a SrgbTexture2d {
    match texturez.get(&prop.texture2) {
        Some(texture) => &texture.texture,
        None => &texturez.get(&0).unwrap().texture,
    }
}
