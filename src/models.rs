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