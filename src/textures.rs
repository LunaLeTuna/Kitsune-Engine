use std::path::Path;

pub struct Texture {
    pub name: String,
    pub texture: glium::texture::SrgbTexture2d,
}

pub fn craft(location: String, display: &glium::Display) -> Texture {

    let image = image::open(&Path::new(&location)).ok().unwrap();
    let mut img = image.into_rgba16();

    let image_dimensions = img.dimensions();
    let image = glium::texture::RawImage2d::from_raw_rgba_reversed(&img.as_raw(), image_dimensions);
    let map = glium::texture::SrgbTexture2d::new(display, image).unwrap();
    Texture { 
        name: String::from ("nya"),
        texture: map,
    }
}