#include"GameScene.h"
#include "BaseDirectX.h"
#include "../WindowsAPI/WinAPI.h"
#include "Input.h"
#include "../Sound/Sound.h"
#include "viewport.h"
#include "../Particle/Particle3D.h"
#include "../imgui/ImguiControl.h"
#include "../Sound/Sound.h"
#include "../Light/Light.h"
#include "vec3.h"
#include "../FBXObject/FBXObject.h"
#include "../Shader/ShaderManager.h"
#include "../3DObjectParticle/3DObjectParticle.h"

GameScene::GameScene()
{
	SceneNum = TITLE;
}

GameScene::~GameScene()
{
	delete(light);
	/*delete(object);
	delete(model);*/
}

void GameScene::SceneManageUpdateAndDraw()
{
	Input::Update();
	WindowsAPI::CheckMsg();
	light->Update();
	switch (SceneNum)
	{
	case TITLE:
		TitleUpdate();
		TitleDraw();
		break;
	case SELECT:
		SelectUpdate();
		SelectDraw();
		break;
	case GAME:
		GameUpdate();
		GameDraw();
		break;
	case RESULT:
		ResultUpdate();
		ResultDraw();
		break;
	case END:
		EndUpdate();
		EndDraw();
		break;
	default:
		break;
	}
}

void GameScene::Init()
{
	BaseDirectX::Set();
	//�T�E���h
	Sound::CreateVoice();
	//SRV�̃A�h���X�m��
	BaseDirectX::GetAdress();
	//�J����������
	Camera::Init();
	Camera::eye = { 0, 0, -15.0 };
	Camera::target = { 0, 0, 0 };
	Camera::Update();
	//Imgui�̏�����
	Imgui::Init();
	//���C�g�̏�����
	Light::StaticInitialize(BaseDirectX::dev.Get());
	ShaderManager::LoadShaders();
	// 3D�p�[�e�B�N���ÓI������
	ParticleControl::Init();
	//�C���v�b�g������
	Input::KeySet(WindowsAPI::w, WindowsAPI::hwnd);
	//FBX�n
	FbxLoader::GetInstance()->Initialize(BaseDirectX::dev.Get());
	FBXObject::SetDevice(BaseDirectX::dev.Get());
	FBXObject::CreateGraphicsPipeline();
	//���C�g������
	light = Light::Create();
	//���f�����ׂĂɃ��C�g��K�p
	Model::SetLight(light);


	//�|�X�g�G�t�F�N�g�̏�����
	postEffect.Initialize();
	ObjectParticles::LoadModels();

	/*model = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	object = new FBXObject;
	object->Initialize();
	object->SetModel(model);
	object->PlayAnimation();*/
	//sample.CreateModel("newOserro", ShaderManager::playerShader);
	//sample.each.rotation.x = 0;
	maru.CreateModel("maru", ShaderManager::playerShader);
	each[0].CreateConstBuff0();
	each[0].CreateConstBuff1();
	each[1].position.m128_f32[0] = -5.0f;
	each[1].position.m128_f32[1] = -3.0f;
	each[1].CreateConstBuff0();
	each[1].CreateConstBuff1();
}

void GameScene::TitleUpdate()
{
	if (Input::KeyTrigger(DIK_SPACE))
	{
		t = 0;
		each[1].position.m128_f32[0] = -5.0f;
		each[1].position.m128_f32[1] = -3.0f;
	}
	t += 0.02f;
	XMFLOAT3 pos = ConvertXMVECTORtoXMFLOAT3(each[0].position);
	each[0].position = ConvertXMFLOAT3toXMVECTOR(Cannon(5.0f, 5.0f, t));
	float res = ShlomonMath::AirResistance(0.1f, Lenght(each[0].position, pos));
	float res2 = ShlomonMath::Friction(0.1f, ShlomonMath::VerticalDrag(1.0f));
	each[0].position.m128_f32[0] -= res / 10;
	each[0].position.m128_f32[1] -= res / 10;
	each[1].position.m128_f32[0] += ShlomonMath::EqualSpeed(0.1f, t);
	each[1].position.m128_f32[0] -= res2 / 100;
	LightUpdate();
}

void GameScene::SelectUpdate()
{

}

void GameScene::GameUpdate()
{

}

void GameScene::ResultUpdate()
{

}

void GameScene::EndUpdate()
{

}

void GameScene::TitleDraw()
{
	//PostEffect��PreDraw
	//postEffect.PreDraw();
	//ObjectParticles::Draw();
	//Draw3DObject(sample);
	BaseDirectX::clearColor[0] = 0.0f;
	BaseDirectX::clearColor[1] = 0.0f;
	BaseDirectX::clearColor[2] = 0.0f;
	BaseDirectX::clearColor[3] = 0.0f;
	BaseDirectX::UpdateFront();
	//PostEffect��Draw
	//postEffect.Draw();
	maru.Update(&each[0]);
	Draw3DObject(maru);
	maru.Update(&each[1]);
	Draw3DObject(maru);
	//�X�v���C�g�̕`��-------------------------
	Imgui::DrawImGui();
	//�`��R�}���h�����܂�
	BaseDirectX::UpdateBack();
}

void GameScene::SelectDraw()
{
	//PostEffect��PreDraw
	postEffect.PreDraw();



	//PostEffect��PostDraw
	postEffect.PostDraw();

	BaseDirectX::clearColor[0] = 0.0f;
	BaseDirectX::clearColor[1] = 0.0f;
	BaseDirectX::clearColor[2] = 0.0f;
	BaseDirectX::clearColor[3] = 0.0f;
	BaseDirectX::UpdateFront();
	//PostEffect��Draw
	postEffect.Draw();

	//�X�v���C�g�̕`��-------------------------
	//titleSprite.SpriteDraw();

	Imgui::DrawImGui();
	//�`��R�}���h�����܂�
	BaseDirectX::UpdateBack();
}

void GameScene::GameDraw()
{
	//PostEffect��PreDraw
	postEffect.PreDraw();

	BaseDirectX::clearColor[0] = 0.0f;
	BaseDirectX::clearColor[1] = 0.0f;
	BaseDirectX::clearColor[2] = 0.0f;
	BaseDirectX::clearColor[3] = 0.0f;
	BaseDirectX::UpdateFront();
	//PostEffect��Draw
	postEffect.Draw();

	//�X�v���C�g�̕`��-------------------------
	//titleSprite.SpriteDraw();
	Imgui::DrawImGui();
	//�`��R�}���h�����܂�
	BaseDirectX::UpdateBack();
}

void GameScene::ResultDraw()
{
	//PostEffect��PreDraw
	postEffect.PreDraw();



	//PostEffect��PostDraw
	postEffect.PostDraw();

	BaseDirectX::clearColor[0] = 0.0f;
	BaseDirectX::clearColor[1] = 0.0f;
	BaseDirectX::clearColor[2] = 0.0f;
	BaseDirectX::clearColor[3] = 0.0f;
	BaseDirectX::UpdateFront();
	//PostEffect��Draw
	postEffect.Draw();

	//�X�v���C�g�̕`��-------------------------
	//titleSprite.SpriteDraw();

	Imgui::DrawImGui();
	//�`��R�}���h�����܂�
	BaseDirectX::UpdateBack();
}

void GameScene::EndDraw()
{
	//PostEffect��PreDraw
	postEffect.PreDraw();



	//PostEffect��PostDraw
	postEffect.PostDraw();

	BaseDirectX::clearColor[0] = 0.0f;
	BaseDirectX::clearColor[1] = 0.0f;
	BaseDirectX::clearColor[2] = 0.0f;
	BaseDirectX::clearColor[3] = 0.0f;
	BaseDirectX::UpdateFront();
	//PostEffect��Draw
	postEffect.Draw();

	//�X�v���C�g�̕`��-------------------------
	//titleSprite.SpriteDraw();

	Imgui::DrawImGui();
	//�`��R�}���h�����܂�
	BaseDirectX::UpdateBack();
}

void GameScene::LightUpdate()
{
	light->SetPointLightActive(0, false);
	light->SetSpotLightActive(0, Imgui::spotLight1);
	//light->SetCircleShadowActive(0, false);
	light->SetPointLightPos(0, XMFLOAT3(pointLightPos));
	light->SetPointLightAtten(0, XMFLOAT3(pointLightAtten));
	light->SetPointLightColor(0, XMFLOAT3(pointLightColor));
	light->SetSpotLightDir(0, XMVECTOR({ spotLightDir[0], spotLightDir[1], spotLightDir[2], 0 }));
	light->SetSpotLightPos(0, XMFLOAT3(spotLightPos));
	light->SetSpotLightColor(0, XMFLOAT3(Imgui::lightColor[0], Imgui::lightColor[1], Imgui::lightColor[2]));
	light->SetSpotLightAtten(0, XMFLOAT3(spotLightAtten));
	light->SetSpotLightAngle(0, XMFLOAT2(spotLightAngle));
}

XMFLOAT3 GameScene::Cannon(float vx, float v0, float t)
{
	XMFLOAT3 result{};
	result.x = ShlomonMath::EqualSpeed(vx, t);
	result.y = ShlomonMath::VerticalityUpcast(v0, t);
	return result;
}
