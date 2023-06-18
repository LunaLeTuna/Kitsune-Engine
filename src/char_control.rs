use std::collections::HashMap;
use glium::{Depth, Display, DrawParameters, Program, Surface, VertexBuffer};
use nalgebra::{Vector3, Vector2};
use winit::{event::VirtualKeyCode, dpi::Position};

use crate::{props::{Prop, phytype, physhape}, cameras::Camera, ke_units::{Vec2, radians}, physic_props::PhysWorld};

#[derive(Clone, PartialEq)]
pub enum character_type {
    Third,
    Second,
    NoClip,
    Scripted,
    Disabled,
}

pub struct Character {
    pub body: i32, //props are the ke entity base, supplying physics and maybe a model
    pub camera: i32,

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

    pub character_type: character_type
}

fn pivot_point(place: Vector2<f32>, center: Vector2<f32>, rot:f32) -> Vector3<f32>{
    let resultx = rot.cos()*(place.x-center.x)-rot.sin()*(place.y-center.y)+center.x;
    let resulty = rot.sin()*(place.x-center.x)+rot.cos()*(place.y-center.y)+center.y;
    Vector3::new(resultx, 0.0, resulty)
} 

impl Character {
    pub fn new(character_type:character_type, display: &Display, propz: &mut HashMap<i32, Prop>, phys_world: &mut PhysWorld, camera_map: &mut HashMap<i32, Camera> ) -> Character {
        
        let mut womp = Prop::new("nya :3".to_owned());
        womp.model = 0;
        womp.texture1 = 0;
        womp.texture2 = 0;
        womp.phys_shape = physhape::Box;
        womp.phys_type = phytype::Dynamic;
        womp.position = Vector3::new(5.0, 16.0, 0.0);
        womp.render = false;
        phys_world.create_phy(&mut womp);
        phys_world.lock_rotations(&womp);
        
        let new_id_prop = propz.len() as i32;
        propz.insert(new_id_prop, womp);

        
        let (width, height) = display.get_framebuffer_dimensions();

        let mut main_cam = Camera::craft(Vec2 {
            x: width as f32,
            y: height as f32,
        });
    
        //main_cam.set_rotation(Vector3::new(-0.5, 0.0, 0.3));
        main_cam.position = Vector3::zeros();
        main_cam.parent_prop = new_id_prop;
        main_cam.parent_offset = Vector3::new(0.0, 0.5, 0.0);
    
        main_cam.refresh();

        let new_id_cam = camera_map.len() as i32;
        camera_map.insert(new_id_cam, main_cam);

        Character{
            body: new_id_prop,
            camera: new_id_cam,
            speed: 1.0,
            run_speed: 5.0,
            additive_speed: 0.0,
            is_running: false,
            is_croutched: false,
            character_type,
            first_mouse_move: true,
            last_mouse_pos: Vector2::new(800.0 / 2.0, 600.0 / 2.0),
            yaw: -90.0,
            pitch: 0.0,
            camfont: Vector3::new(0.0,1.0,0.0),
        }
    }

    pub fn apply_force(&mut self, phys_world: &mut PhysWorld, propz: &mut HashMap<i32, Prop>, force: Vector3<f32>){
        phys_world.apply_force(propz.get(&self.body).unwrap(), force);
    }

    pub fn interp_key(&mut self, phys_world: &mut PhysWorld, propz: &mut HashMap<i32, Prop>, a: VirtualKeyCode){
        match self.character_type {
            character_type::Third => {
                match a {
                    winit::event::VirtualKeyCode::W => {
                        self.apply_force(phys_world, propz, pivot_point(Vector2::x(), Vector2::zeros(), radians(self.yaw)))
                    }
                    winit::event::VirtualKeyCode::S => {
                        self.apply_force(phys_world, propz, pivot_point(-Vector2::x(), Vector2::zeros(), radians(self.yaw)))
                    }
                    winit::event::VirtualKeyCode::A => {
                        self.apply_force(phys_world, propz, pivot_point(-Vector2::y(), Vector2::zeros(), radians(self.yaw)))
                    }
                    winit::event::VirtualKeyCode::D => {
                        self.apply_force(phys_world, propz, pivot_point(Vector2::y(), Vector2::zeros(), radians(self.yaw)))
                    }
                    winit::event::VirtualKeyCode::Space => {
                        self.apply_force(phys_world, propz, Vector3::new(0.0, 2.0, 0.0))
                    }
                    _ => ()
                }
            },
            character_type::Second => todo!(),
            character_type::NoClip => todo!(),
            character_type::Scripted => todo!(),
            character_type::Disabled => todo!(),
        }
    }

    pub fn interp_mouse(&mut self, phys_world: &mut PhysWorld, propz: &mut HashMap<i32, Prop>, camera_map: &mut HashMap<i32, Camera>, a: Vector2<f32>, screen_size: Vector2<f32>){
        match self.character_type {
            character_type::Third => {
                //print("mouse x: " + event.xpos);
                //print("mouse y: " + event.ypos);

                let a = a-(screen_size/2.0);

                let sensitivity = 0.1; // change this value to your liking
                self.yaw += (a.x*sensitivity)%360.0;
                self.pitch -= a.y*sensitivity;

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
            character_type::Second => todo!(),
            character_type::NoClip => todo!(),
            character_type::Scripted => todo!(),
            character_type::Disabled => todo!(),
        }
    }
}