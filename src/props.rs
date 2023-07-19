use std::{collections::HashMap, ops::Mul};

use nalgebra::{Rotation3, Vector3, Unit, Matrix4};

use crate::shaders::ShadvType;

#[derive(Clone, PartialEq)]
pub enum phytype {
    Collider,
    Dynamic,
    NULL,
}
#[derive(Clone, PartialEq)]
pub enum physhape {
    Box,
    Ball,
    NULL,
}


#[derive(Clone)]
pub struct Prop {
    pub name: String,
    pub shader: i32,
    pub shader_vars: HashMap<String, ShadvType>,
    pub model: i32,
    pub texture1: i32,
    pub texture2: i32,
    pub position: Vector3<f32>,
    pub rotation: Rotation3<f32>,
    pub rotation_cached: nalgebra::Matrix<f32, nalgebra::Const<4>, nalgebra::Const<4>, nalgebra::ArrayStorage<f32, 4, 4>>,
    pub scale: Vector3<f32>,
    pub render: bool,
    pub phys_type: phytype,
    pub phys_shape: physhape,
    pub phys_id: i32,
}

impl Prop {
    pub fn new(name: String) -> Prop {
        Prop {
            name,
            shader: 0,
            shader_vars: HashMap::new(),
            model: 0,
            position: Vector3::new(0.0, 0.0, 0.0),
            scale: Vector3::new(1.0, 1.0, 1.0),
            texture1: 0,
            texture2: 1,
            rotation: Rotation3::new(Vector3::zeros()),
            rotation_cached: Matrix4::<f32>::new_nonuniform_scaling(&Vector3::new(1.0, 1.0, 1.0)).into(),
            render: true,
            phys_type: phytype::NULL,
            phys_shape: physhape::NULL,
            phys_id: -1,
        }
    }

    pub fn set_rotation(&mut self, target: Vector3<f32>) {
        self.rotation = nalgebra::Rotation3::from_euler_angles(target.x,target.y,target.z);
        let model = Rotation3::from_axis_angle(&Unit::new_normalize(Vector3::new(1.0, 0.0, 0.0)), self.rotation.euler_angles().0).matrix().to_homogeneous();
        let model = Rotation3::from_axis_angle(&Unit::new_normalize(Vector3::new(0.0, 1.0, 0.0)), self.rotation.euler_angles().1).matrix().to_homogeneous().mul(model);
        let model = Rotation3::from_axis_angle(&Unit::new_normalize(Vector3::new(0.0, 0.0, 1.0)), self.rotation.euler_angles().2).matrix().to_homogeneous().mul(model);
        self.rotation_cached = model;
    }
}
