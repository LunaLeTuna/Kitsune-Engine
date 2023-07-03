// https://github.com/glium/glium/pull/1987
// why they didn't fix it to actuall be in glium idk
// but this is so pog I had to yoink it :3
// thank you DoeringChristian

use std::{collections::HashMap, string};

use glium::uniforms::{AsUniformValue, UniformValue, Uniforms};

/// Stores Uniforms dynamicly in a HashMap.
#[derive(Clone)]
pub struct DynamicUniforms<'a> {
    map: HashMap<String, UniformValue<'a>>,
}

#[allow(clippy::new_without_default)]
impl<'a> DynamicUniforms<'a> {
    /// Creates new DynamicUniforms
    pub fn new() -> Self { Self { map: HashMap::new() } }

    /// Add a value to the DynamicUniforms
    #[inline]
    pub fn add(&mut self, key: String, value: &'a dyn AsUniformValue) {
        self.map.insert(key, value.as_uniform_value());
    }
}

impl Uniforms for DynamicUniforms<'_> {
    fn visit_values<'a, F: FnMut(&str, UniformValue<'a>)>(&'a self, mut output: F) {
        for (key, value) in self.map.iter() {
            output(key, *value);
        }
    }
}

#[macro_export]
macro_rules! dynamic_uniform{
    () => {
        $crate::uniforms::DynamicUniforms::new()
    };

    ($($field:ident: $value:expr), *,) => {
        {
            let mut tmp = DynamicUniforms::new();
            $(
                tmp.add(stringify!($field), $value);
            )*
            tmp
        }
    };
}
