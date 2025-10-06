#version 450

layout(location = 0) in vec3 position;

layout(push_constant) uniform Push {
    mat4 light_space_model_matrix;
} push;

void main() {
    gl_Position = push.light_space_model_matrix * vec4(position, 1.0);
}