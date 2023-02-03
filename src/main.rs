#[macro_use]
extern crate glium;

use std::{io::Cursor, collections::HashMap};

use nalgebra::{Vector2, Vector3, Rotation3, Matrix4};

pub mod cameras;
pub mod shaders;
pub mod models;
pub mod textures;
pub mod props;

use {models::Model, textures::Texture, props::Prop, shaders::Shader};

fn main() {
    #[allow(unused_imports)]
    use glium::{glutin, Surface};

    let event_loop = glutin::event_loop::EventLoop::new();
    let wb = glutin::window::WindowBuilder::new();
    let cb = glutin::ContextBuilder::new().with_depth_buffer(24);
    let display = glium::Display::new(wb, cb, &event_loop).unwrap();

    let mut propz: Vec<Prop> = Vec::new();
    let mut modelz: HashMap<i32, Model> = HashMap::new();
    let mut texturez: HashMap<i32, Texture> = HashMap::new();

    modelz.insert(0, models::load_obj(format!("./pig.obj"),&display));

    texturez.insert(0,textures::craft(String::from("./engine_dependent/ellie_def/PiggoTexture.png"),&display));
    texturez.insert(1,textures::craft(String::from("./engine_dependent/ellie_def/PiggoTexture.png"),&display));

    let pig = props::Prop{
        name: String::from ("nya"),
        model: 0,
        position: Vector3::new(0.0,0.0,0.0),
        scale: Vector3::new(1.0,1.0,1.0),
        texture1: 0,
        texture2: 1,
        rotation: Vector3::new(0.0,0.0,0.0),
    };

    propz.push(pig);


    let program = shaders::craft(format!("./shaders/base"), &display);

    let start = std::time::Instant::now();
    event_loop.run(move |event, _, control_flow| {
        let next_frame_time = std::time::Instant::now() +
            std::time::Duration::from_nanos(16_666_667);
        *control_flow = glutin::event_loop::ControlFlow::WaitUntil(next_frame_time);

        match event {
            glutin::event::Event::WindowEvent { event, .. } => match event {
                glutin::event::WindowEvent::CloseRequested => {
                    *control_flow = glutin::event_loop::ControlFlow::Exit;
                    return;
                },
                _ => {},
            },
            glutin::event::Event::NewEvents(cause) => match cause {
                glutin::event::StartCause::ResumeTimeReached { .. } => (),
                glutin::event::StartCause::Init => (),
                _ => {},
            },
            _ => {},
        }

        let mut target = display.draw();
        target.clear_color_and_depth((0.2, 0.3, 0.3, 1.0), 1.0);

        let t = (std::time::Instant::now() - start).as_secs_f32() * 2.0;

        let (width, height) = target.get_dimensions();
        let mut _main_cam = cameras::craft(Vector2::new(width as f32, height as f32));
        _main_cam.look_at(Vector3::new(0.0, 0.0, 0.0));
        _main_cam.position = Vector3::new(5.0*t.sin(), 0.0, -6.0);
        // dbg!(5.0*t.sin());
        _main_cam.refresh();

        let light = [1.4, 0.4, 0.7f32];

        let params = glium::DrawParameters {
            depth: glium::Depth {
                test: glium::draw_parameters::DepthTest::IfLess,
                write: true,
                .. Default::default()
            },
            .. Default::default()
        };
        

        for prop in &propz {
            let mut model = Matrix4::new_scaling(1.0);
            // model = model.add(Matrix4::new_rotation(prop.rotation));
            model = model.append_translation(&prop.position);
            let binding = *model.as_ref();


            target.draw(&modelz.get(&prop.model).unwrap().verts, glium::index::NoIndices(glium::index::PrimitiveType::TrianglesList), &program.program,
            &uniform! { model: binding, view: _main_cam.view_drop(), perspective: _main_cam.project_drop(), u_light: light,
                        diffuse_tex: &texturez.get(&prop.texture1).unwrap().texture, normal_tex: &texturez.get(&prop.texture2).unwrap().texture },
            &params).unwrap();
        }



        target.finish().unwrap();
    });
}