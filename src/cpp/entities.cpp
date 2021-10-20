#include "../hpp/entities.hpp"

namespace crow {

void crow::entities::allocate(unsigned int n)
{
	current_size += n;
	world_matrix.resize(current_size);
	velocities.resize(current_size);
	framexbind.resize(current_size);
	a_meshes.resize(current_size);
	s_meshes.resize(current_size);
	vertex_buffer.resize(current_size);
	index_buffer.resize(current_size);
	s_resource_view.resize(current_size);
	emissive.resize(current_size);
	specular.resize(current_size);
}

void entities::init_entity(unsigned int n)
{
	if (n < current_size) {
		world_matrix[n] = DirectX::XMMatrixIdentity();
		velocities[n] = DirectX::XMFLOAT3{ 0.f,0.f,0.f };
		framexbind[n] = nullptr;
		a_meshes[n] = nullptr;
		s_meshes[n] = nullptr;
		vertex_buffer[n] = nullptr;
		index_buffer[n] = nullptr;
		s_resource_view[n] = nullptr;
		emissive[n] = nullptr;
		specular[n] = nullptr;
	}
}

void entities::pop_back()
{
	if (current_size > 0) {
		world_matrix.pop_back();

		velocities.pop_back();

		if (framexbind.back()) {
			delete framexbind.back();
		}
		framexbind.pop_back();

		if (a_meshes.back()) {
			delete a_meshes.back();
		}
		a_meshes.pop_back();

		if (s_meshes.back()) {
			delete s_meshes.back();
		}
		s_meshes.pop_back();

		if (vertex_buffer.back()) {
			vertex_buffer.back()->Release();
		}
		vertex_buffer.pop_back();

		if (index_buffer.back()) {
			index_buffer.back()->Release();
		}
		index_buffer.pop_back();

		if (s_resource_view.back()) {
			s_resource_view.back()->Release();
		}
		s_resource_view.pop_back();

		if (emissive.back()) {
			emissive.back()->Release();
		}
		emissive.pop_back();

		if (specular.back()) {
			specular.back()->Release();
		}
		specular.pop_back();

		--current_size;
	}
}

DirectX::XMFLOAT3 entities::get_world_position(size_t const index) const {
	DirectX::XMFLOAT4X4 w;
	DirectX::XMStoreFloat4x4(&w, world_matrix[index]);

	DirectX::XMFLOAT3 result;
	result.x = w.m[3][0];
	result.y = w.m[3][1];
	result.z = w.m[3][2];
	return result;
}

void entities::set_world_position(size_t const index, float const x, float const y,
                        float const z) {
	if (index < current_size) {
		DirectX::XMFLOAT4X4 w;
		DirectX::XMStoreFloat4x4(&w, world_matrix[index]);
		w.m[3][0] = x;
		w.m[3][1] = y;
		w.m[3][2] = z;
		
		world_matrix[index] = DirectX::XMLoadFloat4x4(&w);
	}
}

void entities::update_transform_data(size_t const index, float dt) {
	if (index < current_size) {
		DirectX::XMFLOAT4X4 w;
		DirectX::XMStoreFloat4x4(&w, world_matrix[index]);
		w.m[3][0] += velocities[index].x;
		w.m[3][1] += velocities[index].y;
		w.m[3][2] += velocities[index].z;
		world_matrix[index] = DirectX::XMLoadFloat4x4(&w);
	}
}

entities::~entities()
{
	while (current_size > 0) {
		pop_back();
	}
}

} // namespace crow
