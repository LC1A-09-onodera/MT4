#pragma once
#include "../3DModel/Model.h"

namespace PeraPolygon
{
	class PeraPolygonEach
	{
	public:
		XMFLOAT3 speed{};
		XMFLOAT3 acc{};
		XMFLOAT3 addRotation{};
		XMFLOAT3 subRotation{};
		EachInfo each;
		float alpha;
		int time;
		void Init(XMFLOAT3& createPosition, XMFLOAT3 &scale, int life);
		void Update();
		void Draw();
		EachInfo GetEach() { return this->each; }
	};
	class PeraModel
	{
	public:
		Model peraPolygon;
		list<PeraPolygonEach> eachs;
		list<list<PeraPolygonEach>::iterator> deleteEachs;
	};
	class PeraModels
	{
	public:
		static PeraModel peraSample;
		static PeraModel peraNumbers[10];
		static void AddPolygon(PeraModel& peraPolygon, XMFLOAT3& emitter,XMFLOAT3 &scale, int amount, int life);
		static void Exprotion(PeraModel& peraPolygon, XMFLOAT3& emitter, XMFLOAT3 &scale, int amount, int life);
		static void Thunder(PeraModel &peraPolygon, XMFLOAT3 &emitter, XMFLOAT3 &scale, XMFLOAT3 rotation, int Length,int life);
		static void NotMove(PeraModel &peraPolygon, XMFLOAT3 &emitter, XMFLOAT3 &scale, XMFLOAT3 rotation, int life);
		static void CreatePeraPolygons();
		static void CreatePera(PeraModel& peraPolygon);
		static void Update();
		static void DeleteUpdate(PeraModel &perapolygon);
		static void Draw();
		static void PeraDraw(PeraModel &perapolygon);
	};
}