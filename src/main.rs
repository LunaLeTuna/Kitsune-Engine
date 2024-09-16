use boa_engine::{builtins::{json, string}, value};
use buffer::Buffer;
use cameras::Camera;
use char_control::{Character, character_type};
use config::keconfig;
use keyboard_keynames::key_layout::KeyLayout;
use menu_system::{menuimage, menutext, KEchar, KEfont, KEmenuTypes};
use glium::{glutin::{ContextBuilder}, index::{NoIndices, PrimitiveType}, DepthTest, framebuffer::SimpleFrameBuffer, texture::{DepthStencilTexture2d, DepthTexture2d}, implement_vertex};
use kbf::{load, Environment};
use ke_units::{Vec2, radians};
use lights::PointLight;
use models::{Model, Vertex, Vertex2D};
use multiplayer::Sockreq;
use nalgebra::{Matrix3, Matrix4, Rotation, Rotation3, Unit, Vector2, Vector3, Vector4};
use physic_props::*;
use props::{Prop, phytype, physhape, proptype};
use rapier3d::crossbeam::channel::{Receiver, Sender};
use rust_socketio::{ClientBuilder, Payload, RawClient};
use socketioxide::SocketIo;
use script::ScriptSpace;
use serde_json::{json, Value};
use shaders::{ShadvType, Shader};
use smol::{lock::{RwLockReadGuard, futures}, future::FutureExt};
use textures::Texture;
use winit::{dpi::LogicalPosition, event::{DeviceEvent, Event, StartCause, WindowEvent}, event_loop::{ControlFlow, EventLoop}, window::{CursorGrabMode, Fullscreen, WindowBuilder}};
use glium::{Depth, Display, DrawParameters, Program, Surface, VertexBuffer};
use glium::texture::SrgbTexture2d;
use axum::{routing::get, http::Request};
use axum::Server;
use steamworks::AppId;
use steamworks::Client;
use steamworks::FriendFlags;
use steamworks::PersonaStateChange;

use std::{borrow::BorrowMut, collections::VecDeque, f32::consts::PI, fmt::Debug, fs, hash::Hash, ops::Mul, path, sync::{mpsc::channel, Arc, Mutex, RwLock}, thread::sleep, time::{Duration, SystemTime, UNIX_EPOCH}};
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
pub mod menu_system;
pub mod shaders;
pub mod textures;
pub mod buffer;
pub mod kbf;
pub mod vmf;
pub mod physic_props;
pub mod char_control;
pub mod script;
pub mod multiplayer;

pub enum KERequest {
    Create_Shader(i32, String),
    Create_Model(i32, String),
    Create_Model_From_Magic(i32,Vec<Vertex>),
    Create_Texture(i32, String, u32, u32),
    Pin_Texture_Buffer(i32,i32),
    Delete_Prop(i32),
    Create_Buffer(String),
    Create_Camera(String),
    Pin_Buffer_Camera(i32,i32),
    Phys_Prop_Push(i32,Vector3<f32>),
    Phys_Prop_Push_SideOnly(i32,Vector2<f32>),
    load_map(String),
    js_push(String, String),
    copy_prop_phys_pose(i32),
    window_cursor_lock(bool),
    exit,
    NULL,
}

lazy_static::lazy_static! {
    // the Vec<i32> is refrences to the prop hashmap, maybe at some point move worlds in to PROPS with HASHMAP<i32,HASHMAP<i32,Prop>>... maybe thats stupid :P
    static ref WORLDS: RwLock<HashMap<i32, (Environment,Vec<i32>)>> = RwLock::new(HashMap::new());
    static ref PROPS: RwLock<HashMap<i32, Prop>> = RwLock::new(HashMap::new());
    static ref FONTS: RwLock<HashMap<i32, KEfont>> = RwLock::new(HashMap::new());
    static ref MENUS: RwLock<HashMap<i32, KEmenuTypes>> = RwLock::new(HashMap::new());
    static ref CAMERAS: RwLock<HashMap<i32, Camera>> = RwLock::new(HashMap::new());
    static ref MAIN_CAM: RwLock<i32> = RwLock::new(0);
    static ref LIGHTS: RwLock<Vec<PointLight>> = RwLock::new(Vec::new());
    static ref SCREEN_SIZE: RwLock<Vec2> = RwLock::new(Vec2{x:250.0,y:250.0});
    //count is for scripts, so script item creation knows what id to give a new item
    pub static ref SHADER_COUNT: RwLock<i32> = RwLock::new(0);
    pub static ref MENU_COUNT: RwLock<i32> = RwLock::new(0);
    pub static ref MODEL_COUNT: RwLock<i32> = RwLock::new(0);
    pub static ref TEXTURE_COUNT: RwLock<i32> = RwLock::new(0);
    pub static ref CAMERA_COUNT: RwLock<i32> = RwLock::new(0);
    pub static ref BUFFER_COUNT: RwLock<i32> = RwLock::new(0);
    pub static ref PW: RwLock<PhysWorld> = RwLock::new(PhysWorld::init_phys_world());
    pub static ref REQUESTS: RwLock<Vec<KERequest>> = RwLock::new(Vec::new());
    pub static ref FIRST: Arc<RwLock<String>> = Arc::new(RwLock::new(format!("")));
}


fn main(){


    let (client, single) = Client::init().unwrap();

    let _cb = client.register_callback(|p: PersonaStateChange| {
        println!("Got callback: {:?}", p);
    });

    let utils = client.utils();
    println!("Utils:");
    println!("AppId: {:?}", utils.app_id());
    println!("UI Language: {}", utils.ui_language());

    let apps = client.apps();
    println!("Apps");
    println!("IsInstalled(480): {}", apps.is_app_installed(AppId(3202780)));
    println!("InstallDir(480): {}", apps.app_install_dir(AppId(3202780)));
    println!("BuildId: {}", apps.app_build_id());
    println!("AppOwner: {:?}", apps.app_owner());
    println!("Langs: {:?}", apps.available_game_languages());
    println!("Lang: {}", apps.current_game_language());
    println!("Beta: {:?}", apps.current_beta_name());

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

    if(!keconf.headless){
        v.set_cursor_grab(CursorGrabMode::Locked);
        v.set_cursor_visible(false);
    }

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
        //backface_culling: glium::draw_parameters::BackfaceCullingMode::CullingDisabled,
        ..Default::default()
    };

    let screenparams = DrawParameters {
        depth: Depth {
            test: DepthTest::IfLess,
            write: false,
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
        //backface_culling: glium::draw_parameters::BackfaceCullingMode::CullClockwise,
        backface_culling: glium::draw_parameters::BackfaceCullingMode::CullingDisabled,
        ..Default::default()
    };

    let mut _uniform = dynamic_uniform::DynamicUniforms::new();

    // this is where assets are stored :3
    // gonna call these lists
    let mut propz = PROPS.write().unwrap();
    let mut modelz: HashMap<i32, Model> = HashMap::new();
    let mut texturez: HashMap<i32, Texture> = HashMap::new();
    let mut texturez2: Vec<SrgbTexture2d> = Vec::new();
    let mut shaderz: HashMap<i32, Shader> = HashMap::new();
    let mut shader_vars: HashMap<String, ShadvType> = HashMap::new();
    let mut bufferz: HashMap<i32, SimpleFrameBuffer> = HashMap::new();
    bufferz.reserve(4);


    let thekeylayout = KeyLayout::new().unwrap();


    // this sets up engine default assets
    {
        if keconf.override_shader.len()==0 {
            let de_shader = _KE_MAIN_DEPENDENTS.to_owned()+"/shaders/base";
            shaderz.insert(0, Shader::craft(&de_shader, &display));

            let de_shader = _KE_MAIN_DEPENDENTS.to_owned()+"/shaders/brush";
            shaderz.insert(1, Shader::craft(&de_shader, &display));

            let de_shader = _KE_MAIN_DEPENDENTS.to_owned()+"/shaders/model";
            shaderz.insert(2, Shader::craft(&de_shader, &display));

            let de_shader = _KE_MAIN_DEPENDENTS.to_owned()+"/shaders/screen/direct";
            shaderz.insert(3, Shader::craft(&de_shader, &display));

            let de_shader = _KE_MAIN_DEPENDENTS.to_owned()+"/shaders/screen/text";
            shaderz.insert(4, Shader::craft(&de_shader, &display));

            let de_shader = _KE_MAIN_DEPENDENTS.to_owned()+"/missing";
            shaderz.insert(5, Shader::craft(&de_shader, &display));

            *SHADER_COUNT.write().expect("RwLock poisoned") += 6;
        }else{
            for oss in keconf.override_shader {
                let mut shader_zcount = SHADER_COUNT.write().expect("RwLock poisoned");
                let de_shader = oss;
                shaderz.insert(*shader_zcount, Shader::craft(&de_shader, &display));

                *shader_zcount += 1;
            }
        }

        let pig_model = _KE_MAIN_DEPENDENTS.to_owned()+"/ellie_def/pig.obj";
        modelz.insert(0, models::load_obj(&pig_model, &display));

        let box_model = _KE_MAIN_DEPENDENTS.to_owned()+"/model/brush.obj";
        modelz.insert(1, models::load_obj(&box_model, &display));

        let missing_model = _KE_MAIN_DEPENDENTS.to_owned()+"/missing.obj";
        modelz.insert(2, models::load_obj(&missing_model, &display));

        *MODEL_COUNT.write().expect("RwLock poisoned") += 3;

        
        let pig_skin = _KE_MAIN_DEPENDENTS.to_owned()+"/ellie_def/pig.png";
        texturez.insert(0, Texture::craft(&pig_skin, &display));

        let missing_texture = _KE_MAIN_DEPENDENTS.to_owned()+"/missing.png";
        texturez.insert(1, Texture::craft(&missing_texture, &display));

        *TEXTURE_COUNT.write().expect("RwLock poisoned") += 2;

        // propz.insert(0, Prop::new("nya".into()));

        // let a = propz.get_mut(&0).unwrap();
        // a.shader = 0;
        // a.model = 0;
        // a.texture1 = 0;
        // a.texture2 = 0;
    }

    //init physics system
    let mut phys_world = PW.write().unwrap();

    let mut loadmap = |map:String, propz: &mut std::sync::RwLockWriteGuard<HashMap<i32, Prop>>, texturez: &mut HashMap<i32, Texture>, modelz: &mut HashMap<i32, Model>, phys_world: &mut std::sync::RwLockWriteGuard<PhysWorld>| {

        let map = load(&("./maps/".to_owned()+&map));

        
        let txCount = (texturez.len() as i32)-1;

        for tx in map.textures {
            texturez.insert(txCount+tx.0, Texture::craft(&("./textures/".to_owned()+&tx.1), &display));
            *TEXTURE_COUNT.write().expect("RwLock poisoned") += 1;
        }

        let mdCount = (modelz.len() as i32)-1;

        for md in map.models {
            if(md.1.ends_with(".obj")){
                modelz.insert(mdCount+md.0, models::load_obj(&("./models/".to_owned()+&md.1), &display));
            }else if(md.1.ends_with(".fbx")){
                modelz.insert(mdCount+md.0, models::load_fbx(&("./models/".to_owned()+&md.1), &display));
            }else if(md.1.ends_with(".gltf") || md.1.ends_with(".glb")){
                modelz.insert(mdCount+md.0, models::load_gltf(&("./models/".to_owned()+&md.1), &display));
            }
            *MODEL_COUNT.write().expect("RwLock poisoned") += 1;
        }

        let shCount = (shaderz.len() as i32)-1;

        for sh in map.shaders {
            let de_shader = "./shaders/".to_owned()+&sh.1;
            shaderz.insert(shCount+sh.0, Shader::craft(&de_shader, &display));
            
            *SHADER_COUNT.write().expect("RwLock poisoned") += 1;
        }

        let mut partnp = propz.len() as i32;

        for np in map.props {
            let mut np = np;
            phys_world.create_phy(&mut np, modelz);

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

            // if(np.transparency != 1.0){
            //     trans_props.push(partnp);
            // }

            propz.insert(partnp, np);
            partnp+=1;
        }

        let mut lightz = LIGHTS.write().unwrap();
        let mut partnp = lightz.len();

        for nl in map.lights {
            lightz.insert(partnp, nl);
            partnp+=1;
        }

        (map.environment, map.scripts)
    };

    let (world_emv, scripz) = loadmap(keconf.default_map, &mut propz, &mut texturez, &mut modelz, &mut phys_world);

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


    let mut _main_buffer = 0;
    

    // this gets the window's size and creates the camera to reflect it
    let (width, height) = display.get_framebuffer_dimensions();
    let mut main_cam = Camera::craft(Vec2 {
        x: width as f32,
        y: height as f32,
    });

    //main_cam.set_rotation(Vector3::new(-0.5, 0.0, 0.3));
    main_cam.position = Vector3::new(0.0, -10.0, -90.0);

    let mut _main_camerac = MAIN_CAM.write().unwrap();
    let mut camera_mapc = CAMERAS.write().unwrap();
    main_cam.refresh();
    camera_mapc.insert(0, main_cam);

    let mut real_char: Character;

    real_char = Character::new(keconf.char_pov, &display, &mut propz, &mut modelz, &mut phys_world, &mut camera_mapc);
    *_main_camerac = real_char.camera;

    drop(camera_mapc);
    drop(_main_camerac);

    if world_emv.spawnpoints.len() != 0 {
        real_char.tp(&mut phys_world, &mut propz, world_emv.spawnpoints[0]);
    }

    let mut loop_wawa: f32 = 0.0;

    let mut screen_size = Vector2::new(width as f32, height as f32);
    let mut a = SCREEN_SIZE.write().unwrap();
    *a = Vec2 {
        x: width as f32,
        y: height as f32,
    };
    drop(a);

    drop(propz);

    let mut js_world = ScriptSpace::new();
    js_world.pinpropz(); //adds functions to js to minipulate props

    keconf.run_script.iter().for_each(|scrpt| {
        js_world.add_script("./scripts/".to_owned()+&scrpt);
    });
    

    for scr in scripz {
        dbg!("awa!", scr.clone());
        js_world.add_script("./scripts/".to_owned()+&scr);
    }

    js_world.run();

    if(keconf.is_server && keconf.has_multiplayer){
        js_world.run_server();
    }

    //networking simple
    // ";w; oh nyoooooo networkin is sow haarrrdua"
    // not anymore, shut up; I did it for you righ here :3

    //yeah soooo.... this is crap
    //need to re do it some day

    if(keconf.is_server && keconf.has_multiplayer){
        std::thread::spawn(move || {

            async fn w(){
                let (layer, io) = SocketIo::new_layer();

                io.ns("/", |socket, auth: Value| async move {
                    println!("Socket connected on / namespace with id: {}", socket.id);

                    socket.join(socket.id.to_string());

                    socket.emit("update", json!({
                        "type":"server_your_ticket",
                        "id":socket.id,
                    }));

                    let binding = FIRST.clone();
                    let firs = binding.read().unwrap().to_string();
                    if *firs == format!("") {
                        let mut firs = FIRST.write().unwrap();
                        *firs = socket.id.to_string();
                        //for some reason, the first connection dosn't get the join emit, so...
                        //we gonna do this
                        socket.emit("update", json!({
                            "type":"join",
                            "id":socket.id,
                        }));
                    }

                        socket.to(firs).emit("update", json!({
                            "type":"join",
                            "id":socket.id,
                        }));
        
                    socket.on("update", |socket, data: Value, bin, _| async move {

                        let mut dat: Option<Value> = None;
                        
                        // probasbly want to do match instead of unwrap here so there isn't some odd security thing problem
                        // bleehhhh not doing that
                        let to = match data.get("to") {
                            Some(a) => {
                                dat = Some(data.get("data").unwrap().clone());
                                a.to_string()
                            },
                            None => {
                                dat = Some(data);
                                format!("\"\"")
                            }
                        };
                        //dbg!(to.clone());

                        let data = dat.unwrap();//.get("data").unwrap();

                        let binding = FIRST.clone();
                        let firs = binding.read().unwrap().to_string();
                        
                        let ishost = {
                            firs == socket.id.to_string()
                        };
                        if(to == "\"\""){
                            socket.bin(bin).broadcast().except(socket.id.to_string()).emit("update", json!({"type":"update","fromid":socket.id,"ishost":ishost,"data":data})).ok();
                        }else{
                           socket.bin(bin).to(to).emit("update", json!({"type":"update","fromid":socket.id,"ishost":ishost,"data":data})).ok();
                        }
                        // sender.send(Sockreq { 
                        //     name: format!("create"), 
                        //     data: data
                        //  });
                    });
        
                    // // Add a callback triggered when the socket receive an 'acb' event
                    // // Ackknowledge the message with the ack callback
                    // socket.on("update", |_, data: Value, bin, ack| async move {
                    //     println!("Received acb event: {:?} {:?}", data, bin);
                    //     ack.bin(bin).send(data).ok();
                    // });
                    // Add a callback triggered when the socket disconnect
                    // The reason of the disconnection will be passed to the callback
                    socket.on_disconnect(|socket, reason| async move {
                        println!("Socket.IO disconnected: {} {}", socket.id, reason);
                    });
                });
        
                let app = axum::Router::new()
                .route("/", get(|| async { "this is a kitsune engine exclusive" }))
                .layer(layer);
        
                let _ = Server::bind(&"0.0.0.0:3000".parse().unwrap())
                    .serve(app.into_make_service())
                    .await;
            }

            let tokio_thread = tokio::runtime::Builder::new_current_thread()
                .enable_all()
                .build()
                .unwrap();

            tokio_thread
                .block_on(w());

            
        });
        sleep(Duration::from_secs(2))
        }

        let network_requests: Arc<Mutex<HashMap<u64,Sockreq>>> = Arc::new(Mutex::new(HashMap::new()));
        let next_slot: Arc<Mutex<u64>> = Arc::new(Mutex::new(0));
        let (sender, receiver) = channel::<String>();
        let (senderEmit, receiverEmit) = channel::<Value>();

        let socket = if(!keconf.headless && keconf.has_multiplayer){

            let nr_clone = network_requests.clone();
            let ns_clone = next_slot.clone();

            let sendera = sender.clone();

        let socket = ClientBuilder::new("http://localhost:3000/")
            .namespace("/")
            .on("update", move |payload: Payload, socket: RawClient| {

                match payload {
                    Payload::String(whatt) => {
                        sender.send(whatt);
                        //js_world.triggerlis("wah".to_string(), serde_json::from_str("&str").unwrap());
                        //println!("Received: {}", str);
                    },
                    Payload::Binary(bin_data) => {},
                }
        })
        .on("update", move |payload: Payload, socket: RawClient| {

            match payload {
                Payload::String(whatt) => {
                    sendera.send(whatt);
                    //js_world.triggerlis("wah".to_string(), serde_json::from_str("&str").unwrap());
                    //println!("Received: {}", str);
                },
                Payload::Binary(bin_data) => {},
            }
    })
            .on("error", |err, _| eprintln!("Error: {:#?}", err))
            .connect()
            .expect("Connection failed");

        //sleep(Duration::from_secs(2));

        // let json_payload = json!({"position": {"x":0,"y":0,"z":0}, "velocity": {"x":0,"y":0,"z":0}});
        // socket
        //     .emit("update", json_payload)
        //     .expect("Server unreachable");

        js_world.pinemit(senderEmit);

        Some(socket)
    }else{
        js_world.pindummyemit();
        None
    };

    let (width, height) = display.get_framebuffer_dimensions();
    let lastscreen_texture = glium::texture::SrgbTexture2d::empty(&display, width, height).unwrap();
    let lastscreen_depth_texture = glium::texture::DepthTexture2d::empty(&display, width, height).unwrap();

    let mut lastscreenbuffer = glium::framebuffer::SimpleFrameBuffer::with_depth_buffer(&display, &lastscreen_texture, &lastscreen_depth_texture).unwrap();
    lastscreenbuffer.clear_color(1.0, 0.0, 1.0, 1.0);
    lastscreenbuffer.clear_depth(1.0);

    let (width, height) = display.get_framebuffer_dimensions();
    let screen_texture = glium::texture::SrgbTexture2d::empty(&display, width, height).unwrap();
    let screen_depth_texture = glium::texture::DepthTexture2d::empty(&display, width, height).unwrap();

    let mut screenbuffer = glium::framebuffer::SimpleFrameBuffer::with_depth_buffer(&display, &screen_texture, &screen_depth_texture).unwrap();
    screenbuffer.clear_color(1.0, 0.0, 1.0, 1.0);
    screenbuffer.clear_depth(1.0);
    bufferz.insert(0, screenbuffer);

    *BUFFER_COUNT.write().expect("RwLock poisoned") += 1;

    let screen_vertex = {
        glium::VertexBuffer::new(
            &display,
            &[
                Vertex2D {
                    position: [-1.0, -1.0],
                    tex_coords: [0.0, 0.0],
                },
                Vertex2D {
                    position: [-1.0, 1.0],
                    tex_coords: [0.0, 1.0],
                },
                Vertex2D {
                    position: [1.0, 1.0],
                    tex_coords: [1.0, 1.0],
                },
                Vertex2D {
                    position: [1.0, -1.0],
                    tex_coords: [1.0, 0.0],
                },
                Vertex2D {
                    position: [-1.0, -1.0],
                    tex_coords: [0.0, 0.0],
                },
            ],
        )
        .unwrap()
    };



    // shadow stuff for later awawawawawa
    // let shadow_map_size = 1024;
    // let shadow_texture = glium::texture::DepthTexture2d::empty(&display, shadow_map_size, shadow_map_size).unwrap();

    // let mut shadowbuffer = glium::framebuffer::SimpleFrameBuffer::depth_only(&display, &shadow_texture).unwrap();
    // shadowbuffer.clear_color(1.0, 1.0, 1.0, 1.0);
    // shadowbuffer.clear_depth(1.0);

    let mut nanos = SystemTime::now()
    .duration_since(UNIX_EPOCH)
    .unwrap()
    .as_nanos();
    let mut last_nanos = nanos;
    let mut start = nanos;

    for _amewoemwa in 0..keconf.buffers{
        //let projecto = texturez.get_mut(&0).unwrap();
        //projecto.bebufpointer = true;
        //projecto.bebufto = 0;
        //let nam = texturez.len() as i32;
        let awsa = glium::texture::SrgbTexture2d::empty(&display, width, height).unwrap();
        texturez2.push(awsa);
        //*TEXTURE_COUNT.write().expect("RwLock poisoned") += 1;
        //let screen_texture = texturez.get(&0).unwrap();
    }
    for _amewoemwa in 0..keconf.buffers{
        let awsa = &texturez2[_amewoemwa as usize];

         
        bufferz.insert(1+(_amewoemwa as i32), glium::framebuffer::SimpleFrameBuffer::with_depth_buffer(&display, awsa, &screen_depth_texture).unwrap());
    };

    {
        let mut meowww = FONTS.write().unwrap();

        meowww.insert(0, KEfont::new("fonts/text".to_string(), &mut texturez, &display));
    }

    // { //menu item test
    //     let mut meowww = MENUS.write().unwrap();

    //     meowww.insert(0, KEmenuTypes::image(menuimage{
    //         position: Vector2::new(0.0, 0.0),
    //         size: Vector2::new(0.2, 0.2),
    //         texture: 1,
    //         shader: 3,
    //         uv: Vector2::new(0.0, 0.0),
    //     }));
    // }

    // { //menu item test
    //     let mut meowww = MENUS.write().unwrap();

    //     meowww.insert(0, KEmenuTypes::text(menutext{
    //         position: Vector2::new(-0.5, 0.5),
    //         size: Vector2::new(0.2, 0.2),
    //         shader: 4,
    //         font: 0,
    //         text: "nnnyaaa meowmeowmeowmoew".to_string(),
    //     }));
    // }

    let mut cursor_locked = true;
    
    event_loop.run_return(|event, _, control_flow| {

        let mut nanos = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_nanos();

        let mut delta_time = (nanos - last_nanos) as f32*0.0000001;
        //dbg!(delta_time, nanos, last_nanos);
        last_nanos = nanos;

        if(keconf.is_server && keconf.has_multiplayer){
            js_world.job_server_runs(delta_time);
        }

        if(!keconf.headless){
            js_world.job_runs(delta_time);
        }

        *control_flow = ControlFlow::Poll;

        // let binding = network_requests.clone();
        // let what = binding.lock().unwrap();

        // for (index, packet) in what.iter() {
        //     //print!("{}", index);
        //     //js_world.triggerlis(&"wah".to_string(), &packet.data);
        // }

        if(!keconf.headless){
            match receiver.try_recv() {
                Ok(packet) => {
                    js_world.triggerlis(&"client_update".to_string(), &packet);
                }
                _ => (),
            }

            match receiverEmit.try_recv() {
                Ok(packet) => {
                    let socket = socket.as_ref().unwrap();
                        socket
                        .emit("update", packet)
                        .expect("Server unreachable");
                }
                _ => (),
            }
        }
        

        // womp this is where events from the window thingamabob does things
        // pretty much we are gonna quoue mouse and keyboards
        let mut lightz = LIGHTS.try_write().unwrap();

        match event {
            Event::WindowEvent { event, .. } => match event {
                WindowEvent::Resized(_) => {
                    let mut _main_camera = MAIN_CAM.write().unwrap();
                    let mut camera_map = CAMERAS.write().unwrap();
                    
                    for ele in &bufferz {
                        
                    }
                    
                    let main_cam = camera_map.get_mut(&_main_camera).unwrap();
                    //update main_cam's projection with screen
                    let (width, height) = display.get_framebuffer_dimensions();
                    screen_size = Vector2::new(width as f32, height as f32);
                    main_cam.reproject(screen_size);
                }
                WindowEvent::KeyboardInput { device_id, input, is_synthetic } => {
                    
                    let how = {
                        match input.state {
                            winit::event::ElementState::Pressed => "pressed",
                            winit::event::ElementState::Released => "released",
                        }
                    };

                    
                    let nemweiw  = KeyLayout::get_key_as_string(&thekeylayout, input.scancode);
                    //let mut rqs = REQUESTS.write().unwrap();
                    js_world.triggerlis(&"keypress".to_string(), &json!(
                        {
                            "how": how,
                            "which": input.scancode,
                            "code": nemweiw,
                            "is_synthetic": is_synthetic
                        }
                    ).to_string());
                    //drop(rqs);
                    let mut propz: std::sync::RwLockWriteGuard<HashMap<i32, Prop>> = PROPS.try_write().unwrap();
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
                        if(!keconf.headless){
                            let mut propz: std::sync::RwLockWriteGuard<HashMap<i32, Prop>> = PROPS.try_write().unwrap();
                            let mut _main_camera = MAIN_CAM.write().unwrap();
                            let mut camera_map = CAMERAS.write().unwrap();
                            let (width, height) = display.get_framebuffer_dimensions();
                            let a = Vector2::new(delta.0 as f32, delta.1 as f32);
                            real_char.interp_mouse(&mut phys_world, &mut propz, &mut camera_map, a, screen_size, delta_time*keconf.mouse_sensitivity);
                            
                            if cursor_locked {
                                v.set_cursor_position(LogicalPosition::new(width/2, height/2));
                            }
                        }
                    }
                    _ => {}
                }
            }
            _ => (),
        }

        {let mut a = REQUESTS.write().unwrap();

        

        for requ in &*a {
            match requ {
                KERequest::Create_Shader(ida, location) => {
                    if location == "" {
                        //modelz.insert(*ida, models); do somethin
                    }else{
                        shaderz.insert(*ida, Shader::craft(&format!("./shaders/{location}"), &display));
                    }
                    
                    //*MODEL_COUNT.write().expect("RwLock poisoned") += 1;
                },
                KERequest::Create_Model(ida, location) => {
                    if location == "" {
                        //modelz.insert(*ida, models); do somethin
                    }else{
                        modelz.insert(*ida, models::load_obj(&format!("./models/{location}"), &display));
                    }
                    
                    //*MODEL_COUNT.write().expect("RwLock poisoned") += 1;
                },
                KERequest::Create_Model_From_Magic(modelID, mesh) => {
                    let modael = modelz.get_mut(&modelID).unwrap();
                    modael.verts.write(&mesh);
                },
                KERequest::Create_Texture(ida, local, width, height) => {
                    
                        
                    if local == "" {
                        HashMap::insert(&mut texturez, *ida, Texture::craft_lorp(*width, *height, &display)); //later when not lazy, make width and high customizable
                    }else{
                        HashMap::insert(&mut texturez, *ida, Texture::craft(&format!("./textures/{local}"), &display));
                    }
                },
                KERequest::Pin_Texture_Buffer(wheret, placed) => {
                    let projecto = texturez.get_mut(&wheret).unwrap();
                    projecto.bebufpointer = true;
                    projecto.bebufto = *placed as usize;
                    
                },
                KERequest::Delete_Prop(propid) => {
                    let mut propz: std::sync::RwLockWriteGuard<HashMap<i32, Prop>> = PROPS.try_write().unwrap();
                    let w = propz.get_mut(&propid).unwrap();
                    todo!();
                    //phys_world;
                },
                KERequest::Create_Buffer(_) => todo!(),
                KERequest::Create_Camera(_) => todo!(),
                KERequest::Pin_Buffer_Camera(_, _) => todo!(),
                KERequest::Phys_Prop_Push(propid, flyto) => {
                    let mut propz: std::sync::RwLockWriteGuard<HashMap<i32, Prop>> = PROPS.try_write().unwrap();
                    phys_world.apply_force(propz.get_mut(&propid).unwrap(), *flyto);
                },
                KERequest::Phys_Prop_Push_SideOnly(propid, flyto) => {
                    let mut propz: std::sync::RwLockWriteGuard<HashMap<i32, Prop>> = PROPS.try_write().unwrap();
                    phys_world.apply_force_xz(propz.get_mut(&propid).unwrap(), *flyto);
                },
                KERequest::load_map(locala) => {
                   //loadmap(locala.to_string(), &mut propz, &mut texturez, &mut modelz, &mut phys_world);
                },
                KERequest::js_push(namre, dattaa) => {
                    js_world.triggerlis(&namre, &dattaa);
                },
                KERequest::copy_prop_phys_pose(propid) => {
                    let mut propz: std::sync::RwLockWriteGuard<HashMap<i32, Prop>> = PROPS.try_write().unwrap();
                    phys_world._sync_phys_prop(propz.get_mut(&propid).unwrap(), CopyWhat::All);
                },
                KERequest::window_cursor_lock(is) => {
                    if *is {
                        v.set_cursor_grab(CursorGrabMode::Locked);
                        v.set_cursor_visible(false);
                        cursor_locked = true;
                    } else {
                        v.set_cursor_grab(CursorGrabMode::None);
                        v.set_cursor_visible(true);
                        cursor_locked = false;
                    }
                },
                KERequest::exit => {
                    control_flow.set_exit()
                },
                KERequest::NULL => todo!(),
            }
        };

        (a).clear();}

        if(keconf.shader_hotswap){
            for x in 0..shaderz.len() as i32 {
                let mut sh = shaderz.get_mut(&x).unwrap();
                let name = sh.url.clone();
                let metadataF = fs::metadata(format!("{name}.frag")).expect("failed to check shader file");
                let metadataV = fs::metadata(format!("{name}.vert")).expect("failed to check shader file");

                if(metadataF.modified().unwrap() != sh.time_changed_f || metadataV.modified().unwrap() != sh.time_changed_v) {
                    sh.time_changed_f = metadataF.modified().unwrap();
                    sh.time_changed_v = metadataV.modified().unwrap();
                    *sh = Shader::craft(&name, &display);
                    println!("{} has been updated", name);
                }
            }
        }

        if(keconf.shader_hotswap){
            let mut Faona = FONTS.write().unwrap();
            for x in 0..Faona.len() as i32 {
                let mut sh = Faona.get_mut(&x).unwrap();
                let name = sh.location.clone();
                let metadata = fs::metadata(format!("{name}.json")).expect("failed to check shader file");

                if(metadata.modified().unwrap() != sh.time_changed) {
                    sh.time_changed = metadata.modified().unwrap();
                    *sh = KEfont::new(name.to_string(), &mut texturez, &display);
                    println!("{} has been updated", name);
                }
            }
        }
        

        //
        // this loop is pretty much the main render pipeline
        // from this line and down
        //

        // let mut to_remove: Vec<i32> = Vec::new();

        loop_wawa += 1.0;

        if(keconf.headless){
            let mut propz: std::sync::RwLockWriteGuard<HashMap<i32, Prop>> = PROPS.try_write().unwrap();
            //step physics world
            phys_world.step();

            for po in propz.iter_mut() {
                let prop = po.1;
                phys_world._sync_prop(prop, CopyWhat::All);
            };
        }
        

        if(!keconf.headless){
            let mut propz: std::sync::RwLockWriteGuard<HashMap<i32, Prop>> = PROPS.try_write().unwrap();

            real_char.step(&mut phys_world, &mut propz, delta_time);

            //step physics world
            phys_world.step();


            // start drawing frame
            let mut target = display.draw();
            target.clear_color_and_depth((world_emv.skyColor.x, world_emv.skyColor.y, world_emv.skyColor.z, 1.0), 1.0);

            let mut camera_map = CAMERAS.write().unwrap();

            for camz in 0..camera_map.len() {

                let mut _main_camera = MAIN_CAM.write().unwrap();
            
                let main_cam = camera_map.get_mut(&(camz as i32)).unwrap();

                main_cam.refresh();

            let mut screenbuffer = bufferz.get_mut(&main_cam.draw_buffer_to).unwrap();
            screenbuffer.clear_color_and_depth((world_emv.skyColor.x, world_emv.skyColor.y, world_emv.skyColor.z, 1.0), 1.0);

            //if cam prop parent moved, we move cam
            if main_cam.parent_prop != -1 {
                main_cam.position = propz.get(&main_cam.parent_prop).unwrap().position+main_cam.parent_offset;
                //main_cam.look_at(Vector3::new(0.0, 0.0, 0.0)); //funny testing
                main_cam.refresh();
            }

            if main_cam.disabled {continue};


            

            let mut trans_props: Vec<i32> = Vec::new();

            // now in theory one could get all the closest props and push refrences of those props in to a list
            // then replace propz here to that list to implement some sorta calling
            // i'ma do that later
            let lastdepthtext = glium::uniforms::Sampler::new(&lastscreen_depth_texture)
            .magnify_filter(glium::uniforms::MagnifySamplerFilter::Nearest)
            .minify_filter(glium::uniforms::MinifySamplerFilter::Nearest)
            .depth_texture_comparison(Some(glium::uniforms::DepthTextureComparison::LessOrEqual));
        
            for po in 0..propz.len() {
                let pid = (po) as i32;

                let prop = propz.get_mut(&pid).unwrap();

                if prop.parent_prop != -1 {
                    prop.position = PROPS.read().unwrap().get(&prop.parent_prop).unwrap().position+prop.parent_offset;
                    //main_cam.look_at(Vector3::new(0.0, 0.0, 0.0)); //funny testing
                }
                
                //sync physics prop to visual prop or vis versa
                if prop.phys_type == phytype::DynamicCollider {
                    phys_world._sync_phys_prop(prop, CopyWhat::All);
                }else if prop.phys_type == phytype::Dynamic {
                    phys_world._sync_prop(prop, CopyWhat::All);
                }
                
                if !prop.render {continue;}
                if prop.face_cam {
                    if(main_cam.position.z<prop.position.z){
                        prop.look_at(main_cam.position-prop.position);
                    }else{
                        prop.look_at(prop.position-main_cam.position);
                    }
                }
                if prop.transparency != 1.0 {
                    trans_props.push(pid); //put it in a list to retry in the transparency pass
                    continue;
                }
                

                render_prop(loop_wawa, prop, main_cam, &shader_vars, &world_emv, &lightz, &lastscreen_texture, &lastdepthtext, &mut screenbuffer, &texturez, &texturez2, &mut target, &modelz, &shaderz, &params);

                // if prop.position.y < -2.0 {
                //     to_remove.push(*po.0);
                // }
            };

            lastscreenbuffer.blit_buffers_from_simple_framebuffer(&screenbuffer, &glium::Rect { left: 0, bottom: 0, width: width, height: height }, &glium::BlitTarget { left: 0, bottom: 0, width: width as i32, height: height as i32 }, glium::uniforms::MagnifySamplerFilter::Nearest, glium::BlitMask { color: true, depth: true, stencil: false });
        
            for po in &trans_props {
                let prop = propz.get_mut(&po).unwrap();
                if prop.transparency == 0.0 {continue;}

                if !prop.render {continue;}

                
                render_prop(loop_wawa, prop, main_cam, &shader_vars, &world_emv, &lightz, &lastscreen_texture, &lastdepthtext, &mut screenbuffer, &texturez, &texturez2, &mut target, &modelz, &shaderz, &params);
            };
        }
            let mut screenbuffer: &mut SimpleFrameBuffer = bufferz.get_mut(&_main_buffer).unwrap();

            screen_compile(loop_wawa, &screen_vertex, &3, &shader_vars, &screen_texture, &lastscreen_depth_texture, &mut screenbuffer, &mut target, &shaderz, &screenparams);

            let mut menuz: std::sync::RwLockWriteGuard<HashMap<i32, KEmenuTypes>> = MENUS.try_write().unwrap();
            for po in 0..(menuz.len() as i32) {
                let menuitem = menuz.get_mut(&po).unwrap();

                match menuitem {
                    KEmenuTypes::image(imelm) => {
                        if !imelm.render { continue };
                        render_menu(loop_wawa, imelm.position, imelm.size, Vector4::zeros(), &screen_vertex, &imelm.shader, &shader_vars, &get_texture_raw(&texturez, imelm.texture, &texturez2), &screen_texture, &lastscreen_depth_texture, &mut screenbuffer, &mut target, &shaderz, &screenparams);
                    },
                    KEmenuTypes::text(imelm) => {
                        if !imelm.render { continue };

                        let Faona = FONTS.write().unwrap();
                        let fonta = {
                            Faona.get(&imelm.font).unwrap()
                        };

                        let mut currentwidth: f32 = 0.0;
                        let mut currentheight: f32 = 0.0;
                        for charas in imelm.text.chars() {

                            let width = width as f32;
                            let height = height as f32;

                            if(charas == '\n'){//TODO: fix this
                                currentwidth=0.0;
                                currentheight=(currentheight)-(fonta.default_line_height as f32);
                                continue;
                            }

                            let fontchar = match fonta.chars.get(&charas) {
                                Some(a) => {a},
                                None => {
                                    dbg!("!!!! couldn't fine char in font !!!!");
                                    fonta.chars.get(&'?').unwrap()
                                },
                            };

                            let uv = Vector2::new(fontchar.pos.x as f32,fontchar.pos.y as f32);

                            
                            let x = 0.003*width;
                            let y = 0.01*height;
                            let size = Vector2::new(x/width,y/height);

                            currentwidth += ((x/width)*10.0)*width;

                            let x = (0.0*width)+(currentwidth);
                            let y = (0.0*height)+(currentheight);
                            let pos = Vector2::new(x/width, y/height)+imelm.position;
                            
                            

                            
                            render_menu(loop_wawa, pos, size, Vector4::new(fontchar.pos.x as f32, fontchar.pos.y as f32, fontchar.size.x as f32, fontchar.size.y as f32), &screen_vertex, &imelm.shader, &shader_vars, &get_texture_raw(&texturez, fonta.texture, &texturez2), &screen_texture, &lastscreen_depth_texture, &mut screenbuffer, &mut target, &shaderz, &screenparams);
                        }
                        
                    },
                }

                
                //render_prop(loop_wawa, prop, main_cam, &shader_vars, &world_emv, &lightz, &lastscreen_texture, &mut screenbuffer, &texturez, &texturez2, &mut target, &modelz, &shaderz, &params);
            };

            //target.blit_buffers_from_simple_framebuffer(&screenbuffer, &glium::Rect { left: 0, bottom: 0, width: width, height: height }, &glium::BlitTarget { left: 0, bottom: 0, width: width as i32, height: height as i32 }, glium::uniforms::MagnifySamplerFilter::Nearest, glium::BlitMask { color: true, depth: true, stencil: false });
            
            // finish frame and put on window probably
            target.finish().unwrap();
        }
        

        // for amongus in to_remove {
        //     dbg!(amongus);
        //     let prop = propz.get_mut(&amongus).unwrap();
        //     phys_world.remove_phy(prop);
        //     propz.remove(&amongus);
        // }
    });
}

fn screen_compile(loop_wawa: f32, screen_model: &VertexBuffer<Vertex2D>, screen_shader: &i32, shader_vars: &HashMap<String, ShadvType>, screen_texture: &SrgbTexture2d, screen_depth_texture: &DepthTexture2d, screenbuffer: &mut  SimpleFrameBuffer, target: &mut glium::Frame, shaderz: &HashMap<i32, Shader>, params: &DrawParameters<'_>){
    let mut uniform = dynamic_uniform::DynamicUniforms::new();
    
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
            ShadvType::Texture(_) => () //this feature is probably not going to be used all too much, so why bother
        }
    }

    uniform.add("time".to_owned(), &loop_wawa);

    uniform.add("matrix".to_owned(), &[
        [1.0, 0.0, 0.0, 0.0],
        [0.0, 1.0, 0.0, 0.0],
        [0.0, 0.0, 1.0, 0.0],
        [0.0, 0.0, 0.0, 1.0f32]
    ]);

    uniform.add("screenbuffer".to_owned(), screen_texture);
    uniform.add("texturez".to_owned(), screen_texture);
    
    let binding = glium::uniforms::Sampler::new(screen_depth_texture)
    .magnify_filter(glium::uniforms::MagnifySamplerFilter::Nearest)
    .minify_filter(glium::uniforms::MinifySamplerFilter::Nearest)
    .depth_texture_comparison(Some(glium::uniforms::DepthTextureComparison::LessOrEqual));
    uniform.add("screenbufferdepth".to_owned(), &binding);

    let (width, height) = screenbuffer.get_dimensions();
    let binding = [width as f32,height as f32];
    uniform.add("framebufferSize".to_owned(), &binding);

    // here we draw the prop on the frame
    target
        .draw(
            screen_model,
            NoIndices(PrimitiveType::TriangleStrip),
            &shaderz.get(screen_shader).unwrap().program.as_ref().unwrap(),
            &uniform,
            params,
        )
        .unwrap();

}

fn render_prop(loop_wawa: f32, prop: &mut Prop, main_cam: &mut Camera, shader_vars: &HashMap<String, ShadvType>, world_emv: &Environment, lightz: &Vec<PointLight>, screen_texture: &SrgbTexture2d, screendep_texture: &glium::uniforms::Sampler<DepthTexture2d>,  screenbuffer: &mut  SimpleFrameBuffer, texturez: &HashMap<i32, Texture>, texturez2: &Vec<SrgbTexture2d>, target: &mut glium::Frame, modelz: &HashMap<i32, Model>, shaderz: &HashMap<i32, Shader>, params: &DrawParameters<'_>) {
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
            ShadvType::Texture(value) => {
                uniform.add(name, get_texture_raw(texturez, *value, texturez2));
            },
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
            ShadvType::Texture(value) => {
                uniform.add(name, get_texture_raw(texturez, *value, texturez2));
            },
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

    uniform.add("diffuse_tex".to_string(), get_texture(texturez, prop, 0, texturez2));
    uniform.add("normal_tex".to_string(), &1.0);

    uniform.add("material.diffuse".to_owned(), get_texture(texturez, prop, 0, texturez2));
    uniform.add("material.specular".to_owned(), &0.0);
    uniform.add("material.shininess".to_owned(), &0.5);

    // let sb = &glium::uniforms::Sampler::new(&screen_texture)
    // .magnify_filter(glium::uniforms::MagnifySamplerFilter::Nearest)
    // .minify_filter(glium::uniforms::MinifySamplerFilter::Nearest);

    uniform.add("screenbuffer".to_owned(), screen_texture);
    uniform.add("screenbufferdepth".to_owned(), screendep_texture);

    let (width, height) = screenbuffer.get_dimensions();
    let binding = [width as f32,height as f32];
    uniform.add("framebufferSize".to_owned(), &binding);

    uniform.add("time".to_owned(), &loop_wawa);

    let mut currrent_text = 0;
    for text in &prop.textures {
        uniform.add("texture".to_string()+&(currrent_text+1).to_string(), get_texture(texturez, prop, currrent_text, texturez2));
        currrent_text+=1;
    }

    // here we draw the prop on the frame
    screenbuffer
        .draw(
            get_model(modelz, prop),
            NoIndices(PrimitiveType::TrianglesList),
            &get_shader(shaderz, prop),
            &uniform,
            params,
        )
        .unwrap();
}

fn render_menu(loop_wawa: f32, pos: Vector2<f32>, scal: Vector2<f32>, uv: Vector4<f32>, screen_model: &VertexBuffer<Vertex2D>, screen_shader: &i32, shader_vars: &HashMap<String, ShadvType>, texturea: &SrgbTexture2d, screen_texture: &SrgbTexture2d, screen_depth_texture: &DepthTexture2d, screenbuffer: &mut  SimpleFrameBuffer, target: &mut glium::Frame, shaderz: &HashMap<i32, Shader>, params: &DrawParameters<'_>){
    let mut uniform = dynamic_uniform::DynamicUniforms::new();
    
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
            ShadvType::Texture(_) => (),
        }
    }

    uniform.add("time".to_owned(), &loop_wawa);


    //-(((loop_wawa*0.01) as f64) as f32)*0.2

    let binding = [
        [scal.x, 0.0, 0.0, 0.0],
        [0.0, scal.y, 0.0, 0.0],
        [0.0, 0.0, 1.0, 0.0],
        [pos.x, pos.y, 0.0, 1.0f32]
    ];
    uniform.add("matrix".to_owned(), &binding);

    uniform.add("screenbuffer".to_owned(), screen_texture);

    uniform.add("texturez".to_owned(), texturea);

    let uva = uv.xz()/texturea.get_width() as f32;
    let uvb = Vector2::new(uv.y, uv.w)/texturea.get_height().unwrap() as f32;

    let binding = [uva.x, uvb.x, uva.y, uvb.y];
    uniform.add("uv".to_owned(), &binding);
    
    let binding = glium::uniforms::Sampler::new(screen_depth_texture)
    .magnify_filter(glium::uniforms::MagnifySamplerFilter::Nearest)
    .minify_filter(glium::uniforms::MinifySamplerFilter::Nearest)
    .depth_texture_comparison(Some(glium::uniforms::DepthTextureComparison::LessOrEqual));
    uniform.add("screenbufferdepth".to_owned(), &binding);

    let (width, height) = screenbuffer.get_dimensions();
    let binding = [width as f32,height as f32];
    uniform.add("framebufferSize".to_owned(), &binding);

    // here we draw the prop on the frame
    target
        .draw(
            screen_model,
            NoIndices(PrimitiveType::TriangleStrip),
            &shaderz.get(screen_shader).unwrap().program.as_ref().unwrap(),
            &uniform,
            params,
        )
        .unwrap();

}

fn get_shader<'a>(shadersz: &'a HashMap<i32, Shader>, prop: &Prop) -> &'a Program {
    match shadersz.get(&prop.shader) {
        Some(shader) => {
            match &shader.program {
                Some(real) => &real,
                None => &shadersz.get(&5).unwrap().program.as_ref().unwrap(),
            }
        },
        None => &shadersz.get(&5).unwrap().program.as_ref().unwrap(),
    }
}

fn get_model<'a>(modelz: &'a HashMap<i32, Model>, prop: &Prop) -> &'a VertexBuffer<models::Vertex> {
    match modelz.get(&prop.model) {
        Some(model) => &model.verts,
        None => &modelz.get(&2).unwrap().verts,
    }
}

fn get_texture<'a>(texturez: &'a HashMap<i32, Texture>, prop: &Prop, texture: usize, texturez2: &'a Vec<SrgbTexture2d>) -> &'a SrgbTexture2d {
    //dbg!(prop.textures[texture]);
    match texturez.get(&prop.textures[texture]) {
        Some(texture) => {
            if texture.bebufpointer {
                return texturez2.get(texture.bebufto).unwrap();
            }
            &texture.texture
        },
        None => &texturez.get(&1).unwrap().texture,
    }
}

fn get_texture_raw<'a>(texturez: &'a HashMap<i32, Texture>, ida: i32, texturez2: &'a Vec<SrgbTexture2d>) -> &'a SrgbTexture2d {
    //dbg!(prop.textures[texture]);
    match texturez.get(&ida) {
        Some(texture) => {
            if texture.bebufpointer {
                return texturez2.get(texture.bebufto).unwrap();
            }
            &texture.texture
        },
        None => &texturez.get(&1).unwrap().texture,
    }
}