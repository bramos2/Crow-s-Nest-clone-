#include <fstream>
#include "../hpp/d3d11_renderer_impl.hpp"
#include "../hpp/blob.hpp"
#include "../hpp/debug_renderer.hpp"
#include "../hpp/DDSTextureLoader.hpp"

namespace crow {

	impl_t::impl_t(void* window_handle, view_t& default_view)
	{
		{
			hwnd = (HWND)window_handle;
			//LoadData(); //terrain for engine dev
			//exporting and loading data from our fbx using our library
			//export_fbx_mesh("../Renderer/Assets/Run.fbx");
			/*load_anim_data("res/animations/Run.anim", animationClip);
			load_bin_data("res/meshes/Run.bin", meshInds, meshVerts);
			load_mat_data("res/textures/Run.mat", filePaths, materials);*/
			//modifying file paths for textures/materials
			/*std::string folPath = "res/textures/";
			for (int i = 0; i < filePaths.size(); ++i)
			{
				size_t spos = filePaths[i].find_first_of('\\');
				filePaths[i].at(spos) = '/';
				filePaths[i].erase(remove(filePaths[i].begin(), filePaths[i].end(), '\\'), filePaths[i].end());
				filePaths[i] = folPath + filePaths[i];
				replaceExt(filePaths[i], "dds");
			}
			diffuseTexturePath = filePaths[0];
			if (filePaths.size() > 1)
			{
				emissiveTexturePath = filePaths[1];
				specularTexturePath = filePaths[2];
			}
			else
			{
				emissiveTexturePath = specularTexturePath = "";
			}*/

			////getting inverted bind pose
			//for (int i = 0; i < animationClip.frames[0].joints.size(); ++i) //preparing the bind pose
			//{
			//	InvBindPose[i] = XMMatrixInverse(nullptr, XMLoadFloat4x4(&animationClip.frames[0].joints[i].transform));
			//}

			create_device_and_swapchain();

			create_main_render_target();

			setup_depth_stencil();

			setup_rasterizer();

			//CreateTexture(diffuseTexturePath, emissiveTexturePath, specularTexturePath);

			CreateSamplerState();

			create_shaders_and_input_layout();

			create_vertex_buffers();

			create_constant_buffers();

			// initializing projection matrix
			float aspect = view_port[VIEWPORT::DEFAULT].Width / view_port[VIEWPORT::DEFAULT].Height;
			default_view.proj_mat = (float4x4_a&)XMMatrixPerspectiveFovLH(3.1415926f / 4.0f, aspect, 0.01f, 100.0f);
			//vt.proj_mat = (float4x4_a&)XMMatrixPerspectiveFovLH(fov, aspect, nd, fd);

			// note: please don't use lookatLH for the view matrix since it doesnt give the rotation vector, which complicates things
			default_view.init();
			default_view.position.y = 5.f;
			default_view.position.z = -10.f;
			default_view.rotation.x = -20.f; // degrees
			default_view.update_rotation_matrix();
			default_view.update();
		}
	}

	//void impl_t::replaceExt(std::string& s, const std::string& newExt)
	//{
	//	std::string::size_type i = s.rfind('.', s.length());

	//	if (i != std::string::npos) {
	//		s.replace(i + 1, newExt.length(), newExt);
	//	}
	//}

	//void impl_t::load_mat_data(const char* file_path, std::vector<std::string>& filePaths, std::vector<mat_t>& materials)
	//{
	//	uint32_t materialsCount;
	//	uint32_t pathCount;

	//	std::fstream file{ file_path, std::ios_base::in | std::ios_base::binary };
	//	assert(file.is_open());

	//	if (!file.is_open())
	//	{
	//		assert(false);
	//		return;
	//	}

	//	file.read((char*)&materialsCount, sizeof(uint32_t));
	//	materials.resize(materialsCount);
	//	file.read((char*)materials.data(), materialsCount * sizeof(mat_t));
	//	file.read((char*)&pathCount, sizeof(uint32_t));
	//	filePaths.resize(pathCount);
	//	for (int i = 0; i < pathCount; ++i)
	//	{
	//		std::string curr;
	//		uint32_t stringSize;
	//		file.read((char*)&stringSize, sizeof(uint32_t));
	//		curr.resize(stringSize);
	//		file.read((char*)curr.data(), stringSize * sizeof(char));
	//		filePaths[i] = curr;
	//	}

	//	file.close();
	//}

	//void impl_t::load_bin_data(const char* file_path, std::vector<uint32_t>& mesh_inds, std::vector<mesh_vertex>& mesh_verts)
	//{
	//	uint32_t indexCount;
	//	uint32_t vertexCount;

	//	std::fstream file{ file_path, std::ios_base::in | std::ios_base::binary };

	//	assert(file.is_open());

	//	if (!file.is_open())
	//	{
	//		assert(false);
	//		return;
	//	}
	//	//retrieving index data
	//	file.read((char*)&indexCount, sizeof(uint32_t));
	//	mesh_inds.resize(indexCount);
	//	file.read((char*)mesh_inds.data(), indexCount * sizeof(uint32_t));

	//	//retrieving vertex data
	//	file.read((char*)&vertexCount, sizeof(uint32_t));
	//	mesh_verts.resize(vertexCount);
	//	file.read((char*)mesh_verts.data(), vertexCount * sizeof(mesh_vertex));

	//	file.close();
	//}

	//void impl_t::load_anim_data(const char* file_path, aClip& animationClip)
	//{
	//	uint32_t frameCount;

	//	std::fstream file{ file_path, std::ios_base::in | std::ios_base::binary };
	//	assert(file.is_open());

	//	if (!file.is_open())
	//	{
	//		assert(false);
	//		return;
	//	}

	//	//read duration as double into animation clip 
	//	file.read((char*)&animationClip.duration, sizeof(double));
	//	//then read the number of frames as uint32_t
	//	file.read((char*)&frameCount, sizeof(uint32_t));
	//	//loop for each frame
	//	for (int f = 0; f < frameCount; ++f)
	//	{
	//		//create keyframe
	//		kFrame currentFrame;
	//		//read current frame time as double
	//		file.read((char*)&currentFrame.time, sizeof(double));
	//		//read the number of joints in this frame as uint32_t
	//		uint32_t jointCount;
	//		file.read((char*)&jointCount, sizeof(uint32_t));
	//		//resize joints container inside this keyframe
	//		currentFrame.joints.resize(jointCount);
	//		//read the data into the joints container as number of joints in this frame * size of joint_x
	//		file.read((char*)currentFrame.joints.data(), jointCount * sizeof(j_x));
	//		//push keyframe into animation clip
	//		animationClip.frames.push_back(currentFrame);
	//	}

	//	file.close();
	//}

	//void impl_t::TranslateJoints(kFrame& frame, float3e translation)
	//{
	//	for (int i = 0; i < frame.joints.size(); ++i)
	//	{
	//		frame.joints[i].transform.m[3][0] += translation.x;
	//		frame.joints[i].transform.m[3][1] += translation.y;
	//		frame.joints[i].transform.m[3][2] += translation.z;
	//	}
	//}

void impl_t::draw_path(std::vector<float2e> path, float4e color)
{
	for (auto& p : path) {
		float3e pos = float3e(p.x, 0.f, p.y);
		float3e des = float3e(p.x, 1.f, p.y);

		crow::debug_renderer::add_line(pos, des, color);
	}
}

void impl_t::drawJointTransform(j_x joint, float3e translation)
	{
		float3e pos = float3e(joint.transform.m[3][0] + translation.x, joint.transform.m[3][1] + translation.y,
			joint.transform.m[3][2] + translation.z);

		float3e xAxis = float3e(joint.transform.m[0][0] / 2 + pos.x, joint.transform.m[0][1] / 2 + pos.y, joint.transform.m[0][2] / 2 + pos.z);
		float3e yAxis = float3e(joint.transform.m[1][0] / 2 + pos.x, joint.transform.m[1][1] / 2 + pos.y, joint.transform.m[1][2] / 2 + pos.z);
		float3e zAxis = float3e(joint.transform.m[2][0] / 2 + pos.x, joint.transform.m[2][1] / 2 + pos.y, joint.transform.m[2][2] / 2 + pos.z);

		crow::debug_renderer::add_line(pos, xAxis, crow::float4e(1.0f, 0, 0, 1.0f));
		crow::debug_renderer::add_line(pos, yAxis, crow::float4e(0, 1.0f, 0, 1.0f));
		crow::debug_renderer::add_line(pos, zAxis, crow::float4e(0, 0, 1.0f, 1.0f));
	}

	void impl_t::drawKeyFrame(kFrame frame, float3e translation)
	{
		for (int i = 0; i < frame.joints.size(); ++i)
		{
			drawJointTransform(frame.joints[i], translation);
			int p = frame.joints[i].parent_index;
			if (p >= 0)
			{
				float3e currPos = float3e(frame.joints[i].transform.m[3][0] + translation.x,
					frame.joints[i].transform.m[3][1] + translation.y, frame.joints[i].transform.m[3][2] + translation.z);
				float3e parentPos = float3e(frame.joints[p].transform.m[3][0] + translation.x,
					frame.joints[p].transform.m[3][1] + translation.y, frame.joints[p].transform.m[3][2] + translation.z);
				crow::debug_renderer::add_line(currPos, parentPos, float4e(1, 1, 1, 1));
			}
		}
	}

	//kFrame impl_t::GetTweenFrame(aClip _animationClip, double t)
	//{
	//	double d = 0;
	//	int prev = 0;
	//	int next = 0;
	//	//the frame we will be drawing
	//	std::vector<j_x> tween_frame;
	//	tween_frame.resize(_animationClip.frames[0].joints.size());

	//	//find where t falls in
	//	for (int i = 1; i < _animationClip.frames.size() - 1; ++i)
	//	{
	//		if (t >= _animationClip.frames[i].time)
	//		{
	//			prev = i;
	//			next = i + 1;
	//		}
	//	}

	//	d = (t - _animationClip.frames[prev].time) / (_animationClip.frames[next].time - _animationClip.frames[prev].time);
	//	const kFrame& prevFrame = _animationClip.frames[prev];
	//	const kFrame& nextFrame = _animationClip.frames[next];

	//	for (int j = 0; j < tween_frame.size(); ++j)
	//	{
	//		tween_frame[j].parent_index = prevFrame.joints[j].parent_index;

	//		XMVECTOR q1 = XMQuaternionRotationMatrix(XMLoadFloat4x4(&prevFrame.joints[j].transform));
	//		XMVECTOR q2 = XMQuaternionRotationMatrix(XMLoadFloat4x4(&nextFrame.joints[j].transform));
	//		XMVECTOR qs = XMQuaternionSlerp(q1, q2, d);

	//		XMStoreFloat4x4(&tween_frame[j].transform, XMMatrixRotationQuaternion(qs));

	//		tween_frame[j].transform.m[3][0] = (nextFrame.joints[j].transform.m[3][0] - prevFrame.joints[j].transform.m[3][0])
	//			* d + prevFrame.joints[j].transform.m[3][0];

	//		tween_frame[j].transform.m[3][1] = (nextFrame.joints[j].transform.m[3][1] - prevFrame.joints[j].transform.m[3][1])
	//			* d + prevFrame.joints[j].transform.m[3][1];

	//		tween_frame[j].transform.m[3][2] = (nextFrame.joints[j].transform.m[3][2] - prevFrame.joints[j].transform.m[3][2])
	//			* d + prevFrame.joints[j].transform.m[3][2];

	//	}

	//	kFrame result;
	//	result.joints = tween_frame;
	//	result.time = t;

	//	//drawKeyFrame(result);
	//	return result;
	//}

	void impl_t::draw_cube(view_t& view)
	{
		// Cube drawing
		context->VSSetShader(vertex_shader[VERTEX_SHADER::BUFFERLESS_CUBE], nullptr, 0);
		context->PSSetShader(pixel_shader[PIXEL_SHADER::BUFFERLESS_CUBE], nullptr, 0);
		context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);

		MVP_t mvp;
		mvp.modeling = XMMatrixTranspose(XMMatrixIdentity());
		mvp.projection = view.proj_final;
		mvp.view = view.view_final;
		context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::MVP], 0, NULL, &mvp, 0, 0);

		context->Draw(36, 0);
	}

	//void impl_t::draw_terrain(view_t& view)
	//{
	//	//	//only setting necesary components
	//	//	UINT stride = sizeof(end::colored_vertex);
	//	//	UINT offset = 0;
	//	//	context->IASetVertexBuffers(0, 1, &vertex_buffer[VERTEX_BUFFER::TERRAIN_VERTEX], &stride, &offset);
	//	//	context->UpdateSubresource(vertex_buffer[VERTEX_BUFFER::TERRAIN_VERTEX], 0, nullptr, terr.data(), 0, 0);

	//	//	//context->IASetInputLayout(input_layout[INPUT_LAYOUT::COLORED_VERTEX]);

	//	//	//context->VSSetShader(vertex_shader[VERTEX_SHADER::COLORED_VERTEX], nullptr, 0);
	//	//	//context->PSSetShader(pixel_shader[PIXEL_SHADER::COLORED_VERTEX], nullptr, 0);
	//	//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	//	//context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);

	//	///*	MVP_t mvp;
	//	//	mvp.modeling = XMMatrixTranspose(XMMatrixIdentity());
	//	//	mvp.projection = XMMatrixTranspose((XMMATRIX&)view.proj_mat);
	//	//	mvp.view = XMMatrixTranspose(XMMatrixInverse(nullptr, (XMMATRIX&)view.view_mat));
	//	//	context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::MVP], 0, NULL, &mvp, 0, 0);*/

	//	//	context->Draw(vert_count, 0);
	//}

	void impl_t::draw_debug_lines(view_t& view)
	{
		// debug_line drawing
		// set vb
		UINT stride = sizeof(crow::colored_vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &vertex_buffer[VERTEX_BUFFER::COLORED_VERTEX], &stride, &offset);
		// update the vb using updatesubresource
		context->UpdateSubresource(vertex_buffer[VERTEX_BUFFER::COLORED_VERTEX], 0, nullptr, crow::debug_renderer::get_line_verts(), 0, 0);
		// set input layout
		context->IASetInputLayout(input_layout[INPUT_LAYOUT::COLORED_VERTEX]);
		// set vs
		context->VSSetShader(vertex_shader[VERTEX_SHADER::COLORED_VERTEX], nullptr, 0);
		// set a cb in the vs
		context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);
		// set ps
		context->PSSetShader(pixel_shader[PIXEL_SHADER::COLORED_VERTEX], nullptr, 0);
		// set topology (line_list)
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		// update the cb using updatesubresource
		MVP_t mvp;
		mvp.modeling = XMMatrixTranspose(XMMatrixIdentity());
		mvp.projection = view.proj_final;
		mvp.view = view.view_final;
		context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::MVP], 0, NULL, &mvp, 0, 0);
		// draw!
		context->Draw(crow::debug_renderer::get_line_vert_count(), 0);
		crow::debug_renderer::clear_lines();
	}

	//void impl_t::draw_mesh(view_t& view)
	//{
	//	UINT stride = sizeof(crow::mesh_vertex);
	//	UINT offset = 0;

	//	context->IASetVertexBuffers(0, 1, &vertex_buffer[VERTEX_BUFFER::ANIM_MESH], &stride, &offset);
	//	context->UpdateSubresource(vertex_buffer[VERTEX_BUFFER::ANIM_MESH], 0, nullptr, meshVerts.data(), 0, 0);
	//	context->IASetInputLayout(input_layout[INPUT_LAYOUT::ANIM_MESH]);
	//	context->VSSetShader(vertex_shader[VERTEX_SHADER::ANIM_MESH], nullptr, 0);
	//	context->PSSetShader(pixel_shader[PIXEL_SHADER::ANIM_MESH], nullptr, 0);
	//	context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::ANIM_MESH]);
	//	context->IASetIndexBuffer(index_buffer[INDEX_BUFFER::ANIM_MESH], DXGI_FORMAT_R32_UINT, 0);

	//	context->PSSetShaderResources(0, 1, &sResourceView[SUBRESOURCE_VIEW::DIFFUSE]);
	//	context->PSSetShaderResources(1, 1, &sResourceView[SUBRESOURCE_VIEW::EMISSIVE]);
	//	context->PSSetShaderResources(2, 1, &sResourceView[SUBRESOURCE_VIEW::SPECULAR]);
	//	context->PSSetSamplers(0, 1, &samplerState[STATE_SAMPLER::DEFAULT]);

	//	meshCB.modeling = XMMatrixTranspose(XMMatrixIdentity());
	//	meshCB.projection = view.proj_final;
	//	meshCB.view = view.view_final;
	//	context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::ANIM_MESH], 0, NULL, &meshCB, 0, 0);

	//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	//time to draw, please work
	//	context->DrawIndexed((int)meshInds.size(), 0, 0);

	//}

	void impl_t::draw_entities(crow::entities& entities, std::vector<size_t> inds, view_t view)
	{
		MCB_s s_buff;
		MCB_t a_buff;

		a_buff.projection = s_buff.projection = view.proj_final;
		a_buff.view = s_buff.view = view.view_final;

		for (int i = 0; i < inds.size(); i++) {
			bool animated = (entities.mesh_ptrs[inds[i]]->a_mesh != nullptr);
			UINT offset = 0;
			UINT stride = 0;
			if (!animated) {
				// binding
				stride = sizeof(crow::vert_s);
				context->IASetVertexBuffers(0, 1, &entities.mesh_ptrs[inds[i]]->vertex_buffer, &stride, &offset);
				context->UpdateSubresource(entities.mesh_ptrs[inds[i]]->vertex_buffer, 0, nullptr, entities.mesh_ptrs[inds[i]]->s_mesh->vertices.data(), 0, 0);

				context->IASetInputLayout(input_layout[INPUT_LAYOUT::STATIC_MESH]);
				context->VSSetShader(vertex_shader[VERTEX_SHADER::STATIC_MESH], nullptr, 0);
				context->PSSetShader(pixel_shader[PIXEL_SHADER::STATIC_MESH], nullptr, 0);
				context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::STATIC_MESH]);
				context->IASetIndexBuffer(entities.mesh_ptrs[inds[i]]->index_buffer, DXGI_FORMAT_R32_UINT, 0);

				if (entities.s_resource_view[inds[i]]) {
					context->PSSetShaderResources(0, 1, &entities.s_resource_view[inds[i]]);
				}
				if (entities.emissive[inds[i]]) {
					context->PSSetShaderResources(1, 1, &entities.emissive[inds[i]]);
				}
				if (entities.specular[inds[i]]) {
					context->PSSetShaderResources(2, 1, &entities.specular[inds[i]]);
				}
				context->PSSetSamplers(0, 1, &samplerState[STATE_SAMPLER::DEFAULT]);

				s_buff.world = XMMatrixTranspose(entities.world_matrix[inds[i]]);
				context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::STATIC_MESH], 0, NULL, &s_buff, 0, 0);

				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				// drawing
				context->DrawIndexed((int)entities.mesh_ptrs[inds[i]]->s_mesh->indicies.size(), 0, 0);

			}
			else {
				// binding
				stride = sizeof(crow::vert_a);
				context->IASetVertexBuffers(0, 1, &entities.mesh_ptrs[inds[i]]->vertex_buffer, &stride, &offset);
				context->UpdateSubresource(entities.mesh_ptrs[inds[i]]->vertex_buffer, 0, nullptr, entities.mesh_ptrs[inds[i]]->a_mesh->vertices.data(), 0, 0);

				context->IASetInputLayout(input_layout[INPUT_LAYOUT::ANIM_MESH]);
				context->VSSetShader(vertex_shader[VERTEX_SHADER::ANIM_MESH], nullptr, 0);
				context->PSSetShader(pixel_shader[PIXEL_SHADER::ANIM_MESH], nullptr, 0);
				context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::ANIM_MESH]);
				context->IASetIndexBuffer(entities.mesh_ptrs[inds[i]]->index_buffer, DXGI_FORMAT_R32_UINT, 0);

				if (entities.s_resource_view[inds[i]]) {
					context->PSSetShaderResources(0, 1, &entities.s_resource_view[inds[i]]);
				}
				if (entities.emissive[inds[i]]) {
					context->PSSetShaderResources(1, 1, &entities.emissive[inds[i]]);
				}
				if (entities.specular[inds[i]]) {
					context->PSSetShaderResources(2, 1, &entities.specular[inds[i]]);
				}

				a_buff.modeling = XMMatrixTranspose(entities.world_matrix[inds[i]]);

				// TODO: adjust this index to accomodate for future animations
				for (size_t j = 0; j < 30; ++j) {
					a_buff.matrices[j] = entities.framexbind[inds[i]][j];
				}
				context->PSSetSamplers(0, 1, &samplerState[STATE_SAMPLER::DEFAULT]);
				context->UpdateSubresource(constant_buffer[CONSTANT_BUFFER::ANIM_MESH], 0, NULL, &a_buff, 0, 0);

				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				// drawing
				context->DrawIndexed((int)entities.mesh_ptrs[inds[i]]->a_mesh->indicies.size(), 0, 0);
			}
		}
	}

	//bool impl_t::InputUpdate(float delta, std::bitset<256> bm, float dx, float dy)
	//{
	//	float3e prev = float3e(a1.m[3][0], a1.m[3][1], a1.m[3][2]);
	//	XMMATRIX temp = XMLoadFloat4x4(&a1);

	//	//rotation
	//	if (bm[(int)VK_LEFT] == 1)
	//		temp = XMMatrixMultiply(XMMatrixRotationY(-delta), temp);

	//	if (bm[(int)VK_RIGHT] == 1)
	//		temp = XMMatrixMultiply(XMMatrixRotationY(delta), temp);

	//	//translation
	//	if (bm[(int)VK_UP] == 1)
	//		temp = XMMatrixMultiply(XMMatrixTranslation(0, 0, 5.0f * delta), temp);
	//	if (bm[(int)VK_DOWN] == 1)
	//		temp = XMMatrixMultiply(XMMatrixTranslation(0, 0, 5.0f * -delta), temp);

	//	if (bm[(int)'N'] == 1)
	//	{
	//		keyframeIndex += 1;
	//		//std::cout << "current key frame: " << keyframeIndex << std::endl;
	//	}

	//	if (bm[(int)'B'] == 1)
	//		b_on = !b_on;

	//	XMStoreFloat4x4(&a1, temp);
	//	float3e curr = float3e(a1.m[3][0], a1.m[3][1], a1.m[3][2]);
	//	if (curr.x != prev.x || curr.y != prev.y || curr.z != prev.z)
	//		return true;

	//	return false;
	//}

	void impl_t::update(float delta) {
		/*aTime += delta;
		if (aTime >= animationClip.duration)
			aTime = animationClip.frames[1].time;*/

		//keyFrame tweenFrame = animationClip.frames[keyframeIndex];
		//kFrame tweenFrame = GetTweenFrame(animationClip, aTime);
		//drawKeyFrame(tweenFrame, float3e(5, 0, 0));

		/*meshCB.lightColor = float3(1.0f, 0.0f, 0.0f);
		meshCB.lightPos = float3(0.0f, 0.0f, 0.0f);
		meshCB.lightPower = 55.0f;*/
		//meshCB.surfaceShininess = 1.0f;

		//meshCB.matrices;

		//for (int i = 0; i < tweenFrame.joints.size(); ++i)
		//{
		//	XMMATRIX cMatrix = XMMatrixMultiply(InvBindPose[i], XMLoadFloat4x4(&tweenFrame.joints[i].transform));
		//	meshCB.matrices[i] = XMMatrixTranspose(cMatrix);
		//}

		//DrawGrid();
	}

	//void impl_t::update(float delta, std::bitset<256> bm, float dx = 0.f, float dy = 0.f)
	//{

	//	InputUpdate(delta, bm, dx, dy);
	//	//vbox.center = (float3e&)a1.m[3];

	//	//add_aabb(vbox, float4e(0, 0, 1, 1));

	//	/*if (keyframeIndex >= animationClip.frames.size())
	//		keyframeIndex = 0;*/

	//		/*if (kfi >= animationClip.frames.size())
	//			kfi = 0;*/

	//	aTime += delta;
	//	if (aTime >= animationClip.duration)
	//		aTime = animationClip.frames[1].time;

	//	//keyFrame tweenFrame = animationClip.frames[keyframeIndex];
	//	keyFrame tweenFrame = GetTweenFrame(animationClip, aTime);
	//	drawKeyFrame(tweenFrame, float3e(5, 0, 0));

	//	meshCB.lightColor = float3(1.0f, 0.0f, 0.0f);
	//	meshCB.lightPos = float3(0.0f, 0.0f, 0.0f);
	//	meshCB.lightPower = 55.0f;
	//	meshCB.surfaceShininess = 1.0f;

	//	meshCB.matrices;

	//	for (int i = 0; i < tweenFrame.joints.size(); ++i)
	//	{
	//		XMMATRIX cMatrix = XMMatrixMultiply(InvBindPose[i], XMLoadFloat4x4(&tweenFrame.joints[i].transform));
	//		meshCB.matrices[i] = XMMatrixTranspose(cMatrix);
	//	}

	//	DrawGrid();

	//}

	impl_t::~impl_t()
	{
		// TODO:
		//Clean-up
		//
		// In general, release objects in reverse order of creation

		for (auto& ptr : constant_buffer)
			safe_release(ptr);

		for (auto& ptr : pixel_shader)
			safe_release(ptr);

		for (auto& ptr : vertex_shader)
			safe_release(ptr);

		for (auto& ptr : input_layout)
			safe_release(ptr);

		for (auto& ptr : index_buffer)
			safe_release(ptr);

		for (auto& ptr : vertex_buffer)
			safe_release(ptr);

		for (auto& ptr : rasterState)
			safe_release(ptr);

		for (auto& ptr : depthStencilState)
			safe_release(ptr);

		for (auto& ptr : depthStencilView)
			safe_release(ptr);

		for (auto& ptr : render_target)
			safe_release(ptr);

		for (auto& ptr : samplerState)
			safe_release(ptr);

		for (auto& ptr : sResourceView)
			safe_release(ptr);

		safe_release(context);
		safe_release(swapchain);
		safe_release(device);

		// ImGui deinit
		ImGui_ImplDX11_Shutdown();
		ImGui::DestroyContext();
	}

	void impl_t::set_render_target_view()
	{
		const float4e black{ 0.0f, 0.0f, 0.0f, 1.0f };

		context->OMSetDepthStencilState(depthStencilState[STATE_DEPTH_STENCIL::DEFAULT], 1);
		context->OMSetRenderTargets(1, &render_target[VIEW_RENDER_TARGET::DEFAULT], depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);

		context->ClearRenderTargetView(render_target[VIEW_RENDER_TARGET::DEFAULT], black.data());
		context->ClearDepthStencilView(depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		context->RSSetState(rasterState[STATE_RASTERIZER::DEFAULT]);
		context->RSSetViewports(1, &view_port[VIEWPORT::DEFAULT]);
	}

	void impl_t::present(unsigned int vsync)
	{
		swapchain->Present(vsync, 0);
	}

	void impl_t::create_device_and_swapchain()
	{
		RECT crect;
		GetClientRect(hwnd, &crect);

		// Setup the viewport
		D3D11_VIEWPORT& vp = view_port[VIEWPORT::DEFAULT];

		vp.Width = (float)crect.right;
		vp.Height = (float)crect.bottom;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		// Setup swapchain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = crect.right;
		sd.BufferDesc.Height = crect.bottom;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		D3D_FEATURE_LEVEL  FeatureLevelsSupported;

		const D3D_FEATURE_LEVEL lvl[] =
		{
			D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_2, D3D_FEATURE_LEVEL_9_1
		};

		UINT createDeviceFlags = 0;

#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, lvl, _countof(lvl), D3D11_SDK_VERSION, &sd, &swapchain, &device, &FeatureLevelsSupported, &context);

		if (hr == E_INVALIDARG)
		{
			hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &lvl[1], _countof(lvl) - 1, D3D11_SDK_VERSION, &sd, &swapchain, &device, &FeatureLevelsSupported, &context);
		}

		assert(!FAILED(hr));

		// ALL IMGUI SETUP GOES HERE
		ImGui::CreateContext();
		ImGui_ImplDX11_Init(device, context);
		// this prevents imgui from making an ini file.
        ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;
	}

	void impl_t::create_main_render_target()
	{
		ID3D11Texture2D* pBackBuffer;
		// Get a pointer to the back buffer
		HRESULT hr = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			(LPVOID*)&pBackBuffer);

		assert(!FAILED(hr));

		// Create a render-target view
		device->CreateRenderTargetView(pBackBuffer, NULL,
			&render_target[VIEW_RENDER_TARGET::DEFAULT]);

		pBackBuffer->Release();
	}

	void impl_t::setup_depth_stencil()
	{
		/* DEPTH_BUFFER */
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		ID3D11Texture2D* depthStencilBuffer;

		ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

		depthBufferDesc.Width = (UINT)view_port[VIEWPORT::DEFAULT].Width;
		depthBufferDesc.Height = (UINT)view_port[VIEWPORT::DEFAULT].Height;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		HRESULT hr = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);

		assert(!FAILED(hr));

		/* DEPTH_STENCIL */
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;

		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);

		assert(!FAILED(hr));

		depthStencilBuffer->Release();

		/* DEPTH_STENCIL_DESC */
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState[STATE_DEPTH_STENCIL::DEFAULT]);

		assert(!FAILED(hr));
	}

	void impl_t::setup_rasterizer()
	{
		D3D11_RASTERIZER_DESC rasterDesc;

		ZeroMemory(&rasterDesc, sizeof(rasterDesc));

		rasterDesc.AntialiasedLineEnable = true;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = false;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		HRESULT hr = device->CreateRasterizerState(&rasterDesc, &rasterState[STATE_RASTERIZER::DEFAULT]);

		assert(!FAILED(hr));
	}

	void impl_t::create_shaders_and_input_layout()
	{
		//shaders for cube
		binary_blob_t vs_blob = load_binary_blob("vs_cube.cso");
		binary_blob_t ps_blob = load_binary_blob("ps_cube.cso");

		HRESULT hr = device->CreateVertexShader(vs_blob.data(), vs_blob.size(), NULL, &vertex_shader[VERTEX_SHADER::BUFFERLESS_CUBE]);
		assert(!FAILED(hr));

		hr = device->CreatePixelShader(ps_blob.data(), ps_blob.size(), NULL, &pixel_shader[PIXEL_SHADER::BUFFERLESS_CUBE]);
		assert(!FAILED(hr));

		//shaders and input layout for colored vertex
		{
			binary_blob_t vs_colored_vertex_blob = load_binary_blob("vs_coloredvertex.cso");
			binary_blob_t ps_colored_vertex_blob = load_binary_blob("ps_coloredvertex.cso");

			hr = device->CreateVertexShader(vs_colored_vertex_blob.data(), vs_colored_vertex_blob.size(), NULL, &vertex_shader[VERTEX_SHADER::COLORED_VERTEX]);
			assert(!FAILED(hr));

			hr = device->CreatePixelShader(ps_colored_vertex_blob.data(), ps_colored_vertex_blob.size(), NULL, &pixel_shader[PIXEL_SHADER::COLORED_VERTEX]);
			assert(!FAILED(hr));


			D3D11_INPUT_ELEMENT_DESC ILdesc[] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			hr = device->CreateInputLayout(ILdesc, ARRAYSIZE(ILdesc), vs_colored_vertex_blob.data(), vs_colored_vertex_blob.size(), &input_layout[INPUT_LAYOUT::COLORED_VERTEX]);
		}

		//shaders and input layout for fbx mesh
		{
			binary_blob_t vs_mesh_vertex_blob = load_binary_blob("vs_meshvertex.cso");
			binary_blob_t ps_mesh_vertex_blob = load_binary_blob("ps_meshvertex.cso");

			hr = device->CreateVertexShader(vs_mesh_vertex_blob.data(), vs_mesh_vertex_blob.size(), NULL, &vertex_shader[VERTEX_SHADER::ANIM_MESH]);
			assert(!FAILED(hr));

			hr = device->CreatePixelShader(ps_mesh_vertex_blob.data(), ps_mesh_vertex_blob.size(), NULL, &pixel_shader[PIXEL_SHADER::ANIM_MESH]);
			assert(!FAILED(hr));

			D3D11_INPUT_ELEMENT_DESC mILdesc[] =
			{
				{"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			hr = device->CreateInputLayout(mILdesc, ARRAYSIZE(mILdesc), vs_mesh_vertex_blob.data(), vs_mesh_vertex_blob.size(), &input_layout[INPUT_LAYOUT::ANIM_MESH]);
		}

		// TODO: CREATE INPUT LAYOUT FOR STATIC MESH
		// TODO: CREATE VERTEX PIXEL SHADER FOR STATIC MESH
		{
			binary_blob_t vs_mesh_vertex_blob = load_binary_blob("vs_smeshvertex.cso");
			binary_blob_t ps_mesh_vertex_blob = load_binary_blob("ps_smeshvertex.cso");

			hr = device->CreateVertexShader(vs_mesh_vertex_blob.data(), vs_mesh_vertex_blob.size(), NULL, &vertex_shader[VERTEX_SHADER::STATIC_MESH]);
			assert(!FAILED(hr));

			hr = device->CreatePixelShader(ps_mesh_vertex_blob.data(), ps_mesh_vertex_blob.size(), NULL, &pixel_shader[PIXEL_SHADER::STATIC_MESH]);
			assert(!FAILED(hr));

			D3D11_INPUT_ELEMENT_DESC mILdesc[] =
			{
				{"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};

			hr = device->CreateInputLayout(mILdesc, ARRAYSIZE(mILdesc), vs_mesh_vertex_blob.data(), vs_mesh_vertex_blob.size(), &input_layout[INPUT_LAYOUT::STATIC_MESH]);
		}
	}

	void impl_t::create_constant_buffers()
	{
		D3D11_BUFFER_DESC mvp_bd;
		ZeroMemory(&mvp_bd, sizeof(mvp_bd));

		mvp_bd.Usage = D3D11_USAGE_DEFAULT;
		mvp_bd.ByteWidth = sizeof(MVP_t);
		mvp_bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		mvp_bd.CPUAccessFlags = 0;

		HRESULT hr = device->CreateBuffer(&mvp_bd, NULL, &constant_buffer[CONSTANT_BUFFER::MVP]);

		//may need a new constant buffer for my fbx project
		D3D11_BUFFER_DESC mcb_bd;
		ZeroMemory(&mcb_bd, sizeof(mcb_bd));

		mcb_bd.Usage = D3D11_USAGE_DEFAULT;
		mcb_bd.ByteWidth = sizeof(MCB_t);
		mcb_bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		mcb_bd.CPUAccessFlags = 0;

		hr = device->CreateBuffer(&mcb_bd, NULL, &constant_buffer[CONSTANT_BUFFER::ANIM_MESH]);

		D3D11_BUFFER_DESC mcbs_bd;
		ZeroMemory(&mcbs_bd, sizeof(mcbs_bd));

		mcbs_bd.Usage = D3D11_USAGE_DEFAULT;
		mcbs_bd.ByteWidth = sizeof(MCB_s);
		mcbs_bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		mcbs_bd.CPUAccessFlags = 0;

		hr = device->CreateBuffer(&mcbs_bd, NULL, &constant_buffer[CONSTANT_BUFFER::STATIC_MESH]);
	}

	void impl_t::create_vertex_buffers()
	{
		HRESULT hr;
		//buffer for debug lines
		CD3D11_BUFFER_DESC desc = CD3D11_BUFFER_DESC(
			sizeof(crow::colored_vertex) * crow::debug_renderer::get_line_vert_capacity(),
			D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA srd = { 0 };
		srd.pSysMem = crow::debug_renderer::get_line_verts();

		hr = device->CreateBuffer(&desc, &srd, &vertex_buffer[VERTEX_BUFFER::COLORED_VERTEX]);

		////buffer for terrain mesh
		//CD3D11_BUFFER_DESC desc2 = CD3D11_BUFFER_DESC(
		//	sizeof(end::colored_vertex) * vert_count,
		//	D3D11_BIND_VERTEX_BUFFER);
		//D3D11_SUBRESOURCE_DATA srd2 = { 0 };
		//srd2.pSysMem = terr.data();

		//hr = device->CreateBuffer(&desc2, &srd2, &vertex_buffer[VERTEX_BUFFER::TERRAIN_VERTEX]);

		//{
		//	//vertex buffer for fbx models
		//	CD3D11_BUFFER_DESC desc3 = CD3D11_BUFFER_DESC(
		//		sizeof(crow::mesh_vertex) * meshVerts.size(),
		//		D3D11_BIND_VERTEX_BUFFER);
		//	D3D11_SUBRESOURCE_DATA srd3 = { 0 };
		//	srd3.pSysMem = meshVerts.data();

		//	hr = device->CreateBuffer(&desc3, &srd3, &vertex_buffer[VERTEX_BUFFER::ANIM_MESH]);

		//	//Index Buffer for fbx model
		//	D3D11_SUBRESOURCE_DATA iData = {};
		//	iData.pSysMem = meshInds.data();
		//	CD3D11_BUFFER_DESC iDesc = {};
		//	iDesc.Usage = D3D11_USAGE_DEFAULT;
		//	iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		//	iDesc.CPUAccessFlags = 0;
		//	iDesc.ByteWidth = sizeof(uint32_t) * (int)meshInds.size();

		//	hr = device->CreateBuffer(&iDesc, &iData, &index_buffer[INDEX_BUFFER::ANIM_MESH]);
		//}
	}

	void impl_t::create_vertex_buffer(ID3D11Buffer*& vertex_buffer, ID3D11Buffer*& index_buffer, crow::mesh_a& mesh)
	{
		HRESULT hr;
		//vertex buffer for fbx models
		CD3D11_BUFFER_DESC desc = CD3D11_BUFFER_DESC(
			sizeof(crow::vert_a) * mesh.vertices.size(),
			D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA srd = { 0 };
		srd.pSysMem = mesh.vertices.data();

		hr = device->CreateBuffer(&desc, &srd, &vertex_buffer);

		//Index Buffer for fbx model
		D3D11_SUBRESOURCE_DATA iData = {};
		iData.pSysMem = mesh.indicies.data();
		CD3D11_BUFFER_DESC iDesc = {};
		iDesc.Usage = D3D11_USAGE_DEFAULT;
		iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iDesc.CPUAccessFlags = 0;
		iDesc.ByteWidth = sizeof(uint32_t) * (int)mesh.indicies.size();

		hr = device->CreateBuffer(&iDesc, &iData, &index_buffer);
	}

	void impl_t::create_vertex_buffer(ID3D11Buffer*& vertex_buffer, ID3D11Buffer*& index_buffer, crow::mesh_s& mesh)
	{
		HRESULT hr;
		//vertex buffer for fbx models
		CD3D11_BUFFER_DESC desc = CD3D11_BUFFER_DESC(
			sizeof(crow::vert_s) * mesh.vertices.size(),
			D3D11_BIND_VERTEX_BUFFER);
		D3D11_SUBRESOURCE_DATA srd = { 0 };
		srd.pSysMem = mesh.vertices.data();

		hr = device->CreateBuffer(&desc, &srd, &vertex_buffer);

		//Index Buffer for fbx model
		D3D11_SUBRESOURCE_DATA iData = {};
		iData.pSysMem = mesh.indicies.data();
		CD3D11_BUFFER_DESC iDesc = {};
		iDesc.Usage = D3D11_USAGE_DEFAULT;
		iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iDesc.CPUAccessFlags = 0;
		iDesc.ByteWidth = sizeof(uint32_t) * (int)mesh.indicies.size();

		hr = device->CreateBuffer(&iDesc, &iData, &index_buffer);
	}

	void impl_t::CreateTextures_old(std::string diff_filename, std::string emis_filename, std::string spec_filename)
	{
		// load texture here -------------------------
		//diffuse texture
		std::wstring wstr = std::wstring(diff_filename.begin(), diff_filename.end());
		const wchar_t* wcstrD = wstr.c_str();
		HRESULT hr = CreateDDSTextureFromFile(device, wcstrD, nullptr, &sResourceView[SUBRESOURCE_VIEW::DIFFUSE]);

		//emissive texture
		wstr = std::wstring(emis_filename.begin(), emis_filename.end());
		const wchar_t* wcstrE = wstr.c_str();
		hr = CreateDDSTextureFromFile(device, wcstrE, nullptr, &sResourceView[SUBRESOURCE_VIEW::EMISSIVE]);

		//specular texture
		wstr = std::wstring(spec_filename.begin(), spec_filename.end());
		const wchar_t* wcstrS = wstr.c_str();
		hr = CreateDDSTextureFromFile(device, wcstrS, nullptr, &sResourceView[SUBRESOURCE_VIEW::SPECULAR]);
	}

	void impl_t::create_texture(std::string filename, ID3D11ShaderResourceView*& sresourceview)
	{
		std::wstring wstr = std::wstring(filename.begin(), filename.end());
		const wchar_t* wcstrD = wstr.c_str();
		HRESULT hr = CreateDDSTextureFromFile(device, wcstrD, nullptr, &sresourceview);
	}

	// we are loading textures individually now and storing them in the texture container
	void impl_t::create_text_sresources(std::vector<std::string> text_filenames, mesh_info& m)
	{
		/*for (size_t i = 0; i < text_filenames.size(); ++i) {
			std::wstring wstr = std::wstring(text_filenames[i].begin(), text_filenames[i].end());
			const wchar_t* wcstrD = wstr.c_str();
			HRESULT hr = CreateDDSTextureFromFile(device, wcstrD, nullptr, &s_resource_view[i]);
		}*/
		/*if (!text_filenames.empty())
		{
			std::wstring wstr = std::wstring(text_filenames[0].begin(), text_filenames[0].end());
			const wchar_t* wcstrD = wstr.c_str();
			HRESULT hr = CreateDDSTextureFromFile(device, wcstrD, nullptr, &m.s_resource_view);

			if (text_filenames.size() > 1) {
				std::wstring wstr = std::wstring(text_filenames[1].begin(), text_filenames[1].end());
				const wchar_t* wcstrD = wstr.c_str();
				HRESULT hr = CreateDDSTextureFromFile(device, wcstrD, nullptr, &m.emissive);
			}
			if (text_filenames.size() > 2) {
				std::wstring wstr = std::wstring(text_filenames[2].begin(), text_filenames[2].end());
				const wchar_t* wcstrD = wstr.c_str();
				HRESULT hr = CreateDDSTextureFromFile(device, wcstrD, nullptr, &m.specular);
			}
		}*/
	}

	void impl_t::create_imgui_texture(std::string filename, ID3D11ShaderResourceView*& /* ?????????????????????? */ texture) {
		std::wstring wstr = std::wstring(filename.begin(), filename.end());
		const wchar_t* wcstrD = wstr.c_str();
		HRESULT hr = CreateDDSTextureFromFile(device, wcstrD, nullptr, &texture);
		std::string message = std::system_category().message(hr);
	}

	void impl_t::CreateSamplerState()
	{
		//Creating sampler
		D3D11_SAMPLER_DESC sd = {};
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		HRESULT hr = device->CreateSamplerState(&sd, &samplerState[STATE_SAMPLER::DEFAULT]);
	}

	/*float impl_t::ManhDistance(float3e a, float3e b)
	{
		return fabsf(a.x - b.x) + fabsf(a.y - b.y) + fabsf(a.z - b.z);
	}*/

	//aabb_t impl_t::ComputeBounds(aabb_t a, aabb_t b)
	//{
	//	float3e min, max;
	//	/*max.x = curr->aabb().extents.x > n.aabb().extents.x ? curr->aabb().extents.x : n.aabb().extents.x;
	//	max.y = curr->aabb().extents.y > n.aabb().extents.y ? curr->aabb().extents.y : n.aabb().extents.y;
	//	max.x = curr->aabb().extents.z > n.aabb().extents.z ? curr->aabb().extents.z : n.aabb().extents.z;*/
	//	aabb_t result;

	//	float3e c, d;
	//	c = a.center + a.extents;
	//	d = b.center + b.extents;
	//	max.x = c.x > d.x ? c.x : d.x;
	//	max.y = c.y > d.y ? c.y : d.y;
	//	max.z = c.z > d.z ? c.z : d.z;

	//	c = a.center - a.extents;
	//	d = b.center - b.extents;
	//	min.x = c.x < d.x ? c.x : d.x;
	//	min.y = c.y < d.y ? c.y : d.y;
	//	min.z = c.z < d.z ? c.z : d.z;

	//	result.center = (min + max) / 2.0f;
	//	result.extents = max - result.center;

	//	return result;
	//}

	//bool impl_t::CheckCollision(aabb_t a, aabb_t b)
	//{
	//	float3e mina = a.center - a.extents;
	//	float3e minb = b.center - b.extents;

	//	float3e maxa = a.center + a.extents;
	//	float3e maxb = b.center + b.extents;

	//	if (maxa.x < minb.x || mina.x > maxb.x) return false;
	//	if (maxa.y < minb.y || mina.y > maxb.y) return false;
	//	if (maxa.z < minb.z || mina.z > maxb.z) return false;

	//	return true;
	//}

	void impl_t::drawXMatrix(XMFLOAT4X4 M)
	{
		crow::float3e pos = crow::float3e(M.m[3][0], M.m[3][1], M.m[3][2]);

		crow::float3e xAxis = crow::float3e(M.m[0][0] + pos.x, M.m[0][1] + pos.y, M.m[0][2] + pos.z);
		crow::float3e yAxis = crow::float3e(M.m[1][0] + pos.x, M.m[1][1] + pos.y, M.m[1][2] + pos.z);
		crow::float3e zAxis = crow::float3e(M.m[2][0] + pos.x, M.m[2][1] + pos.y, M.m[2][2] + pos.z);
		crow::debug_renderer::add_line(pos, xAxis, crow::float4e(1.0f, 0, 0, 1.0f));
		crow::debug_renderer::add_line(pos, yAxis, crow::float4e(0, 1.0f, 0, 1.0f));
		crow::debug_renderer::add_line(pos, zAxis, crow::float4e(0, 0, 1.0f, 1.0f));
	}

	//void impl_t::add_aabb(aabb_t box, float4e color)
	//{
	//	//when values are 0 data gets corrupted, I will probably fix it? this mea
	//	if (box.extents.y == 0)
	//	{
	//		float3e p[4];
	//		int w = 0;
	//		for (float x = -box.extents.x; x <= box.extents.x; x += box.extents.x * 2)
	//		{
	//			for (float z = -box.extents.z; z <= box.extents.z; z += box.extents.z * 2)
	//			{
	//				p[w++] = float3e(x + box.center.x, box.center.y, z + box.center.z);
	//			}
	//		}

	//		/*for (int j = 0; j < 4; ++j)
	//		{

	//		}*/

	//		crow::debug_renderer::add_line(p[0], p[1],
	//			color);

	//		crow::debug_renderer::add_line(p[0], p[2],
	//			color);

	//		crow::debug_renderer::add_line(p[2], p[3],
	//			color);

	//		crow::debug_renderer::add_line(p[1], p[3],
	//			color);

	//		return;
	//	}


	//	//std::vector<float3> p;
	//	float3e points[8];
	//	int i = 0;
	//	for (float x = -box.extents.x; x <= box.extents.x; x += box.extents.x * 2)
	//	{
	//		for (float y = -box.extents.y; y <= box.extents.y; y += box.extents.y * 2)
	//		{
	//			for (float z = -box.extents.z; z <= box.extents.z; z += box.extents.z * 2)
	//			{
	//				if (i >= 8)
	//					int yeet = 7;
	//				//p.push_back(float3(x + box.center.x, y + box.center.y, z + box.center.z));
	//				points[i++] = float3e(x + box.center.x, y + box.center.y, z + box.center.z);
	//			}
	//		}
	//	}

	//	//now draw lines between points
	//	for (int j = 0; j < 8; ++j)
	//	{
	//		//draw z axis lines
	//		if (/*j % 2 == 0*/ j == 0 || j == 2 || j == 4 || j == 6)
	//		{
	//			crow::debug_renderer::add_line(points[j], points[j + 1],
	//				color);
	//		}
	//		//draw x axis lines
	//		if (j <= 3)
	//		{
	//			crow::debug_renderer::add_line(points[j], points[j + 4],
	//				color);
	//		}
	//		//draw y axis lines
	//		if (/*points[j].y < box.center.y*/ j == 0 || j == 1 || j == 4 || j == 5)
	//		{
	//			if (j + 2 >= 8)
	//				int nFrenchFries = 0;

	//			crow::debug_renderer::add_line(points[j], points[j + 2],
	//				color);
	//		}
	//	}
	//}

	void impl_t::VecToRow(XMFLOAT4X4* m, XMFLOAT3 v, int row)
	{
		row -= 1;
		m->m[row][0] = v.x;
		m->m[row][1] = v.y;
		m->m[row][2] = v.z;
	}

	XMFLOAT4X4 impl_t::LookAtM(XMFLOAT4X4 a, XMFLOAT4X4 b)
	{
		//z axis
		for (int i = 0; i < 3; ++i)
		{
			a.m[2][i] = b.m[3][i] - a.m[3][i];
		}
		XMFLOAT3 zf3 = XMFLOAT3(a.m[2][0], a.m[2][1], a.m[2][2]);
		XMVECTOR zv = XMLoadFloat3(&zf3);
		zv = XMVector3Normalize(zv);
		XMStoreFloat3(&zf3, zv);

		//x axis
		XMFLOAT3 up = XMFLOAT3(0, 1.0f, 0);
		XMVECTOR upv = XMLoadFloat3(&up);
		XMVECTOR xv = XMVector3Cross(upv, zv);
		xv = XMVector3Normalize(xv);

		//y axis
		XMVECTOR yv = XMVector3Cross(zv, xv);
		yv = XMVector3Normalize(yv);

		XMFLOAT3 xf3, yf3;
		XMStoreFloat3(&xf3, xv);
		XMStoreFloat3(&yf3, yv);

		VecToRow(&a, xf3, 1);
		VecToRow(&a, yf3, 2);
		VecToRow(&a, zf3, 3);

		return a;
	}

	XMFLOAT4X4 impl_t::TurnToM(XMFLOAT4X4 a, XMFLOAT4X4 b, float delta)
	{
		//vector pointing to targe
		XMFLOAT3 target = XMFLOAT3((b.m[3][0] - a.m[3][0]), (b.m[3][1] - a.m[3][1]), (b.m[3][2] - a.m[3][2]));
		XMVECTOR td = XMLoadFloat3(&target);

		XMFLOAT3 nx = XMFLOAT3(a.m[0][0], a.m[0][1], a.m[0][2]);
		XMVECTOR nxv = XMLoadFloat3(&nx);

		XMFLOAT3 ny = XMFLOAT3(a.m[1][0], a.m[1][1], a.m[1][2]);
		XMVECTOR nyv = XMLoadFloat3(&ny);

		XMMATRIX rotM = XMMatrixIdentity();
		float d = XMVectorGetX(XMVector3Dot(td, nyv));
		if (d != 0)
		{
			//rotate a by d on x axis
			rotM = XMMatrixMultiply(XMMatrixRotationX(-d * delta), rotM);
		}

		d = XMVectorGetX(XMVector3Dot(td, nxv));
		if (d != 0)
		{
			//rotate a by d on y axis
			rotM = XMMatrixMultiply(XMMatrixRotationY(d * delta), rotM);
		}

		XMMATRIX temp = XMLoadFloat4x4(&a);
		temp = XMMatrixMultiply(rotM, temp);
		XMStoreFloat4x4(&a, temp);

		XMFLOAT3 zf3 = XMFLOAT3(a.m[2][0], a.m[2][1], a.m[2][2]);
		XMVECTOR zv = XMLoadFloat3(&zf3);
		zv = XMVector3Normalize(zv);
		XMStoreFloat3(&zf3, zv);

		//x axis
		XMFLOAT3 up = XMFLOAT3(0, 1.0f, 0);
		XMVECTOR upv = XMLoadFloat3(&up);
		XMVECTOR xv = XMVector3Cross(upv, zv);
		xv = XMVector3Normalize(xv);

		//y axis
		XMVECTOR yv = XMVector3Cross(zv, xv);
		yv = XMVector3Normalize(yv);

		XMFLOAT3 xf3, yf3;
		XMStoreFloat3(&xf3, xv);
		XMStoreFloat3(&yf3, yv);

		VecToRow(&a, xf3, 1);
		VecToRow(&a, yf3, 2);
		VecToRow(&a, zf3, 3);

		return a;
	}

	void impl_t::DrawGrid()
	{
		const float size = 50;
		const float spacing = 2;
		const float lines = size / spacing;
		float dxz = -size / 2;
		//float z = dxz;
		const float cz = dxz;
		const float cx = dxz;
		int i = 0;

		for (int p = 0; p < lines; ++p)
		{
			//linex
			crow::debug_renderer::add_line(
				crow::float3e(cx, 0.0f, dxz), // first point
				crow::float3e(cx + size, 0.0f, dxz),  // second point
				crow::float4e(1.0, 1.0f, 1.0f, 1.0f));   // color for both
			//linez
			crow::debug_renderer::add_line(
				crow::float3e(dxz, 0.0f, cz), // first point
				crow::float3e(dxz, 0.0f, cz + size),  // second point
				crow::float4e(1.0, 1.0f, 1.0f, 1.0f));  // color for both
			//z += spacing;
			dxz += spacing;
		}
		//drawing last line of the grid
		crow::debug_renderer::add_line(
			crow::float3e(cx, 0.0f, dxz), // first point
			crow::float3e(cx + size, 0.0f, dxz),  // second point
			crow::float4e(1.0, 1.0f, 1.0f, 1.0f));   // color for both

		crow::debug_renderer::add_line(
			crow::float3e(dxz, 0.0f, cz), // first point
			crow::float3e(dxz, 0.0f, cz + size),  // second point
			crow::float4e(1.0, 1.0f, 1.0f, 1.0f));  // color for both
	}

} // namespace crow