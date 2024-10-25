use std::{borrow::Borrow, collections::HashMap, os::raw};

use crate::{char_control::character_type, fs_system::grab, ke_units::{parsef, parsei}, ALLOWFILEGRAB};

pub struct keconfig {
    pub steam_id: i32,
    pub default_map: String,
    pub run_script: Vec<String>,
    pub char_pov: character_type, //character controller type
    pub hotswap: bool, //to update assets in realtime by modifying their file
    pub shader_hotswap: bool,
    pub debug: bool, //print to console
    pub headless: bool,
    pub is_server: bool,
    pub has_multiplayer: bool,
    pub override_shader: Vec<String>,
    pub mouse_sensitivity: f32,
    pub char_speed: f32,
    pub buffers: u8,
    pub garbage_collect: bool
}

#[repr(u8)]
#[derive(Debug)]
pub enum kect { //keconftypes
    bool(bool),
    float(f32),
    int(i32),
    u8(u8),
    string(String),
    vecstring(Vec<String>),
    chartype(character_type),
    null,
}

impl keconfig {
    pub fn parse(location: String) -> keconfig{
        let mut conf = keconfig{
            steam_id: -1,
            default_map: "".to_string(),
            char_pov: character_type::Disabled,
            run_script: Vec::new(),
            hotswap: false,
            debug: false,
            headless: false,
            is_server: false,
            shader_hotswap: false,
            has_multiplayer: false,
            override_shader: Vec::new(),
            mouse_sensitivity: 4.0,
            char_speed: 0.0,
            buffers: 0,
            garbage_collect: false
        };

        // let mut keconf: HashMap<&str, kect> = HashMap::new();

        // keconf.insert("default_map", kect::string("".to_owned()));
        // keconf.insert("char_pov", kect::chartype(character_type::Disabled));
        // keconf.insert("run_script", kect::vecstring(Vec::new()));
        // keconf.insert("shader_hotswap", kect::bool(false));
        // keconf.insert("font_hotswap", kect::bool(false));
        // keconf.insert("debug", kect::bool(false));
        // keconf.insert("debug", kect::bool(false));
        // keconf.insert("debug", kect::bool(false));
        // keconf.insert("debug", kect::bool(false));
        // keconf.insert("debug", kect::bool(false));


        let file = grab(&location);
        for line in file.lines() {
            let dat: Vec<&str> = line.split_whitespace().collect();
            match dat.as_slice() {
                ["allow_grab_file"] => {
                    *ALLOWFILEGRAB.write().unwrap() = true;
                }
                ["garbage_collect"] => {
                    conf.garbage_collect = true;
                }
                ["enable_steam", sid] => {
                    conf.steam_id = parsei(sid);
                }
                ["default_map", stm] => {
                    conf.default_map = stm.to_string();
                    continue;
                }
                ["char_pov", stm] => {
                    match *stm {
                        "firstperson" => {
                            conf.char_pov = character_type::First
                        }
                        "fly" => {
                            conf.char_pov = character_type::NoClip
                        }
                        "script" => {
                            conf.char_pov = character_type::Scripted
                        }
                        "side" => {
                            conf.char_pov = character_type::Scroller
                        }
                        "disable" => {
                            conf.char_pov = character_type::Disabled
                        }
                        _ => {}
                    }
                    continue;
                }
                ["runtime_script", stm] => {
                    let rawlist = stm.to_string();
                    let rawlist: Vec<&str> = rawlist.split(";").collect();
                    let mut rawlist: Vec<String> = rawlist.iter().map(|&s|s.into()).collect();
                    conf.run_script.append(&mut rawlist);
                    continue;
                },
                ["mouse_sensitivity", stm] => {
                    conf.mouse_sensitivity = parsef(stm);
                    continue;
                },
                ["buffers", stm] => {
                    conf.buffers = parsei(stm) as u8;
                    continue;
                },
                ["shader_hotswap"] => {
                    conf.shader_hotswap = true;
                    continue;
                }
                ["debug"] => {
                    conf.debug = true;
                    continue;
                }
                ["headless"] => {
                    conf.headless = true;
                    continue;
                }
                ["has_multiplayer"] => {
                    conf.has_multiplayer = true;
                    continue;
                }
                ["is_server"] => {
                    conf.is_server = true;
                    continue;
                }
                ["override_shader", stm] => {
                    let rawlist = stm.to_string();
                    let rawlist: Vec<&str> = rawlist.split(";").collect();
                    let rawlist: Vec<String> = rawlist.iter().map(|&s|s.into()).collect();
                    conf.override_shader = rawlist;
                    continue;
                }
                _ => {}
            }
        }

        conf
    }
}