#pragma once

#include <array>
#include "view.hpp"
#include "debug_renderer.hpp"
#include "math_types.hpp"

// Note: You are free to make adjustments/additions to the declarations provided here.

namespace crow
{
	struct sphere_t { float3e center = { 0.f,0.f,0.f }; float radius = 0; }; //Alterative: using sphere_t = float4;

	struct aabb_t { float3e center= { 0.f,0.f,0.f };  float3e extents= { 0.f,0.f,0.f };
	}; //Alternative: aabb_t { float3 min; float3 max; };

	//offset is distance from origin to plane in direction of normal
	struct plane_t { float3e normal = { 0.f,0.f,0.f };  float offset = 0.f; };  //Alterative: using plane_t = float4;

	using frustum_t = std::array<plane_t, 6>;

	// Calculates the plane of a triangle from three points.
	plane_t calculate_plane(float3e a, float3e b, float3e c)
	{
		plane_t result;
		result.normal = a.cross((a - b), (a - c));
		result.normal = result.normal.normalize(result.normal);
		//D = -(A * x1 + B * y1 + C * z1)
		result.offset = -(result.normal.x * a.x + result.normal.y * a.y + result.normal.z * a.z);
		return result;
	}

	// Calculates a frustum (6 planes) from the input view parameter.
	//
	// Calculate the eight corner points of the frustum. 
	// Use your debug renderer to draw the edges.
	// 
	// Calculate the frustum planes.
	// Use your debug renderer to draw the plane normals as line segments.
	void calculate_frustum(frustum_t& frustum, const view_t& view)
	{
		float fov = 2.0f * atan(1.0f / view.proj_mat[1].y);
		float aspectRatio = view.proj_mat[1].y / view.proj_mat[0].x;
		float zfar = -(2.0f * view.proj_mat[3].z) / (2.0f * view.proj_mat[2].z - 2.0f);
		float znear = ((view.proj_mat[2].z - 1.0f) * zfar) / (view.proj_mat[2].z + 1.0f);

		float Hnear = 2.0f * tan(fov / 2.0f) * znear;
		float Hfar = 2.0f * tan(fov / 2.0f) * zfar;
		float Wnear = Hnear * aspectRatio;
		float Wfar = Hfar * aspectRatio;

		float3e Cnear = view.view_mat[3].xyz + view.view_mat[2].xyz * znear;
		float3e Cfar = view.view_mat[3].xyz + view.view_mat[2].xyz * zfar;

		float3e camX = float3e(view.view_mat[0].x, view.view_mat[0].y, view.view_mat[0].z);
		float3e camY = float3e(view.view_mat[1].x, view.view_mat[1].y, view.view_mat[1].z);

		float3e points[8];

		points[0] = Cfar + camY * (Hfar * 0.5f) - camX * (Wfar * 0.5f);
		points[1] = Cfar + camY * (Hfar * 0.5f) + camX * (Wfar * 0.5f);
		points[2] = Cfar - camY * (Hfar * 0.5f) - camX * (Wfar * 0.5f);
		points[3] = Cfar - camY * (Hfar * 0.5f) + camX * (Wfar * 0.5f);

		points[4] = Cnear + camY * (Hnear * 0.5f) - camX * (Wnear * 0.5f);
		points[5] = Cnear + camY * (Hnear * 0.5f) + camX * (Wnear * 0.5f);
		points[6] = Cnear - camY * (Hnear * 0.5f) - camX * (Wnear * 0.5f);
		points[7] = Cnear - camY * (Hnear * 0.5f) + camX * (Wnear * 0.5f);
		
		//drawing frustum
		for (int i = 0; i < 8; ++i)
		{
			if (i <= 3)
			{
				crow::debug_renderer::add_line(points[i], points[i + 4],
					float4e(1,1,0,1));
			}

			if (i % 2 == 0)
			{
				crow::debug_renderer::add_line(points[i], points[i + 1],
					float4e(1, 1, 0, 1));
			}
			if (i == 0 || i == 1 || i == 4 || i == 5)
			{
				crow::debug_renderer::add_line(points[i], points[i + 2],
					float4e(1, 1, 0, 1));
			}
		}

		//far and near
		frustum[0] = crow::calculate_plane(points[0], points[1], points[3]);
		frustum[1] = crow::calculate_plane(points[7], points[5], points[4]);
		//right and left
		frustum[2] = crow::calculate_plane(points[1], points[5], points[7]);
		frustum[3] = crow::calculate_plane(points[4], points[0], points[2]);
		//up and down
		frustum[4] = crow::calculate_plane(points[1], points[0], points[4]);
		frustum[5] = crow::calculate_plane(points[2], points[3], points[7]);

		float3e pcenters[6];
		pcenters[0] = (points[0] + points[1] + points[2] + points[3]) / 4;
		pcenters[1] = (points[4] + points[5] + points[7] + points[6]) / 4;

		pcenters[2] = (points[1] + points[5] + points[7] + points[3]) / 4;
		pcenters[3] = (points[4] + points[0] + points[2] + points[6]) / 4;

		pcenters[4] = (points[0] + points[1] + points[4] + points[5]) / 4;
		pcenters[5] = (points[2] + points[3] + points[7] + points[6]) / 4;

		//drawing normals
		for (int i = 0; i < 6; ++i)
		{
			crow::debug_renderer::add_line(pcenters[i], pcenters[i] + frustum[i].normal,
				float4e(1, 0, 1, 1), float4e(0, 1, 1, 1));
		}
	}

	// Calculates which side of a plane the sphere is on.
	//
	// Returns -1 if the sphere is completely behind the plane.
	// Returns 1 if the sphere is completely in front of the plane.
	// Otherwise returns 0 (Sphere overlaps the plane)
	int classify_sphere_to_plane(const sphere_t& sphere, const plane_t& plane)
	{
		int result = 0;
		float3e a;
		float c = a.dot(sphere.center, plane.normal) + plane.offset + sphere.radius;
		/*if (c > 0)
			c -= sphere.radius;
		else
			c += sphere.radius;*/
		//float c = a.dot(sphere.center, plane.normal) - plane.offset;

		if (c < 0.0f  /*c < sphere.radius*/)
			result = -1;
		else if (c > 0.0f  /*c > sphere.radius*/)
			result = 1;

		return result;
	}

	// Calculates which side of a plane the aabb is on.
	//
	// Returns -1 if the aabb is completely behind the plane.
	// Returns 1 if the aabb is completely in front of the plane.
	// Otherwise returns 0 (aabb overlaps the plane)
	// MUST BE IMPLEMENTED UsING THE PROJECTED RADIUS TEST
	int classify_aabb_to_plane(const aabb_t& aabb, const plane_t& plane)
	{
		sphere_t s;
		float3e a = plane.normal;
		
		for (int i = 0; i < 3; ++i)
		{
			if (a[i] < 0.0f)
				a[i] *= -1;
		}

		//dot aabb max with modified plane normal to get radius
		//use obtained radius to check aabb as sphere
		s.center = aabb.center;
		s.radius = a.dot(aabb.extents, a);
		return classify_sphere_to_plane(s, plane);
	}

	// Determines if the aabb is inside the frustum.
	//
	// Returns false if the aabb is completely behind any plane.
	// Otherwise returns true.
	bool aabb_to_frustum(const aabb_t& aabb, const frustum_t& frustum)
	{
		for (int i = 0; i < 6; ++i)
		{
			if (classify_aabb_to_plane(aabb, frustum[i]) == -1)
			{
				return false;
			}
		}
		return true;
	}
}