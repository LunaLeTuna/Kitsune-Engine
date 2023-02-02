#[macro_use]
extern crate glium;

use std::io::Cursor;

use nalgebra::Vector2;

mod cameras {
    use std::f32::consts::PI;

    use nalgebra::{Vector3, Vector2, Matrix4, Rotation, Rotation3, Matrix, Perspective3};

    pub struct camera{
        pub name: String,
        pub projection: Perspective3<f32>,

        pub position: Vector3<f32>,
        pub rotation: Rotation<f32,3>,
        pub up: Vector3<f32>,

        pub near: f32,
        pub far: f32,
        pub fov: f32,
        pub aspect: f32
    }

    fn radians(degrees: f32) -> f32 {
        degrees*PI/180.0
    }

    fn reproject(cam: &mut camera, screen: Vector2<f32>){
        cam.projection = Perspective3::new(screen.x/screen.y,radians(cam.fov), cam.near, cam.far);
    }

    fn look_at(cam: &mut camera, target: Vector3<f32>){
        let meow:Vector3<f32> = Vector3::new(cam.position.x+target.x,cam.position.y+target.y,cam.position.z+target.z);
        cam.rotation = nalgebra::Rotation3::look_at_lh(&meow, &cam.up);
    }

    fn refresh(cam: &mut camera){
        let mut model = Matrix4::new_scaling(1.0);
        model.transform_vector(&cam.position);
        println!("{}", model);
    }

    pub fn projectDROP(cam: camera) -> [[f32; 4]; 4] {
        let binding = cam.projection.to_homogeneous();
        let pogger =binding.as_ref();
        *pogger
    }

    pub fn craft(screen: Vector2<f32>) -> camera {
        let mut pog = camera { name: "meow".to_owned(),
            up: Vector3::y(), 
            far: 1024.0, 
            near: 0.1, 
            fov: 60.0,
            projection: Perspective3::new(screen.y/screen.x,radians(60.0), 0.1, 1024.0),
            position: Vector3::new(0.0, 0.0, 0.0),
            rotation: Rotation3::new(Vector3::new(0.0, 0.0, 0.0)),
            aspect: screen.x/screen.y,
        };
        reproject(&mut pog, screen);
        pog
    }
}

#[cfg(test)]
mod pog{
    
}

mod shaders {
    use std::fs;

    pub struct Shader {
        name: String,
        pub program: glium::Program,
    }

    pub fn craft(location: String, display: &glium::Display) -> Shader{

        let vert_sause = fs::read_to_string(format!("{location}.vert")).unwrap();

        let frag_sause = fs::read_to_string(format!("{location}.frag")).unwrap();

        let prg= glium::Program::from_source(display, &vert_sause, &frag_sause, None).unwrap();
    
        Shader {
            name: String::from ("nya"),
            program: prg,
        }
    }
}

mod models {
    use std::error::Error;
    use std::io::{BufReader, BufRead};
    use std::fs::File;

    #[derive(Copy, Clone)]
    pub struct Vertex {
        position: [f32; 3],
        normal: [f32; 3],
        tex_coords: [f32; 2],
    }

    implement_vertex!(Vertex, position, normal, tex_coords);

    pub fn convert_ke_to_gl(ver: &[Vertex], display: &glium::Display) -> glium::VertexBuffer<Vertex>{
        let amongus = glium::vertex::VertexBuffer::new(display, ver).unwrap();
        amongus
    }

    pub struct Model {
        pub name: String,
        pub verts: glium::VertexBuffer<Vertex>,
    }

    pub fn load_obj(location: String, display: &glium::Display) -> Model{

        let raw_content = File::open(location).unwrap();
        let buffed = BufReader::new(raw_content);
    
        let mut pos:Vec<[f32; 3]> = Vec::new();
        let mut nor:Vec<[f32; 3]> = Vec::new();
        let mut tex:Vec<[f32; 2]> = Vec::new();

        let mut vf:Vec<usize> = Vec::new();
        let mut tf:Vec<usize> = Vec::new();
        let mut nf:Vec<usize> = Vec::new();

        for line in buffed.lines() {
            let meow = line.unwrap();
            let mut dat = meow.split_whitespace();

            let ltype = dat.next().unwrap();

            if ltype == "v" {
                let x:f32 = dat.next().unwrap().parse().unwrap();
                let y:f32 = dat.next().unwrap().parse().unwrap();
                let z:f32 = dat.next().unwrap().parse().unwrap();
                pos.push([x,y,z]);
            }
            if ltype == "vn" {
                let x:f32 = dat.next().unwrap().parse().unwrap();
                let y:f32 = dat.next().unwrap().parse().unwrap();
                let z:f32 = dat.next().unwrap().parse().unwrap();
                nor.push([x,y,z]);
            }
            if ltype == "vt" {
                let x:f32 = dat.next().unwrap().parse().unwrap();
                let y:f32 = dat.next().unwrap().parse().unwrap();
                tex.push([x,y]);
            }
            if ltype == "f" {
                let mut faced = dat.next().unwrap().split("/");
                vf.push(faced.next().unwrap().parse().unwrap());
                tf.push(faced.next().unwrap().parse().unwrap());
                nf.push(faced.next().unwrap().parse().unwrap());
                faced = dat.next().unwrap().split("/");
                vf.push(faced.next().unwrap().parse().unwrap());
                tf.push(faced.next().unwrap().parse().unwrap());
                nf.push(faced.next().unwrap().parse().unwrap());
                faced = dat.next().unwrap().split("/");
                vf.push(faced.next().unwrap().parse().unwrap());
                tf.push(faced.next().unwrap().parse().unwrap());
                nf.push(faced.next().unwrap().parse().unwrap());
            }
        }

        let mut final_v:Vec<Vertex> = Vec::new();

        let mut i = 0;
        let size = vf.len();

        println!("{} {}", size, pos.len());

        while size>i {
            final_v.push(Vertex { position: pos[vf[i]-1], normal: nor[nf[i]-1], tex_coords: tex[tf[i]-1] });
            i+=1;
        }

        Model {
            name: String::from ("nya"),
            verts: convert_ke_to_gl(&final_v, display)
        }
    }
}

mod props {
    use nalgebra::Vector3;

    use crate::{shaders::Shader, models::Model};

    pub struct Prop<'a> {
        model: &'a Model,
        position: Vector3<f32>,
        scale: Vector3<f32>
    }

    pub fn render_prop(shader: &Shader, prop: &Prop, display: &glium::Display) {

    }
}

fn main() {
    #[allow(unused_imports)]
    use glium::{glutin, Surface};

    let event_loop = glutin::event_loop::EventLoop::new();
    let wb = glutin::window::WindowBuilder::new();
    let cb = glutin::ContextBuilder::new().with_depth_buffer(24);
    let display = glium::Display::new(wb, cb, &event_loop).unwrap();

    let a = models::load_obj(format!("./pig.obj"),&display);
    let shape = a.verts;


    let image = image::load(Cursor::new(&include_bytes!("../engine_dependent/ellie_def/PiggoTexture.png")),
                            image::ImageFormat::Png).unwrap().to_rgba8();
    let image_dimensions = image.dimensions();
    let image = glium::texture::RawImage2d::from_raw_rgba_reversed(&image.into_raw(), image_dimensions);
    let diffuse_texture = glium::texture::SrgbTexture2d::new(&display, image).unwrap();

    let image = image::load(Cursor::new(&include_bytes!("../engine_dependent/ellie_def/PiggoTexture.png")),
                            image::ImageFormat::Png).unwrap().to_rgba8();
    let image_dimensions = image.dimensions();
    let image = glium::texture::RawImage2d::from_raw_rgba_reversed(&image.into_raw(), image_dimensions);
    let normal_map = glium::texture::Texture2d::new(&display, image).unwrap();


    let program = shaders::craft(format!("./shaders/base"), &display).program;

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
                _ => return,
            },
            glutin::event::Event::NewEvents(cause) => match cause {
                glutin::event::StartCause::ResumeTimeReached { .. } => (),
                glutin::event::StartCause::Init => (),
                _ => return,
            },
            _ => return,
        }

        let mut target = display.draw();
        target.clear_color_and_depth((0.2, 0.3, 0.3, 1.0), 1.0);

        let t = (std::time::Instant::now() - start).as_secs_f32() * 2.0;
        let ang = t.sin();
        let (c, s) = (ang.cos(), ang.sin());
        let model = [
            [  c, 0.0,   s, 0.0],
            [0.0, 1.0, 0.0, 0.0],
            [ -s, 0.0,   c, 0.0],
            [0.0, 0.0, 0.0, 1.0f32]
        ];

        let view = view_matrix(&[0.5, 0.2, 9.0], &[-0.5, -0.2, 3.0], &[0.0, 1.0, 0.0]);

        let (width, height) = target.get_dimensions();
        let mut _main_cam = cameras::craft(Vector2::new(width as f32, height as f32));

        let perspective = {
            let (width, height) = target.get_dimensions();
            let aspect_ratio = height as f32 / width as f32;

            let fov: f32 = 3.141592 / 3.0;
            let zfar = 1024.0;
            let znear = 0.1;

            let f = 1.0 / (fov / 2.0).tan();

            [
                [f *   aspect_ratio   ,    0.0,              0.0              ,   0.0],
                [         0.0         ,     f ,              0.0              ,   0.0],
                [         0.0         ,    0.0,  (zfar+znear)/(zfar-znear)    ,   1.0],
                [         0.0         ,    0.0, -(2.0*zfar*znear)/(zfar-znear),   0.0],
            ]
        };

        let light = [1.4, 0.4, 0.7f32];

        let params = glium::DrawParameters {
            depth: glium::Depth {
                test: glium::draw_parameters::DepthTest::IfLess,
                write: true,
                .. Default::default()
            },
            .. Default::default()
        };

        target.draw(&shape, glium::index::NoIndices(glium::index::PrimitiveType::TriangleStrip), &program,
                    &uniform! { model: model, view: view, perspective: cameras::projectDROP(_main_cam),
                                u_light: light, diffuse_tex: &diffuse_texture, normal_tex: &normal_map },
                    &params).unwrap();
        target.finish().unwrap();
    });
}


fn view_matrix(position: &[f32; 3], direction: &[f32; 3], up: &[f32; 3]) -> [[f32; 4]; 4] {
    let f = {
        let f = direction;
        let len = f[0] * f[0] + f[1] * f[1] + f[2] * f[2];
        let len = len.sqrt();
        [f[0] / len, f[1] / len, f[2] / len]
    };

    let s = [up[1] * f[2] - up[2] * f[1],
             up[2] * f[0] - up[0] * f[2],
             up[0] * f[1] - up[1] * f[0]];

    let s_norm = {
        let len = s[0] * s[0] + s[1] * s[1] + s[2] * s[2];
        let len = len.sqrt();
        [s[0] / len, s[1] / len, s[2] / len]
    };

    let u = [f[1] * s_norm[2] - f[2] * s_norm[1],
             f[2] * s_norm[0] - f[0] * s_norm[2],
             f[0] * s_norm[1] - f[1] * s_norm[0]];

    let p = [-position[0] * s_norm[0] - position[1] * s_norm[1] - position[2] * s_norm[2],
             -position[0] * u[0] - position[1] * u[1] - position[2] * u[2],
             -position[0] * f[0] - position[1] * f[1] - position[2] * f[2]];

    [
        [s_norm[0], u[0], f[0], 0.0],
        [s_norm[1], u[1], f[1], 0.0],
        [s_norm[2], u[2], f[2], 0.0],
        [p[0], p[1], p[2], 1.0],
    ]
}