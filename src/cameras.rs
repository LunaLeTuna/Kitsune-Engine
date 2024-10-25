use std::f32::consts::PI;

use nalgebra::{Matrix4, Perspective3, Rotation3, Vector3, Vector2};

use crate::ke_units::Vec2;

#[derive(Clone)]
pub struct Camera {
    pub name: String,
    pub projection: Perspective3<f32>,
    pub view: Matrix4<f32>,
    pub position: Vector3<f32>,
    pub rotation: Rotation3<f32>,
    pub up: Vector3<f32>,

    pub disabled: bool,

    pub draw_buffer_to: i32,
    pub world: u32,

    pub parent_prop: i32, // prop id
    pub parent_offset: Vector3<f32>,

    pub near: f32,
    pub far: f32,
    pub fov: f32,
    pub aspect: f32,
}

impl Camera {
    pub fn craft(screen: Vec2) -> Self {
        let near = 0.1;
        let far = 1024.0;
        let fov = 60.0;

        let aspect = screen.x / screen.y;

        Self {
            name: "meow".into(),
            up: Vector3::y(),
            far,
            near,
            fov: 60.0,
            projection: Perspective3::new(aspect, radians(fov), near, far),
            position: Vector3::zeros(),
            rotation: Rotation3::new(Vector3::zeros()),
            draw_buffer_to:0,
            world: 0,
            aspect,
            view: Matrix4::new_scaling(1.0),
            parent_prop: -1,
            parent_offset: Vector3::zeros(),
            disabled: false
        }
    }

    pub fn set_rotation(&mut self, target: Vector3<f32>) { self.rotation = nalgebra::Rotation3::from_euler_angles(target.x,target.y,target.z); }

    pub fn look_at(&mut self, target: Vector3<f32>) {
        self.rotation = nalgebra::Rotation3::look_at_lh(&(target*-1.0), &self.up);
    }

    pub fn project_drop(&self) -> [[f32; 4]; 4] {
        let binding = self.projection.to_homogeneous();
        let pogger = binding.as_ref();

        *pogger
    }

    pub fn view_drop(&self) -> [[f32; 4]; 4] { self.view.try_into().unwrap() }

    pub fn refresh(&mut self) {
        let mut model = self.rotation.matrix().to_homogeneous();
        model = model.prepend_translation(&(&self.position*-1.0));

        self.view = model;
    }

    pub fn reproject(&mut self, screen: Vector2<f32>) {
        self.projection = Perspective3::new(screen.x / screen.y, radians(self.fov), self.near, self.far);
    }
}

fn radians(degrees: f32) -> f32 { degrees * PI / 180.0 }
