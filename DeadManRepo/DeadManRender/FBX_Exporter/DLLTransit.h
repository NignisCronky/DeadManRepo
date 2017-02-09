#pragma once

#include <vector>

#ifdef FBXEXPORT_EXPORTS
#define DLLEXPORT1 __declspec(dllexport)
#else
#define DLLEXPORT1 __declspec(dllimport)
#endif // FBXEXPORT_EXPORTS

struct FL3Vert
{
	float x;
	float y;
	float z;
};
struct FL3Norm
{
	float x;
	float y;
	float z;
};
struct FL2UV
{
	float u;
	float v;
};


struct VertexInfo
{
	FL3Vert vert;
	FL3Norm norm;
	FL2UV uv;
};




namespace EXP
{
	class DLLEXPORT1 DLLTransit
	{
	private:
		bool LoadScene(const char* inFileName, const char* inOutputPath);
		bool LoadScene(const char* inFileName);
	public:

//		std::vector<VertexInfo> getVerticies(std::vector<VertexInfo> &returnData, const char* inFileName);
		void getVerticies(std::vector<VertexInfo> &returnData, const char* inFileName);


		

	};
}