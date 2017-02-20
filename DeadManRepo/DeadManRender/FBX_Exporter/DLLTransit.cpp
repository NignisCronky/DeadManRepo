#include "DLLTransit.h"
#include "FBXExport.h"

FBXExport fbxObject;


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
/*
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

	//	toReturn.bones.x.w =
			tmp[i].mVertexBlendingInfos;

		returnData.push_back(toReturn);
	}

	//	return returnData;

}
*/
void EXP::DLLTransit::saveFiletoBin(const char* inFileName, const char* binFileName)
{
	FBXExport* exporter = new FBXExport();

	exporter->Initialize();
	exporter->LoadScene(inFileName);


	FbxScene* tmpScene = exporter->getScene();
	FbxNode* tmpNode = tmpScene->GetRootNode();

	exporter->InitFBX();
//	exporter->ExportFBX();

	int tmpNodeCount = tmpScene->GetNodeCount();

	exporter->ProcessControlPoints(tmpNode);
	exporter->ProcessMesh(tmpNode);

	std::vector<PNTIWVertex> SendData = exporter->getverts();

	

	int vertSize = (int)exporter->getverts().size();
	
	std::ofstream outbinFile(binFileName, std::ios::binary);
	//outbinFile.open("fbx.bin", std::ios::binary);

	/////// Tmp numbers //



	if (outbinFile.is_open())
	{
		for (int i = 0; i < vertSize; i++)
		{
			VertexInfo toReturn;
			// 1. Number of Weights and Indicies
			toReturn.numIndicies = SendData[i].mVertexBlendingInfos.size();
			outbinFile.write((char*)&toReturn.numIndicies, sizeof(unsigned int));
			// 2 Store all weights and Indicies
			for (size_t j = 0; j < toReturn.numIndicies; j++)
			{
				toReturn.blendWeights.push_back((float)SendData[i].mVertexBlendingInfos[j].mBlendingWeight);
				toReturn.boneIndices.push_back(SendData[i].mVertexBlendingInfos[j].mBlendingIndex);
			}
			for (size_t j = 0; j < toReturn.numIndicies; j++)
			{
				outbinFile.write((char*)&toReturn.blendWeights[j], sizeof(float));
				outbinFile.write((char*)&toReturn.boneIndices[j], sizeof(int));
			}
//	unsigned int sizeofindices;

//	std::vector<int> boneIndices;
			// 3. Store Verts
	//		for (size_t i = 0; i < toReturn.numIndicies; i++)
	//		{				
	//			outbinFile.write((char*)&toReturn.boneIndices[i], sizeof(int));
	//		}
//	unsigned int CtrlPntIndeics;
//			outbinFile.write((char*)&toReturn.CtrlPntIndeics, sizeof(unsigned int));
			
			//	Float3 vert;
			toReturn.vert.x = SendData[i].mPosition.x;
			toReturn.vert.y = SendData[i].mPosition.y;
			toReturn.vert.z = SendData[i].mPosition.z;

		
			//	Float3 norm;
			toReturn.norm.x = SendData[i].mNormal.x;
			toReturn.norm.y = SendData[i].mNormal.y;
			toReturn.norm.z = SendData[i].mNormal.z;

			//	Float2 uv;
			toReturn.uv.u = SendData[i].mUV.x;
			toReturn.uv.v = SendData[i].mUV.y;

			outbinFile.write((char*)&toReturn.vert, sizeof(Float3));
			outbinFile.write((char*)&toReturn.norm, sizeof(Float3));
			outbinFile.write((char*)&toReturn.uv, sizeof(Float2));
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
	int something = 0;

	while (!inbinFile.eof())
	{
		VertexInfo toReturn;
	//	unsigned int sizeofweights;
		inbinFile.read((char*)&toReturn.numIndicies, sizeof(unsigned int));
	//	std::vector<float> blendWeights;

		for (size_t j = 0; j < toReturn.numIndicies; j++)
		{
			float blendwets;
			int boneind;
			inbinFile.read((char*)&blendwets, sizeof(float));
			inbinFile.read((char*)&boneind, sizeof(int));

			toReturn.blendWeights.push_back(blendwets);
			toReturn.boneIndices.push_back(boneind);


		}


	//	for (size_t i = 0; i < toReturn.numIndicies; i++)
	//	{
	//		float _sizeofWeights;
	//		inbinFile.read((char*)&_sizeofWeights, sizeof(float));
	//		toReturn.blendWeights.push_back(_sizeofWeights);
	//	}

	//	unsigned int sizeofindices;
	//	inbinFile.read((char*)&toReturn.numIndicies, sizeof(unsigned int));
	//	std::vector<int> boneIndices;
	//	for (size_t i = 0; i < toReturn.numIndicies; i++)
	//	{
	//		float _sizeofindices;
	//		inbinFile.read((char*)&_sizeofindices, sizeof(int));
	//		toReturn.blendWeights.push_back(_sizeofindices);
	//	}

	//	unsigned int CtrlPntIndeics;
	//	inbinFile.read((char*)&toReturn.CtrlPntIndeics, sizeof(unsigned int));


	//	Float3 vert;
		inbinFile.read((char*)&toReturn.vert, sizeof(Float3));
	//	Float3 norm;
		inbinFile.read((char*)&toReturn.norm, sizeof(Float3));
	//	Float2 uv;
		inbinFile.read((char*)&toReturn.uv, sizeof(Float2));
		if (inbinFile.eof())
			break;
		returnData.push_back(toReturn);
	}
	inbinFile.close();
}
