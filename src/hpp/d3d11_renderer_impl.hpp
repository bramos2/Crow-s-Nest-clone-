#pragma once

#define WIN32_LEAN_AND_MEAN
//#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11_2.h>
#include <DirectXMath.h>
#include <bitset>
#include <algorithm>
#include <vector>
//#include <random>
//#include <iostream>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DXGI.lib")

//#include "renderer.hpp"
#include <imgui_impl_dx11.h>
#include "math_types.hpp"
#include "view.hpp"
#include "mesh.hpp"
#include "entities.hpp"
#include "../shader/mvp.hlsli"
#include "../shader/mcb.hlsli"
#include "../shader/scb.hlsli"
//#include "frustum_culling.hpp"
//#include "pools.hpp"


//#include "bvh.h"

//#include "FbxLibrary.h"

// NOTE: This header file must *ONLY* be included by renderer.cpp

namespace
{
	template<typename T>
	void safe_release(T* t)
	{
		if (t)
			t->Release();
	}
	float nd = 2.0f;
	float fd = 10.0f;
	float fov = 3.1415926f / 4.0f;
	//crow::view_t vt;
	bool b_on = false;

	int keyframeIndex = 0;
	float kfi = 0;
	double aTime = 0;
}

namespace crow
{
	struct VIEWPORT {
		enum { DEFAULT = 0, COUNT };
	};

	struct CONSTANT_BUFFER {
		enum { MVP = 0, ANIM_MESH, STATIC_MESH, COUNT };
	};

	struct VERTEX_SHADER {
		enum { BUFFERLESS_CUBE = 0, COLORED_VERTEX, ANIM_MESH, STATIC_MESH, COUNT };
	};

	struct PIXEL_SHADER {
		enum { BUFFERLESS_CUBE = 0, COLORED_VERTEX, ANIM_MESH, STATIC_MESH, COUNT };
	};

	struct VIEW_RENDER_TARGET {
		enum { DEFAULT = 0, COUNT };
	};

	struct INDEX_BUFFER {
		enum { DEFAULT = 0, ANIM_MESH, COUNT };
	};

	struct INPUT_LAYOUT {
		enum { COLORED_VERTEX = 0, ANIM_MESH, STATIC_MESH, COUNT };
	};

	struct STATE_RASTERIZER {
		enum { DEFAULT = 0, COUNT };
	};

	struct VIEW_DEPTH_STENCIL {
		enum { DEFAULT = 0, COUNT };
	};

	struct STATE_DEPTH_STENCIL {
		enum { DEFAULT = 0, COUNT };
	};

	struct VERTEX_BUFFER {
		enum { COLORED_VERTEX = 0, TERRAIN_VERTEX, ANIM_MESH, COUNT };
	};

	struct SUBRESOURCE_VIEW {
		enum { DIFFUSE = 0, EMISSIVE, SPECULAR, COUNT };
	};

	struct STATE_SAMPLER {
		enum { DEFAULT = 0, COUNT };
	};

	using namespace DirectX;

	/*struct Particle
	{
		crow::float3e pos;
		crow::float3e prev_pos;
		crow::float4e color;
		crow::float3e vel;
		float life_span;
	};*/

	//template <typename T, int16_t N>
	/*struct Emitter_sp
	{
		end::float3e pos;
		end::float4e color;
		sorted_pool_t<Particle, 1024> pool;
	};*/

	//template <typename T>
	//struct Emitter_fp
	//{
	//	end::float3e pos;
	//	end::float4e color;
	//	sorted_pool_t<T, 256> pool; // this should be a SORTED pool of indices (ints)
	//};

	/*struct Triangle
	{
		float3e verts[3];
		float3e centroid;
		aabb_t box;
	};*/

	struct impl_t
	{
		// platform/api specific members, functions, etc.
		// Device, swapchain, resource views, states, etc. can be members here
		HWND hwnd;

		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* context = nullptr;
		IDXGISwapChain* swapchain = nullptr;

		ID3D11RenderTargetView* render_target[VIEW_RENDER_TARGET::COUNT]{};

		ID3D11DepthStencilView* depthStencilView[VIEW_DEPTH_STENCIL::COUNT]{};

		ID3D11DepthStencilState* depthStencilState[STATE_DEPTH_STENCIL::COUNT]{};

		ID3D11RasterizerState* rasterState[STATE_RASTERIZER::COUNT]{};

		ID3D11Buffer* vertex_buffer[VERTEX_BUFFER::COUNT]{};

		ID3D11Buffer* index_buffer[INDEX_BUFFER::COUNT]{};

		ID3D11InputLayout* input_layout[INPUT_LAYOUT::COUNT]{};

		ID3D11VertexShader* vertex_shader[VERTEX_SHADER::COUNT]{};

		ID3D11PixelShader* pixel_shader[PIXEL_SHADER::COUNT]{};

		ID3D11Buffer* constant_buffer[CONSTANT_BUFFER::COUNT]{};

		D3D11_VIEWPORT				view_port[VIEWPORT::COUNT]{};

		ID3D11ShaderResourceView* sResourceView[SUBRESOURCE_VIEW::COUNT]{};

		ID3D11SamplerState* samplerState[STATE_SAMPLER::COUNT]{};

		/* Add more as needed...
		ID3D11SamplerState*			sampler_state[STATE_SAMPLER::COUNT]{};

		ID3D11BlendState*			blend_state[STATE_BLEND::COUNT]{};
		*/

		// Constructor for renderer implementation
		impl_t(void* window_handle, view_t& default_view);

		/* DEV5 helper functions
		* *****
		*/
		
		//draws joint with given translation
		void drawJointTransform(j_x joint, float3e translation);

		//draws entire frame skeleton with given translation
		void drawKeyFrame(kFrame frame, float3e translation);

		/*Draw Functions
		* *******
		*/
		void draw_cube(view_t& view);

		void draw_debug_lines(view_t& view);

		void draw_entities(crow::entities& entities, std::vector<size_t> inds, view_t view);

		/*Update functions
		* *******
		*/

		void update(float delta);

		/* Implementation functions
		* *********
		*/

		~impl_t();

		void set_render_target_view();

		void present(unsigned int vsync);

		void create_device_and_swapchain();

		void create_main_render_target();

		void setup_depth_stencil();

		void setup_rasterizer();

		void create_shaders_and_input_layout();

		void create_constant_buffers();

		void create_vertex_buffers();

		void create_vertex_buffer(ID3D11Buffer*& vertex_buffer, ID3D11Buffer*& index_buffer, crow::mesh_a& mesh);

		void create_vertex_buffer(ID3D11Buffer*& vertex_buffer, ID3D11Buffer*& index_buffer, crow::mesh_s& mesh);

		void CreateTextures_old(std::string diff_filename, std::string emis_filename, std::string spec_filename);

		void create_texture(std::string filepath, ID3D11ShaderResourceView*& sresourceview);

		void create_text_sresources(std::vector<std::string> text_filenames, mesh_info& m);

		void create_imgui_texture(std::string filename, ID3D11ShaderResourceView*& texture);

		void CreateSamplerState();

		/* Engine Dev functions
		* ***********
		*/
		void drawXMatrix(XMFLOAT4X4 M);

		void VecToRow(XMFLOAT4X4* m, XMFLOAT3 v, int row);

		//makes a look to b
		XMFLOAT4X4 LookAtM(XMFLOAT4X4 a, XMFLOAT4X4 b);

		XMFLOAT4X4 TurnToM(XMFLOAT4X4 a, XMFLOAT4X4 b, float delta);

		void DrawGrid();

		////allocates particles into sorted emitter array
		//void set_sorted_particles(Emitter_sp& emitter, int particles)
		//{
		//	for (unsigned int i = 0; i < particles; ++i)
		//	{
		//		int16_t indx = emitter.pool.alloc();
		//		if (indx == -1)
		//			return;

		//		Particle p;
		//		p.pos = p.prev_pos = emitter.pos;
		//		p.color = emitter.color;
		//		p.vel = end::float3e(randFloat(-5.0f, 5.0f), randFloat(-5.0f, 5.0f), randFloat(-5.0f, 5.0f));
		//		p.life_span = randFloat(0.3f, 3.0f);

		//		emitter.pool[indx] = p;
		//	}
		//}
		//// allocates particles and index of particle in free pool and sorted pool
		//void set_free_particles(Emitter_fp<int16_t>& emitter, pool_t<Particle, 1024>& pPool, int particles)
		//{
		//	// first you need to allocate a particle in the SharedParticleFreePool, and check if the alloc() succeeds
		//	// then, if it succeeds, you will need to call Allocate() on the emitter's SortedPoolOfIndices,
		//	// and, if that alloc() succeeds, add the particle index that was returned by FreePool's Alloc()
		//	// at the location that was just activated in the emitter's sorted pool of indices.
		//	// then initialize the activated particle in the shared free pool

		//	for (int i = 0; i < particles; ++i)
		//	{
		//		int spinx = pPool.alloc();
		//		if (spinx == -1)
		//			return;

		//		int eminx = emitter.pool.alloc();
		//		if (eminx == -1)
		//			return;

		//		emitter.pool[eminx] = spinx;

		//		Particle p;
		//		p.pos = p.prev_pos = emitter.pos;
		//		p.color = emitter.color;
		//		p.vel = end::float3e(randFloat(-5.0f, 5.0f), randFloat(-5.0f, 5.0f), randFloat(-5.0f, 5.0f));
		//		p.life_span = randFloat(0.3f, 3.0f);

		//		pPool[spinx] = p;
		//	}
		//}

		////updates particle position based on delta
		//void update_sorted_particles(Emitter_sp& emitter, float delta)
		//{
		//	for (int i = 0; i < emitter.pool.size(); ++i)
		//	{
		//		Particle& p = emitter.pool[i];
		//		if (p.life_span <= 0.0f)
		//		{
		//			emitter.pool.free(i);
		//			--i;
		//		}
		//		else
		//		{
		//			p.prev_pos = p.pos;
		//			p.pos += p.vel * delta;
		//			p.pos.y -= 0.1f * delta;
		//			p.life_span -= delta;

		//			end::debug_renderer::add_line(
		//				p.prev_pos,
		//				p.pos,
		//				spEmitter.color
		//			);
		//		}
		//	}
		//}

		////updates particles position and index  of free pool based on delta
		//void update_and_draw_fp(Emitter_fp<int16_t>& emitter, pool_t<Particle, 1024>& pPool, float delta)
		//{
		//	for (int i = 0; i < emitter.pool.size(); ++i)
		//	{
		//		int inx = emitter.pool[i];
		//		Particle& p = pPool[inx];
		//		if (p.life_span <= 0.0f)
		//		{
		//			pPool.free(inx);
		//			emitter.pool.free(i);
		//			--i;
		//		}
		//		else
		//		{
		//			p.prev_pos = p.pos;
		//			p.pos += p.vel * delta;
		//			p.pos.y -= 0.1f * delta;
		//			p.life_span -= delta;

		//			end::debug_renderer::add_line(
		//				p.prev_pos,
		//				p.pos,
		//				emitter.color
		//			);
		//		}
		//	}
		//}

	};

} // namespace crow