#version 460

layout(location = 0) in vec3 p_frag_color;
layout(location = 0) out vec4 out_color;

void main() {
    out_color = vec4(p_frag_color, 1.0);
}
