use glium::texture::{RawImage2d, SrgbTexture2d};
use glium::Display;

pub struct Texture {
    pub name: String,
    pub texture: SrgbTexture2d,
}

impl Texture {
    pub fn new(location: &str, display: &Display) -> Self {
        let image = image::open(location).ok().unwrap();
        let img = image.into_rgba16();

        let image_dimensions = img.dimensions();
        let image = RawImage2d::from_raw_rgba_reversed(img.as_raw(), image_dimensions);
        let map = SrgbTexture2d::new(display, image).unwrap();

        Self {
            name: "nya".into(),
            texture: map,
        }
    }
}
