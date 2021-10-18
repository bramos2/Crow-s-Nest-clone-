//#include "renderer.hpp"
//
////#ifdef FSGD_END_USE_D3D
////#include "d3d11_renderer_impl.hpp"
////#endif
//
//namespace crow
//{
//	//renderer_t::renderer_t(native_handle_type window_handle)
//	//{
//	//	//p_impl = new impl_t(window_handle, default_view);
//	//}
//
//	/*
//	renderer_t::renderer_t(renderer_t&& other)
//	{
//		std::swap(p_impl, other.p_impl);
//	}
//	*/
//
//	renderer_t::renderer_t()
//	{
//	}
//
//	renderer_t::~renderer_t()
//	{
//		// Clean up implementation
//		//delete p_impl;
//	}
//
//	void renderer_t::update(float delta, std::bitset<256> bm, float dx, float dy)
//	{
//		////rotate camera
//		//XMMATRIX camera = XMLoadFloat4x4A(&(XMFLOAT4X4A&)default_view.view_mat);
//		//camera = XMMatrixInverse(nullptr, camera);
//		//
//		//XMVECTOR vPosStore = camera.r[3];
//
//		///*if (dx != 0.0f)
//		//{
//		//	camera = XMMatrixMultiply(camera, XMMatrixRotationY(-dx * delta * 0.5f));
//		//}
//
//		//if (dy != 0.0f)
//		//{
//		//	camera = XMMatrixMultiply(camera, XMMatrixRotationX(-dy * delta * 0.5f));
//		//}*/
//
//		////camera.r[3] = vPosStore;
//
//		////translate camera
//		//if (bm[(int)'W'] == 1)
//		//{
//		//	camera = XMMatrixMultiply(camera, XMMatrixTranslation(0, 0, -5.0f * delta));
//		//}
//		//if (bm[(int)'S'] == 1)
//		//{
//		//	camera = XMMatrixMultiply(camera, XMMatrixTranslation(0, 0, 5.0f * delta));
//		//}
//		//if (bm[(int)'A'] == 1)
//		//{
//		//	camera = XMMatrixMultiply(camera, XMMatrixTranslation(5.0f * delta, 0, 0));
//		//}
//		//if (bm[(int)'D'] == 1)
//		//{
//		//	camera = XMMatrixMultiply(camera, XMMatrixTranslation(-5.0f * delta,0, 0));
//		//}
//
//		//default_view.view_mat = (float4x4_a&)XMMatrixInverse(nullptr, camera);
//		////default_view.view_mat = (float4x4_a&)camera;
//
//		//p_impl->update(delta, bm, dx, dy);
//	}
//
//	void renderer_t::update(float delta)
//	{
//		//p_impl->update(delta);
//	}
//
//	//void renderer_t::draw(view_t& view)
//	//{
//	//	// set RTV and DSV
//	//	//**p_impl->set_render_target_view();
//	//	// draw views...
//	//	//p_impl->draw_cube(default_view);
//	//	//**p_impl->draw_debug_lines(default_view);
//	//	//p_impl->draw_terrain(default_view);
//	//	//**p_impl->draw_mesh(default_view);
//	//	// draw views...
//	//	// draw views...
//	//	// present
//	//	//**p_impl->present(1);
//	//}
//}