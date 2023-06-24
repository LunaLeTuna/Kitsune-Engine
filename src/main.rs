use cameras::Camera;
use char_control::{Character, character_type};
use glium::{glutin::ContextBuilder, index::{NoIndices, PrimitiveType}, DepthTest};
use kbf::load;
use ke_units::Vec2;
use models::Model;
use nalgebra::{Vector3, Vector2};
use physic_props::*;
use props::{Prop, phytype, physhape};
use shaders::{ShadvType, Shader};
use textures::Texture;
use winit::{event_loop::{EventLoop, ControlFlow}, window::{WindowBuilder, CursorGrabMode}, event::{StartCause, Event, WindowEvent, DeviceEvent}, dpi::LogicalPosition};
use glium::{Depth, Display, DrawParameters, Program, Surface, VertexBuffer};
use glium::texture::SrgbTexture2d;

use std::{borrow::BorrowMut, time::{SystemTime, UNIX_EPOCH}};
use std::collections::HashMap;
use winit::platform::run_return::EventLoopExtRunReturn;

pub mod fs_system;
pub mod cameras;
pub mod dynamic_uniform;
pub mod ke_units;
pub mod models;
pub mod props;
pub mod shaders;
pub mod textures;
pub mod kbf;
pub mod physic_props;
mod char_control;


lazy_static::lazy_static! {
}


fn main(){

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
        ..Default::default()
    };

    let mut _uniform = dynamic_uniform::DynamicUniforms::new();



    // this is where assets are stored :3
    // gonna call these lists
    let mut propz: HashMap<i32, Prop> = HashMap::new();
    let mut modelz: HashMap<i32, Model> = HashMap::new();
    let mut texturez: HashMap<i32, Texture> = HashMap::new();
    let mut shaderz: HashMap<i32, Shader> = HashMap::new();
    let mut shader_vars: HashMap<String, ShadvType> = HashMap::new();



    // this sets up engine default assets
    {
        let de_shader = _KE_MAIN_DEPENDENTS.to_owned()+"/shaders/base";
        shaderz.insert(0, Shader::craft(&de_shader, &display));

        let pig_model = _KE_MAIN_DEPENDENTS.to_owned()+"/ellie_def/pig.obj";
        modelz.insert(0, models::load_obj(&pig_model, &display));

        let pig_skin = _KE_MAIN_DEPENDENTS.to_owned()+"/ellie_def/pig.png";
        texturez.insert(0, Texture::craft(&pig_skin, &display));

        let box_model = _KE_MAIN_DEPENDENTS.to_owned()+"/box.obj";
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

    {
        let map = load("./maps/ke_test.kbf");
        let mut partnp = 0;
        for np in map {
            let mut np = np;
            phys_world.create_phy(&mut np);
            propz.insert(partnp, np);
            partnp+=1;
        }
    }

    {
        let mut womp = Prop::new("nya :3".to_owned());
        womp.model = 0;
        womp.texture1 = 0;
        womp.texture2 = 0;
        womp.phys_shape = physhape::Box;
        womp.phys_type = phytype::Dynamic;
        womp.position = Vector3::new(0.0, 16.0, 0.0);
        phys_world.create_phy(&mut womp);
        
        propz.insert((propz.len() as i32), womp);
    }

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

    let mut real_char = Character::new(character_type::Third, &display, &mut propz, &mut phys_world, &mut camera_map);
    _main_camera = real_char.camera;

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

    event_loop.borrow_mut().run_return(|event, _, control_flow| {
        *control_flow = ControlFlow::Poll;

        current_timestamp = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_millis() as f64;

        let mut delta_time = ((current_timestamp - last_timestamp)*0.1) as f32;
        //dbg!(delta_time);
        last_timestamp = current_timestamp;

        // womp this is where events from the window thingamabob does things
        // pretty much we are gonna quoue mouse and keyboards
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
                        // v.set_cursor_position(LogicalPosition::new((width/2), (height/2)));
                    }
                    _ => {}
                }
            }
            _ => (),
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
        target.clear_color_and_depth((0.2, 0.3, 0.3, 1.0), 1.0);



        // if (loop_wawa%20.0) == 0.0 {
        //     let mut womp = Prop::new("nya :3".to_owned());
        //     womp.model = 0;
        //     womp.texture1 = 0;
        //     womp.texture2 = 0;
        //     womp.phys_shape = physhape::Box;
        //     womp.phys_type = phytype::Dynamic;
        //     womp.position = Vector3::new(0.0, 16.0, 0.0);
        //     womp.shader_vars.insert("Color".to_string(), ShadvType::Vec3(Vector3::new(0.0,0.0,0.0)));
        //     phys_world.create_phy(&mut womp);
            
        //     propz.insert((propz.len() as i32), womp);
        // }
        // loop_wawa+=1.0;

        // let mut to_remove: Vec<i32> = Vec::new();


        // now in theory one could get all the closest props and push refrences of those props in to a list
        // then replace propz here to that list to implement some sorta calling
        // i'ma do that later
        for po in &mut propz {
            let prop = po.1;
            
            //sync physics prop to visual prop
            phys_world._sync_prop(prop, CopyWhat::All);

            if !prop.render {continue;}

            //this is where all the shader values get pushed so we can send them to gpu
            let mut uniform = dynamic_uniform::DynamicUniforms::new();

            //this does all the prop's 3d model translation stuff
            let model = &mut prop.rotation.matrix().to_homogeneous();
            model.append_nonuniform_scaling_mut(&prop.scale);
            let model = model.append_translation(&prop.position);
            let binding = *model.as_ref();
            uniform.add("model", &binding);

            //camera translations
            let view = main_cam.view_drop();
            uniform.add("view", &view);
            let perspective = main_cam.project_drop();
            uniform.add("perspective", &perspective);

            uniform.add("u_light", &light);
            uniform.add("diffuse_tex", &*get_texture(&texturez, prop)); // TODO: need to make this not &*, because thats bad probably
            uniform.add("normal_tex", &*get_texture2(&texturez, prop));

            // shader globle vars
            for (name, value) in &shader_vars {
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

            // here we draw the prop on the frame
            target
                .draw(
                    get_model(&modelz, prop),
                    NoIndices(PrimitiveType::TrianglesList),
                    get_shader(&shaderz, prop),
                    &uniform,
                    &params,
                )
                .unwrap();

            // if prop.position.y < -2.0 {
            //     to_remove.push(*po.0);
            // }
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

fn get_texture<'a>(texturez: &'a HashMap<i32, Texture>, prop: &Prop) -> &'a SrgbTexture2d {
    match texturez.get(&prop.texture1) {
        Some(texture) => &texture.texture,
        None => &texturez.get(&0).unwrap().texture,
    }
}

fn get_texture2<'a>(texturez: &'a HashMap<i32, Texture>, prop: &Prop) -> &'a SrgbTexture2d {
    match texturez.get(&prop.texture2) {
        Some(texture) => &texture.texture,
        None => &texturez.get(&0).unwrap().texture,
    }
}
