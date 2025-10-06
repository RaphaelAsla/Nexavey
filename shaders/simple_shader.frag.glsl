#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragPosWorld;
layout(location = 2) in vec3 fragNormalWorld;
layout(location = 3) in vec2 fragUV;
layout(location = 4) in vec4 fragPosLightSpace;

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

layout(set = 1, binding = 0) uniform sampler2D texture_sampler;
layout(set = 2, binding = 0) uniform sampler2D shadow_map;

layout(push_constant) uniform Push {
    mat4 model_matrix;
    mat4 normal_matrix;
    mat4 light_space_matrix;
    int material_index;
} push;

float textureProj(vec4 shadowCoord, vec2 off)
{
    float shadow = 1.0;
    if (shadowCoord.z > -1.0 && shadowCoord.z < 1.0)
    {
        vec2 sampleCoords = (shadowCoord.xy / shadowCoord.w) * 0.5 + 0.5 + off;
        float dist = texture(shadow_map, sampleCoords).r;
        float currentDepth = (shadowCoord.z / shadowCoord.w) - 0.005;
        if (shadowCoord.w > 0.0 && currentDepth > dist)
        {
            shadow = 0.0;
        }
    }
    return shadow;
}

float filterPCF(vec4 sc)
{
    ivec2 tex_dim = textureSize(shadow_map, 0);
    float scale = 1.5;
    float dx = scale * 1.0 / float(tex_dim.x);
    float dy = scale * 1.0 / float(tex_dim.y);

    float shadow_factor = 0.0;
    int count = 0;
    int range = 4;

    for (int x = -range; x <= range; x++)
    {
        for (int y = -range; y <= range; y++)
        {
            shadow_factor += textureProj(sc, vec2(dx * x, dy * y));
            count++;
        }
    }
    return shadow_factor / count;
}

void main() {
    vec3 diffuse_light = vec3(0.0);
    vec3 specular_light = vec3(0.0);
    vec3 surface_normal = normalize(fragNormalWorld);

    vec3 camera_pos_world = ubo.inverse_view_matrix[3].xyz;
    vec3 view_direction = normalize(camera_pos_world - fragPosWorld);

    vec3 directional_light_pos = vec3(2.0, -2.0, -2.0);
    vec3 directional_light_color = vec3(0.8, 0.8, 0.8);
    float directional_light_intensity = 3.0;

    vec3 direction_to_directional_light = normalize(directional_light_pos - fragPosWorld);
    float directional_cos_angle = max(dot(surface_normal, direction_to_directional_light), 0.0);

    float shadow = filterPCF(fragPosLightSpace);

    diffuse_light += directional_light_color * directional_light_intensity * directional_cos_angle * shadow;

    for (int i = 0; i < ubo.light_count; ++i) {
        vec3 direction_to_light = ubo.point_lights[i].position.xyz - fragPosWorld;

        float attenuation = 1.0 / dot(direction_to_light, direction_to_light);
        direction_to_light = normalize(direction_to_light);

        float cos_angle_incident = max(dot(surface_normal, direction_to_light), 0.0);
        vec3 intensity = ubo.point_lights[i].color.xyz * ubo.point_lights[i].color.w * attenuation;

        diffuse_light += intensity * cos_angle_incident;

        if (push.material_index == 0) {
            continue;
        }

        vec3 half_angle_vector = normalize(view_direction + direction_to_light);
        float blinn_phong_exponent = dot(surface_normal, half_angle_vector);
        blinn_phong_exponent = clamp(blinn_phong_exponent, 0.0, 1.0);
        blinn_phong_exponent = pow(blinn_phong_exponent, 512.0);
        specular_light += intensity * blinn_phong_exponent;
    }

    vec3 texture_color = texture(texture_sampler, fragUV).xyz;

    vec3 ambient_light = ubo.ambient_light_color.xyz * ubo.ambient_light_color.w * 0.5;
    vec3 final_color = (ambient_light + diffuse_light) * texture_color + specular_light;
    outColor = vec4(final_color, 1.0) * vec4(fragColor, 1.0);
}
