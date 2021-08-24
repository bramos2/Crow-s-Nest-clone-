#version 460 core

layout (location = 0) out vec2 outUV;

void main() {
	// fullscreen triangle without having to use vertex/index buffers
	// this is clock-wise, culling must be off or set to cull CCW
	// https://www.saschawillems.de/blog/2016/08/13/vulkan-tutorial-on-rendering-a-fullscreen-quad-without-buffers/
	outUV = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
	gl_Position = vec4(outUV * 2.0f - 1.0f, 0.0f, 1.0f);
}
