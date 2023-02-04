use nalgebra::Vector3;

pub struct Prop {
    pub name: String,
    pub model: i32,
    pub texture1: i32,
    pub texture2: i32,
    pub position: Vector3<f32>,
    pub rotation: Vector3<f32>,
    pub scale: Vector3<f32>,
}
