#include "entities.hpp"

namespace crow {

void crow::entities::allocate(unsigned int n)
{
	current_size += n;
	world_matrix.resize(current_size);
	velocity.resize(current_size);
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
		velocity[n] = DirectX::XMFLOAT3{ 0.f,0.f,0.f };
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

		velocity.pop_back();

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

entities::~entities()
{
	while (current_size > 0) {
		pop_back();
	}
}

} // namespace crow
