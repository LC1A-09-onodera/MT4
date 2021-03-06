#include"PostEffect.h"
#include"d3dx12.h"
#include "../WindowsAPI/WinAPI.h"
#include <d3dcompiler.h>
#include "../BaseDirectX/Input.h"

#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;
UINT PostEffect::frameTime;

PostEffect::PostEffect()
{
	frameTime = 0;
}

void PostEffect::Initialize()
{
	HRESULT result;
	/*VertexPosUv Spritevertices[] = {
		{{ -0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
		{{ -0.5f, +0.5f, 0.0f}, {0.0f, 0.0f}},
		{{ +0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
		{{ +0.5f, +0.5f, 0.0f}, {1.0f, 0.0f}},
	};*/
	VertexPosUv Spritevertices[] = {
		{{ -1.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
		{{ -1.0f, +1.0f, 0.0f}, {0.0f, 0.0f}},
		{{ +1.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
		{{ +1.0f, +1.0f, 0.0f}, {1.0f, 0.0f}},
	};
	CreateGraphicsPipelineState();
	
	int sizevb = sizeof(Spritevertices);

	//頂点マップ
	BaseDirectX::result = BaseDirectX::dev->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(sizevb), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff));
	VertexPosUv* vertMap = nullptr;
	BaseDirectX::result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, Spritevertices, sizeof(Spritevertices));
	vertBuff->Unmap(0, nullptr);
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(Spritevertices);
	vbView.StrideInBytes = sizeof(Spritevertices[0]);
	BaseDirectX::result = BaseDirectX::dev->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&constBuff));
	
	//ガウスブラー用
	CalcWeightGaussian(weights, NumWeight, 8.0f);
	XMFLOAT4 weight0 = { weights[0], weights[1] ,weights[2] ,weights[3] };
	XMFLOAT4 weight1 = { weights[4], weights[5] ,weights[6] ,weights[7] };
	//定数マップ
	ConstBufferData* constMap = nullptr;
	BaseDirectX::result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = XMMatrixIdentity();
	constMap->weight0 = weight0;
	constMap->weight1 = weight1;
	constMap->frameTime = frameTime;
	constBuff->Unmap(0, nullptr);

	//テクスチャバッファの作成
	renderTarget.TexInit();
	//SRV用デスクリプタヒープ
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = PostEffect::texNum;//mulchTerxter用に2に変更

	result = BaseDirectX::dev->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV));
	assert(SUCCEEDED(result));
	//SRV
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	for (int i = 0;i < PostEffect::texNum;i++)
	{
		//BaseDirectX::dev->CreateShaderResourceView(texBuff[0].Get(), &srvDesc, descHeapSRV->GetCPUDescriptorHandleForHeapStart());
		BaseDirectX::dev->CreateShaderResourceView(renderTarget.texBuff[i].Get(), &srvDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(descHeapSRV->GetCPUDescriptorHandleForHeapStart(), i, BaseDirectX::dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	}

	renderTarget.RTVInit();
	//深度バッファ
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, WindowsAPI::window_width, WindowsAPI::window_height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	result = BaseDirectX::dev->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &depthResDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0), IID_PPV_ARGS(&depthBuff));
	assert(SUCCEEDED(result));
	//DSV
	D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc{};
	DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescHeapDesc.NumDescriptors = 1;
	result = BaseDirectX::dev->CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&descHeapDSV));
	assert(SUCCEEDED(result));
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	BaseDirectX::dev->CreateDepthStencilView(depthBuff.Get(), &dsvDesc, descHeapDSV->GetCPUDescriptorHandleForHeapStart());
}

void PostEffect::Draw()
{
	if (Input::KeyTrigger(DIK_0))
	{
		static int tex = 0;
		tex = (tex + 1) % 2;
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
		srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;
		BaseDirectX::dev->CreateShaderResourceView(renderTarget.texBuff[tex].Get(), &srvDesc, descHeapSRV->GetCPUDescriptorHandleForHeapStart());
	}
	//ワールド行列更新
	matWorld = XMMatrixIdentity();
	matWorld *= XMMatrixRotationZ(rotation);
	matWorld *= XMMatrixTranslationFromVector(position);
	//転送
	ConstBufferData* constMap = nullptr;
	BaseDirectX::result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->mat = matWorld * common.matProjection;
	XMFLOAT4 weight0 = { weights[0], weights[1] ,weights[2] ,weights[3] };
	XMFLOAT4 weight1 = { weights[4], weights[5] ,weights[6] ,weights[7] };
	constMap->weight0 = weight0;
	constMap->weight1 = weight1;
	constMap->frameTime = frameTime;
	frameTime++;
	constBuff->Unmap(0, nullptr);

	//パイプラインステート設定
	BaseDirectX::cmdList->SetPipelineState(pipelineState.Get());
	//ルートシグネチャ
	BaseDirectX::cmdList->SetGraphicsRootSignature(rootSignature.Get());
	//プリミティブ形状
	BaseDirectX::cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 頂点バッファの設定
	BaseDirectX::cmdList->IASetVertexBuffers(0, 1, &this->vbView);

	ID3D12DescriptorHeap* ppHeaps[] = { descHeapSRV.Get() };
	// デスクリプタヒープをセット
	BaseDirectX::cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	// 定数バッファビューをセット
	BaseDirectX::cmdList->SetGraphicsRootConstantBufferView(0, this->constBuff->GetGPUVirtualAddress());
	// シェーダリソースビューをセット
	//いちまいの時
	//BaseDirectX::cmdList->SetGraphicsRootDescriptorTable(1, descHeapSRV->GetGPUDescriptorHandleForHeapStart());
	
	//mulchTexの時
	BaseDirectX::cmdList->SetGraphicsRootDescriptorTable(1, CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 0, BaseDirectX::dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));
	BaseDirectX::cmdList->SetGraphicsRootDescriptorTable(2, CD3DX12_GPU_DESCRIPTOR_HANDLE(descHeapSRV->GetGPUDescriptorHandleForHeapStart(), 1, BaseDirectX::dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)));

	// 描画コマンド
	BaseDirectX::cmdList->DrawInstanced(4, 1, 0, 0);
}

void PostEffect::PreDraw()
{
	for (int i = 0;i < RenderTarget::renderNum;i++)
	{
		BaseDirectX::cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.texBuff[i].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
	}
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH[RenderTarget::renderNum];
	for (int i = 0;i < RenderTarget::renderNum;i++)
	{
		//rtvH[i] = descHeapRTV->GetCPUDescriptorHandleForHeapStart();
		rtvH[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(renderTarget.descHeapRTV->GetCPUDescriptorHandleForHeapStart(), i, BaseDirectX::dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	}
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = descHeapDSV->GetCPUDescriptorHandleForHeapStart();
	BaseDirectX::cmdList->OMSetRenderTargets(2, rtvH, false, &dsvH);
	
	CD3DX12_VIEWPORT viewports[RenderTarget::renderNum];
	CD3DX12_RECT scissorRects[RenderTarget::renderNum];
	for (int i = 0; i < RenderTarget::renderNum; i++)
	{
		viewports[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, WindowsAPI::window_width, WindowsAPI::window_height);
		scissorRects[i] = CD3DX12_RECT(0, 0 ,WindowsAPI::window_width, WindowsAPI::window_height);
	}
	BaseDirectX::cmdList->RSSetViewports(2, viewports);
	BaseDirectX::cmdList->RSSetScissorRects(2, scissorRects);
	for (int i = 0; i < RenderTarget::renderNum; i++)
	{
		BaseDirectX::cmdList->ClearRenderTargetView(rtvH[i], renderTarget.clearColor, 0, nullptr);
	}
	BaseDirectX::cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0.0f, 0, nullptr);

}

void PostEffect::PostDraw()
{
	for (int i = 0; i < RenderTarget::renderNum; i++)
	{
		BaseDirectX::cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.texBuff[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}
}

void PostEffect::CreateGraphicsPipelineState()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;
	ComPtr<ID3DBlob> psBlob;
	ComPtr<ID3DBlob> errorBlob;

	//頂点シェーダーの読み込みピクセルシェーダーの読み込み頂点レイアウトの読み込み
	// 頂点シェーダの読み込みとコンパイル
	BaseDirectX::result = D3DCompileFromFile(L"Resource/HLSL/PostEffectTestVS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vsBlob, &errorBlob);

	if (FAILED(BaseDirectX::result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";

		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ピクセルシェーダの読み込みとコンパイル
	BaseDirectX::result = D3DCompileFromFile(L"Resource/HLSL/PostEffectTestPS.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &psBlob, &errorBlob);

	if (FAILED(BaseDirectX::result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};
	//グラフィックスパイプラインの設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());
	//サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//デプステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;//常に上書き
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.RenderTarget[1] = blenddesc;

	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = texNum;
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	gpipeline.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
	gpipeline.SampleDesc.Count = 1;

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV0;
	descRangeSRV0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//テクスチャ1
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1;
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);//テクスチャ2

	CD3DX12_ROOT_PARAMETER rootparams[3];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);//定数バッファビューとして初期化
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);

	//スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MINIMUM_MIN_MAG_MIP_POINT);
	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = BaseDirectX::dev->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	gpipeline.pRootSignature = rootSignature.Get();
	result = BaseDirectX::dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));
}

void PostEffect::CalcWeightGaussian(float* weightsTbl, int sizeOfWeightsTbl, float sigma)
{// 重みの合計を記録する変数を定義する
	float total = 0;

	// ここからガウス関数を用いて重みを計算している
	// ループ変数のxが基準テクセルからの距離
	for (int x = 0; x < sizeOfWeightsTbl; x++)
	{
		weightsTbl[x] = expf(-0.5f * (float)(x * x) / sigma);
		total += 2.0f * weightsTbl[x];
	}

	// 重みの合計で除算することで、重みの合計を1にしている
	for (int i = 0; i < sizeOfWeightsTbl; i++)
	{
		weightsTbl[i] /= total;
	}
}
