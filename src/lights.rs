use nalgebra::{Rotation3, Vector3};

pub struct PointLight {
    pub name: String,

    pub position: Vector3<f32>,
    pub ambient: Vector3<f32>,
    pub diffuse: Vector3<f32>,
    pub specular: Vector3<f32>,

    pub constant: f32,
    pub linear: f32,
    pub quadratic: f32
}

impl PointLight {
    pub fn new () -> PointLight {
        PointLight {
            position: Vector3::new(0.0,0.0,0.0),
            ambient:  Vector3::new(0.05,0.05,0.05),
            diffuse:  Vector3::new(0.8,0.8,0.8),
            specular:  Vector3::new(1.0,1.0,1.0),
            constant: 1.0,
            linear: 0.09,
            quadratic: 0.032,
            name: "".to_owned(),
        }
    }
}