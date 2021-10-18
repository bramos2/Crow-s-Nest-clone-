#include "pch.h"
#include <fstream>
#include <assert.h>
#include <array>
#include "FbxLibrary.h"
#include "FbxUtils.h"

int get_scene_poly_count(const char* fbx_file_path)
{
    int result = -1;
    // Scene pointer, set by call to create_and_import
    FbxScene* scene = nullptr;
    // Create the FbxManager and import the scene from file
    FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);
    // Check if manager creation failed
    if (sdk_manager == nullptr)
        return result;
    //If the scene was imported...
    if (scene != nullptr)
    {
        //No errors to report, so start polygon count at 0
        result = 0;
        // Get the count of geometry objects in the scene
        int geo_count = scene->GetGeometryCount();
        for (int i = 0; i < geo_count; ++i)
        {
            //Get geometry number 'i' 
            FbxGeometry* geo = scene->GetGeometry(i);
            // If it's not a mesh, skip it
            // Geometries might be some other type like nurbs
            if (geo->GetAttributeType() != FbxNodeAttribute::eMesh)
                continue;
            // Found a mesh, add its polygon count to the result
            FbxMesh* mesh = (FbxMesh*)geo;
            result += mesh->GetPolygonCount();
        }
    }
    //Destroy the manager
    sdk_manager->Destroy();
    //Return the polygon count for the scene
    return result;
}

void export_fbx_mesh(const char* fbx_file_path)
{
    //Create scene
    FbxScene* scene = nullptr;

    //material containers
    std::vector<std::string> filePaths;
    std::vector<material_t> materials;
    std::string texFileName = "";

    //Create manager
    FbxManager* lSdkManager = create_and_import(fbx_file_path, scene);

    //Mesh container
    SimpleMesh<SimpleVertex> mesh;

    //fbxjoint container
    std::vector<fbxJoint> fbxjoints;
    //for testing purposes only
    std::vector<joint> testjoints;

    animClip animationClip; //bind pose will be in the first keyframe

    //getting animation data into containers
    //retrieving the skeleton root, bind pose
    GetBindPoseRoot(scene, fbxjoints);
    //retrieving all child nodes from our root node and forming our skeleton
    FormSkeleton(fbxjoints);
    //reading data for debuging purposes only
    //GetJoints(fbxjoints, testjoints);
    //creating the animation clip from our skeleton nodes
    GetAnimationClip(scene, fbxjoints, animationClip);

    std::vector<influence_set> controlPointInf;
    //controlPointInf.resize(mesh.vertexList.size());

    //load data into mesh for exporting
    ProcessFBXMesh(scene->GetRootNode(), mesh, texFileName, controlPointInf, fbxjoints, scene);
    
    Compactify(mesh);

    //retrieving materials and filepaths
    GetMaterials(scene, materials, filePaths);

    //writting file for material data
    {
        //getting material data
        std::string matFilePath = fbx_file_path;
        replaceExt(matFilePath, "mat");

        std::ofstream file(matFilePath.data(), std::ios::trunc | std::ios::binary | std::ios::out);
        assert(file.is_open());

        uint32_t materialsCount = (uint32_t)materials.size();
        uint32_t filePathsCount = (uint32_t)filePaths.size();

        file.write((const char*)&materialsCount, sizeof(uint32_t));
        file.write((const char*)materials.data(), sizeof(material_t) * materials.size());
        file.write((const char*)&filePathsCount, sizeof(uint32_t));
        for (int i = 0; i < filePaths.size(); ++i)
        {
            uint32_t pathSize = (uint32_t)filePaths[i].size();
            file.write((const char*)&pathSize, sizeof(uint32_t));
            file.write((const char*)filePaths[i].data(), sizeof(char) * filePaths[i].size());
        }

        file.close();
    }




    //writing file for mesh
    {
        //filename for mesh data
        std::string binFilePath = fbx_file_path;
        replaceExt(binFilePath, "bin");

        std::ofstream file(binFilePath.data(), std::ios::trunc | std::ios::binary | std::ios::out);
        assert(file.is_open());

        //getting count of vertices and indicies in mesh for readability 
        uint32_t indexCount = (uint32_t)mesh.indicesList.size();
        uint32_t vertexCount = (uint32_t)mesh.vertexList.size();

        //writing count and data of indices and vertices into file
        file.write((const char*)&indexCount, sizeof(uint32_t));
        file.write((const char*)mesh.indicesList.data(), sizeof(uint32_t) * mesh.indicesList.size());
        file.write((const char*)&vertexCount, sizeof(uint32_t));
        file.write((const char*)mesh.vertexList.data(), sizeof(SimpleVertex) * mesh.vertexList.size());

        file.close();
    }


    //writting file for animation data
    {
        //filename for animation data
        std::string animFilePath = fbx_file_path;
        replaceExt(animFilePath, "anim");

        std::ofstream file(animFilePath.data(), std::ios::trunc | std::ios::binary | std::ios::out);
        assert(file.is_open());

        uint32_t totalFrames = (uint32_t)animationClip.frames.size();
        //first writing out the duration of the animation clip of "double" size
        file.write((const char*)&animationClip.duration, sizeof(double));
        //writing out the total amount of frames in the clip of "uint32_t" size
        file.write((const char*)&totalFrames, sizeof(uint32_t));
        //looping throught every frame
        for (int f = 0; f < totalFrames; ++f) //all writes inside this loop represent a keyframe
        {
            //writing the time for the current frame of "double" size
            file.write((const char*)&animationClip.frames[f].time, sizeof(double));
            uint32_t jointsCount = animationClip.frames[f].joints.size();
            //writing the size of the joints array
            file.write((const char*)&jointsCount, sizeof(uint32_t));
            //writing the data inside of the joints array of "joint_f" size
            file.write((const char*)animationClip.frames[f].joints.data(), sizeof(joint_x) * animationClip.frames[f].joints.size());
        }

        file.close();
    }

    //destroying scene
    scene->Destroy();
}

