#[macro_use]
extern crate glium;

use std::io::Cursor;

use nalgebra::{Vector2, Vector3, Rotation3};

pub mod cameras;
pub mod shaders;
pub mod models;
pub mod textures;
pub mod props;

fn main() {
    #[allow(unused_imports)]
    use glium::{glutin, Surface};

    let event_loop = glutin::event_loop::EventLoop::new();
    let wb = glutin::window::WindowBuilder::new();
    let cb = glutin::ContextBuilder::new().with_depth_buffer(24);
    let display = glium::Display::new(wb, cb, &event_loop).unwrap();

    let mut modelz:Vec<Model> = Vec::new();
    let mut texturez:Vec<Texture> = Vec::new();

    modelz.push(models::load_obj(format!("./pig.obj"),&display));
    texturez.push(textures::craft(String::from("./engine_dependent/ellie_def/PiggoTexture.png"),&display));
    texturez.push(textures::craft(String::from("./engine_dependent/ellie_def/PiggoTexture.png"),&display));

    let pig = props::Prop{
        name: String::from ("nya"),
        model: &modelz[0],
        position: Vector3::new(0.0,0.0,0.0),
        scale: Vector3::new(1.0,1.0,1.0),
        texture1: &texturez[0],
        texture2: &texturez[0],
        rotation: Vector3::new(0.0,0.0,0.0),
    };


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
        let ang = t.sin();
        let (c, s) = (ang.cos(), ang.sin());
        let _model = [
            [  c, 0.0,   s, 0.0],
            [0.0, 1.0, 0.0, 0.0],
            [ -s, 0.0,   c, 0.0],
            [0.0, 0.0, 0.0, 1.0f32]
        ];

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
        render_prop(&mut target,&pig,&_main_cam,&params,&program);
        target.finish().unwrap();
    });
}