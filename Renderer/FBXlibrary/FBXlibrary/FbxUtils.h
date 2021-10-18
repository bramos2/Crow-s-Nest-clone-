#pragma once
#include <fbxsdk.h>
#include <vector>
#include <string>
#include <DirectXMath.h>


//#define MAX_INFLUENCE 4


struct SimpleVertex
{
	float Pos[3];    // 0 = x, 1 = y, 2 = z
	float Normal[3]; // 0 = x, 1 = y, 2 = z
	float Tex[2];    // 0 = x, 1 = y
	int joints[4];   //joint indicies
	float weights[4]; //joint weights
};

//struct SkinnedVertex
//{
//	float Pos[3];
//	float Normal[3];
//	float Tex[2];
//	int joints[4];
//	float weights[4];
//};

struct Influence
{
	int joint;
	float weight = 0;
};

template <typename T>
struct SimpleMesh
{
	std::vector<T> vertexList;
	std::vector<int> indicesList;
};

struct fbxJoint
{
	FbxNode* node;
	int parent_index;
};

struct joint
{
	float global_xform[16];
	int parent_index;
};

struct mfloat4x4
{
	float transform[4][4];
};

//struct joint_f
//{
//	float transform[4][4];
//	int parent_index;
//};

struct joint_x
{
	DirectX::XMFLOAT4X4 transform;
	int parent_index;
};

struct keyFrame
{
	double time;
	std::vector<joint_x> joints;
};

struct animClip
{
	double duration;
	std::vector<keyFrame> frames;
};


struct material_t
{
	enum eComponent {EMISSIVE = 0, DIFFUSE, SPECULAR, SHININESS, COUNT};

	struct component_t
	{
		float value[3] = { 0.0f,0.0f,0.0f };
		float factor = 0.0f;
		int64_t input = -1;
	};

	component_t& operator[](int i) { return components[i]; }
	const component_t& operator[](int i)const { return components[i]; }

private:
	component_t components[COUNT];

};

const int MAX_INFLUENCE = 4;
using influence_set = std::array<Influence, MAX_INFLUENCE>;
using file_path_t = std::array<char, 260>;

void replaceExt(std::string& s, const std::string& newExt);

FbxManager* create_and_import(const char* fbx_file_path, FbxScene*& lScene);

void ProcessFBXMesh(FbxNode* Node, SimpleMesh<SimpleVertex>& simpleMesh,
	std::string& textureFilename, std::vector<influence_set>& cpInf, std::vector<fbxJoint>& fbxjoints, FbxScene* scene);

void Compactify(SimpleMesh<SimpleVertex>& simpleMesh);

void  GetBindPoseRoot(FbxScene* scene, std::vector<fbxJoint>& joints);

void FormSkeleton(std::vector<fbxJoint>& joints);

void GetJoints(std::vector<fbxJoint>& fbxJoints, std::vector<joint>& joints);

void GetAnimationClip(FbxScene* scene, std::vector<fbxJoint>& fbxjoints, animClip& animationClip);

void GetMaterials(FbxScene* scene, std::vector<material_t>& materials, std::vector<std::string>& paths);