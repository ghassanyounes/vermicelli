#version 460

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 color;

// No correlation between in/out locations


layout(push_constant) uniform Push {
    mat2 transform;
    vec2 offset;
    vec3 color;
} push;
void main() {
    // Remember that order matters when it comes to matrix multiplication!
    gl_Position = vec4(push.transform * position + push.offset, 0.0, 1.0);
}