use std::{fs::File, io::BufReader};

use glium::{vertex::VertexBuffer, implement_vertex};

use fbxcel_dom::any::AnyDocument;
use gltf::Gltf;

use crate::{fs_system::grab, ke_units::{parsef, parse}};

#[derive(Copy, Clone)]
pub struct Vertex {
    position: [f32; 3],
    normal: [f32; 3],
    tex_coords: [f32; 2],
}

implement_vertex!(Vertex, position, normal, tex_coords);

pub fn convert_ke_to_gl(ver: &[Vertex], display: &glium::Display) -> VertexBuffer<Vertex> {
    VertexBuffer::new(display, ver).unwrap()
}

pub struct Model<'a> {
    pub name: &'a str,
    pub verts: VertexBuffer<Vertex>,
}

pub fn load_obj<'a>(location: &str, display: &'a glium::Display) -> Model<'a> {
    let file = grab(location);

    let mut pos = vec![];
    let mut nor = vec![];
    let mut tex = vec![];

    let mut vf = vec![];
    let mut tf = vec![];
    let mut nf = vec![];

    for line in file.lines() {
        let dat: Vec<&str> = line.split_whitespace().collect();

        match dat.as_slice() {
            // vertices
            ["v", x, y, z] => {
                pos.push([parsef(x), parsef(y), parsef(z)]);
            }
            // vertex normals
            ["vn", x, y, z] => {
                nor.push([parsef(x), parsef(y), parsef(z)]);
            }
            // texture coordinates
            ["vt", x, y] => {
                tex.push([parsef(x), parsef(y)]);
            }
            // faces
            ["f", face_1, face_2, face_3] => {
                [face_1, face_2, face_3].iter().for_each(|face| {
                    // this would be much better with itertools package
                    let mut split = face.split('/');
                    let (vertex, texture, normal) =
                        (split.next().unwrap(), split.next().unwrap(), split.next().unwrap());

                    // subtract 1 because obj is 1 indexed
                    vf.push(parse(vertex) - 1);
                    tf.push(parse(texture) - 1);
                    nf.push(parse(normal) - 1);
                });
            }
            _ => {}
        }
    }

    let mut final_v: Vec<Vertex> = Vec::new();

    // dbg!(vf.len(), pos.len());

    for ((&v, &n), &t) in vf.iter().zip(nf.iter()).zip(tf.iter()) {
        final_v.push(Vertex {
            position: pos[v],
            normal: nor[n],
            tex_coords: tex[t],
        });
    }

    dbg!(location,pos.len(),nor.len(),tex.len());

    Model {
        name: "nya",
        verts: convert_ke_to_gl(&final_v, display),
    }
}

pub fn load_fbx<'a>(location: &str, display: &'a glium::Display) -> Model<'a> {
    let file = File::open(location).expect("Failed to open file");
    let reader = BufReader::new(file);

    match AnyDocument::from_seekable_reader(reader).expect("Failed to load document") {
        AnyDocument::V7400(ver, doc) => {
            println!("Loaded FBX DOM successfully: FBX version = {:?}", ver);
            for scene in doc.scenes() {
                println!("Scene object: object_id={:?}", scene.object_id());
                let root_id = scene
                    .root_object_id()
                    .expect("Failed to get root object ID");
                println!("\tRoot object ID: {:?}", root_id);
            }
        }
        _ => panic!("FBX version unsupported by this example"),
    }

    let mut final_v: Vec<Vertex> = Vec::new();

    Model {
        name: "nya",
        verts: convert_ke_to_gl(&final_v, display),
    }
}

pub fn load_gltf<'a>(location: &str, display: &'a glium::Display) -> Model<'a> {
    //get it to work with buffer stuff later
    // let file = File::open(location).expect("Failed to open file");
    // let reader = BufReader::new(file);

    let (gltf, buffers, images) = gltf::import(location).unwrap();

    let mut pos: Vec<[f32; 3]> = vec![];
    let mut nor: Vec<[f32; 3]> = vec![];
    let mut tex: Vec<[f32; 2]> = vec![];

    let mut vf: Vec<usize> = vec![];
    let mut tf: Vec<usize> = vec![];
    let mut nf: Vec<usize> = vec![];

    for mesh in gltf.meshes() {
        // println!("Mesh #{}", mesh.index());
        for primitive in mesh.primitives() {
            // println!("- Primitive #{}", primitive.index());
            let reader = primitive.reader(|buffer| Some(&buffers[buffer.index()]));
            if let Some(iter) = reader.read_normals() {
                for vertex_normal in iter {
                    // println!("{:?}", vertex_normal);
                    nor.push(vertex_normal);
                }
            }
            if let Some(iter) = reader.read_positions() {
                for vertex_position in iter {
                    pos.push(vertex_position);
                    tex.push([0.5,0.5]);
                }
            }
        }
    }

    let mut final_v: Vec<Vertex> = Vec::new();
 
    for ((&v, &n), &t) in pos.iter().zip(nor.iter()).zip(tex.iter()) {
        final_v.push(Vertex {
            position: v,
            normal: n,
            tex_coords: t,
        });
    }

    // dbg!(location,pos.len(),nor.len(),tex.len());

    Model {
        name: "nya",
        verts: convert_ke_to_gl(&final_v, display),
    }
}
