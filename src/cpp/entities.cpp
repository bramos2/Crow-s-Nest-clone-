#include "../hpp/entities.hpp"

namespace crow {

void crow::entities::allocate(unsigned int n)
{
	current_size += n;
	world_matrix.resize(current_size);
	velocities.resize(current_size);
	mesh_ptrs.resize(current_size);
}

void entities::init_entity(unsigned int n)
{
	if (n < current_size) {
		world_matrix[n] = DirectX::XMMatrixIdentity();
		velocities[n] = DirectX::XMFLOAT3{ 0.f,0.f,0.f };
		mesh_ptrs[n] = nullptr;
	}
}

void entities::pop_back()
{
	// there is no heap data stored here, so there is no need to do any real cleanup
	if (current_size > 0) {
		world_matrix.pop_back();
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
