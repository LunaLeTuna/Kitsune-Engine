use glium::texture::{RawImage2d, SrgbTexture2d};
use glium::Display;

use crate::TEXTURE_COUNT;

pub struct Texture {
    pub name: String,
    pub texture: SrgbTexture2d,
    pub bebufpointer: bool,
    pub bebufto: usize
}

impl Texture {
    pub fn craft(location: &str, display: &Display) -> Self {
        let image = image::open(location).ok().unwrap();
        let img = image.into_rgba16();

        let image_dimensions = img.dimensions();
        let image = RawImage2d::from_raw_rgba_reversed(img.as_raw(), image_dimensions);
        let map = SrgbTexture2d::new(display, image).unwrap();

        Self {
            name: "nya".into(),
            texture: map,
            bebufpointer: false,
            bebufto: 0
        }
    }

    pub fn craft_lorp(width: u32, height: u32, display: &Display) -> Self {
        Self {
            name: "nya".into(),
            texture: glium::texture::SrgbTexture2d::empty(display, width, height).unwrap(),
            bebufpointer: false,
            bebufto: 0
        }
    }
}
