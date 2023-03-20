use nalgebra::{Vector3, Rotation3};

#[derive(Clone, Copy)]
pub struct Prop<'a> {
    pub name: &'a str,
    pub shader: i32,
    pub model: i32,
    pub texture1: i32,
    pub texture2: i32,
    pub position: Vector3<f32>,
    pub rotation: Rotation3<f32>,
    pub scale: Vector3<f32>,
    pub render: bool
}
impl Prop<'_> {
    pub fn set_rotation(&mut self, target: Vector3<f32>) {
        self.rotation = nalgebra::Rotation3::new(target);
    }
}
