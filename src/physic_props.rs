use std::collections::HashMap;

use rapier3d::prelude::{RigidBodyBuilder, RigidBodyHandle, RigidBodySet, RigidBodyType};

use crate::props::Prop;

pub enum CopyWhat {
    All,
    OnlyRotation,
    OnlyPosition,
}

pub struct PhysWorld {
    ridgid_world: RigidBodySet,
    phys_handles: HashMap<i32, RigidBodyHandle>,
}

impl PhysWorld {
    pub fn init_phys_world(_name: String) -> PhysWorld {
        PhysWorld {
            ridgid_world: RigidBodySet::new(),
            phys_handles: HashMap::new(),
        }
    }

    pub fn create_static_rigidbody() {
        // fixed
        todo!()
    }

    pub fn create_dynamic_rigidbody(&mut self, prop: &mut Prop) {
        let mut rigid_body = RigidBodyBuilder::new(RigidBodyType::Dynamic)
        .translation(prop.position)
        //.rotation(prop.rotation.into()) //gotta figure out a effetion waty to do this
        .can_sleep(true)
        .ccd_enabled(false) //ponder this later
        .build();

        rigid_body.set_rotation(prop.rotation.into(), false);

        let id = self.phys_handles.len() as i32;

        let rb_id = self.ridgid_world.insert(rigid_body);
        self.phys_handles.insert(id, rb_id);
        prop.phys_id = id;
    }

    // for copying properties of prop to it's phys counter part
    fn _sync_phys_prop(&mut self, prop: Prop, ctype: CopyWhat) {
        if prop.phys_id == -1 {
            return;
        };

        let rb_id = self.phys_handles.get_mut(&prop.phys_id).unwrap();
        let rb = self.ridgid_world.get_mut(*rb_id).unwrap();

        match ctype {
            CopyWhat::All => {
                rb.set_position(prop.position.into(), false);
                rb.set_rotation(prop.rotation.into(), false);
            }
            CopyWhat::OnlyRotation => {
                rb.set_rotation(prop.rotation.into(), false);
            }
            CopyWhat::OnlyPosition => {
                rb.set_position(prop.position.into(), false);
            }
        }
    }

    // for copying position & rotation of phys prop to it's counter part
    fn _sync_prop(&mut self, prop: &mut Prop, ctype: CopyWhat) {
        if prop.phys_id == -1 {
            return;
        };

        let rb_id = self.phys_handles.get_mut(&prop.phys_id).unwrap();
        let rb = self.ridgid_world.get_mut(*rb_id).unwrap();

        match ctype {
            CopyWhat::All => {
                prop.position = *rb.translation();
                prop.rotation = rb.rotation().to_rotation_matrix();
            }
            CopyWhat::OnlyRotation => {
                prop.rotation = rb.rotation().to_rotation_matrix();
            }
            CopyWhat::OnlyPosition => {
                prop.position = *rb.translation();
            }
        }
    }
}
