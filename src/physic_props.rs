use std::collections::HashMap;

use nalgebra::{Vector3, Vector2, Rotation3};
use rapier3d::prelude::*;

use crate::{props::{Prop, phytype}, ke_units::Vec3, models::Model};

pub enum CopyWhat {
    All,
    OnlyRotation,
    OnlyPosition,
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
    last_ID: i32
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
            last_ID: 0
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
        collider.set_rotation(Rotation3::new(prop.rotation).into());
        self.colliders.insert(collider);


        // I don't think static collisions would need their prop to know its phys_id
        // too lazy to fix up rn
        // let id = self.phys_handles.len() as i32;
        // let rb_id = self.ridgid_world.insert(rigid_body);
        // prop.phys_id = id;
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

    pub fn create_phy(&mut self, prop: &mut Prop, modelz: &HashMap<i32, Model>) {
        if prop.phys_type == phytype::Collider {
            match prop.phys_shape {
                crate::props::physhape::Box => {
                    self.create_collider_block(prop);
                },
                crate::props::physhape::Ball => todo!(),
                crate::props::physhape::Model => {
                    // let vertacies: Vec<Vector3<f32>>  = vec![];
                    // for po in  {
                        
                    // }
                    
                    // let _ = ColliderBuilder::trimesh(vertacies, );
                    todo!()
                },
                crate::props::physhape::NULL => {}, // don't do anything :3
            }
        } else if prop.phys_type == phytype::Dynamic {
            match prop.phys_shape {
                crate::props::physhape::Box => {
                    self.create_dynamic_box(prop);
                },
                crate::props::physhape::Ball => todo!(),
                crate::props::physhape::Model => todo!(),
                crate::props::physhape::NULL => {}, // don't do anything :3
            }
        }
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
                rb.set_rotation(Rotation3::new(prop.rotation).into(), false);
            }
            CopyWhat::OnlyRotation => {
                rb.set_rotation(Rotation3::new(prop.rotation).into(), false);
            }
            CopyWhat::OnlyPosition => {
                rb.set_position(prop.position.into(), false);
            }
        }
    }

    // for copying position & rotation of phys prop to it's counter part
    pub fn _sync_prop(&mut self, prop: &mut Prop, ctype: CopyWhat) {
        if prop.phys_id == -1 {
            return;
        };

        let rb_id = self.phys_handles.get(&prop.phys_id).unwrap();
        let rb = self.ridgid_world.get(*rb_id).unwrap();

        match ctype {
            CopyWhat::All => {
                prop.position = *rb.translation();
                let vecpog = rb.rotation().euler_angles();
                prop.set_rotation(Vector3::new(vecpog.0, vecpog.1, vecpog.2));
            }
            CopyWhat::OnlyRotation => {
                let vecpog = rb.rotation().euler_angles();
                prop.set_rotation(Vector3::new(vecpog.0, vecpog.1, vecpog.2));
            }
            CopyWhat::OnlyPosition => {
                prop.position = *rb.translation();
            }
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
}
