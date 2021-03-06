#include "../hpp/debug_renderer.hpp"
#include <array>

// Anonymous namespace
namespace
{
	// Declarations in an anonymous namespace are global BUT only have internal linkage.
	// In other words, these variables are global but are only visible in this source file.

	// Maximum number of debug lines at one time (i.e: Capacity)
	constexpr size_t MAX_LINE_VERTS = 4096; 
	constexpr size_t MAX_TEX_VERTS = 4096; 

	// CPU-side buffer of debug-line verts
	// Copied to the GPU and reset every frame.
	size_t line_vert_count = 0;
	std::array<crow::colored_vertex, MAX_LINE_VERTS> line_verts;

	size_t tex_vert_count = 0;
	std::array<crow::textured_vertex, MAX_TEX_VERTS> tex_verts;
}

namespace crow
{
	namespace debug_renderer
	{
		void add_line(float3e point_a, float3e point_b, float4e color_a, float4e color_b)
		{
			// Add points to debug_verts, increments debug_vert_count
			if (line_vert_count < MAX_LINE_VERTS)
			{
				line_verts[line_vert_count].pos = point_a;
				line_verts[line_vert_count].color = color_a;
				line_vert_count++;
				line_verts[line_vert_count].pos = point_b;
				line_verts[line_vert_count].color = color_b;
				line_vert_count++;
			}
		}

		void clear_lines()
		{
			// Resets debug_vert_count
			line_vert_count = 0;
		}

		const colored_vertex* get_line_verts()
		{ 
			// Does just what it says in the name
			return line_verts.data();
		}

		size_t get_line_vert_count() 
		{ 
			// Does just what it says in the name
			return line_vert_count;
		}

		size_t get_line_vert_capacity()
		{
			// Does just what it says in the name
			return MAX_LINE_VERTS;
		}


		// textured particle processing starts here //

		void add_texture(float3e pos, float size, float transparency, void* texture) {
			if (tex_vert_count < MAX_TEX_VERTS) {
				tex_verts[tex_vert_count].pos = pos;
				tex_verts[tex_vert_count].size = size;
				tex_verts[tex_vert_count].transparency = transparency;
				tex_verts[tex_vert_count].texture = texture;
				tex_vert_count++;
			}
			
		}

		void clear_textures()
		{
			// Resets debug_vert_count
			tex_vert_count = 0;
		}

		const textured_vertex* get_tex_verts()
		{ 
			// Does just what it says in the name
			return tex_verts.data();
		}

		size_t get_tex_vert_count() 
		{ 
			// Does just what it says in the name
			return tex_vert_count;
		}

		size_t get_tex_vert_capacity()
		{
			// Does just what it says in the name
			return MAX_TEX_VERTS;
		}
	}
}