#[macro_use]
extern crate glium;

pub mod cameras;
pub mod models;
pub mod props;
pub mod shaders;
pub mod textures;

use std::collections::HashMap;
use std::time::{Duration, Instant};

use cameras::Camera;
use glium::draw_parameters::DepthTest;
use glium::glutin::event_loop::{ControlFlow, EventLoop};
use glium::glutin::window::WindowBuilder;
use glium::glutin::ContextBuilder;
use glium::index::{NoIndices, PrimitiveType};
use glium::{Depth, Display, DrawParameters, Surface};
use nalgebra::{Matrix4, Vector2, Vector3};
use textures::Texture;
use winit::event::{Event, StartCause, WindowEvent};

fn main() {
    let event_loop = EventLoop::new();
    let wb = WindowBuilder::new();
    let cb = ContextBuilder::new().with_depth_buffer(24);
    let display = Display::new(wb, cb, &event_loop).unwrap();

    let mut propz = vec![];
    let mut modelz = HashMap::new();
    let mut texturez = HashMap::new();

    modelz.insert(0, models::load_obj("./pig.obj", &display));

    texturez.insert(
        0,
        Texture::new("./engine_dependent/ellie_def/PiggoTexture.png", &display),
    );
    texturez.insert(
        1,
        Texture::new("./engine_dependent/ellie_def/PiggoTexture.png", &display),
    );

    let pig = props::Prop {
        name: "nya".into(),
        model: 0,
        position: Vector3::new(0.0, 0.0, 0.0),
        scale: Vector3::new(1.0, 1.0, 1.0),
        texture1: 0,
        texture2: 1,
        rotation: Vector3::new(0.0, 0.0, 0.0),
    };

    propz.push(pig);

    let program = shaders::craft("./shaders/base", &display);

    let start = Instant::now();

    event_loop.run(move |event, _, control_flow| {
        let next_frame_time = Instant::now() + Duration::from_nanos(16_666_667);

        *control_flow = ControlFlow::WaitUntil(next_frame_time);

        match event {
            Event::WindowEvent { event, .. } => {
                if event == WindowEvent::CloseRequested {
                    *control_flow = ControlFlow::Exit;
                    return;
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

        let mut main_cam = Camera::new(Vector2::new(width as f32, height as f32));

        main_cam.look_at(Vector3::new(0.0, 0.0, 0.0));
        main_cam.position = Vector3::new(5.0 * t.sin(), 0.0, -6.0);

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

        for prop in &propz {
            let mut model = Matrix4::new_scaling(1.0);

            model = model.append_translation(&prop.position);

            let binding = *model.as_ref();

            let uniform = uniform! {
                model: binding,
                view: main_cam.view_drop(),
                perspective: main_cam.project_drop(),
                u_light: light,
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
