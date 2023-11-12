use serde_json::Value;

pub struct Sockreq {
    pub name: String,
    pub data: Value
}