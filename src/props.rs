use std::{sync::{Arc, RwLock}, collections::HashMap};

use nalgebra::{Vector3, Rotation3};

use crate::shaders::{shadvType};

#[derive(Clone)]
pub struct Prop {
    pub name: String,
    pub shader: i32,
    pub shader_vars: HashMap<String, shadvType>,
    pub model: i32,
    pub texture1: i32,
    pub texture2: i32,
    pub position: Vector3<f32>,
    pub rotation: Rotation3<f32>,
    pub scale: Vector3<f32>,
    pub render: bool
}

impl Prop {
    pub fn new(name: String) -> Prop{
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
            render: true
        }
    }

    pub fn set_rotation(&mut self, target: Vector3<f32>) {
        self.rotation = nalgebra::Rotation3::new(target);
    }
}
