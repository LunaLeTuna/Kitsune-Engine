use std::{f32::consts::PI, ops::Add};

use nalgebra::{Matrix4, Perspective3, Rotation, Rotation3, Vector2, Vector3};

pub struct Camera {
    pub name: String,
    pub projection: Perspective3<f32>,
    pub view: Matrix4<f32>,
    pub position: Vector3<f32>,
    pub rotation: Rotation3<f32>,
    pub up: Vector3<f32>,

    pub near: f32,
    pub far: f32,
    pub fov: f32,
    pub aspect: f32,
}

impl Camera {
    pub fn craft(screen: Vector2<f32>) -> Self {
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
            aspect,
            view: Matrix4::new_scaling(1.0),
        }
    }

    pub fn set_rotation(&mut self, target: Vector3<f32>) {
        self.rotation = nalgebra::Rotation3::new(target);
    }

    pub fn look_at(&mut self, target: Vector3<f32>) {
        self.rotation = nalgebra::Rotation3::look_at_lh(&(self.position + target), &self.up);
    }

    pub fn project_drop(&self) -> [[f32; 4]; 4] {
        let binding = self.projection.to_homogeneous();
        let pogger = binding.as_ref();

        *pogger
    }

    pub fn view_drop(&self) -> [[f32; 4]; 4] {
        // dbg!(pogger);
        self.view.try_into().unwrap()
    }

    pub fn refresh(&mut self) {
        let mut model = self.rotation.matrix().to_homogeneous();
        model = model.prepend_translation(&self.position);

        self.view = model;
    }

    pub fn reproject(&mut self, screen: Vector2<f32>) {
        self.projection = Perspective3::new(screen.x / screen.y, radians(self.fov), self.near, self.far);
    }
}

fn radians(degrees: f32) -> f32 { degrees * PI / 180.0 }
