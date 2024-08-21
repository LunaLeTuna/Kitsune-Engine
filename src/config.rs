use std::{borrow::Borrow, os::raw};

use crate::{char_control::character_type, fs_system::grab, ke_units::{parsef, parsei}};

pub struct keconfig {
    pub default_map: String,
    pub run_script: Vec<String>,
    pub char_pov: character_type, //character controller type
    pub hotswap: bool, //to update assets in realtime by modifying their file
    pub shader_hotswap: bool,
    pub debug: bool, //print to console
    pub headless: bool,
    pub is_server: bool,
    pub has_multiplayer: bool,
    pub mouse_sensitivity: f32,
    pub char_speed: f32,
    pub buffers: u8
}

impl keconfig {
    pub fn parse(location: String) -> keconfig{
        let mut conf = keconfig{
            default_map: "".to_string(),
            char_pov: character_type::Disabled,
            run_script: Vec::new(),
            hotswap: false,
            debug: false,
            headless: false,
            is_server: false,
            shader_hotswap: false,
            has_multiplayer: false,
            mouse_sensitivity: 4.0,
            char_speed: 0.0,
            buffers: 0
        };

        let file = grab(&location);
        for line in file.lines() {
            let dat: Vec<&str> = line.split_whitespace().collect();
            match dat.as_slice() {
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
                    let rawlist: Vec<String> = rawlist.iter().map(|&s|s.into()).collect();
                    conf.run_script = rawlist;
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
                _ => {}
            }
        }

        conf
    }
}