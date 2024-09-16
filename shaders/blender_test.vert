void main() {
    gl_Position = viewProjectionMatrix * modelMatrix * vec4(position, 1.0);
}