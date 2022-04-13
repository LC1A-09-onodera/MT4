#include "peraPolygon.h"
#include "../Shader/ShaderManager.h"
#include "../DX12operator.h"

namespace PeraPolygon
{
	PeraModel PeraModels::peraSample;
	PeraModel PeraModels::peraNumbers[10];

	void PeraModels::AddPolygon(PeraModel& peraPolygon, XMFLOAT3& emitter, XMFLOAT3& scale, int amount, int life)
	{
		for (int i = 0;i < amount;i++)
		{
			PeraPolygonEach polygon;
			polygon.Init(emitter, scale, life);
			polygon.speed.x = ShlomonMath::Cos(360 / amount * i) * 0.01f;
			polygon.speed.y = ShlomonMath::Sin(360 / amount * i) * 0.01f;
			peraPolygon.eachs.push_back(polygon);
		}
	}

	void PeraModels::Exprotion(PeraModel& peraPolygon, XMFLOAT3& emitter, XMFLOAT3& scale, int amount, int life)
	{
		PeraPolygonEach polygon;
		polygon.Init(emitter, scale, life);

		peraPolygon.eachs.push_back(polygon);
	}

	void PeraModels::Thunder(PeraModel& peraPolygon, XMFLOAT3& emitter, XMFLOAT3& scale, XMFLOAT3 rotation, int Length, int life)
	{
		PeraPolygonEach polygon;
		polygon.Init(emitter, scale, life);
		polygon.each.rotation = rotation;
		peraPolygon.eachs.push_back(polygon);
	}

	void PeraModels::NotMove(PeraModel& peraPolygon, XMFLOAT3& emitter, XMFLOAT3& scale, XMFLOAT3 rotation, int life)
	{
		
	}

	void PeraModels::CreatePeraPolygons()
	{
		peraSample.peraPolygon.CreateModel("Thunder", ShaderManager::playerShader);
		CreatePera(peraSample);
		peraNumbers[0].peraPolygon.CreateModel("number1",ShaderManager::thunderShader);
		CreatePera(peraNumbers[0]);
	}

	void PeraModels::CreatePera(PeraModel& peraPolygon)
	{
		peraPolygon.peraPolygon.mesh.vertices.clear();
		peraPolygon.peraPolygon.mesh.indices.clear();
		peraPolygon.peraPolygon.mesh.vertices.shrink_to_fit();
		peraPolygon.peraPolygon.mesh.indices.shrink_to_fit();
		//vertex
		Vertex sample;
		sample.pos = { -0.5f, 0.5f, 0.0f };
		sample.normal = {0, 0, -1};
		sample.uv = { 0, 0 };
		peraPolygon.peraPolygon.mesh.vertices.push_back(sample);
		sample.pos = { -0.5f, -0.5f, 0.0f };
		sample.normal = { 0, 0, -1 };
		sample.uv = { 0, 1 };
		peraPolygon.peraPolygon.mesh.vertices.push_back(sample);
		sample.pos = { 0.5f, -0.5f, 0.0f };
		sample.normal = { 0, 0, -1 };
		sample.uv = { 1, 1 };
		peraPolygon.peraPolygon.mesh.vertices.push_back(sample);
		sample.pos = { 0.5f, 0.5f, 0.0f };
		sample.normal = { 0, 0, -1 };
		sample.uv = { 1, 0 };
		peraPolygon.peraPolygon.mesh.vertices.push_back(sample);
		//index
		peraPolygon.peraPolygon.mesh.indices.push_back(1);
		peraPolygon.peraPolygon.mesh.indices.push_back(0);
		peraPolygon.peraPolygon.mesh.indices.push_back(3);
		peraPolygon.peraPolygon.mesh.indices.push_back(2);
		peraPolygon.peraPolygon.mesh.indices.push_back(1);
		peraPolygon.peraPolygon.mesh.indices.push_back(3);
		peraPolygon.peraPolygon.Init(0);
	}

	void PeraModels::Update()
	{
		DeleteUpdate(peraSample);
		DeleteUpdate(peraNumbers[0]);
	}

	void PeraModels::DeleteUpdate(PeraModel& perapolygon)
	{
		for (auto sampleitr = perapolygon.eachs.begin(); sampleitr != perapolygon.eachs.end(); ++sampleitr)
		{
			sampleitr->Update();
			if (sampleitr->time <= 0)
			{
				perapolygon.deleteEachs.push_back(sampleitr);
			}
		}
		for (auto sampleitr2 = perapolygon.deleteEachs.begin(); sampleitr2 != perapolygon.deleteEachs.end(); ++sampleitr2)
		{
			perapolygon.eachs.erase(*sampleitr2);
		}
		perapolygon.deleteEachs.clear();
	}

	void PeraModels::Draw()
	{
		PeraDraw(peraSample);
		PeraDraw(peraNumbers[0]);
	}

	void PeraModels::PeraDraw(PeraModel &perapolygon)
	{
		for (auto sampleitr = perapolygon.eachs.begin(); sampleitr != perapolygon.eachs.end(); ++sampleitr)
		{
			perapolygon.peraPolygon.Update(&sampleitr->GetEach());
			Draw3DObject(perapolygon.peraPolygon);
		}
	}

	void PeraPolygonEach::Init(XMFLOAT3& createPosition, XMFLOAT3& scale, int life)
	{
		each.position = ConvertXMFLOAT3toXMVECTOR(createPosition);
		each.CreateConstBuff0();
		each.CreateConstBuff1();
		time = life;
		each.scale = scale;
	}

	void PeraPolygonEach::Update()
	{
		each.position = ConvertXMFLOAT3toXMVECTOR(each.position + speed);
		speed = speed + acc;
		each.rotation = each.rotation + addRotation;
		addRotation = addRotation + subRotation;
		time--;
	}
}