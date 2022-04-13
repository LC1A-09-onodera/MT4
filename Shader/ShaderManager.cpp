#include"ShaderManager.h"

HLSLShader ShaderManager::playerShader;
HLSLShader ShaderManager::thunderShader;
//HLSLShader ShaderManager::translucentShader;
void ShaderManager::LoadShaders()
{
	playerShader.vsBlob = LoadShader(L"Resource/HLSL/OBJVertexShader.hlsl", "vs_5_0");
	playerShader.psBlob = LoadShader(L"Resource/HLSL/OBJPixelShader.hlsl", "ps_5_0");
	thunderShader.vsBlob = LoadShader(L"Resource/HLSL/ThunderVS.hlsl", "vs_5_0");
	thunderShader.psBlob = LoadShader(L"Resource/HLSL/ThunderPS.hlsl", "ps_5_0");
	/*translucentShader.vsBlob = LoadShader(L"Resource/HLSL/OBJVertexShader.hlsl", "vs_5_0");
	translucentShader.psBlob = LoadShader(L"Resource/HLSL/transluentPixelShader.hlsl", "ps_5_0");*/
}
