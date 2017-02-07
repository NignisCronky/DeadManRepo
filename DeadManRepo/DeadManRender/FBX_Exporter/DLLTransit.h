#pragma once



#ifdef FBXEXPORT_EXPORTS
#define DLLEXPORT1 __declspec(dllexport)
#else
#define DLLEXPORT1 __declspec(dllimport)
#endif // FBXEXPORT_EXPORTS


class DLLTransit
{
public:
	DLLTransit();
	~DLLTransit();
};

