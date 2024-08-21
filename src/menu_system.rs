use std::{char, collections::HashMap, fs, time::SystemTime};

use glium::Display;
use glutin::api::egl::display;
use nalgebra::Vector2;
use serde::{Deserialize, Serialize};
use serde_json::{json, Value};

use crate::{fs_system::grab, textures::Texture, TEXTURE_COUNT};



pub enum KEmenuTypes {
    image(menuimage),
    text(menutext)
}

#[derive(Serialize, Deserialize)]
pub struct RawKEchar {
    pub Char: String,
    pub pos: Vec<i32>,
    pub size: Vec<i32>
}

pub struct KEchar {
    pub Char: char,
    pub pos: Vector2<i32>,
    pub size: Vector2<i32>
}

pub struct KEfont {
    pub texture: i32,
    pub location: String,
    pub time_changed: SystemTime,
    pub chars: HashMap<char,KEchar>,
    pub default_line_height: i32
}

#[derive(Clone)]
pub struct menuimage {
    pub position: Vector2<f32>,
    pub size: Vector2<f32>,
    pub texture: i32,
    pub shader: i32,
    pub uv: Vector2<f32>,
    pub render: bool
}

#[derive(Clone)]
pub struct menutext {
    pub position: Vector2<f32>,
    pub size: Vector2<f32>,
    pub text: String,
    pub font: i32,
    pub shader: i32,
    pub render: bool
}

impl KEfont {
    pub fn new(location: String, texturez: &mut HashMap<i32, Texture>, display: &Display) -> KEfont {

        let texture = {
            let mut texturecount = TEXTURE_COUNT.write().unwrap();
            *texturecount = *texturecount + 1;
            texturez.insert(*texturecount, Texture::craft(&format!("{location}.png"), &display));

            *texturecount
        };

        let (chars, metadata, default_line_height) = {
            let raw_data = grab(&format!("{location}.json"));
            let metadata = fs::metadata(format!("{location}.json")).expect("failed to check font json file");
            let char_data: Value = serde_json::from_str(&raw_data).unwrap();
            let default_line_height = char_data["default_line_height"].as_i64().unwrap() as i32;
            let aw = char_data["characters"].as_array().unwrap();

            let mut chars = HashMap::new();

            for chardata in aw.iter() {
                let char: Vec<char> = chardata["char"].as_str().unwrap().chars().collect();
                let char: char = char[0];

                let rawdt = chardata["pos"].as_array().unwrap();
                let x = rawdt[0].as_i64().unwrap() as i32;
                let y = rawdt[1].as_i64().unwrap() as i32;
                let pos = Vector2::new(x, y);

                let rawdt = chardata["size"].as_array().unwrap();
                let x = rawdt[0].as_i64().unwrap() as i32;
                let y = rawdt[1].as_i64().unwrap() as i32;
                let size = Vector2::new(x, y);



                let _final = KEchar {
                    Char: char,
                    pos,
                    size,
                };
                
                chars.insert(char, _final);

                
            }
            (chars, metadata, default_line_height)
        };

        KEfont{
            texture,
            location,
            time_changed: metadata.modified().unwrap(),
            chars,
            default_line_height
        }
    }
}