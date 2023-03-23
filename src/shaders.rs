use std::{fs, time::SystemTime};

use glium::Program;
use nalgebra::{Vector3, Vector2};

#[cfg(debug_assertions)]
pub struct Shader {
    pub name: String,
    pub program: Program,
    pub url: String,
    pub timeChangedF: SystemTime,
    pub timeChangedV: SystemTime
}

#[cfg(not(debug_assertions))]
pub struct Shader {
    pub name: String,
    pub program: Program,
}

#[derive(Clone, Copy)]
pub enum shadvType {
    bool(bool),
    integer(i32),
    float(f32),
    vec2(Vector2<f32>),
    vec3(Vector3<f32>)
}

pub fn craft(location: &str, display: &glium::Display) -> Shader {
    let vert_sause = fs::read_to_string(format!("{location}.vert")).unwrap();

    let frag_sause = fs::read_to_string(format!("{location}.frag")).unwrap();

    let prg = Program::from_source(display, &vert_sause, &frag_sause, None).unwrap();

    #[cfg(debug_assertions)]
    let sh = {
        let metadataF = fs::metadata(format!("{location}.frag")).expect("failed to check shader file");
        let metadataV = fs::metadata(format!("{location}.vert")).expect("failed to check shader file");

        Shader {
            name: "nya".into(),
            program: prg,
            url: location.to_owned(),
            timeChangedF: metadataF.modified().unwrap(),
            timeChangedV: metadataV.modified().unwrap(),
        }
    };

    #[cfg(not(debug_assertions))]
    let sh = Shader {
        name: "nya".into(),
        program: prg,
    };

    sh
}
