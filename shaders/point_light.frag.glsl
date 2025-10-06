#version 450

layout(location = 0) in vec2 fragOffset;

layout(location = 0) out vec4 outColor;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection_matrix;
    mat4 view_matrix;
    mat4 inverse_view_matrix;
    vec4 ambient_light_color;
    PointLight point_lights[10];
    int light_count;
} ubo;

layout(push_constant) uniform Push {
    vec4 position;
    vec4 color;
    float radius;
} push;

const float M_PI = 3.14159265358979323846;

void main() {
    float dist = sqrt(dot(fragOffset, fragOffset));
    if (dist >= 1.0) {
        discard;
    }
    outColor = vec4(push.color.xyz, mix(0.0, 1.0, 1.0 - dist));
}
