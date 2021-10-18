#include "game_manager.hpp"


namespace crow {
	void game_manager::init_app(void* window_handle)
	{
		p_impl = new impl_t(window_handle, view);
		timer.Restart();

		entities.allocate(2);
		for (size_t i = 0; i < entities.current_size; i++)
		{
			entities.init_entity(i);
		}

		mesh_a temp;
		LoadBinData("../Renderer/Assets/Run.bin", temp);
		std::vector<std::string> paths;
		std::vector<material_a> mats;
		LoadMatData("../Renderer/Assets/Run.mat", paths, mats);
		animClip animc;
		LoadAnimData("../Renderer/Assets/Run.anim", animc);


		entities.s_meshes[0] = new mesh_s(clip_a_mesh(temp));
		entities.s_meshes[1] = new mesh_s(clip_a_mesh(temp));
		for (size_t i = 0; i < 2; i++)
		{
			p_impl->create_vertex_buffer(entities.vertex_buffer[i], entities.index_buffer[i], *entities.s_meshes[i]);
			p_impl->create_text_sresources(paths, entities, i);
		}
		DirectX::XMFLOAT4X4 w1;
		DirectX::XMStoreFloat4x4(&w1, entities.world_matrix[0]);
		w1.m[3][0] = 5.f;
		entities.world_matrix[0] = DirectX::XMLoadFloat4x4(&w1);
		int stop = 0;
	}

	void game_manager::update()
	{
		timer.Signal();
		p_impl->update(static_cast<float>(timer.Delta()));
	}

	void game_manager::render()
	{
		p_impl->set_render_target_view();
		p_impl->draw_debug_lines(view);
		//p_impl->draw_mesh(view);
		std::vector<uint32_t> temp_inds;
		temp_inds.push_back(0);
		temp_inds.push_back(1);
		p_impl->draw_entities(entities, temp_inds, view);
		p_impl->present(1);
	}

	void game_manager::set_bitmap(std::bitset<256>& bitmap)
	{
		bmap = bitmap;
	}

	game_manager::game_manager()
	{

	}

	game_manager::~game_manager()
	{
		delete p_impl;
	}

}