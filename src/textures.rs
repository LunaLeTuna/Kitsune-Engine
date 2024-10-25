use glium::texture::{RawImage2d, SrgbTexture2d};
use glium::Display;

use crate::{STRINGSLOADED, TEXTURE_COUNT};

pub struct Texture {
    pub name: String,
    pub texture: SrgbTexture2d,
    pub bebufpointer: bool,
    pub bebufto: usize,
    pub url: String
}

impl Texture {
    pub fn craft(location: &str, display: &Display) -> Self {
        let image = match image::open(location) {
            Ok(a) => a,
            Err(_) => {
                dbg!("can't load", location);
                image::DynamicImage::new_rgb8(5, 5)
            },
        };
        let img = image.into_rgba16();

        let image_dimensions = img.dimensions();
        let image = RawImage2d::from_raw_rgba_reversed(img.as_raw(), image_dimensions);
        let map = SrgbTexture2d::new(display, image).unwrap();

        let mut loaded = STRINGSLOADED.write().unwrap();
        (*loaded).push(location.to_string());

        Self {
            name: "nya".into(),
            texture: map,
            bebufpointer: false,
            bebufto: 0,
            url: location.to_string()
        }
    }

    pub fn craft_lorp(width: u32, height: u32, display: &Display) -> Self {
        Self {
            name: "nya".into(),
            texture: glium::texture::SrgbTexture2d::empty(display, width, height).unwrap(),
            bebufpointer: false,
            bebufto: 0,
            url: format!("betternot")
        }
    }
}
