#include "pch.h"
#include "FbxUtils.h"
#include <array>
#include <string>

// Add FBX mesh process function declaration here
void replaceExt(std::string& s, const std::string& newExt)
{
	std::string::size_type i = s.rfind('.', s.length());

	if (i != std::string::npos) {
		s.replace(i + 1, newExt.length(), newExt);
	}

}

std::string getFileName(const std::string& s) {

	// look for '\\' first
	char sep = '/';

	size_t i = s.rfind(sep, s.length());
	if (i != std::string::npos) {
		return(s.substr(i + 1, s.length() - i));
	}
	else // try '/'
	{
		sep = '\\';
		size_t i = s.rfind(sep, s.length());
		if (i != std::string::npos) {
			return(s.substr(i + 1, s.length() - i));
		}
	}
	return("");
}

FbxManager* create_and_import(const char* fbx_file_path, FbxScene*& lScene)
{
	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager* lSdkManager = FbxManager::Create();
	// Create the IO settings object.
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);
	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(fbx_file_path, -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		return nullptr;
	}
	// Create a new scene so that it can be populated by the imported file.
	lScene = FbxScene::Create(lSdkManager, "imported_scene");
	// Import the contents of the file into the scene.
	lImporter->Import(lScene);
	lImporter->Destroy();
	return lSdkManager;
}

void GetSkinnedMeshData(FbxScene* scene, std::vector<fbxJoint>& fbxjoints, std::vector<influence_set>& cpInf)
{
	const int poseCount = scene->GetPoseCount();

	//finding our bind pose
	for (int i = 0; i < poseCount; ++i)
	{
		FbxPose* pose = scene->GetPose(i);
		if (pose->IsBindPose())
		{
			const int count = pose->GetCount();

			//finding our mesh
			for (int j = 0; j < count; ++j)
			{
				FbxNode* node = pose->GetNode(j);
				FbxMesh* currMesh = node->GetMesh();

				if (currMesh)
				{
					const int defCount = currMesh->GetDeformerCount();

					//finding the deformer that is skin
					for (int d = 0; d < defCount; ++d)
					{
						FbxDeformer* deformer = currMesh->GetDeformer(d);
						if (deformer->Is<FbxSkin>())
						{
							FbxSkin* skin = FbxCast<FbxSkin>(deformer);
							const int clusterCount = skin->GetClusterCount();

							//looping throught each cluster in the skin
							for (int c = 0; c < clusterCount; ++c)
							{
								FbxCluster* cluster = skin->GetCluster(c);
								FbxNode* linkNode = cluster->GetLink();
								int JOINT_INDEX = 0;


								//finding the joint index in our vector of fbxnode
								for (int joint = 0; joint < fbxjoints.size(); ++joint)
								{
									if (fbxjoints[joint].node == linkNode) {
										JOINT_INDEX = joint;
										break;
									}
								}


								for (int cpoint = 0; cpoint < cluster->GetControlPointIndicesCount(); ++cpoint)
								{
									float WEIGHT = (float)cluster->GetControlPointWeights()[cpoint];
									int CONTROL_POINT_INDEX = cluster->GetControlPointIndices()[cpoint];
									Influence influence;
									influence.joint = JOINT_INDEX;
									influence.weight = WEIGHT;
									influence_set newinf = cpInf[CONTROL_POINT_INDEX];

									for (int setInd = 0; setInd < MAX_INFLUENCE; ++setInd)
									{
										if (cpInf[CONTROL_POINT_INDEX][setInd].weight < influence.weight) {
											cpInf[CONTROL_POINT_INDEX][setInd] = influence;
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void LoadUVInformation(FbxMesh* pMesh, std::vector<SimpleVertex>& vertexList)
{
	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);

	//iterating over all uv sets
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
	{
		//get lUVSetIndex-th uv set
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

		if (!lUVElement)
			continue;

		// only support mapping mode eByPolygonVertex and eByControlPoint
		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			return;

		//index array, where holds the index referenced to the uv data
		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

		//iterating through the data by polygon
		const int lPolyCount = pMesh->GetPolygonCount();

		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;

					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					//User TODO:
					//Print out the value of UV(lUVValue) or log it to a file
					vertexList[lUVIndex].Tex[0] = (float)(lUVValue[0]);
					vertexList[lUVIndex].Tex[1] = 1.0f - (float)(lUVValue[1]);
				}
			}
		}
		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

						//User TODO:
						//Print out the value of UV(lUVValue) or log it to a file
						vertexList[lPolyIndexCounter].Tex[0] = (float)(lUVValue[0]);
						vertexList[lPolyIndexCounter].Tex[1] = 1.0f - (float)(lUVValue[1]);
						lPolyIndexCounter++;
					}
				}
			}
		}
	}
}

void ProcessFBXMesh(FbxNode* Node, SimpleMesh<SimpleVertex>& simpleMesh, 
	std::string& textureFilename, std::vector<influence_set>& cpInf, std::vector<fbxJoint>& fbxjoints,
	FbxScene* scene)
{
	int childrenCount = Node->GetChildCount();
	float scale = 1.0f;

	// check each child node for a FbxMesh
	for (int i = 0; i < childrenCount; i++)
	{
		FbxNode* childNode = Node->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();

		// Found a mesh on this node
		if (mesh != NULL)
		{
			// Get index count from mesh
			int numVertices = mesh->GetControlPointsCount();

			// Resize the vertex vector to size of this mesh
			simpleMesh.vertexList.resize(numVertices);
			cpInf.resize(numVertices);

			//getting skinned mesh data
			GetSkinnedMeshData(scene, fbxjoints, cpInf);

			//================= Process Vertices ===============
			for (int j = 0; j < numVertices; j++)
			{
				FbxVector4 vert = mesh->GetControlPointAt(j);
				simpleMesh.vertexList[j].Pos[0] = (float)vert.mData[0] / scale;
				simpleMesh.vertexList[j].Pos[1] = (float)vert.mData[1] / scale;
				simpleMesh.vertexList[j].Pos[2] = (float)vert.mData[2] / scale;
				float wSum = 0;

				for (int l = 0; l < 4; ++l) //skinned mesh data
				{
					simpleMesh.vertexList[j].joints[l] = cpInf[j][l].joint;
					wSum += simpleMesh.vertexList[j].weights[l] = cpInf[j][l].weight;
				}

				for (int l = 0; l < 4; ++l) //normalizing weights
				{
					simpleMesh.vertexList[j].weights[l] /= wSum;
				}
				// Generate random normal for first attempt at getting to render
				//simpleMesh.vertexList[j].Normal = RAND_NORMAL;
			}

			int numIndices = mesh->GetPolygonVertexCount();

			// No need to allocate int array, FBX does for us
			int* indices = mesh->GetPolygonVertices();

			// Fill indiceList
			simpleMesh.indicesList.resize(numIndices);
			memcpy(simpleMesh.indicesList.data(), indices, numIndices * sizeof(int));

			// Get the Normals array from the mesh
			FbxArray<FbxVector4> normalsVec;
			mesh->GetPolygonVertexNormals(normalsVec);

			// Declare a new vector for the expanded vertex data
			// Note the size is numIndices not numVertices
			std::vector<SimpleVertex> vertexListExpanded;
			vertexListExpanded.resize(numIndices);

			// align (expand) vertex array and set the normals
			for (int j = 0; j < numIndices; j++)
			{
				// copy the original vertex position to the new vector
				// by using the index to look up the correct vertex
				// this is the "unindexing" step
				vertexListExpanded[j].Pos[0] = simpleMesh.vertexList[indices[j]].Pos[0];
				vertexListExpanded[j].Pos[1] = simpleMesh.vertexList[indices[j]].Pos[1];
				vertexListExpanded[j].Pos[2] = simpleMesh.vertexList[indices[j]].Pos[2];
				// copy normal data directly, no need to unindex
				vertexListExpanded[j].Normal[0] = (float)normalsVec.GetAt(j)[0];
				vertexListExpanded[j].Normal[1] = (float)normalsVec.GetAt(j)[1];
				vertexListExpanded[j].Normal[2] = (float)normalsVec.GetAt(j)[2];
				
				vertexListExpanded[j].joints[0] = simpleMesh.vertexList[indices[j]].joints[0];
				vertexListExpanded[j].joints[1] = simpleMesh.vertexList[indices[j]].joints[1];
				vertexListExpanded[j].joints[2] = simpleMesh.vertexList[indices[j]].joints[2];
				vertexListExpanded[j].joints[3] = simpleMesh.vertexList[indices[j]].joints[3];

				vertexListExpanded[j].weights[0] = simpleMesh.vertexList[indices[j]].weights[0];
				vertexListExpanded[j].weights[1] = simpleMesh.vertexList[indices[j]].weights[1];
				vertexListExpanded[j].weights[2] = simpleMesh.vertexList[indices[j]].weights[2];
				vertexListExpanded[j].weights[3] = simpleMesh.vertexList[indices[j]].weights[3];
			}

			LoadUVInformation(mesh, vertexListExpanded);

			// make new indices to match the new vertexListExpanded
			std::vector<int> indicesList;
			indicesList.resize(numIndices);
			for (int j = 0; j < numIndices; j++)
			{
				indicesList[j] = j; //literally the index is the count
			}

			// copy working data to the global SimpleMesh
			simpleMesh.indicesList = indicesList;
			simpleMesh.vertexList = vertexListExpanded;

			//================= Texture ========================================

			int materialCount = childNode->GetSrcObjectCount<FbxSurfaceMaterial>();

			for (int index = 0; index < materialCount; index++)
			{
				FbxSurfaceMaterial* material = (FbxSurfaceMaterial*)childNode->GetSrcObject<FbxSurfaceMaterial>(index);

				if (material != NULL)
				{
					//cout << "\nmaterial: " << material->GetName() << std::endl;
					// This only gets the material of type sDiffuse, you probably need to traverse all Standard Material Property by its name to get all possible textures.
					FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

					// Check if it's layeredtextures
					int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

					if (layeredTextureCount > 0)
					{
						for (int j = 0; j < layeredTextureCount; j++)
						{
							FbxLayeredTexture* layered_texture = FbxCast<FbxLayeredTexture>(prop.GetSrcObject<FbxLayeredTexture>(j));
							int lcount = layered_texture->GetSrcObjectCount<FbxTexture>();

							for (int k = 0; k < lcount; k++)
							{
								FbxFileTexture* texture = FbxCast<FbxFdileTexture>(layered_texture->GetSrcObject<FbxTexture>(k));
								// Then, you can get all the properties of the texture, include its name
								const char* textureName = texture->GetFileName();
								textureFilename = std::string(textureName);
								textureFilename = getFileName(textureFilename);

								/*replaceExt(textureFilename, "dds");*/
							}
						}
					}
					else
					{
						// Directly get textures
						int textureCount = prop.GetSrcObjectCount<FbxTexture>();
						for (int j = 0; j < textureCount; j++)
						{
							FbxFileTexture* texture = FbxCast<FbxFileTexture>(prop.GetSrcObject<FbxTexture>(j));
							// Then, you can get all the properties of the texture, include its name
							const char* textureName = texture->GetFileName();
							textureFilename = std::string(textureName);
							textureFilename = getFileName(textureFilename);
							replaceExt(textureFilename, "dds");
							FbxProperty p = texture->RootProperty.Find("Filename");

						}
					}
				}
			}
		}
		// did not find a mesh here so recurse
		else
			ProcessFBXMesh(childNode, simpleMesh, textureFilename, cpInf, fbxjoints, scene);
	}
}

void Compactify(SimpleMesh<SimpleVertex>& simpleMesh)
{
	// Using vectors because we don't know what size we are
	// going to need until the end
	std::vector<SimpleVertex> compactedVertexList;
	std::vector<int> indicesList;

	// initialize running index
	int compactedIndex = 0;

	// for each vertex in the expanded array
	// compare to the compacted array for a matching
	// vertex, if found, skip adding and set the index
	for (SimpleVertex vertSimpleMesh : simpleMesh.vertexList)
	{
		bool found = false;
		int foundIndex = 0;
		// search for match with the rest in the array
		for (SimpleVertex vertCompactedList : compactedVertexList)
		{
			if (vertSimpleMesh.Pos[0] == vertCompactedList.Pos[0] &&
				vertSimpleMesh.Pos[1] == vertCompactedList.Pos[1] &&
				vertSimpleMesh.Pos[2] == vertCompactedList.Pos[2] &&
				vertSimpleMesh.Normal[0] == vertCompactedList.Normal[0] &&
				vertSimpleMesh.Normal[1] == vertCompactedList.Normal[1] &&
				vertSimpleMesh.Normal[2] == vertCompactedList.Normal[2] &&
				vertSimpleMesh.Tex[0] == vertCompactedList.Tex[0] &&
				vertSimpleMesh.Tex[1] == vertCompactedList.Tex[1] &&
				vertSimpleMesh.joints[0] == vertCompactedList.joints[0] &&
				vertSimpleMesh.joints[1] == vertCompactedList.joints[1] &&
				vertSimpleMesh.joints[2] == vertCompactedList.joints[2] &&
				vertSimpleMesh.joints[3] == vertCompactedList.joints[3] &&
				vertSimpleMesh.weights[0] == vertCompactedList.weights[0] &&
				vertSimpleMesh.weights[1] == vertCompactedList.weights[1] &&
				vertSimpleMesh.weights[2] == vertCompactedList.weights[2] &&
				vertSimpleMesh.weights[3] == vertCompactedList.weights[3]
				)
			{
				indicesList.push_back(foundIndex);
				found = true;
				break;
			}
			foundIndex++;
		}
		// didn't find a duplicate so keep (push back) the current vertex
		// and increment the index count and push back that index as well
		if (!found)
		{
			compactedVertexList.push_back(vertSimpleMesh);
			indicesList.push_back(compactedIndex);
			compactedIndex++;
		}
	}

	// copy working data to the global SimpleMesh
	simpleMesh.indicesList = indicesList;
	simpleMesh.vertexList = compactedVertexList;
}

void GetAnimationClip(FbxScene* scene, std::vector<fbxJoint>& fbxjoints, animClip& animationClip)
{
	FbxAnimStack* fas = scene->GetCurrentAnimationStack();
	FbxTimeSpan span = fas->GetLocalTimeSpan();
	FbxTime timer = span.GetDuration();
	animationClip.duration = timer.GetSecondDouble();
	float duration = (float)animationClip.duration;
	int frameCount = (int)timer.GetFrameCount(FbxTime::EMode::eFrames24);

	for (int frame = 0; frame < frameCount; ++frame)
	{
		keyFrame keyframe;
	
		timer.SetFrame(frame, FbxTime::EMode::eFrames24);
		keyframe.time = timer.GetSecondDouble();

		for (int j = 0; j < fbxjoints.size(); ++j)
		{
			joint_x joint;
			joint.parent_index = fbxjoints[j].parent_index;
			FbxAMatrix mtx = fbxjoints[j].node->EvaluateGlobalTransform(timer);
			
			for (int r = 0; r < 4; ++r)
			{
				for (int c = 0; c < 4; ++c)
				{
					joint.transform.m[r][c] = (float)mtx[r][c];
				}
			}
			keyframe.joints.push_back(joint);
		}
		animationClip.frames.push_back(keyframe);
	}
}

void GetJoints(std::vector<fbxJoint>& fbxJoints, std::vector<joint>& joints)
{
	for (int i = 0; i < fbxJoints.size(); ++i)
	{
		joint curr;
		curr.parent_index = fbxJoints[i].parent_index;
		//retrieving transform from node
		FbxMatrix gTransform = fbxJoints[i].node->EvaluateGlobalTransform();
		for (int y = 0, j = 0; y < 4; ++y)
		{
			for (int x = 0; x < 4; ++x)
			{
				curr.global_xform[j++] = gTransform.Get(y, x);
			}
		}
		joints.push_back(curr);
	}
}

//will pack the vector with skeleton joint nodes from the root node inside the vector
void FormSkeleton(std::vector<fbxJoint>& fbxJoints)
{
	for (int i = 0; i < fbxJoints.size(); ++i)
	{
		for (int j = 0; j < fbxJoints[i].node->GetChildCount(); ++j)
		{
			FbxNode* child = fbxJoints[i].node->GetChild(j);
			FbxSkeleton* childSkeleton = child->GetSkeleton();
			if (childSkeleton)
			{
				fbxJoint c;
				c.node = child;
				c.parent_index = i;
				fbxJoints.push_back(c);
			}
		}
	}
}

//Find the skeleton root node and adds it into the vector
void GetBindPoseRoot(FbxScene* scene, std::vector<fbxJoint>& fbxjoints)
{
	const int poseCount = scene->GetPoseCount();

	for (int i = 0; i < poseCount; ++i)
	{
		FbxPose* pose = scene->GetPose(i);
		if (pose->IsBindPose())
		{
			const int count = pose->GetCount();

			for (int j = 0; j < count; ++j) //find root skeleton node
			{
				FbxNode* node = pose->GetNode(j);
				FbxSkeleton* currSkeleton = node->GetSkeleton();

				if (currSkeleton && currSkeleton->IsSkeletonRoot())
				{
					fbxJoint c;
					c.node = node;
					c.parent_index = -1;
					fbxjoints.push_back(c);
					return;
				}
			}
		}
	}
}

void GetMaterials(FbxScene* scene, std::vector<material_t>& materials, std::vector<std::string>& paths)
{
	int numMat = scene->GetMaterialCount();

	for (int m = 0; m < numMat; ++m)
	{
		material_t mMat;
		FbxSurfaceMaterial* mat = scene->GetMaterial(m);

		if (mat->Is<FbxSurfaceLambert>() == false)
			continue;
		//diffuse data
		FbxSurfaceLambert* lam = (FbxSurfaceLambert*)mat;
		FbxDouble3 diffuseColor = lam->Diffuse.Get();
		FbxDouble diffuseFactor = lam->DiffuseFactor.Get();
		
		//loading diffuse data into our material
		for (int i = 0; i < 3; ++i)
			mMat[mMat.DIFFUSE].value[i] = (float)diffuseColor[i];
		mMat[mMat.DIFFUSE].factor = (float)diffuseFactor;

		//diffuse filepath
		if (FbxFileTexture* fileTexture = lam->Diffuse.GetSrcObject<FbxFileTexture>())
		{
			const char* fileName = fileTexture->GetRelativeFileName();
			std::string fp = std::string(fileName);
			//file_path_t filePath;
			//strcpy(filePath.data(), fileName);
			mMat[mMat.DIFFUSE].input = paths.size();
			paths.push_back(fp);
		}

		//Emissive data
		FbxDouble3 emissiveColor = lam->Emissive.Get();
		FbxDouble emissiveFactor = lam->EmissiveFactor.Get();

		//loading emissive data into our material
		for (int i = 0; i < 3; ++i)
			mMat[mMat.EMISSIVE].value[i] = (float)emissiveColor[i];
		mMat[mMat.EMISSIVE].factor = (float)emissiveFactor;

		//emissive filepath
		if (FbxFileTexture* fileTexture = lam->Emissive.GetSrcObject<FbxFileTexture>())
		{
			const char* fileName = fileTexture->GetRelativeFileName();
			std::string fp = std::string(fileName);
			//file_path_t filePath;
			//strcpy(filePath.data(), fileName);
			mMat[mMat.EMISSIVE].input = paths.size();
			paths.push_back(fp);
		}

		//specular data
		if (mat->Is<FbxSurfacePhong>())
		{
			FbxSurfacePhong* pho = (FbxSurfacePhong*)mat;

			FbxDouble3 specularColor = pho->Specular.Get();
			FbxDouble specularFactor = pho->SpecularFactor.Get();

			//loading specular data into our material
			for (int i = 0; i < 3; ++i)
				mMat[mMat.SPECULAR].value[i] = (float)specularColor[i];
			mMat[mMat.SPECULAR].factor = (float)specularFactor;

			//specular filepath
			if (FbxFileTexture* fileTexture = pho->Specular.GetSrcObject<FbxFileTexture>())
			{
				const char* fileName = fileTexture->GetRelativeFileName();
				std::string fp = std::string(fileName);
				//file_path_t filePath;
				//strcpy(filePath.data(), fileName);
				mMat[mMat.SPECULAR].input = paths.size();
				paths.push_back(fp);
			}
		}

		materials.push_back(mMat);
	}
}