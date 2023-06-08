use std::fs;

//Need to eventually move all parts to this like textures and shaders

pub fn grab(location: &str) -> String {
    let file = fs::read_to_string(location).unwrap();
    file
}