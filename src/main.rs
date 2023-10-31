use cameras::Camera;
use char_control::{Character, character_type};
use config::keconfig;
use glium::{glutin::ContextBuilder, index::{NoIndices, PrimitiveType}, DepthTest};
use kbf::{load, Environment};
use ke_units::{Vec2, radians};
use lights::PointLight;
use models::Model;
use nalgebra::{Vector3, Vector2, Matrix3, Matrix4, Rotation, Rotation3, Unit};
use physic_props::*;
use props::{Prop, phytype, physhape, proptype};
use script::ScriptSpace;
use shaders::{ShadvType, Shader};
use smol::lock::RwLockReadGuard;
use textures::Texture;
use winit::{event_loop::{EventLoop, ControlFlow}, window::{WindowBuilder, CursorGrabMode}, event::{StartCause, Event, WindowEvent, DeviceEvent}, dpi::LogicalPosition};
use glium::{Depth, Display, DrawParameters, Program, Surface, VertexBuffer};
use glium::texture::SrgbTexture2d;

use std::{borrow::BorrowMut, time::{SystemTime, UNIX_EPOCH}, ops::Mul, f32::consts::PI, collections::VecDeque, fs, sync::RwLock, path};
use std::collections::HashMap;
use winit::platform::run_return::EventLoopExtRunReturn;

pub mod config;
pub mod fs_system;
pub mod cameras;
pub mod dynamic_uniform;
pub mod ke_units;
pub mod lights;
pub mod models;
pub mod props;
pub mod shaders;
pub mod textures;
pub mod kbf;
pub mod physic_props;
pub mod char_control;
pub mod script;


lazy_static::lazy_static! {
    static ref PROPS: RwLock<HashMap<i32, Prop>> = RwLock::new(HashMap::new());
}

fn main(){


    let keconf = keconfig::parse("./KE_config".to_string());


    // project settings
    // which this will allow for the engine executable can
    // keep diffrent examples in their own paths, but by default
    // the engine starts at .
    // or it's current path

    let _KE_MAIN_PATH = ".";
    let _KE_MAIN_DEPENDENTS = "./engine_dependent";




    // init gl and window
    let mut event_loop = EventLoop::new();
    let wb = WindowBuilder::new();
    let cb = ContextBuilder::new().with_depth_buffer(24);
    let display = Display::new(wb, cb, &event_loop).unwrap();

    let binding = &display.gl_window();
    let v = binding.window();
    v.set_cursor_grab(CursorGrabMode::Locked);
    v.set_cursor_visible(false);

    let params = DrawParameters {
        depth: Depth {
            test: DepthTest::IfLess,
            write: true,
            ..Default::default()
        },
        blend: glium::Blend {
            color: glium::BlendingFunction::Addition {
                source: glium::LinearBlendingFactor::SourceAlpha,
                destination: glium::LinearBlendingFactor::OneMinusSourceAlpha,
            },
            alpha: glium::BlendingFunction::Addition {
                source: glium::LinearBlendingFactor::One,
                destination: glium::LinearBlendingFactor::DestinationAlpha,
            },
            constant_value: (0.0, 0.0, 0.0, 0.0),
        },
        backface_culling: glium::draw_parameters::BackfaceCullingMode::CullClockwise,
        // backface_culling: glium::draw_parameters::BackfaceCullingMode::CullingDisabled,
        ..Default::default()
    };

    let mut _uniform = dynamic_uniform::DynamicUniforms::new();

    // this is where assets are stored :3
    // gonna call these lists
    let mut propz = PROPS.write().unwrap();
    let mut modelz: HashMap<i32, Model> = HashMap::new();
    let mut texturez: HashMap<i32, Texture> = HashMap::new();
    let mut shaderz: HashMap<i32, Shader> = HashMap::new();
    let mut shader_vars: HashMap<String, ShadvType> = HashMap::new();

    let mut trans_props: Vec<i32> = Vec::new();


    // this sets up engine default assets
    {
        let de_shader = _KE_MAIN_DEPENDENTS.to_owned()+"/shaders/base";
        shaderz.insert(0, Shader::craft(&de_shader, &display));

        let de_shader = _KE_MAIN_DEPENDENTS.to_owned()+"/shaders/brush";
        shaderz.insert(1, Shader::craft(&de_shader, &display));

        let de_shader = _KE_MAIN_DEPENDENTS.to_owned()+"/shaders/model";
        shaderz.insert(2, Shader::craft(&de_shader, &display));

        let pig_model = _KE_MAIN_DEPENDENTS.to_owned()+"/ellie_def/pig.obj";
        modelz.insert(0, models::load_obj(&pig_model, &display));

        let pig_skin = _KE_MAIN_DEPENDENTS.to_owned()+"/ellie_def/pig.png";
        texturez.insert(0, Texture::craft(&pig_skin, &display));

        let box_model = _KE_MAIN_DEPENDENTS.to_owned()+"/model/brush.obj";
        modelz.insert(1, models::load_obj(&box_model, &display));

        // propz.insert(0, Prop::new("nya".into()));

        // let a = propz.get_mut(&0).unwrap();
        // a.shader = 0;
        // a.model = 0;
        // a.texture1 = 0;
        // a.texture2 = 0;
    }

    //init physics system
    let mut phys_world = PhysWorld::init_phys_world();

    let (world_emv, lightz) = {

        let map = load(&("./maps/".to_owned()+&keconf.default_map));

        let txCount = (texturez.len() as i32)-1;

        for tx in map.textures {
            texturez.insert(txCount+tx.0, Texture::craft(&("./textures/".to_owned()+&tx.1), &display));
        }

        let mdCount = (modelz.len() as i32)-1;

        for md in map.models {
            if(md.1.ends_with(".obj")){
                modelz.insert(mdCount+md.0, models::load_obj(&("./models/".to_owned()+&md.1), &display));
            }else if(md.1.ends_with(".fbx")){
                modelz.insert(mdCount+md.0, models::load_fbx(&("./models/".to_owned()+&md.1), &display));
            }else if(md.1.ends_with(".gltf")){
                modelz.insert(mdCount+md.0, models::load_gltf(&("./models/".to_owned()+&md.1), &display));
            }
            
        }

        let shCount = (shaderz.len() as i32)-1;

        for sh in map.shaders {
            let de_shader = "./shaders/".to_owned()+&sh.1;
            shaderz.insert(shCount+sh.0, Shader::craft(&de_shader, &display));
        }


        let mut partnp = propz.len() as i32;

        for np in map.props {
            let mut np = np;
            phys_world.create_phy(&mut np, &modelz);

            let mut current_new_texture = 0 as usize;
            let textsize = np.textures.len()-1;
            
            while current_new_texture < textsize {
                np.textures[current_new_texture] = np.textures[current_new_texture]+txCount;
                current_new_texture+=1;
            }

            if(np.proptype == proptype::Model_static){
                np.model = np.model+mdCount;
            }

            if(np.shader_non_defalt){
                np.shader = np.shader+shCount;
            }

            if(np.transparency != 1.0){
                trans_props.push(partnp);
            }

            propz.insert(partnp, np);
            partnp+=1;
        }

        (map.environment, map.lights)
    };

    // {
    //     let mut womp = Prop::new("nya :3".to_owned());
    //     womp.model = 0;
    //     womp.textures = vec![0,0];
    //     womp.phys_shape = physhape::Box;
    //     womp.phys_type = phytype::Dynamic;
    //     womp.position = Vector3::new(0.0, 16.0, 0.0);
    //     phys_world.create_phy(&mut womp, &modelz);
        
    //     propz.insert((propz.len() as i32), womp);
    // }

    // {
    //     let mut womp = Prop::new("nya :3".to_owned());
    //     womp.model = 1;
    //     womp.textures = vec![0,0];
    //     womp.position = Vector3::new(-2.0, 2.0, -30.0);
    //     womp.set_rotation(Vector3::new(radians(45.0),radians(0.0),radians(0.0)));
        
    //     propz.insert((propz.len() as i32), womp);
    // }

    // eventually make this an array of lights inside a hashmap list
    let light = [1.4, 0.4, 0.7f32];

    // main cam is just id of the camera map
    let mut _main_camera = 0;
    let mut camera_map = HashMap::new();

    // this gets the window's size and creates the camera to reflect it
    let (width, height) = display.get_framebuffer_dimensions();
    let mut main_cam = Camera::craft(Vec2 {
        x: width as f32,
        y: height as f32,
    });

    //main_cam.set_rotation(Vector3::new(-0.5, 0.0, 0.3));
    main_cam.position = Vector3::new(0.0, -10.0, -90.0);

    main_cam.refresh();
    camera_map.insert(0, main_cam);

    let mut real_char = Character::new(keconf.char_pov, &display, &mut propz, &modelz, &mut phys_world, &mut camera_map);
    _main_camera = real_char.camera;

    
        
    if world_emv.spawnpoints.len() != 0 {
        real_char.tp(&mut phys_world, &mut propz, world_emv.spawnpoints[0]);
    }

    // thread scheduler :3
    // now i'm not sure how other engines do it
    // nor how this is going to work without lag
    // but not gonna do this rn
    // too lazy
    //
    //
    // First thread:
    // handles when other threads fire
    // next handles changes to props that other threads request
    // last handles rendering
    //
    //
    // Second thread:
    // This thread handles physics and entity interactions
    // Then handles Scripting
    //
    //
    // Third thread:
    // Loads map files
    // Gets assets
    // probably works with networking

    let mut loop_wawa: f32 = 0.0;

    let (width, height) = display.get_framebuffer_dimensions();

    let mut screen_size = Vector2::new(width as f32, height as f32);

    let mut current_timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_millis() as f64;

    let mut last_timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_millis() as f64;

    drop(propz);

    let mut js_world = ScriptSpace::new();
    js_world.pinpropz();
    js_world.add_script("./scripts/".to_owned()+&"testing.js".to_string());
    js_world.run();

    event_loop.borrow_mut().run_return(|event, _, control_flow| {
        let delta_time = ((current_timestamp - last_timestamp)*0.1) as f32;
        //dbg!(delta_time);
        last_timestamp = current_timestamp;
        js_world.job_runs(delta_time);

        *control_flow = ControlFlow::Poll;

        current_timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_millis() as f64;

        

        // womp this is where events from the window thingamabob does things
        // pretty much we are gonna quoue mouse and keyboards
        let mut propz = PROPS.try_write().unwrap();
        match event {
            Event::WindowEvent { event, .. } => match event {
                WindowEvent::Resized(_) => {
                    let main_cam = camera_map.get_mut(&_main_camera).unwrap();
                    //update main_cam's projection with screen
                    let (width, height) = display.get_framebuffer_dimensions();
                    screen_size = Vector2::new(width as f32, height as f32);
                    main_cam.reproject(screen_size);
                }
                WindowEvent::KeyboardInput { device_id, input, is_synthetic } => {
                    real_char.interp_key(&mut phys_world, &mut propz, input, delta_time);
                }
                WindowEvent::CloseRequested { .. } => {
                    *control_flow = ControlFlow::Exit;
                }
                _ => {}
            },
            Event::NewEvents(cause) => match cause {
                StartCause::ResumeTimeReached { .. } => (),
                StartCause::Init => (),
                _ => {}
            },
            Event::DeviceEvent { device_id, event } => {
                match event {
                    DeviceEvent::MouseMotion { delta } => {
                        let (width, height) = display.get_framebuffer_dimensions();
                        let a = Vector2::new(delta.0 as f32, delta.1 as f32);
                        real_char.interp_mouse(&mut phys_world, &mut propz, &mut camera_map, a, screen_size, delta_time);
                        v.set_cursor_position(LogicalPosition::new(width/2, height/2));
                    }
                    _ => {}
                }
            }
            _ => (),
        }

        #[cfg(debug_assertions)]
        {
            for (index, mut sh) in shaderz.iter_mut() {
                let name = sh.url.clone();
                let metadataF = fs::metadata(format!("{name}.frag")).expect("failed to check shader file");
                let metadataV = fs::metadata(format!("{name}.vert")).expect("failed to check shader file");

                if(metadataF.modified().unwrap() != sh.time_changed_f || metadataV.modified().unwrap() != sh.time_changed_v) {
                    sh.time_changed_f = metadataF.modified().unwrap() ;
                    sh.time_changed_v = metadataV.modified().unwrap() ;
                    // sh = &mut Shader::craft(&name, &display);
                    // shaderz.insert(0, Shader::craft(&name, &display));
                    println!("{} has been updated", name);
                }
            }
        }

        real_char.step(&mut phys_world, &mut propz, delta_time);

        //step physics world
        phys_world.step();

        //if cam prop parent moved, we move cam
        let main_cam = camera_map.get_mut(&_main_camera).unwrap();
        if main_cam.parent_prop != -1 {
            main_cam.position = propz.get(&main_cam.parent_prop).unwrap().position+main_cam.parent_offset;
            //main_cam.look_at(Vector3::new(0.0, 0.0, 0.0)); funny testing
            main_cam.refresh();
        }
        

        //
        // this loop is pretty much the main render pipeline
        // from this line and down
        //

        // start drawing frame
        let mut target = display.draw();
        target.clear_color_and_depth((world_emv.skyColor.x, world_emv.skyColor.y, world_emv.skyColor.z, 1.0), 1.0);

        // let mut to_remove: Vec<i32> = Vec::new();

        loop_wawa += 1.0;


        // now in theory one could get all the closest props and push refrences of those props in to a list
        // then replace propz here to that list to implement some sorta calling
        // i'ma do that later
        for po in propz.iter_mut() {
            let prop = po.1;
            
            //sync physics prop to visual prop
            phys_world._sync_prop(prop, CopyWhat::All);

            if !prop.render {continue;}
            if prop.face_cam {
                if(main_cam.position.z<prop.position.z){
                    prop.look_at(main_cam.position-prop.position);
                }else{
                    prop.look_at(prop.position-main_cam.position);
                }
            }
            if prop.transparency != 1.0 {continue;}

            render_prop(loop_wawa, prop, main_cam, &shader_vars, &world_emv, &lightz, &texturez, &mut target, &modelz, &shaderz, &params);

            // if prop.position.y < -2.0 {
            //     to_remove.push(*po.0);
            // }
        };

        //now later maybe trans props could be fed a screen buffer :3
        for po in &trans_props {
            let prop = propz.get_mut(&po).unwrap();
            if prop.transparency == 0.0 {continue;}

            if !prop.render {continue;}

            render_prop(loop_wawa, prop, main_cam, &shader_vars, &world_emv, &lightz, &texturez, &mut target, &modelz, &shaderz, &params);
        };

        // finish frame and put on window probably
        target.finish().unwrap();

        // for amongus in to_remove {
        //     dbg!(amongus);
        //     let prop = propz.get_mut(&amongus).unwrap();
        //     phys_world.remove_phy(prop);
        //     propz.remove(&amongus);
        // }
    });
}

fn render_prop(loop_wawa: f32, prop: &mut Prop, main_cam: &mut Camera, shader_vars: &HashMap<String, ShadvType>, world_emv: &Environment, lightz: &Vec<PointLight>, texturez: &HashMap<i32, Texture>, target: &mut glium::Frame, modelz: &HashMap<i32, Model<'_>>, shaderz: &HashMap<i32, Shader>, params: &DrawParameters<'_>) {
    //this is where all the shader values get pushed so we can send them to gpu
    let mut uniform = dynamic_uniform::DynamicUniforms::new();

    //this does all the prop's 3d model translation stuff

    let n = [
        Unit::new_unchecked(Vector3::new(-1.0, 0.0, 0.0)),
        Unit::new_unchecked(Vector3::new(0.0, -1.0, 0.0)),
        Unit::new_unchecked(Vector3::new(0.0, 0.0, 1.0)),
    ];

    let r1 = Rotation3::from_axis_angle(&n[2], prop.rotation.z);
    let r2 = Rotation3::from_axis_angle(&n[1], prop.rotation.y);
    let r3 = Rotation3::from_axis_angle(&n[0], prop.rotation.x);

    let d = r2 * r3 * r1;

    let model: nalgebra::Matrix<f32, nalgebra::Const<4>, nalgebra::Const<4>, nalgebra::ArrayStorage<f32, 4, 4>> = d.to_homogeneous();


    // let model = prop.rotation.matrix().to_homogeneous().mul(model);

    // let model = prop.rotation_cached.mul(model);

    let model = model.prepend_nonuniform_scaling(&prop.scale);
    let model = model.append_translation(&prop.position);
    let binding = *model.as_ref();
    uniform.add("model".to_string(), &binding);

    //camera translations
    let view = main_cam.view_drop();
    uniform.add("view".to_string(), &view);
    let perspective = main_cam.project_drop();
    uniform.add("perspective".to_string(), &perspective);

    // shader globle vars
    for (name, value) in shader_vars {
        let name = name.to_string();
        match value {
            ShadvType::Bool(value) => {
                uniform.add(name, value);
            }
            ShadvType::Integer(value) => {
                uniform.add(name, value);
            }
            ShadvType::Float(value) => {
                uniform.add(name, value);
            }
            ShadvType::Vec2(value) => {
                uniform.add(name, value.as_ref());
            }
            ShadvType::Vec3(value) => {
                uniform.add(name, value.as_ref());
            }
        }
    }

    // prop spesific shader vars pushed in to global
    for (name, value) in &prop.shader_vars {
        let name = name.to_string();
        match value {
            ShadvType::Bool(value) => {
                uniform.add(name, value);
            }
            ShadvType::Integer(value) => {
                uniform.add(name, value);
            }
            ShadvType::Float(value) => {
                uniform.add(name, value);
            }
            ShadvType::Vec2(value) => {
                uniform.add(name, value.as_ref());
            }
            ShadvType::Vec3(value) => {
                uniform.add(name, value.as_ref());
            }
        }
    }

    // let mut light = PointLight::new();
    // let index = "0".to_owned();
    // light.position = Vector3::new(1.4, 0.4, 0.7f32);

    uniform.add("LeDirLight.direction".to_owned(), &[-0.2, -1.0, -0.3]);
            
    uniform.add("LeDirLight.ambient".to_owned(), &[1.0, 1.0, 1.0]);
    uniform.add("LeDirLight.diffuse".to_owned(), &*world_emv.skyColor.as_ref());
    uniform.add("LeDirLight.specular".to_owned(), &[0.5, 0.5, 0.5]);

    let mut indexx = 0;
    for light in lightz {
        let index = indexx.to_string();
        uniform.add("pointLights[".to_owned()+&index+"].position", &*light.position.as_ref());
        uniform.add("pointLights[".to_owned()+&index+"].ambient", &*light.ambient.as_ref());
        uniform.add("pointLights[".to_owned()+&index+"].diffuse", &*light.diffuse.as_ref());
        uniform.add("pointLights[".to_owned()+&index+"].specular", &*light.specular.as_ref());
        uniform.add("pointLights[".to_owned()+&index+"].constant", &light.constant);
        uniform.add("pointLights[".to_owned()+&index+"].linear", &light.linear);
        uniform.add("pointLights[".to_owned()+&index+"].quadratic", &light.quadratic);
        indexx+=1;
    }

    uniform.add("NR_POINT_LIGHTS".to_owned(), &indexx);

    uniform.add("trans".to_string(), &prop.transparency);

    uniform.add("diffuse_tex".to_string(), get_texture(texturez, prop, 0));
    uniform.add("normal_tex".to_string(), get_texture(texturez, prop, 1));

    uniform.add("material.diffuse".to_owned(), get_texture(texturez, prop, 0));
    uniform.add("material.specular".to_owned(), get_texture(texturez, prop, 1));
    uniform.add("material.shininess".to_owned(), &0.1);

    uniform.add("time".to_owned(), &loop_wawa);

    let mut currrent_text = 0;
    for text in &prop.textures {
        uniform.add("texture".to_string()+&(currrent_text+1).to_string(), get_texture(texturez, prop, currrent_text));
        currrent_text+=1;
    }

    // here we draw the prop on the frame
    target
        .draw(
            get_model(modelz, prop),
            NoIndices(PrimitiveType::TrianglesList),
            get_shader(shaderz, prop),
            &uniform,
            params,
        )
        .unwrap();
}

fn get_shader<'a>(shadersz: &'a HashMap<i32, Shader>, prop: &Prop) -> &'a Program {
    match shadersz.get(&prop.shader) {
        Some(shader) => &shader.program,
        None => &shadersz.get(&0).unwrap().program,
    }
}

fn get_model<'a>(modelz: &'a HashMap<i32, Model>, prop: &Prop) -> &'a VertexBuffer<models::Vertex> {
    match modelz.get(&prop.model) {
        Some(model) => &model.verts,
        None => &modelz.get(&0).unwrap().verts,
    }
}

fn get_texture<'a>(texturez: &'a HashMap<i32, Texture>, prop: &Prop, texture: usize) -> &'a SrgbTexture2d {
    match texturez.get(&prop.textures[texture]) {
        Some(texture) => &texture.texture,
        None => &texturez.get(&0).unwrap().texture,
    }
}