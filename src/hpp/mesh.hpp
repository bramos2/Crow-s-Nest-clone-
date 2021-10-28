#pragma once

#include <vector>
#include <string>
#include "math_types.hpp"
//#include "interactible.hpp"

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

	struct key_frame
	{
		double time = -1;
		std::vector<joint_x> joints;
	};

	struct anim_clip
	{
		double duration;
		std::vector<key_frame> frames;
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

	struct animator
	{
		key_frame inv_bindpose;
		key_frame tween_frame;
		std::vector<anim_clip> animations;
		bool is_acting = false;
		bool is_running = false;
		bool performed_action = false;

		struct anim_type {
			enum {
				IDLE = 0,
				MOVING,
				DYING,
				COUNT
			};
		};

		unsigned int curr_animation = 0;
		float t = 0.f;
		
		// calculates the current frame to be played based on time and handles playing the animaiton
		void update(DirectX::XMMATRIX*& ent_mat, float dt);

		// sets values to the animation corresponding to given index
		void switch_animation(unsigned int index);

	private:
		void update_tween_frame();
		// returns the multiplications of the inverted bindpose and the updated tween_frame
		DirectX::XMMATRIX* mult_curr_frame();
	};

	// load mesh data from a .bin file and stores it in given mesh
	void load_bin_data(const char* file_path, mesh_a& mesh);

	// loads material data from a .mat file, will store the texture names inside filepaths
	void load_mat_data(const char* file_path, std::vector<std::string>& file_paths, std::vector<material_a>& materials);

	// loads animation froma .anim file and stores it in the given clip
	void load_anim_data(const char* file_path, anim_clip& animationClip);

	// this is not good, could cause weird results in animations. instead translate the world matrix of the entity
	void translate_joints(key_frame& frame, float3e translation);

	// returns the interpolated frame based on time t
	key_frame get_tween_frame(anim_clip _animationClip, double t);

	mesh_s clip_mesh(mesh_a& mesh);

	void replace_ext(std::string& s, const std::string& newExt);

	// inverts the bind pose stored in the animation clip
	void invert_bind_pose(anim_clip& anim_clip);

	// inverts the bind pose and stores it inside the animator
	void get_inverted_bind_pose(key_frame& bind_pose, animator& animator);

	//void mult_invbp_tframe(anim_clip& anim_clip, key_frame& tween_frame, DirectX::XMMATRIX*& ent_mat);

	void scale_matrix(DirectX::XMMATRIX& m, float x = 1.f, float y = 1.f, float z = 1.f);
}