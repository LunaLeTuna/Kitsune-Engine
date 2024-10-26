use std::collections::HashMap;

use crate::{menu_system::{KEfont, KEmenuTypes}, models::Model, props::Prop, shaders::Shader, textures::Texture, FONTS, MENUS, PROPS};

pub enum garbtypes {
    script,
    texture,
    model,
    shader,
}

pub fn rummage_props(propz: &mut HashMap<i32, Prop>) -> Vec<(Vec<i32>)> {
    let mut found: Vec<Vec<i32>> = Vec::new();
    found.push(Vec::new()); // found[0] = Vec::new(); //scripts
    found.push(Vec::new()); // found[1] = Vec::new(); //textures
    found.push(Vec::new()); // found[2] = Vec::new(); //models
    found.push(Vec::new()); // found[3] = Vec::new(); //shaders

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

pub fn rummage_menues(menuz: &mut HashMap<i32, KEmenuTypes>, fontz: &mut HashMap<i32, KEfont>) -> Vec<(Vec<i32>)> {
    let mut found: Vec<Vec<i32>> = Vec::new();
    found.push(Vec::new()); // found[0] = Vec::new(); //scripts
    found.push(Vec::new()); // found[1] = Vec::new(); //textures
    found.push(Vec::new()); // found[2] = Vec::new(); //models
    found.push(Vec::new()); // found[3] = Vec::new(); //shaders

    for pr in menuz {
        let pr = pr.1;
        match pr {
            KEmenuTypes::image(pr) => {
                if !found[1].contains(&pr.texture) {found[1].push(pr.texture);}
                //if !found[2].contains(&&pr.model) {found[2].push(pr.model);}
                if !found[3].contains(&&pr.shader) {found[3].push(pr.shader);}
            },
            KEmenuTypes::text(pr) => {
                //if !found[2].contains(&&pr.model) {found[2].push(pr.model);}
                if !found[3].contains(&&pr.shader) {found[3].push(pr.shader);}
            },
        }
        
    };

    for fong in fontz {
        if !found[1].contains(&fong.1.texture) {found[1].push(fong.1.texture);}
    }
    
    found
}

pub fn clear(texturez: &mut HashMap<i32, Texture>, modelz: &mut HashMap<i32, Model>, shadersz: &mut HashMap<i32, Shader>) {
    let mut propz = PROPS.write().unwrap();
    let mut choppingblock = rummage_props(&mut propz);
    let mut menues = MENUS.write().unwrap();
    let mut fonts = FONTS.write().unwrap();
    let mut cahromvpbnlock = rummage_menues(&mut menues,&mut fonts);

    choppingblock[0].append(&mut cahromvpbnlock[0]);
    choppingblock[1].append(&mut cahromvpbnlock[1]);
    choppingblock[2].append(&mut cahromvpbnlock[2]);
    choppingblock[3].append(&mut cahromvpbnlock[3]);

    let ignorelist = vec![
        vec![],
        vec![0,1],
        vec![0,1,2],
        vec![0,1,2,3,4,5]
    ];

    //textures
    let mut torem = Vec::new();
    for tx in texturez.iter() {
        let tx = tx.0;
        if !choppingblock[1].contains(&tx) {
            if ignorelist[1].contains(&tx) {continue;}
            
            if texturez.get(&tx).unwrap().bebufpointer {
                continue;
            }
            
            torem.push(*tx);
        }
    }
    for tx in torem {
        texturez.remove(&tx);
    }

    //models
    let mut torem = Vec::new();
    for tx in modelz.iter() {
        let tx = tx.0;
        if !choppingblock[2].contains(&tx) {
            if ignorelist[2].contains(&tx) {continue;}
            torem.push(*tx);
        }
    }
    for tx in torem {
        modelz.remove(&tx);
    }

    //shaders
    let mut torem = Vec::new();
    for tx in shadersz.iter() {
        let tx = tx.0;
        if !choppingblock[3].contains(&tx) {
            if ignorelist[3].contains(&tx) {continue;}
            torem.push(*tx);
        }
    }
    for tx in torem {
        shadersz.remove(&tx);
    }
}