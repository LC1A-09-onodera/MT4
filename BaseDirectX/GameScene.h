#pragma once
#include "../3DModel/Model.h"
#include "../Camera/Camera.h"
#include "../Sprite/Sprite.h"
//#include "../Enemy/Enemy.h"
#include "../Light/Light.h"
#include "../RenderTarget/RenderTarget.h"
#include "../PostEffect/PostEffect.h"
#include "../FBXObject/FBXObject.h"
#include "../FbxLoader/FbxLoader.h"
enum GameSceneName{TITLE, SELECT, GAME, END, RESULT};

class GameScene
{
private:
	//�X�V
	void GameUpdate();
	void TitleUpdate();
	void EndUpdate();
	void SelectUpdate();
	void ResultUpdate();
	//�`��
	void GameDraw();
	void TitleDraw();
	void EndDraw();
	void SelectDraw();
	void ResultDraw();
	int SceneNum = TITLE;

public:
	GameScene();
	~GameScene();
	//�V�[���؂�ւ�
	void SceneManageUpdateAndDraw();
	//������
	void Init();
	
	void LightUpdate();

public://�V�[���Ŏg������
	Light *light = nullptr;
	PostEffect post;
	float pointLightPos[3] = {0, 1, 0};
	float pointLightColor[3] = {1, 1, 1};
	float pointLightAtten[3] = {0.3f, 0.1f, 0.1f};

	float spotLightDir[3] = {0, -1, 0};
	float spotLightPos[3] = {0, 1, -100};
	float spotLightColor[3] = {0.9f, 0.9f, 0.9f};
	float spotLightAtten[3] = {0.0f, 0.0f, 0.0f};
	float spotLightAngle[2] = {1.0f, 500.0f};

	float circleShadowDir[3] = {0, -1, 0};
	float circleShadowAtten[3] = {0.5f, 0.6f, 0.0f};
	float circleShadowFactorAngle[2] = {0.0f, 0.0f};

	PostEffect postEffect;

	Model maru;
	EachInfo each[5];
	XMFLOAT3 vel{0, -1, 0};
	float acc;
	float equalSpeed;
	float accSpeed;
	const float G;
	float t;
	void VelFall(XMFLOAT3 vel , float t);
	void AccFall(float acc, float t);
	void EqualFall(float equalSpeed, float t);
	void EqualAccFall(float acc,float t);
	void Fall(float gravity, float t);
};