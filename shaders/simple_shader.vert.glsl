#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPosWorld;
layout(location = 2) out vec3 fragNormalWorld;
layout(location = 3) out vec2 fragUV;
layout(location = 4) out vec4 fragPosLightSpace;

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
    mat4 model_matrix;
    mat4 normal_matrix;
    mat4 light_space_matrix;
    int material_index;
} push;

void main() {
    vec4 position_in_world = push.model_matrix * vec4(position, 1.0);
    gl_Position = ubo.projection_matrix * ubo.view_matrix * position_in_world;

    fragNormalWorld = normalize(mat3(push.normal_matrix) * normal);
    fragPosWorld = position_in_world.xyz;
    fragColor = color;
    fragUV = uv;
    fragPosLightSpace = push.light_space_matrix * position_in_world;
}
