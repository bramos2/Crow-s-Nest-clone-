#pragma once
#include <memory>
#include <bitset>
//#include <fbxsdk.h>
//#include "view.hpp"
//#include "frustum_culling.h"

//#define FSGD_END_USE_D3D

namespace crow
{
	// PImpl idiom ("Pointer to implementation")
	// 'impl' will define the implementation for the renderer elsewhere
	//struct impl_t;

	// APIs/Platforms have their own types for representing a handle to a 'window'
	// They will generally all fit in the 8-bytes of a void*.
	// HWND is actually just a typedef/alias for a void*.
	//using native_handle_type = void*;

	// Interface to the renderer
	//class renderer_t
	//{
	//public:

	//	//renderer_t(native_handle_type window_handle);
	//	//renderer_t(renderer_t&& other);
	//	renderer_t();
	//	~renderer_t();

	//	void update(float delta, std::bitset<256> bm, float dx, float dy);
	//	void update(float delta);

	//	//void draw(view_t& view);

	//	//view_t default_view;

	//private:

	//	


	//	// Pointer to the implementation
	//	//crow::impl_t* p_impl = nullptr;
	//};

	// The following types just defines scopes for enum values.
	// The enum values can be used as indices in arrays.
	// These enum values can be added to as needed.

	
	/* Add more as needed...
	enum STATE_SAMPLER{ DEFAULT = 0, COUNT };

	enum STATE_BLEND{ DEFAULT = 0, COUNT };
	*/
}