use std::ops::Add;

use glium::{Frame, DrawParameters, Surface};
use nalgebra::{Vector3, Matrix4, Rotation};

use crate::{shaders::{Shader, self}, models::Model, cameras::camera, textures::Texture};

pub struct Prop<'a> {
    pub name: String,
    pub model: &'a Model,
    pub texture1: &'a Texture,
    pub texture2: &'a Texture,
    pub position: Vector3<f32>,
    pub rotation: Vector3<f32>,
    pub scale: Vector3<f32>
}

pub fn render_prop(target: &mut Frame, prop: &Prop, _main_cam: &camera,params: &DrawParameters, shader: &Shader) {

    let mut model = Matrix4::new_scaling(1.0);
    model = model.add(Matrix4::new_rotation(prop.rotation));
    model = model.append_translation(&prop.position);
    let binding = *model.as_ref();


    target.draw(&prop.model.verts, glium::index::NoIndices(glium::index::PrimitiveType::TrianglesList), &shader.program,
                &uniform! { model: binding, view: _main_cam.view_drop(), perspective: _main_cam.project_drop(),
                            diffuse_tex: prop.texture1.texture, normal_tex: prop.texture2.texture },
                &params).unwrap();
    }