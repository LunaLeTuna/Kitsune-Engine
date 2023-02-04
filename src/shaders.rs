use std::fs;

use glium::Program;

pub struct Shader {
    pub name: String,
    pub program: Program,
}

pub fn craft(location: &str, display: &glium::Display) -> Shader {
    let vert_sause = fs::read_to_string(format!("{location}.vert")).unwrap();

    let frag_sause = fs::read_to_string(format!("{location}.frag")).unwrap();

    let prg = Program::from_source(display, &vert_sause, &frag_sause, None).unwrap();

    Shader {
        name: "nya".into(),
        program: prg,
    }
}
