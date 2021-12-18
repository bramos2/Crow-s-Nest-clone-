#pragma once

#include "math_types.hpp"
//#include "frustum_culling.h"

// Interface to the debug renderer
namespace crow
{
	namespace debug_renderer
	{

		void add_line(float3e point_a, float3e point_b, float4e color_a, float4e color_b);

		inline void add_line(float3e p, float3e q, float4e color) { add_line(p, q, color, color); }

		void clear_lines();

		const colored_vertex* get_line_verts();

		size_t get_line_vert_count();

		size_t get_line_vert_capacity();

		void add_texture(float3e pos, float size, float transparency, void* texture);

		void clear_textures();

		const textured_vertex* get_tex_verts();

		size_t get_tex_vert_count();

		size_t get_tex_vert_capacity();
	}
}