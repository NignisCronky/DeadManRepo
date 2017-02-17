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
	int asdf = (int)exporter->getverts().size();

	for (int i = 0; i < asdf; i++)
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

void EXP::DLLTransit::saveFiletoBin(const char* inFileName, const char* binFileName)
{
	bool debugBool = false;
	FBXExport* exporter = new FBXExport();

	exporter->Initialize();
	debugBool = exporter->LoadScene(inFileName);


	FbxScene* tmpScene = exporter->getScene();
	FbxNode* tmpNode = tmpScene->GetRootNode();
	int tmpNodeCount = tmpScene->GetNodeCount();

	exporter->ProcessControlPoints(tmpNode);
	exporter->ProcessMesh(tmpNode);

	std::vector<PNTIWVertex> SendData = exporter->getverts();

	int vertSize = (int)exporter->getverts().size();
	
	std::ofstream outbinFile(binFileName, std::ios::binary);
	//outbinFile.open("fbx.bin", std::ios::binary);

	if (outbinFile.is_open())
	{
		for (int i = 0; i < vertSize; i++)
		{
		//	std::to_string(SendData[i].mPosition.x).c_str();
		//	std::string StringData;
		//	StringData.append(std::to_string(SendData[i].mPosition.x).c_str()).append(","
		//	).append(std::to_string(SendData[i].mPosition.y).c_str()).append(","
		//	).append(std::to_string(SendData[i].mPosition.z).c_str()).append(","
		//
		//	).append(std::to_string(SendData[i].mNormal.x).c_str()).append(","
		//	).append(std::to_string(SendData[i].mNormal.y).c_str()).append(","
		//	).append(std::to_string(SendData[i].mNormal.z).c_str()).append(","
		//
		//	).append(std::to_string(SendData[i].mUV.x).c_str()).append(","
		//	).append(std::to_string(SendData[i].mUV.y).c_str());
		//
		//	outbinFile.write(StringData.c_str(), StringData.size());

			VertexInfo toReturn;
			toReturn.norm.x = SendData[i].mNormal.x;
			toReturn.norm.y = SendData[i].mNormal.y;
			toReturn.norm.z = SendData[i].mNormal.z;

			toReturn.vert.x = SendData[i].mPosition.x;
			toReturn.vert.y = SendData[i].mPosition.y;
			toReturn.vert.z = SendData[i].mPosition.z;

			toReturn.uv.u = SendData[i].mUV.x;
			toReturn.uv.v = SendData[i].mUV.y;

			outbinFile.write((char*)&toReturn, sizeof(VertexInfo));
		}
		outbinFile.close();
	}
}

void EXP::DLLTransit::loadFilefromBin(const char* inFileName, std::vector<VertexInfo> &returnData)
{
//	VertexInfo
	VertexInfo pushData;
	std::ifstream inbinFile;

	inbinFile.open(inFileName, std::ios::binary | std::ios::in);
//	inbinFile.seekg(0, inbinFile.end);
//	int fileSize = inbinFile.tellg();
//	inbinFile.seekg(0, inbinFile.beg);

//	char *buffer = new char[fileSize];
//	inbinFile.read(buffer, fileSize);
//	inbinFile.close();
	int something = 0;

	while (!inbinFile.eof())
	{
		VertexInfo toReturn;
		inbinFile.read((char*)&toReturn, sizeof(VertexInfo));
		if (inbinFile.eof())
			break;
		returnData.push_back(toReturn);
	}
}
