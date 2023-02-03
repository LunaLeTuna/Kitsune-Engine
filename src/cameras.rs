    use std::f32::consts::PI;

    use nalgebra::{Vector3, Vector2, Matrix4, Rotation, Rotation3, Matrix, Perspective3};

pub struct camera{
        pub name: String,
        pub projection: Perspective3<f32>,
        pub view: Matrix4<f32>,
        pub position: Vector3<f32>,
        pub rotation: Rotation<f32,3>,
        pub up: Vector3<f32>,

        pub near: f32,
        pub far: f32,
        pub fov: f32,
        pub aspect: f32
    }
    impl camera {
        pub fn look_at(&mut self, target: Vector3<f32>) {
            let meow:Vector3<f32> = Vector3::new(self.position.x+target.x,self.position.y+target.y,self.position.z+target.z);
            self.rotation = nalgebra::Rotation3::look_at_lh(&meow, &self.up);
        }

        pub fn project_drop(&self) -> [[f32; 4]; 4] {
            let binding = self.projection.to_homogeneous();
            let pogger = binding.as_ref();
            *pogger
        }

        pub fn view_drop(&self) -> [[f32; 4]; 4] {
            let pogger = self.view.try_into().unwrap();
            // dbg!(pogger);
            pogger
        }

        pub fn refresh(&mut self) {
            let mut model = Matrix4::new_scaling(1.0);
            model = model.append_translation(&self.position);
            self.view = model;
        }

        pub fn reproject(&mut self, screen: Vector2<f32>) {
            self.projection = Perspective3::new(screen.x/screen.y,radians(self.fov), self.near, self.far);
        }
    }

    fn radians(degrees: f32) -> f32 {
        degrees*PI/180.0
    }

    pub fn craft(screen: Vector2<f32>) -> camera {
        let mut pog = camera { name: "meow".to_owned(),
            up: Vector3::y(), 
            far: 1024.0, 
            near: 0.1, 
            fov: 60.0,
            projection: Perspective3::new(screen.y/screen.x,radians(60.0), 0.1, 1024.0),
            position: Vector3::new(0.0, 0.0, 0.0),
            rotation: Rotation3::new(Vector3::new(0.0, 0.0, 0.0)),
            aspect: screen.x/screen.y,
            view: Matrix4::new_scaling(1.0),
        };
        pog.reproject(screen);
        pog
    }