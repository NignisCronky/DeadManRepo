#include "DLLTransit.h"
#include "FBXExport.h"

FBXExport fbxObject;

//bool DLLTransit::Initialize()
//{
//}
bool EXP::DLLTransit::LoadScene(const char* inFileName, const char* inOutputPath)
{
	bool thisObj = fbxObject.Initialize();
	if (thisObj)
		thisObj = fbxObject.LoadScene(inFileName, inOutputPath);
	fbxObject.ExportFBX();
	return thisObj;
}
bool EXP::DLLTransit::LoadScene(const char* inFileName)
{
	bool thisObj = fbxObject.Initialize();
	if (thisObj)
		thisObj = fbxObject.LoadScene(inFileName);
//	fbxObject.ExportFBX();
	return thisObj;
}

void EXP::DLLTransit::getVerticies(std::vector<VertexInfo> &returnData, const char* inFileName)
{
	FBXExport* exporter = new FBXExport();
	
	exporter->Initialize();
	exporter->LoadScene(inFileName);

	FbxScene* tmpScene = exporter->getScene();
	FbxNode* tmpNode = tmpScene->GetRootNode();
	int tmpNodeCount = tmpScene->GetNodeCount();

	exporter->ProcessControlPoints(tmpNode);
	exporter->ProcessMesh(tmpNode);


	for (int i = 0; i < exporter->getverts().size(); i++)
	{
		VertexInfo toReturn;
		std::vector<PNTIWVertex> tmp = exporter->getverts();

		toReturn.vert.x = tmp[i].mPosition.x;
		toReturn.vert.y = tmp[i].mPosition.y;
		toReturn.vert.z = tmp[i].mPosition.z;

		toReturn.norm.x = tmp[i].mNormal.x;
		toReturn.norm.y = tmp[i].mNormal.y;
		toReturn.norm.z = tmp[i].mNormal.z;

		toReturn.uv.u = tmp[i].mUV.x;
		toReturn.uv.v = tmp[i].mUV.y;

		returnData.push_back(toReturn);
	}

//	return returnData;

}
