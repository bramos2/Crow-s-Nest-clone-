#pragma once

#ifdef FBXLIBRARY_EXPORTS
#define FBXLIBRARY_API __declspec(dllexport)
#else
#define FBXLIBRARY_API __declspec(dllimport)
#endif // FBXLIBRARY_EXPORTS

extern "C" FBXLIBRARY_API int get_scene_poly_count(const char* fbx_file_path);

extern "C" FBXLIBRARY_API void export_fbx_mesh(const char* fbx_file_path);

