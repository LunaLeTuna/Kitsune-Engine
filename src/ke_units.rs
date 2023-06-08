// use serde::Deserialize;
// use deno_core::serde::Serialize;

// #[derive(Deserialize, Default, Debug, Clone, Copy, Serialize)]
// #[serde(rename_all = "camelCase")]
pub struct Vec2 {
    pub x: f32,
    pub y: f32,
}

impl Vec2 {
    pub fn New(x:f32,y:f32) -> Vec2{
        Vec2 {
            x: x,
            y: y,
        }
    }
}

// #[derive(Deserialize, Default, Debug, Serialize)]
// #[serde(rename_all = "camelCase")]
pub struct Vec3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

impl Vec3 {
    pub fn New(x:f32,y:f32,z:f32) -> Vec3{
        Vec3 {
            x: x,
            y: y,
            z: z,
        }
    }
}

pub fn parsef(string: &str) -> f32 { string.parse().unwrap_or(-1.0) }
pub fn parse(string: &str) -> usize { string.parse().unwrap_or(0) }