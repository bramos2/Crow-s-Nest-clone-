#include "../hpp/mesh.hpp"
#include <d3d11_2.h>
#include <fstream>
#include <assert.h>
#include <DirectXMath.h>


namespace crow {

	vert_a::vert_a()
	{
	}

	vert_a::vert_a(const vert_a&)
	{
	}

	vert_s::vert_s()
	{
	}

	vert_s::vert_s(const vert_s&)
	{
	}

	void load_bin_data(const char* file_path, mesh_a& mesh)
	{
		uint32_t indexCount;
		uint32_t vertexCount;

		std::fstream file{ file_path, std::ios_base::in | std::ios_base::binary };

		assert(file.is_open());

		if (!file.is_open())
		{
			assert(false);
			return;
		}
		//retrieving index data
		file.read((char*)&indexCount, sizeof(uint32_t));
		mesh.indicies.resize(indexCount);
		file.read((char*)mesh.indicies.data(), indexCount * sizeof(uint32_t));

		//retrieving vertex data
		file.read((char*)&vertexCount, sizeof(uint32_t));
		mesh.vertices.resize(vertexCount);
		file.read((char*)mesh.vertices.data(), vertexCount * sizeof(vert_a));

		file.close();
	}

	void load_mat_data(const char* file_path, std::vector<std::string>& filePaths, std::vector<material_a>& materials)
	{
		uint32_t materialsCount;
		uint32_t pathCount;

		std::fstream file{ file_path, std::ios_base::in | std::ios_base::binary };
		assert(file.is_open());

		if (!file.is_open())
		{
			assert(false);
			return;
		}

		file.read((char*)&materialsCount, sizeof(uint32_t));
		materials.resize(materialsCount);
		file.read((char*)materials.data(), materialsCount * sizeof(material_a));

		file.read((char*)&pathCount, sizeof(uint32_t));
		filePaths.resize(pathCount);
		for (int i = 0; i < pathCount; ++i)
		{
			std::string curr;
			uint32_t stringSize;
			file.read((char*)&stringSize, sizeof(uint32_t));
			curr.resize(stringSize);
			file.read((char*)curr.data(), stringSize * sizeof(char));
			filePaths[i] = curr;
		}

		file.close();

		std::string folPath = "../res/textures/";
		for (int i = 0; i < filePaths.size(); ++i)
		{
			size_t spos = filePaths[i].find_first_of('\\');
			filePaths[i].at(spos) = '/';
			filePaths[i].erase(remove(filePaths[i].begin(), filePaths[i].end(), '\\'), filePaths[i].end());
			filePaths[i] = folPath + filePaths[i];
			replace_ext(filePaths[i], "dds");
		}
	}

	void load_anim_data(const char* file_path, anim_clip& animationClip)
	{
		uint32_t frameCount;

		std::fstream file{ file_path, std::ios_base::in | std::ios_base::binary };
		assert(file.is_open());

		if (!file.is_open())
		{
			assert(false);
			return;
		}

		//read duration as double into animation clip 
		file.read((char*)&animationClip.duration, sizeof(double));
		//then read the number of frames as uint32_t
		file.read((char*)&frameCount, sizeof(uint32_t));
		//loop for each frame
		for (int f = 0; f < frameCount; ++f)
		{
			//create keyframe
			key_frame currentFrame;
			//read current frame time as double
			file.read((char*)&currentFrame.time, sizeof(double));
			//read the number of joints in this frame as uint32_t
			uint32_t jointCount;
			file.read((char*)&jointCount, sizeof(uint32_t));
			//resize joints container inside this keyframe
			currentFrame.joints.resize(jointCount);
			//read the data into the joints container as number of joints in this frame * size of joint_x
			file.read((char*)currentFrame.joints.data(), jointCount * sizeof(joint_x));
			//push keyframe into animation clip
			animationClip.frames.push_back(currentFrame);
		}

		file.close();
	}

	void translate_joints(key_frame& frame, float3e translation)
	{
		for (int i = 0; i < frame.joints.size(); ++i)
		{
			frame.joints[i].transform.m[3][0] += translation.x;
			frame.joints[i].transform.m[3][1] += translation.y;
			frame.joints[i].transform.m[3][2] += translation.z;
		}
	}

	key_frame get_tween_frame(anim_clip _animationClip, double t)
	{
		double d = 0;
		int prev = 0;
		int next = 0;
		//the frame we will be drawing
		std::vector<joint_x> tween_frame;
		tween_frame.resize(_animationClip.frames[0].joints.size());

		//find where t falls in
		for (int i = 1; i < _animationClip.frames.size() - 1; ++i)
		{
			if (t >= _animationClip.frames[i].time)
			{
				prev = i;
				next = i + 1;
			}
		}

		d = (t - _animationClip.frames[prev].time) / (_animationClip.frames[next].time - _animationClip.frames[prev].time);
		const key_frame& prevFrame = _animationClip.frames[prev];
		const key_frame& nextFrame = _animationClip.frames[next];

		for (int j = 0; j < tween_frame.size(); ++j)
		{
			tween_frame[j].parent_index = prevFrame.joints[j].parent_index;

			DirectX::XMVECTOR q1 = DirectX::XMQuaternionRotationMatrix(XMLoadFloat4x4(&prevFrame.joints[j].transform));
			DirectX::XMVECTOR q2 = DirectX::XMQuaternionRotationMatrix(XMLoadFloat4x4(&nextFrame.joints[j].transform));
			DirectX::XMVECTOR qs = DirectX::XMQuaternionSlerp(q1, q2, d);

			XMStoreFloat4x4(&tween_frame[j].transform, DirectX::XMMatrixRotationQuaternion(qs));

			tween_frame[j].transform.m[3][0] = (nextFrame.joints[j].transform.m[3][0] - prevFrame.joints[j].transform.m[3][0])
				* d + prevFrame.joints[j].transform.m[3][0];

			tween_frame[j].transform.m[3][1] = (nextFrame.joints[j].transform.m[3][1] - prevFrame.joints[j].transform.m[3][1])
				* d + prevFrame.joints[j].transform.m[3][1];

			tween_frame[j].transform.m[3][2] = (nextFrame.joints[j].transform.m[3][2] - prevFrame.joints[j].transform.m[3][2])
				* d + prevFrame.joints[j].transform.m[3][2];

		}

		key_frame result;
		result.joints = tween_frame;
		result.time = t;

		//drawKeyFrame(result);
		return result;
	}


	mesh_s clip_mesh(mesh_a& mesh)
	{
		mesh_s result;

		result.vertices.resize(mesh.vertices.size());
		result.indicies.resize(mesh.indicies.size());

		for (size_t i = 0; i < mesh.vertices.size(); ++i) {
			for (size_t p = 0; p < 3; p++)
			{
				result.vertices[i].pos[p] = mesh.vertices[i].pos[p];
			}

			for (size_t n = 0; n < 3; n++)
			{
				result.vertices[i].normal[n] = mesh.vertices[i].normal[n];
			}

			for (size_t u = 0; u < 2; u++)
			{
				result.vertices[i].uv[u] = mesh.vertices[i].uv[u];
			}

		}

		for (size_t i = 0; i < mesh.indicies.size(); i++)
		{
			result.indicies[i] = mesh.indicies[i];
		}

		return result;
	}

	void replace_ext(std::string& s, const std::string& newExt)
	{
		std::string::size_type i = s.rfind('.', s.length());

		if (i != std::string::npos) {
			s.replace(i + 1, newExt.length(), newExt);
		}
	}

	void invert_bind_pose(anim_clip& anim_clip)
	{
		for (int i = 0; i < anim_clip.frames[0].joints.size(); ++i) //preparing the bind pose
		{
			DirectX::XMMATRIX temp = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&anim_clip.frames[0].joints[i].transform));
			DirectX::XMStoreFloat4x4(&anim_clip.frames[0].joints[i].transform, temp);
		}
	}

	void get_inverted_bind_pose(key_frame& bind_pose, animator& animator)
	{
		const unsigned int size = bind_pose.joints.size();
		animator.inv_bindpose.joints.resize(size);
		animator.inv_bindpose.time = 0.f;

		for (size_t i = 0; i < size; ++i)
		{
			DirectX::XMMATRIX temp = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&bind_pose.joints[i].transform));
			DirectX::XMStoreFloat4x4(&animator.inv_bindpose.joints[i].transform, temp);
		}
	}

	void mult_invbp_tframe(anim_clip& anim_clip, key_frame& tween_frame, DirectX::XMMATRIX*& ent_mat)
	{
		if (ent_mat) {
			delete[] ent_mat;
		}

		if (!tween_frame.joints.empty()) {
			ent_mat = new DirectX::XMMATRIX[tween_frame.joints.size()];
			for (size_t i = 0; i < tween_frame.joints.size(); ++i)
			{
				DirectX::XMMATRIX cMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&anim_clip.frames[0].joints[i].transform), DirectX::XMLoadFloat4x4(&tween_frame.joints[i].transform));
				ent_mat[i] = XMMatrixTranspose(cMatrix);
			}
		}
	}

	void scale_matrix(DirectX::XMMATRIX& m, float x, float y, float z)
	{
		DirectX::XMFLOAT4X4 temp;
		DirectX::XMStoreFloat4x4(&temp, m);

		temp.m[0][0] *= x;
		temp.m[1][1] *= y;
		temp.m[2][2] *= z;

		m = DirectX::XMLoadFloat4x4(&temp);
	}


	void animator::update_tween_frame()
	{
		double d = 0;
		int prev = 0;
		int next = 0;
		//the frame we will be drawing
		std::vector<joint_x> tween_f;
		tween_f.resize(animations[curr_animation].frames[0].joints.size());

		//find where t falls in
		for (int i = 1; i < animations[curr_animation].frames.size() - 1; ++i)
		{
			if (t >= animations[curr_animation].frames[i].time)
			{
				prev = i;
				next = i + 1;
			}
		}

		d = (t - animations[curr_animation].frames[prev].time) / (animations[curr_animation].frames[next].time - animations[curr_animation].frames[prev].time);
		const key_frame& prevFrame = animations[curr_animation].frames[prev];
		const key_frame& nextFrame = animations[curr_animation].frames[next];

		for (int j = 0; j < tween_f.size(); ++j)
		{
			tween_f[j].parent_index = prevFrame.joints[j].parent_index;

			DirectX::XMVECTOR q1 = DirectX::XMQuaternionRotationMatrix(XMLoadFloat4x4(&prevFrame.joints[j].transform));
			DirectX::XMVECTOR q2 = DirectX::XMQuaternionRotationMatrix(XMLoadFloat4x4(&nextFrame.joints[j].transform));
			DirectX::XMVECTOR qs = DirectX::XMQuaternionSlerp(q1, q2, d);

			XMStoreFloat4x4(&tween_f[j].transform, DirectX::XMMatrixRotationQuaternion(qs));

			tween_f[j].transform.m[3][0] = (nextFrame.joints[j].transform.m[3][0] - prevFrame.joints[j].transform.m[3][0])
				* d + prevFrame.joints[j].transform.m[3][0];

			tween_f[j].transform.m[3][1] = (nextFrame.joints[j].transform.m[3][1] - prevFrame.joints[j].transform.m[3][1])
				* d + prevFrame.joints[j].transform.m[3][1];

			tween_f[j].transform.m[3][2] = (nextFrame.joints[j].transform.m[3][2] - prevFrame.joints[j].transform.m[3][2])
				* d + prevFrame.joints[j].transform.m[3][2];

		}

		tween_frame.joints = tween_f;
		tween_frame.time = t;
	}


	DirectX::XMMATRIX* animator::mult_curr_frame()
	{
		DirectX::XMMATRIX* result = nullptr;
		const size_t size = tween_frame.joints.size();

		if (!tween_frame.joints.empty()) {
			result = new DirectX::XMMATRIX[size];

			for (size_t i = 0; i < size; ++i)
			{
				DirectX::XMMATRIX cMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&inv_bindpose.joints[i].transform), DirectX::XMLoadFloat4x4(&tween_frame.joints[i].transform));
				cMatrix = XMMatrixTranspose(cMatrix);
				result[i] = cMatrix;
			}
		}
		return result;
	}

	void animator::update(DirectX::XMMATRIX*& ent_mat, float dt)
	{
		t += dt;
		if (t > animations[curr_animation].duration) {
			if (curr_animation != 0) {
				curr_animation = 0;
			}
			else {
				switch_animation(1);
			}
			t = animations[curr_animation].frames[1].time;
		}

		update_tween_frame();
		if (ent_mat) {
			delete[] ent_mat;
		}
		ent_mat = mult_curr_frame();
	}

	void animator::switch_animation(unsigned int index)
	{
		if (index < animations.size()) {
			curr_animation = index;
			t = animations[curr_animation].frames[1].time;
		}
	}

} // namespace crow