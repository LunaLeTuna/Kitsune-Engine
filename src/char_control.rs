use std::collections::HashMap;
use glium::{Depth, Display, DrawParameters, Program, Surface, VertexBuffer};
use nalgebra::{Vector3, Vector2};
use winit::{event::{VirtualKeyCode, KeyboardInput}, dpi::Position};

use crate::{cameras::Camera, ke_units::{radians, Vec2}, main, models::Model, physic_props::{CopyWhat, PhysWorld}, props::{physhape, phytype, Prop}};

#[derive(Clone, PartialEq, Debug)]
pub enum character_type {
    First,
    Scroller,
    NoClip,
    Scripted,
    Disabled,
}

pub struct Character {
    pub body: i32, //props are the ke entity base, supplying physics and maybe a model
    pub camera: i32,

    //positive, forowrds; nagitave, backwords
    pub forword: f32,
    //negative, left, positive, right
    pub sideways: f32,

    pub first_mouse_move: bool,
    pub last_mouse_pos: Vector2<f32>,
    pub yaw: f32,
    pub pitch: f32,
    pub camfont: Vector3<f32>,

    pub speed: f32,
    pub run_speed: f32,
    pub additive_speed: f32,

    pub is_running: bool,
    pub is_croutched: bool,

    pub character_type: character_type,

    pub world: u32
}

fn pivot_point(place: Vector2<f32>, center: Vector2<f32>, rot:f32) -> Vector2<f32>{
    let resultx = rot.cos()*(place.x-center.x)-rot.sin()*(place.y-center.y)+center.x;
    let resulty = rot.sin()*(place.x-center.x)+rot.cos()*(place.y-center.y)+center.y;
    Vector2::new(resultx, resulty)
} 

impl Character {
    pub fn new(character_type:character_type, display: &Display, propz: &mut HashMap<i32, Prop>, modelz: &mut HashMap<i32, Model>, phys_world: &mut PhysWorld, camera_map: &mut HashMap<i32, Camera> ) -> Character {
        
        let mut womp = Prop::new("TheCharacterController".to_owned());
        womp.model = 0;
        womp.textures = vec![0,0];
        womp.phys_shape = physhape::Capsule;
        womp.phys_type = phytype::Dynamic;
        womp.position = Vector3::new(5.0, 16.0, 0.0);
        womp.render = false;
        phys_world.create_phy(&mut womp, modelz);
        phys_world.lock_rotations(&womp);
        
        let new_id_prop = propz.len() as i32;
        propz.insert(new_id_prop, womp);

        
        let (width, height) = display.get_framebuffer_dimensions();

        let mut main_cam = Camera::craft(Vec2 {
            x: width as f32,
            y: height as f32,
        });
    
        //main_cam.set_rotation(Vector3::new(-0.5, 0.0, 0.3));
        main_cam.name = "TheCharacterCamera".to_string();
        main_cam.position = Vector3::zeros();
        main_cam.parent_prop = new_id_prop;
        main_cam.parent_offset = Vector3::new(0.0, 0.5, 0.0);
    
        main_cam.refresh();

        let new_id_cam = camera_map.len() as i32;
        camera_map.insert(new_id_cam, main_cam);

        Character{
            body: new_id_prop,
            camera: new_id_cam,
            speed: 2.0,
            run_speed: 17.0,
            additive_speed: 0.0,
            is_running: false,
            is_croutched: false,
            character_type,
            first_mouse_move: true,
            last_mouse_pos: Vector2::new(800.0 / 2.0, 600.0 / 2.0),
            yaw: -90.0,
            pitch: 0.0,
            camfont: Vector3::new(0.0,1.0,0.0),
            forword: 0.0,
            sideways: 0.0,
            world: 0
        }
    }

    //I think teleports player... I forgot
    pub fn tp(&mut self, phys_world: &mut PhysWorld, propz: &mut HashMap<i32, Prop>, place: Vector3<f32>){
        let leprop = propz.get_mut(&self.body).unwrap();
        leprop.position = place;
        phys_world._sync_phys_prop(leprop, CopyWhat::All);
    }

    pub fn apply_force(&mut self, phys_world: &mut PhysWorld, propz: &mut HashMap<i32, Prop>, force: Vector2<f32>){
        phys_world.apply_force_xz(propz.get(&self.body).unwrap(), force);
    }

    pub fn step(&mut self, phys_world: &mut PhysWorld, propz: &mut HashMap<i32, Prop>, delta_time: f32){
        match self.character_type {
            character_type::First => {self.apply_force(phys_world, propz, pivot_point(Vector2::new(self.forword, self.sideways), Vector2::zeros(), radians(self.yaw))*self.speed*delta_time)},
            character_type::Scroller => todo!(),
            character_type::NoClip => todo!(),
            character_type::Scripted => {},
            character_type::Disabled => {},
        }
        
    }

    pub fn interp_key(&mut self, phys_world: &mut PhysWorld, propz: &mut HashMap<i32, Prop>, input: KeyboardInput, delta_time: f32){
        let a = input.virtual_keycode.unwrap_or_else(|| winit::event::VirtualKeyCode::NoConvert);
        let state = input.state;
        match self.character_type {
            character_type::First => {
                let speed = self.speed;
                match a {
                    winit::event::VirtualKeyCode::W => {
                        match state {
                            winit::event::ElementState::Pressed => {self.forword = 1.0},
                            winit::event::ElementState::Released => {self.forword = 0.0},
                        }
                    }
                    winit::event::VirtualKeyCode::S => {
                        match state {
                            winit::event::ElementState::Pressed => {self.forword = -1.0},
                            winit::event::ElementState::Released => {self.forword = 0.0},
                        }
                    }
                    winit::event::VirtualKeyCode::A => {
                        match state {
                            winit::event::ElementState::Pressed => {self.sideways = -1.0},
                            winit::event::ElementState::Released => {self.sideways = 0.0},
                        }
                    }
                    winit::event::VirtualKeyCode::D => {
                        match state {
                            winit::event::ElementState::Pressed => {self.sideways = 1.0},
                            winit::event::ElementState::Released => {self.sideways = 0.0},
                        }
                        
                    }
                    winit::event::VirtualKeyCode::Space => {
                        //self.apply_force(phys_world, propz, Vector3::new(0.0, 2.0, 0.0))
                    }
                    _ => ()
                }
            },
            character_type::Scroller => todo!(),
            character_type::NoClip => todo!(),
            character_type::Scripted => todo!(),
            character_type::Disabled => {},
        }
    }

    pub fn interp_mouse(&mut self, phys_world: &mut PhysWorld, propz: &mut HashMap<i32, Prop>, camera_map: &mut HashMap<i32, Camera>, mouse_delta: Vector2<f32>, screen_size: Vector2<f32>, delta_time: f32){
        match self.character_type {
            character_type::First => {
                //print("mouse x: " + event.xpos);
                //print("mouse y: " + event.ypos);

                //let a = a-(screen_size/2.0);

                // if self.first_mouse_move {
                //     self.last_mouse_pos = a;
                //     self.first_mouse_move = false;
                // }

                // let b = a - self.last_mouse_pos;
                // self.last_mouse_pos = a;

                let sensitivity = 0.6; // change this value to your liking
                self.yaw += (mouse_delta.x*sensitivity)%360.0;
                self.pitch -= mouse_delta.y*sensitivity;

                // make sure that when pitch is out of bounds, screen doesn't get flipped
                if self.pitch > 89.0
                    {self.pitch = 89.0;}
                if self.pitch < -89.0
                    {self.pitch = -89.0;}
                    
                let p_cam = camera_map.get_mut(&self.camera).unwrap();

                let mut front = Vector3::zeros();
                front.x = radians(self.yaw).cos() * radians(self.pitch).cos();
                front.y = radians(self.pitch).sin();
                front.z = radians(self.yaw).sin() * radians(self.pitch).cos();
                front.normalize();

                p_cam.look_at(front);

                //p_cam.set_rotation(Vector3::new(self.pitch, self.yaw, 0.0));
            },
            character_type::Scroller => todo!(),
            character_type::NoClip => todo!(),
            character_type::Scripted => todo!(),
            character_type::Disabled => {},
        }
    }
}