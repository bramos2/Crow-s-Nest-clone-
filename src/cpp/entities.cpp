#include "../hpp/entities.hpp"

namespace crow {

	void crow::entities::allocate_and_init(unsigned int n) {
		allocate(n);
		for (size_t i = current_size - n; i < current_size; i++) {
			init_entity(i);
		}
	}

	void crow::entities::allocate(unsigned int n)
	{
		current_size += n;
		world_matrix.resize(current_size);
		velocities.resize(current_size);
		framexbind.resize(current_size);
		mesh_ptrs.resize(current_size);
		//anim_time.resize(current_size);
		//curr_anim.resize(current_size);
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
			mesh_ptrs[n] = nullptr;
			//anim_time[n] = 0;
			//curr_anim[n] = 0;
		}
	}

	void entities::pop_back()
	{
		if (current_size > 0) {
			world_matrix.pop_back();
			velocities.pop_back();

			// the only heap data stored in entities
			delete framexbind.back();
			framexbind.pop_back();

			// DO NOT CALL DELETE ON mesh_ptrs
			mesh_ptrs.pop_back();

			//anim_time.pop_back();
			//curr_anim.pop_back();

			s_resource_view.pop_back();
			emissive.pop_back();
			specular.pop_back();

			--current_size;
		}
	}
	void entities::pop_all() {
	
		while (current_size > 0) {
			pop_back();
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

	void entities::rotate_world_matrix(size_t const index, float x, float y, float z)
	{
		x *= to_radiansf;
		y *= to_radiansf;
		z *= to_radiansf;

		if (index < current_size) {
			if (x != 0.f) {
				world_matrix[index] = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationX(x), world_matrix[index]);
			}
			if (y != 0.f) {
				world_matrix[index] = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(y), world_matrix[index]);
			}
			if (z != 0.f) {
				world_matrix[index] = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationZ(z), world_matrix[index]);
			}
		}
	}

	void entities::scale_world_matrix(size_t const index, float scale)
	{
		if (index < current_size) {
			world_matrix[index] = DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(scale, scale, scale), world_matrix[index]);
		}
	}

	void entities::update_transform_data(size_t const index, float dt) {
		if (index < current_size) {
			DirectX::XMFLOAT4X4 w;
			DirectX::XMStoreFloat4x4(&w, world_matrix[index]);
			w.m[3][0] += velocities[index].x * dt;
			w.m[3][1] += velocities[index].y * dt;
			w.m[3][2] += velocities[index].z * dt;
			world_matrix[index] = DirectX::XMLoadFloat4x4(&w);
		}
	}

	void entities::update_transform_data(float dt) {
		for (int i = 0; i < current_size; i++) {
			update_transform_data(i, dt);
	
		}
	}

	entities::~entities()
	{
		pop_all();
	}

} // namespace crow
