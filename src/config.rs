use crate::{fs_system::grab, char_control::character_type};

pub struct keconfig {
    pub default_map: String,
    pub run_script: String,
    pub char_pov: character_type, //character controller type
    pub hotswap: bool, //to update assets in realtime by modifying their file
    pub debug: bool, //print to console
    pub headless: bool,
    pub is_server: bool
}

impl keconfig {
    pub fn parse(location: String) -> keconfig{
        let mut conf = keconfig{
            default_map: "".to_string(),
            char_pov: character_type::First,
            run_script: "".to_string(),
            hotswap: false,
            debug: false,
            headless: false,
            is_server: false
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
                        _ => {}
                    }
                    continue;
                }
                ["runtime_script", stm] => {
                    conf.run_script = stm.to_string();
                    continue;
                },
                ["hotswap"] => {
                    conf.hotswap = true;
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