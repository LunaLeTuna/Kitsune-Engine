use std::collections::HashMap;

use nalgebra::{vector, Vector3};
use rapier3d::prelude::{ColliderSet, RigidBodySet, RigidBodyBuilder, RigidBodyType, RigidBodyHandle};

use crate::props::Prop;

enum copy_what {
    all,
    only_rotation,
    only_position,
}

pub struct PhysWorld {
    ridgid_world: RigidBodySet,
    phys_handles: HashMap<i32, RigidBodyHandle>
}

impl PhysWorld {
    pub fn init_phys_world(name: String) -> PhysWorld{
        PhysWorld {
            ridgid_world: RigidBodySet::new(),
            phys_handles: HashMap::new()
        }
    }

    pub fn create_static_rigidbody() { //fixed
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

        let ID = self.phys_handles.len() as i32;

        let _handle = self.ridgid_world.insert(rigid_body);
        self.phys_handles.insert(ID, _handle);
        prop.phys_ID = ID;
    }

    //for copying properties of prop to it's phys counter part
    fn sync_phys_prop(&mut self, prop: Prop, ctype: copy_what){
        if prop.phys_ID == -1 {return};
        let id = self.phys_handles.get_mut(&prop.phys_ID).unwrap();
        let p = self.ridgid_world.get_mut(*id).unwrap();
        match ctype {
            copy_what::all => {
                p.set_position(prop.position.into(), false);
                p.set_rotation(prop.rotation.into(), false);
            },
            copy_what::only_rotation => {
                p.set_rotation(prop.rotation.into(), false);
            },
            copy_what::only_position => {
                p.set_position(prop.position.into(), false);
            },
        }
    }

    //for copying position & rotation of phys prop to it's counter part
    fn sync_prop(&mut self, prop: &mut Prop, ctype: copy_what){
        if prop.phys_ID == -1 {return};
        let id = self.phys_handles.get_mut(&prop.phys_ID).unwrap();
        let p = self.ridgid_world.get_mut(*id).unwrap();
        match ctype {
            copy_what::all => {
                prop.position = *p.translation();
                prop.rotation = p.rotation().to_rotation_matrix();
            },
            copy_what::only_rotation => {
                prop.rotation = p.rotation().to_rotation_matrix();
            },
            copy_what::only_position => {
                prop.position = *p.translation();
            },
        }
    }
}