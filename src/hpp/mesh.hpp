#pragma once

#include <vector>
#include <string>
#include "math_types.hpp"

namespace crow {
	
	// mesh data
	struct vert_a
	{
		float pos[3] = { 0.0f, 0.0f, 0.0f };
		float normal[3] = { 0.0f, 0.0f, 0.0f };
		float uv[2] = { 0.0f, 0.0f };
		int joints[4] = { 0,0,0,0 };  //joint indicies
		float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; //joint weights

		vert_a();
		vert_a(const vert_a&);
	};

	struct vert_s
	{
		float pos[3] = { 0.f, 0.f, 0.f };
		float normal[3] = { 0.f, 0.f, 0.f };
		float uv[2] = { 0.0f, 0.0f };

		vert_s();
		vert_s(const vert_s&);
	};

	struct mesh_a {
		std::vector<vert_a> vertices;
		std::vector<uint32_t> indicies;
	};

	struct mesh_s {
		std::vector<vert_s> vertices;
		std::vector<uint32_t> indicies;
	};

	//animation data
	struct joint_x
	{
		DirectX::XMFLOAT4X4 transform;
		int parent_index = -1;
	};

	struct keyFrame
	{
		double time = -1;
		std::vector<joint_x> joints;
	};

	struct animClip
	{
		double duration;
		std::vector<keyFrame> frames;
	};

	// material data
	struct material_a
	{
		enum eComponent { EMISSIVE = 0, DIFFUSE, SPECULAR, SHININESS, COUNT };

		struct component_a
		{
			float value[3] = { 0.0f,0.0f,0.0f };
			float factor = 0.0f;
			int64_t input = -1;
		};

		component_a& operator[](int i) { return components[i]; }
		const component_a& operator[](int i)const { return components[i]; }

	private:
		component_a components[COUNT];
	};

	void LoadBinData(const char* file_path, mesh_a& mesh);
	
	void LoadMatData(const char* file_path, std::vector<std::string>& filePaths, std::vector<material_a>& materials);

	void LoadAnimData(const char* file_path, animClip& animationClip);
	
	void TranslateJoints(keyFrame& frame, float3e translation);

	keyFrame GetTweenFrame(animClip _animationClip, double t);

	mesh_s clip_a_mesh(mesh_a& mesh);

	void replace_ext(std::string& s, const std::string& newExt);
}