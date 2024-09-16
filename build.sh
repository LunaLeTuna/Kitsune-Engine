#! /bin/sh

cargo build --target x86_64-unkown-linux-gnu --release
cargo build --target x86_64-pc-windows-gnu --release