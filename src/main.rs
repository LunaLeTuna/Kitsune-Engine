#[macro_use]
extern crate glium;

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
        normals: [f32; 2],
        tex_coords: [f32; 2],
    }

    implement_vertex!(Vertex, position, normals, tex_coords);

    pub fn convertKEtoGL(ver: &[Vertex], display: &glium::Display) -> glium::VertexBuffer<Vertex>{
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
        let mut nor:Vec<[f32; 2]> = Vec::new();
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
                nor.push([x,y]);
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

        while size<=i {
            final_v.push(Vertex { position: pos[vf[i]], normals: nor[nf[i]], tex_coords: tex[tf[i]] });
            i+=1;
        }

        Model {
            name: String::from ("nya"),
            verts: convertKEtoGL(&final_v, display)
        }
    }
}

fn main() {
    #[allow(unused_imports)]
    use glium::{glutin, Surface};

    let event_loop = glutin::event_loop::EventLoop::new();
    let wb = glutin::window::WindowBuilder::new();
    let cb = glutin::ContextBuilder::new();
    let display = glium::Display::new(wb, cb, &event_loop).unwrap();

    let a = models::load_obj(format!("./pig.obj"),&display);
    let meow = a.verts;

    let indices = glium::index::NoIndices(glium::index::PrimitiveType::TrianglesList);

    let program = shaders::craft(format!("./shaders/base"), &display);

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
        target.clear_color(0.2, 0.3, 0.3, 1.0);
        target.draw(&meow, &indices, &program.program, &glium::uniforms::EmptyUniforms,
                    &Default::default()).unwrap();
        target.finish().unwrap();
    });
}
