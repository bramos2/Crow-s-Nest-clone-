#pragma once
#include <d3d11_2.h>
#include  <vector>
#include <DirectXMath.h>
#include "mesh.hpp"
#include "math_types.hpp"

namespace crow {

	// contains all data necessary for a mesh to be rendered
	struct mesh_info {
		mesh_a* a_mesh;
		mesh_s* s_mesh;
		ID3D11Buffer* vertex_buffer;
		ID3D11Buffer* index_buffer;
		ID3D11ShaderResourceView* s_resource_view;
		ID3D11ShaderResourceView* emissive;
		ID3D11ShaderResourceView* specular;
		anim_clip anim;
		bool is_duplicate;
	};

	// contains all currently loaded game objects
	struct entities {
		std::vector<DirectX::XMMATRIX> world_matrix;
		std::vector<DirectX::XMFLOAT3> velocities;
		std::vector<DirectX::XMMATRIX*> framexbind;
		// pointer to the mesh data/information that will be loaded for the mesh at the relevant index
		// do not use new on this object, only use pointers to pre-existing data
		std::vector<mesh_info*> mesh_ptrs;
		std::vector<float> anim_time;
		// consider the following: use this to determine which animation is playing
		std::vector<float> curr_anim;


		uint32_t current_size = 0;

		// calls both allocate and init enty
		void allocate_and_init(unsigned int n);

		void allocate(unsigned int n);

		void init_entity(unsigned int n);

		void pop_back();
		void pop_all();

		~entities();

		DirectX::XMFLOAT3 entities::get_world_position(size_t const index) const;
		void set_world_position(size_t const index, float const x, float const y,
			float const z);
		// per-frame update the world matrix of the specified object
		// generally, just applies velocity
		void entities::update_transform_data(size_t const index, float dt);
		void entities::update_transform_data(float dt);

	};
	
	enum class entity {
	  WORKER = 0,
	  SPHYNX = 1,
	};
} // namespace crow