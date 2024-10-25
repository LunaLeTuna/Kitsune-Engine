use std::collections::HashMap;

use crate::{models::Model, props::Prop, shaders::Shader, textures::Texture, PROPS};

pub enum garbtypes {
    script,
    texture,
    model,
    shader,
}

pub fn rummage_props(propz: &mut HashMap<i32, Prop>) -> Vec<(Vec<i32>)> {
    let mut found: Vec<Vec<i32>> = Vec::new();
    //found[0] = Vec::new(); //scripts
    found[1] = Vec::new(); //textures
    found[2] = Vec::new(); //models
    found[3] = Vec::new(); //shaders

    for pr in propz {
        let pr = pr.1;
        for tex in &pr.textures {
            if !found[1].contains(&tex) {found[1].push(*tex);}
        }
        if !found[2].contains(&&pr.model) {found[2].push(pr.model);}
        if !found[3].contains(&&pr.shader) {found[3].push(pr.shader);}
    };
    
    found
}

pub fn clear(texturez: &mut HashMap<i32, Texture>, modelz: &mut HashMap<i32, Model>, shadersz: &mut HashMap<i32, Shader>) {
    let mut propz = PROPS.write().unwrap();
    let choppingblock = rummage_props(&mut propz);

    let ignorelist = vec![
        vec![],
        vec![0,1,2,3,4,5],
        vec![0,1,2],
        vec![0,1]
    ];

    //textures
    for tx in 0..*texturez.keys().last().unwrap() {
        if !choppingblock[1].contains(&tx) {
            if ignorelist[1].contains(&tx) {continue;}
            
            if texturez.get(&tx).unwrap().bebufpointer {
                continue;
            }
            
            texturez.remove(&tx);
        }
    }

    //models
    for tx in 0..*modelz.keys().last().unwrap() {
        if !choppingblock[2].contains(&tx) {
            if ignorelist[2].contains(&tx) {continue;}
            modelz.remove(&tx);
        }
    }

    //shaders
    for tx in 0..*shadersz.keys().last().unwrap() {
        if !choppingblock[3].contains(&tx) {
            if ignorelist[3].contains(&tx) {continue;}
            shadersz.remove(&tx);
        }
    }
}