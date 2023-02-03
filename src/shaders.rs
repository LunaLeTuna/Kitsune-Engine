use std::fs;

pub struct Shader {
    name: String,
    pub program: glium::Program,
}

pub fn craft(location: String, display: &glium::Display) -> Shader{

    let vert_sause = fs::read_to_string(format!("{location}.vert")).unwrap();

    let frag_sause = fs::read_to_string(format!("{location}.frag")).unwrap();

    let prg= glium::Program::from_source(display, &vert_sause, &frag_sause, None).unwrap();

    Shader {
        name: String::from ("nya"),
        program: prg,
    }
}