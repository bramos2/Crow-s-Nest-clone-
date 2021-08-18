#version 450 core

// Inputs:
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inNormal;

layout(set = 0, binding = 0) uniform Ubo_Global {
    mat4 projection_view;
} ubo_global;

layout(set = 3, binding = 0) uniform Ubo_Object {
    mat4 model;
} ubo_object;

// Outputs:
layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 outNormal;

out gl_PerVertex {
    vec4 gl_Position;
};

// Behavior:
void main() {
    outColor = inColor;
    outNormal = inNormal;

    gl_Position = ubo_global.projection_view *
        ubo_object.model * vec4(inPos, 4);
}
