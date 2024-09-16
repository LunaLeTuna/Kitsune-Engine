use std::collections::HashMap;

use nalgebra::{Const, OPoint, Point3, Rotation3, Unit, Vector2, Vector3};
use rapier3d::prelude::*;

use crate::{props::{Prop, phytype}, ke_units::Vec3, models::Model};

pub enum CopyWhat {
    All,
    OnlyRotation,
    OnlyPosition,
    OnlyVelocity
}

pub struct PhysWorld {
    gravity: Vector3<f32>,
    integration_parameters: IntegrationParameters,
    physics_pipeline: PhysicsPipeline,
    island_manager: IslandManager,
    broad_phase: BroadPhase,
    narrow_phase: NarrowPhase,
    impulse_joint_set: ImpulseJointSet,
    multibody_joint_set: MultibodyJointSet,
    ccd_solver: CCDSolver,
    ridgid_world: RigidBodySet,
    colliders: ColliderSet,
    phys_handles: HashMap<i32, RigidBodyHandle>,
    last_ID: i32,
    custom_collider_mesh: HashMap<i32,ColliderBuilder>
}

pub struct RaycastResult {
    hit: bool,
    point: Vector3<f32>,
}

impl PhysWorld {
    pub fn init_phys_world() -> PhysWorld {


        PhysWorld {
            gravity: Vector3::new(0.0,-1.0,0.0),
            ridgid_world: RigidBodySet::new(),
            colliders: ColliderSet::new(),
            phys_handles: HashMap::new(),
            integration_parameters: IntegrationParameters::default(),
            physics_pipeline: PhysicsPipeline::new(),
            island_manager: IslandManager::new(),
            broad_phase: BroadPhase::new(),
            narrow_phase: NarrowPhase::new(),
            impulse_joint_set: ImpulseJointSet::new(),
            multibody_joint_set: MultibodyJointSet::new(),
            ccd_solver: CCDSolver::new(),
            last_ID: 0,
            custom_collider_mesh: HashMap::new(),
        }
    }

    pub fn step(&mut self){
        self.physics_pipeline.step(
            &self.gravity,
            &self.integration_parameters,
            &mut self.island_manager,
            &mut self.broad_phase,
            &mut self.narrow_phase,
            &mut self.ridgid_world,
            &mut self.colliders,
            &mut self.impulse_joint_set,
            &mut self.multibody_joint_set,
            &mut self.ccd_solver,
            None,
            &(),
            &(),
          );
    }

    pub fn create_collider_block(&mut self, prop: &mut Prop) {
        let mut collider = ColliderBuilder::cuboid(prop.scale.x, prop.scale.y, prop.scale.z)
        .translation(prop.position)
        .build();


        let n = [
            Unit::new_unchecked(Vector3::new(-1.0, 0.0, 0.0)),
            Unit::new_unchecked(Vector3::new(0.0, -1.0, 0.0)),
            Unit::new_unchecked(Vector3::new(0.0, 0.0, 1.0)),
        ];
        let r1 = Rotation3::from_axis_angle(&n[2], prop.rotation.z);
        let r2 = Rotation3::from_axis_angle(&n[1], prop.rotation.y);
        let r3 = Rotation3::from_axis_angle(&n[0], prop.rotation.x);
        let d = r2 * r3 * r1;


        collider.set_rotation(d.into());
        self.colliders.insert(collider);


        // I don't think static collisions would need their prop to know its phys_id
        // too lazy to fix up rn
        let id = self.phys_handles.len() as i32;
        prop.phys_id = id-1;
    }

    pub fn create_collider_capsule(&mut self, prop: &mut Prop) {
        let mut collider = ColliderBuilder::capsule_y(prop.scale.y, prop.scale.x)
        .translation(prop.position)
        .build();


        let n = [
            Unit::new_unchecked(Vector3::new(-1.0, 0.0, 0.0)),
            Unit::new_unchecked(Vector3::new(0.0, -1.0, 0.0)),
            Unit::new_unchecked(Vector3::new(0.0, 0.0, 1.0)),
        ];
        let r1 = Rotation3::from_axis_angle(&n[2], prop.rotation.z);
        let r2 = Rotation3::from_axis_angle(&n[1], prop.rotation.y);
        let r3 = Rotation3::from_axis_angle(&n[0], prop.rotation.x);
        let d = r2 * r3 * r1;


        collider.set_rotation(d.into());
        self.colliders.insert(collider);


        // I don't think static collisions would need their prop to know its phys_id
        // too lazy to fix up rn
        // let id = self.phys_handles.len() as i32;
        // let rb_id = self.ridgid_world.insert(rigid_body);
        // prop.phys_id = id;
    }

    pub fn create_collider_model(&mut self, prop: &mut Prop, model: &mut Model) {

        let mut collider = {
            match self.custom_collider_mesh.get(&prop.model) {
                Some(a) => a, //return if we do have it
                None => { //make it if we an't got it
                    let mut vertacies = vec![];
                    let mut indiceies: Vec<[u32; 3]> = vec![];
                    for vi in &model.raw_data.0 {
                        vertacies.push(Point3::new(vi[0],vi[1],vi[2]));
                    }
                    for vi in model.raw_data.1.chunks_exact(3) {
                        indiceies.push([vi[0].try_into().unwrap(),vi[1].try_into().unwrap(),vi[2].try_into().unwrap()]);
                    }
                    let collider = ColliderBuilder::trimesh(vertacies, indiceies);
                                
                    let newid = self.custom_collider_mesh.len() as i32;
                    self.custom_collider_mesh.insert(prop.model, collider);
                    self.custom_collider_mesh.get(&prop.model).unwrap()
                },
            }
        }.clone()
        .translation(prop.position)
        .build();

        //I only pray this function works
        //may what ever god(s), if they exist, let this work
        //I'm hungry... I want pizza- but chef boi rd lesben lesagna will do
        //maybe once I eat, this code that I have summoned from a pintagon will make since to me
        //as the only indecation that it probably works is that rust analyzer isn't yelling at me


        let n = [
            Unit::new_unchecked(Vector3::new(-1.0, 0.0, 0.0)),
            Unit::new_unchecked(Vector3::new(0.0, -1.0, 0.0)),
            Unit::new_unchecked(Vector3::new(0.0, 0.0, 1.0)),
        ];
        let r1 = Rotation3::from_axis_angle(&n[2], prop.rotation.z);
        let r2 = Rotation3::from_axis_angle(&n[1], prop.rotation.y);
        let r3 = Rotation3::from_axis_angle(&n[0], prop.rotation.x);
        let d = r2 * r3 * r1;


        collider.set_rotation(d.into());
        self.colliders.insert(collider);


        // I don't think static collisions would need their prop to know its phys_id
        // too lazy to fix up rn
        let id = self.phys_handles.len() as i32;
        prop.phys_id = id-1;
    }

    pub fn create_static_rigidbody() {
        // fixed
        todo!()
    }

    pub fn create_dynamic_box(&mut self, prop: &mut Prop) {
        let collider = ColliderBuilder::cuboid(prop.scale.x, prop.scale.y, prop.scale.z);

        let mut rigid_body = RigidBodyBuilder::new(RigidBodyType::Dynamic)
        .translation(prop.position)
        .rotation(prop.rotation) //gotta figure out a effetion waty to do this
        .can_sleep(true) //cube can a bit eepy
        .ccd_enabled(false) //ponder this later
        .build();

        //rigid_body.set_rotation(prop.rotation.into(), false);

        if(prop.phys_type == phytype::DynamicCollider){
            rigid_body.set_locked_axes(LockedAxes::all(), false);
        }


        self.last_ID += 1;

        let rb_id = self.ridgid_world.insert(rigid_body);

        self.phys_handles.insert(self.last_ID, rb_id);

        self.colliders.insert_with_parent(collider, rb_id, &mut self.ridgid_world);

        prop.phys_id = self.last_ID;
    }

    pub fn create_dynamic_ball(&mut self, prop: &mut Prop) {
        let collider = ColliderBuilder::ball(prop.scale.y);

        let mut rigid_body = RigidBodyBuilder::new(RigidBodyType::Dynamic)
        .translation(prop.position)
        .rotation(prop.rotation) //gotta figure out a effetion waty to do this
        .can_sleep(true) //cube can a bit eepy
        .ccd_enabled(false) //ponder this later
        .build();

        //rigid_body.set_rotation(prop.rotation.into(), false);

        if(prop.phys_type == phytype::DynamicCollider){
            rigid_body.set_locked_axes(LockedAxes::all(), false);
        }


        self.last_ID += 1;

        let rb_id = self.ridgid_world.insert(rigid_body);

        self.phys_handles.insert(self.last_ID, rb_id);

        self.colliders.insert_with_parent(collider, rb_id, &mut self.ridgid_world);

        prop.phys_id = self.last_ID;
    }

    pub fn create_dynamic_capsule(&mut self, prop: &mut Prop) {
        let collider = ColliderBuilder::capsule_y(prop.scale.y, prop.scale.x);

        let mut rigid_body = RigidBodyBuilder::new(RigidBodyType::Dynamic)
        .translation(prop.position)
        .rotation(prop.rotation) //gotta figure out a effetion waty to do this
        .can_sleep(true) //cube can a bit eepy
        .ccd_enabled(false) //ponder this later
        .build();

        //rigid_body.set_rotation(prop.rotation.into(), false);


        self.last_ID += 1;

        let rb_id = self.ridgid_world.insert(rigid_body);

        self.phys_handles.insert(self.last_ID, rb_id);

        self.colliders.insert_with_parent(collider, rb_id, &mut self.ridgid_world);

        prop.phys_id = self.last_ID;
    }

    pub fn remove_phy(&mut self, prop: &mut Prop) {
        if prop.phys_type == phytype::Collider {
            todo!()
        } else if prop.phys_type == phytype::Dynamic {
            let rb_id = self.phys_handles.get(&prop.phys_id).unwrap();
            self.ridgid_world.remove(*rb_id, &mut self.island_manager, &mut self.colliders, &mut self.impulse_joint_set, &mut self.multibody_joint_set, true);
            self.phys_handles.remove(&prop.phys_id);
            prop.phys_id = -1;
        }
    }

    pub fn create_phy(&mut self, prop: &mut Prop, modelz: &mut HashMap<i32, Model>) {
        if prop.phys_type == phytype::Collider {
            match prop.phys_shape {
                crate::props::physhape::Box => {
                    self.create_collider_block(prop);
                },
                crate::props::physhape::Capsule => {
                    self.create_collider_capsule(prop);
                },
                crate::props::physhape::Ball => todo!(),
                crate::props::physhape::Model => {
                    let modelz = modelz.get_mut(&prop.model).unwrap();
                    self.create_collider_model(prop, modelz);
                },
                crate::props::physhape::NULL => {}, // don't do anything :3
            }
        } else if prop.phys_type == phytype::Dynamic || prop.phys_type == phytype::DynamicCollider {
            match prop.phys_shape {
                crate::props::physhape::Box => {
                    self.create_dynamic_box(prop);
                },
                crate::props::physhape::Ball => {
                    self.create_dynamic_ball(prop)
                },
                crate::props::physhape::Capsule => {
                    self.create_dynamic_capsule(prop);
                },
                crate::props::physhape::Model => todo!(),
                crate::props::physhape::NULL => {}, // don't do anything :3
            }
        }
    }

    pub fn delete_prop(&mut self, prop: &Prop, ctype: CopyWhat) {
        if prop.phys_id == -1 {
            return;
        };

        let rb_id = self.phys_handles.get_mut(&prop.phys_id).unwrap();
        let rb = self.ridgid_world.get_mut(*rb_id).unwrap();

        
    }

    // for copying properties of prop to it's phys counter part
    pub fn _sync_phys_prop(&mut self, prop: &Prop, ctype: CopyWhat) {
        if prop.phys_id == -1 {
            return;
        };

        let rb_id = self.phys_handles.get_mut(&prop.phys_id).unwrap();
        let rb = self.ridgid_world.get_mut(*rb_id).unwrap();

        match ctype {
            CopyWhat::All => {
                rb.set_position(prop.position.into(), false);

                let n = [
                    Unit::new_unchecked(Vector3::new(-1.0, 0.0, 0.0)),
                    Unit::new_unchecked(Vector3::new(0.0, -1.0, 0.0)),
                    Unit::new_unchecked(Vector3::new(0.0, 0.0, 1.0)),
                ];
                let r1 = Rotation3::from_axis_angle(&n[2], prop.rotation.z);
                let r2 = Rotation3::from_axis_angle(&n[1], prop.rotation.y);
                let r3 = Rotation3::from_axis_angle(&n[0], prop.rotation.x);
                let d = r2 * r3 * r1;
                rb.set_rotation(d.into(), false);
            }
            CopyWhat::OnlyRotation => {
                rb.set_rotation(Rotation3::new(prop.rotation).into(), false);
            }
            CopyWhat::OnlyPosition => {
                rb.set_position(prop.position.into(), false);
            },
            CopyWhat::OnlyVelocity => {
                rb.set_linvel(prop.velocity.into(), false);
            }
        }
    }

    // for copying position & rotation of phys prop to it's counter part
    pub fn _sync_prop(&mut self, prop: &mut Prop, ctype: CopyWhat) {
        if prop.phys_id == -1 || self.phys_handles.len() == 0 {
            return;
        };

        let rb_id = self.phys_handles.get(&prop.phys_id).unwrap();
        let rb = self.ridgid_world.get(*rb_id).unwrap();

        match ctype {
            CopyWhat::All => {
                prop.position = *rb.translation();
                let vecpog = rb.rotation().euler_angles();
                prop.set_rotation(Vector3::new(vecpog.0, vecpog.1, vecpog.2));
                prop.velocity = *rb.linvel();
            }
            CopyWhat::OnlyRotation => {
                let vecpog = rb.rotation().euler_angles();
                prop.set_rotation(Vector3::new(vecpog.0, vecpog.1, vecpog.2));
            }
            CopyWhat::OnlyPosition => {
                prop.position = *rb.translation();
            }
            CopyWhat::OnlyVelocity => {
                prop.velocity = *rb.linvel();
            },
        }
    }

    pub fn lock_rotations(&mut self, prop: &Prop) {
        if prop.phys_id == -1 {
            return;
        };

        let rb_id = self.phys_handles.get(&prop.phys_id).unwrap();
        let rb = self.ridgid_world.get_mut(*rb_id).unwrap();
        rb.lock_rotations(true,true);
    }

    pub fn apply_force(&mut self, prop: &Prop, push: Vector3<f32>) {
        if prop.phys_id == -1 {
            return;
        };

        let rb_id = self.phys_handles.get(&prop.phys_id).unwrap();
        let rb = self.ridgid_world.get_mut(*rb_id).unwrap();
        rb.set_linvel(push.into(), true);
    }

    pub fn apply_force_xz(&mut self, prop: &Prop, push: Vector2<f32>) {
        if prop.phys_id == -1 {
            return;
        };

        let rb_id = self.phys_handles.get(&prop.phys_id).unwrap();
        let rb = self.ridgid_world.get_mut(*rb_id).unwrap();
        rb.set_linvel(Vector3::new(push.x, rb.linvel().y, push.y).into(), true);
    }

    pub fn raycast(){
        
    }
}
