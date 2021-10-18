#pragma once
#include <d3d11_2.h>
#include  <vector>
#include <DirectXMath.h>
#include "mesh.hpp"
#include "math_types.hpp"

namespace crow {

	struct entities {
		std::vector<DirectX::XMMATRIX> world_matrix;
		std::vector<DirectX::XMFLOAT3> velocity;
		std::vector<DirectX::XMMATRIX*> framexbind;
		std::vector<mesh_a*> a_meshes;
		std::vector<mesh_s*> s_meshes;
		std::vector<ID3D11Buffer*> vertex_buffer;
		std::vector<ID3D11Buffer*> index_buffer;
		std::vector<ID3D11ShaderResourceView*> s_resource_view;
		std::vector<ID3D11ShaderResourceView*> emissive;
		std::vector<ID3D11ShaderResourceView*> specular;


		uint32_t current_size = 0;


		void allocate(unsigned int n);

		void init_entity(unsigned int n);

		void pop_back();

		~entities();
	};

} // namespace crow