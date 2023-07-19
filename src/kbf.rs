//I have no idea how file format should go
//so i'ma barrow brick-hill's brk format for maps
//the kitsune brick file :3

use nalgebra::{Vector3, Rotation3};

use crate::{props::{Prop, phytype, physhape}, fs_system::grab, ke_units::{Vec3, parsef, radians}, shaders::ShadvType, lights::PointLight};

pub struct Environment {
    pub ambient: Vector3<f32>,
    pub skyColor: Vector3<f32>,
    pub sun_intensity: f32,
    pub spawnpoints: Vec<Vector3<f32>>
}

pub struct World {
    pub environment: Environment,
    pub props: Vec<Prop>,
    pub lights: Vec<PointLight>
}

pub fn load(location: &str) -> World{
    let mut neo_prop: Vec<Prop> = vec![];
    let mut neo_light: Vec<PointLight> = vec![];
    let mut env: Environment = Environment{
        ambient: Vector3::new(0.0, 0.0, 0.0),
        skyColor: Vector3::new(0.1372549, 0.509804, 0.2),
        sun_intensity: 300.0,
        spawnpoints: Vec::new(),
    };
    let file = grab(location);

    //
    // this basicly figures by the first line what type of file is being loaded
    // like for pure brks we need to adapt bh rotations to something ke can figure
    // or with bh colors and ke colors
    //
    // 0 is a kbf(kitsune brick file)
    // 1 is a brk(I guess that means brick but like brick hill)
    //
    let mut compatibility = 0;


    let mut line_number = 1;
    let mut current_prop = 1;
    let mut currentn_light = 0;
    let mut inside_obj = false;
    let mut inside_light = false;
    for line in file.lines() {
        let dat: Vec<&str> = line.split_whitespace().collect();

        //lookin at the top of the file
        if line_number == 1 {
            if line == "Kitsune Engine V1" {
                compatibility = 0;
            }
            if line == "B R I C K  W O R K S H O P  V0.2.0.0" {
                compatibility = 1;
                
                //this all so needs a baseplate, which is always included in a brk
                //so this prop id should be 0
                neo_prop.push(Prop::new("legacy_baseplate".into()));
            }
        } 
        
        // this is kbf stuff :3

        else if compatibility == 0 {
            match dat.as_slice() {
                [">AmbientColor", r, g, b] => {
                    env.ambient = Vector3::new(parsef(r),parsef(g),parsef(b));
                    inside_obj = false;
                    continue;
                }
                [">SkyColor", r, g, b] => {
                    env.skyColor = Vector3::new(parsef(r),parsef(g),parsef(b));
                    inside_obj = false;
                    continue;
                }
                [">SunIntensity", intensity] => {
                    env.sun_intensity = parsef(intensity);
                    inside_obj = false;
                    continue;
                }
                [">Baseplate", size, r, g, b, trans] => {
                    let mut baseplate = Prop::new("baseplate".to_string());
                    //baseplate.position = Vector3::new(parsef(size)/2.0,0.0,parsef(size)/2.0);
                    baseplate.scale = Vector3::new(parsef(size)/2.0,0.1,parsef(size)/2.0);
                    baseplate.shader_vars.insert("Color".to_string(), ShadvType::Vec3(Vector3::new(parsef(r),parsef(g),parsef(b))));
                    baseplate.model = 1;
                    baseplate.phys_type = phytype::Collider;
                    baseplate.phys_shape = physhape::Box;
                    neo_prop.push(baseplate);
                    inside_obj = false;
                    continue;
                }
                _ => {}
            }

            //getting their parameters
            if inside_obj {
                let current_brick = neo_prop.get_mut(current_prop).unwrap();
                if dat.first().unwrap().chars().next().unwrap() == '+'{
                    match dat.as_slice() {
                        ["+NAME", namet] => {
                            current_brick.name = namet.to_string();
                        }
                        ["+COLOR", r, g, b] => {
                            current_brick.shader_vars.insert("Color".to_string(), ShadvType::Vec3(Vector3::new(parsef(r),parsef(g),parsef(b))));
                        }
                        ["+TRANS", transparancy] => {
                            //TODO
                        }
                        ["+ROT", x, y ,z] => {
                            current_brick.set_rotation(Vector3::new(radians(parsef(x)), radians(parsef(y)), radians(parsef(z))));
                        }
                        ["+NOCOLLISION"] => {
                            current_brick.phys_type = phytype::NULL;
                            current_brick.phys_shape = physhape::NULL;
                        }
                        _ => {}
                    }
                } else {
                    inside_obj = false;
                    inside_light = false;
                    current_prop += 1;
                }
            }
            if inside_light {
                let current_light = neo_light.get_mut(currentn_light).unwrap();
                if dat.first().unwrap().chars().next().unwrap() == '+'{
                    match dat.as_slice() {
                        ["+NAME", namet] => {
                            current_light.name = namet.to_string();
                        }
                        ["+COLOR", r, g, b] => {
                            current_light.ambient = Vector3::new(parsef(r),parsef(g),parsef(b));
                        }
                        _ => {}
                    }
                } else {
                    inside_obj = false;
                    inside_light = false;
                    currentn_light += 1;
                }
            }
            //this section if just for init of entities
            if !inside_obj && !inside_light {
                match dat.as_slice() {
                    ["Legacy_Brick", x,y,z, xs,ys,zs] => {

                        let mut new_brick = Prop::new("what cat?".to_string());
                        new_brick.position = Vector3::new(parsef(x), parsef(y), parsef(z));
                        new_brick.scale = Vector3::new(parsef(xs)/2.0, parsef(ys)/2.0, parsef(zs)/2.0);
                        new_brick.model = 1;
                        new_brick.phys_type = phytype::Collider;
                        new_brick.phys_shape = physhape::Box;
                        neo_prop.push(new_brick);
                        inside_obj = true;
                    }
                    ["Light", x,y,z, xs,ys,zs] => {

                        let mut new_light = PointLight::new();
                        new_light.position = Vector3::new(parsef(x), parsef(y), parsef(z));
                        neo_light.push(new_light);
                        inside_light = true;
                    }
                    ["Spawn_Point", x,y,z, xs,ys,zs] => {
                        env.spawnpoints.push(Vector3::new(parsef(x), parsef(y), parsef(z)));
                    }
                    _ => {}
                }
            }
        }

        // here is where brk support is
        // not gonna do shapes

        // here getting envirement parameters
        else if line_number <= 8 && compatibility == 1 {
            let lain = dat.as_slice();
            match line_number {
                // ambient
                3 => {
                    env.ambient = Vector3::new(parsef(lain[2]),parsef(lain[1]),parsef(lain[0]));
                }
                // baseColor
                4 => {
                    let baseplate = neo_prop.get_mut(0).unwrap();
                    baseplate.shader_vars.insert("Color".to_string(), ShadvType::Vec3(Vector3::new(parsef(lain[0]),parsef(lain[1]),parsef(lain[2]))));
                }
                // skyColor
                5 => {

                    env.skyColor = Vector3::new(parsef(lain[0]),parsef(lain[1]),parsef(lain[2]));

                    env.ambient = Vector3::new(parsef(lain[0]),parsef(lain[1]),parsef(lain[2]));

                }
                // baseSize
                6 => {
                    let size = parsef(lain[0]);
                    let baseplate = neo_prop.get_mut(0).unwrap();
                    baseplate.position = Vector3::new(size/2.0,0.0,size/2.0);
                    baseplate.scale = Vector3::new(size,0.1,size);
                }
                // sunIntensity
                7 => {
                    env.sun_intensity = parsef(lain[0]);
                }
                // weather... but where are gonna pretend this dosn't exist :3
                // 8 => {
                    
                // }
                _ => {}
            }
        } else if compatibility == 1 {
            let current_brick = neo_prop.get_mut(current_prop-1).unwrap();

            //this section if just for bh bricks
            if inside_obj {
                if dat.first().unwrap().chars().next().unwrap() == '+'{
                    match dat.as_slice() {
                        ["+NAME", namet] => {
                            current_brick.name = namet.to_string();
                        }
                        ["+ROT", rot] => {
                            current_brick.set_rotation(Vector3::new(0.0, parsef(rot), 0.0));
                        }
                        _ => {}
                    }
                } else {
                    inside_obj = false;
                    current_prop += 1;
                }
            }
        
            //getting brick parameters
            if !inside_obj {
                match dat.as_slice() {
                    [x,y,z, xs,ys,zs, r,g,b,a] => {
                        // for brk pos and scale yzx -> xyz

                        let bscale = Vector3::new(parsef(ys), parsef(zs), parsef(xs))/2.0;

                        let mut new_brick = Prop::new("what cat?".to_string());
                        new_brick.position = Vector3::new(parsef(y)+bscale.x/2.0, parsef(z)+bscale.y/2.0, parsef(x)+bscale.z/2.0);
                        new_brick.scale = bscale;
                        new_brick.shader_vars.insert("Color".to_string(), ShadvType::Vec3(Vector3::new(parsef(r),parsef(g),parsef(b))));
                        neo_prop.push(new_brick);
                        inside_obj = true;

                        //old code needing adapting
                        // if(arg.rot)object.rotateY((arg.rot-rot_offset.y)*3.14/180);
                        // else object.rotateY((90-180)*3.14/180);
                        //         object.position.x = arg.pos.y+arg.sca.y/2;
                        //         object.position.y = arg.pos.z+arg.sca.z/2;
                        //         object.position.z = (arg.pos.x+arg.sca.x/2)*-1;
                        //         if(offset!==0){
                        //             if(offset.x!=0)object.translateX(offset.x);
                        //             if(offset.y!=0)object.translateY(offset.y);
                        //             if(offset.z!=0)object.translateZ(offset.z);
                        //         }

                        // if(ignore_bh_rot == false){
                        //         if(arg.rot !== 180 || arg.rot !== 360){
                        //     if(ignore_sca.x!=1)object.scale.x = arg.sca.y;
                        //     if(ignore_sca.y!=1)object.scale.y = arg.sca.z;
                        //     if(ignore_sca.z!=1)object.scale.z = arg.sca.x;
                        //         }else{
                        //     if(translateX.x!=0)object.translateX(rot_offset.x-1);
                        //     if(ignore_sca.x!=1)object.scale.x = arg.sca.x;
                        //     if(ignore_sca.y!=1)object.scale.y = arg.sca.z;
                        //     if(ignore_sca.z!=1)object.scale.z = arg.sca.y;
                        //         }
                        // }else{
                        //     if(ignore_sca.x!=1)object.scale.x = arg.sca.x;
                        //     if(ignore_sca.z!=1)object.scale.z = arg.sca.y;
                        //     if(ignore_sca.y!=1)object.scale.y = arg.sca.z;
                        // }


                    }
                    _ => {}
                }
            }
        }

        // match dat.as_slice() {
        //     // vertices
        //     ["v", x, y, z] => {
        //         pos.push([parsef(x), parsef(y), parsef(z)]);
        //     }
        //     // vertex normals
        //     ["vn", x, y, z] => {
        //         nor.push([parsef(x), parsef(y), parsef(z)]);
        //     }
        //     // texture coordinates
        //     ["vt", x, y] => {
        //         tex.push([parsef(x), parsef(y)]);
        //     }
        //     // faces
        //     ["f", face_1, face_2, face_3] => {
        //         [face_1, face_2, face_3].iter().for_each(|face| {
        //             // this would be much better with itertools package
        //             let mut split = face.split('/');
        //             let (vertex, texture, normal) =
        //                 (split.next().unwrap(), split.next().unwrap(), split.next().unwrap());

        //             // subtract 1 because obj is 1 indexed
        //             vf.push(parse(vertex) - 1);
        //             tf.push(parse(texture) - 1);
        //             nf.push(parse(normal) - 1);
        //         });
        //     }
        //     _ => {}
        // }

        line_number += 1;
    }

    World {
        environment: env,
        props: neo_prop,
        lights: neo_light,
    }
}



// function loadBrk(map) {

//     let FILE = fs.readFileSync(map, "UTF-8");
//     let LINES = FILE.split("\n");
//     let totalLines = 0;
//     let bricks = [];
//     let cam = [];
//     let environment = {};
//     let currentBrick = -1;
//     let scriptWarning = false;

//     for (let line of LINES) {
//         totalLines++;
//         line = line.trim();
//         switch (totalLines) {
//             case 1: {
//                 if (line !== "B R I C K  W O R K S H O P  V0.2.0.0") {
//                     console.error("ERROR: This set was created using an incompatible version of Brick Hill.");
//                     return process.exit(0);
//                 }
//                 continue;
//             }
//             case 3: {
//                 const glColor = line.split(" ");
//                 const RGB = convertRGB(glColor[0], glColor[1], glColor[2]);
//                 environment["ambient"] = rgbToHex(RGB[2], RGB[1], RGB[0]);
//                 continue;
//             }
//             case 4: {
//                 const glColor = line.split(" ");
//                 const RGB = convertRGB(glColor[0], glColor[1], glColor[2]);
//                 environment["baseColor"] = new Vector3(RGB[0]/255, RGB[1]/255, RGB[2]/255);
//                 continue;
//             }
//             case 5: {
//                 const glColor = line.split(" ");
//                 const RGB = convertRGB(glColor[0], glColor[1], glColor[2]);
//                 environment["skyColor"] = rgbToHex(RGB[0], RGB[1], RGB[2]);
//                 continue;
//             }
//             case 6: {
//                 environment["baseSize"] = Number(line);
//                 continue;
//             }
//             case 7: {
//                 environment["sunIntensity"] = Number(line);
//                 continue;
//             }
//             case 8: {
//                 environment["weather"] = Number(line);
//                 continue;
//             }
//         }

//         const DATA = line.split(" ");
//         const ATTRIBUTE = DATA[0].replace("+", "");
//         const VALUE = DATA.slice(1).join(" ");
//         switch (ATTRIBUTE) {
//             case "NAME": {
//                 bricks[currentBrick].name = VALUE;
//                 continue;
//             }
//             case "ROT": {
//                 bricks[currentBrick].rotation = VALUE;
//                 continue;
//             }
//             case "SHAPE": {
//                 bricks[currentBrick].shape = VALUE;
//                 continue;
//             }
//             case "MODEL": {
//                 bricks[currentBrick].model = VALUE;
//                 continue;
//             }
//             case "NOCOLLISION": {
//                 bricks[currentBrick].collision = false;
//                 continue;
//             }
//             case "COLOR": {
//                 const colors = VALUE.split(" ");
//                 const color = convertRGB(colors[0], colors[1], colors[2]);
//                 let team = new Team(teams[teams.length - 1], rgbToHex(color[0], color[1], color[2]));
//                 teams[teams.length - 1] = team;
//                 continue;
//             }
//             case "LIGHT": {
//                 const colors = VALUE.split(' ');
//                 const lightRange = colors[3];
//                 const RGB = convertRGB(colors[0], colors[1], colors[2]);
//                 bricks[currentBrick].lightEnabled = true;
//                 bricks[currentBrick]._lightRange = lightRange;
//                 bricks[currentBrick]._lightColor = rgbToHex(RGB[0], RGB[1], RGB[2]);
//                 continue;
//             }
//             case "SCRIPT": {
//                 if (scriptWarning)
//                     continue;
//                 scriptWarning = true;
//                 console.warn("WARNING: This set contains scripts. Scripts are incompatible with node-hill so they will not be loaded.");
//                 continue;
//             }
//             case "CAMERA_POS": {
//                 cam.pos = VALUE.split(" ");
//                 continue;
//             }
//             case "CAMERA_ROT": {
//                 cam.rot = VALUE.split(" ");
//                 continue;
//             }
//         }
//         if (DATA.length === 10) {
//           const RGB = convertRGB(DATA[6], DATA[7], DATA[8])

//             let brick = {
//               "xPos": Number(DATA[0]),
//               "yPos": Number(DATA[1]),
//               "zPos": Number(DATA[2]),
//               "xScale": Number(DATA[3]),
//               "yScale": Number(DATA[4]),
//               "zScale": Number(DATA[5]),
//               "color": new Vector3(RGB[0]/255, RGB[1]/255, RGB[2]/255),
//               "alpha": Number(DATA[9])
//             }

//             bricks.push(brick);
//             currentBrick++;
//         }
//         // if (DATA[0] && DATA[0] === ">TEAM")
//         //     teams.push(VALUE);
//     }
//     let World = {"Environment":environment, "Bricks":bricks, "Camera":cam}
//     return World;
// }