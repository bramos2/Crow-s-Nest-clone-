#pragma once

#include <DirectXMath.h>
#include "math_types.hpp"
#include "map.hpp"
//#include "frustum_culling.h"

namespace crow
{
	class view_t
	{
	public:
		// rotation in degrees
		float3_a rotation;
		// synonymous with row 4 of the view matrix
		float3_a position;

		// holds temporary matrices
		float4x4_a view_mat;
		float4x4_a proj_mat;
		// rotation matrix, useful for many things
		float4x4 rot_mat;

		// holds final matrices that are sent to the renderer
		DirectX::XMMATRIX view_final;
		DirectX::XMMATRIX proj_final;

		float debug_camera_mspeed = 18.f;
		float debug_camera_rspeed = 60.f;


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

		// initializes all individual components
		void init();
		// updates matrices based on the vectors
		void update();

		// sets rot_mat() to a matrix based on the rotation vector
		void update_rotation_matrix();
	};

	// utilizes mouse_to_ray() and ray_to_floor() to find out where in the floor the
	// mouse is pointing
	float3e mouse_to_floor(view_t camera, float2e mouse_pos, int w, int h);

	// converts the mouse's current position on the screen to a ray that can be used
	// to check for mouse interactions
	float3e mouse_to_ray(view_t camera, float2e mouse_pos, int w, int h);

	// takes in an arbitrary ray (pre-normalization is optional) and its position in
	// space, and figures out the x and z coordinates that the ray will intersect
	// the floor at
	//
	// important! if the ray cannot intersect the floor, then the y coordinate of
	// the returned ray will always be -1. if not, then it will always be 0.
	float3e ray_to_floor(float3e ray_direction, float3e ray_position);

	void update_room_cam(crow::room* active_room, view_t& camera);

	void update_room_cam(float3_a position, float3_a rotation, view_t& camera);
}