pub enum KEBuffType {
    Color_Buffer(String),
    NULL,
}

pub struct Buffer {
    pub name: String,
    pub buffer: KEBuffType
}