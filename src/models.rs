use std::fs;

use glium::vertex::VertexBuffer;

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

fn parsef(string: &str) -> f32 { string.parse().unwrap_or(-1.0) }
fn parse(string: &str) -> usize { string.parse().unwrap_or(0) }

pub fn load_obj<'a>(location: &str, display: &'a glium::Display) -> Model<'a> {
    let file = fs::read_to_string(location).unwrap();

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

    dbg!(vf.len(), pos.len());

    for ((&v, &n), &t) in vf.iter().zip(nf.iter()).zip(tf.iter()) {
        final_v.push(Vertex {
            position: pos[v],
            normal: nor[n],
            tex_coords: tex[t],
        });
    }

    Model {
        name: "nya",
        verts: convert_ke_to_gl(&final_v, display),
    }
}
