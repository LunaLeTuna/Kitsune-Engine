use nalgebra::Vector2;



pub enum KEmenuTypes {
    image(menuimage),
    text(menutext)
}

#[derive(Clone)]
pub struct menuimage {
    pub position: Vector2<f32>,
    pub size: Vector2<f32>,
    pub texture: i32,
    pub uv: Vector2<f32>,
}

#[derive(Clone)]
pub struct menutext {
    pub position: Vector2<f32>,
    pub size: Vector2<f32>,
    pub text: String,
}