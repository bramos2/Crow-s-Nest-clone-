#pragma once

#include "math_types.hpp"
//#include "frustum_culling.h"

namespace crow
{
	class view_t
	{
	public:

		// holds inversed matrix
		float4x4_a view_mat_inv;
		float4x4_a view_mat;
		float4x4_a proj_mat;

		float3e rotation;

		// stores properties of a view
		//
		//	view and projection matrices	(REQUIRED)
		//	type information 				(optional) (Orthographic/Perspective/Cubemap/Shadowmap/etc)
		//	render target id(s)				(optional)
		//	viewport id						(optional)
		//		IMPORTANT: 
		//			Do not store an inverted view matrix.
		//			It will be much easier on you, me, and the CPU this way.
		//			When updating your constant buffers, send an inverted copy of the stored view matrix.
		


		// maintains a visible-set of renderable objects in view (implemented in a future assignment)


		view_t() = default;
		~view_t() = default;
	};
}