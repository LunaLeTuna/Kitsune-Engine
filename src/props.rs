use std::{collections::HashMap, ops::Mul};

use nalgebra::{Rotation3, Vector3, Unit, Matrix4};

use crate::shaders::ShadvType;

#[derive(Clone, PartialEq)]
pub enum phytype {
    Collider,
    Dynamic,
    DynamicCollider,
    NULL,
}
#[derive(Clone, PartialEq)]
pub enum physhape {
    Box,
    Ball,
    Capsule,
    Model,
    NULL,
}

#[derive(Clone, PartialEq)]
pub enum proptype {
    Legacy_Brick,
    Brush,
    Model_static,
    Light,
    NULL,
}


#[derive(Clone)]
pub struct Prop {
    pub name: String, //for getting if script needs it

    pub proptype: proptype,

    pub shader: i32, //what shader we need to render object
    pub shader_vars: HashMap<String, ShadvType>, //prop spesific shader properties

    pub model: i32, //model id

    pub textures: Vec<i32>,
    pub transparency: f32,

    pub position: Vector3<f32>,
    pub rotation: Vector3<f32>,
    pub scale: Vector3<f32>,

    pub velocity: Vector3<f32>,

    pub parent_prop: i32,
    pub parent_offset: Vector3<f32>,

    pub render: bool, //if to render

    pub phys_type: phytype, //if rigid, or collision
    pub phys_shape: physhape, //what shape collision is
    pub phys_id: i32, //id to physics world object

    pub shader_non_defalt: bool,
    pub face_cam: bool,

    pub selfid: i32,
    pub world: u32
}

impl Prop {
    pub fn new(name: String) -> Prop {
        Prop {
            name,
            proptype: proptype::NULL,
            shader: 0,
            shader_vars: HashMap::new(),
            model: 0,
            position: Vector3::new(0.0, 0.0, 0.0),
            scale: Vector3::new(1.0, 1.0, 1.0),
            textures: vec![0,1],
            transparency: 1.0,
            rotation: Vector3::zeros(),
            render: true,
            phys_type: phytype::NULL,
            phys_shape: physhape::NULL,
            phys_id: -1,

            shader_non_defalt: false,
            face_cam: false,
            velocity: Vector3::zeros(),

            parent_prop: -1,
            parent_offset: Vector3::zeros(),

            selfid: -1,
            world: 0
        }
    }

    pub fn set_rotation(&mut self, target: Vector3<f32>) {
        self.rotation = Vector3::new(target.x,target.y,target.z);
    }

    pub fn look_at(&mut self, target: Vector3<f32>) {
        let a = nalgebra::Rotation3::face_towards(&(target), &Vector3::y()).euler_angles();
        self.set_rotation(Vector3::new(a.0,a.1,a.2));
    }
}
