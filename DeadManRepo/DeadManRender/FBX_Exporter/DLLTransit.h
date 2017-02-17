#pragma once

#include <vector>

#ifdef FBXEXPORT_EXPORTS
#define DLLEXPORT1 __declspec(dllexport)
#else
#define DLLEXPORT1 __declspec(dllimport)
#endif // FBXEXPORT_EXPORTS

struct Float4
{
	float x;
	float y;
	float z;
	float w;
};
struct Float3
{
	float x;
	float y;
	float z;
};
struct Float2
{
	float u;
	float v;
};

struct UnsignedInt4
{
	unsigned int x;
	unsigned int y;
	unsigned int z;
	unsigned int w;
};
struct Bone
{
	UnsignedInt4 x;
	Float4 y;
};

struct VertexInfo
{
	Float3 vert;
	Float3 norm;
	Float2 uv;
	Bone bones;
};




namespace EXP
{
	class DLLEXPORT1 DLLTransit
	{
	private:
		bool LoadScene(const char* inFileName, const char* inOutputPath);
		bool LoadScene(const char* inFileName);
		void getVerticies(std::vector<VertexInfo> &returnData, const char* inFileName);
	public:

//		std::vector<VertexInfo> getVerticies(std::vector<VertexInfo> &returnData, const char* inFileName);
		void saveFiletoBin(const char* inFileName, const char* binFileName);
		void loadFilefromBin(const char* inFileName, std::vector<VertexInfo> &returnData);


		

	};
}