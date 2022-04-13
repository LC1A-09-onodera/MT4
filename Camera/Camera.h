#include <DirectXMath.h>
#include "../BaseDirectX/vec3.h"
#include "../DX12operator.h"
using namespace DirectX;

class Camera
{
public:
	//�ʒu
	static Vec3 eye;
	static Vec3 eyePos;
	static Vec3 eyeStartPosition;
	static Vec3 eyeGoalPosition;
	static float shakePower;
	static bool isShake;
	//�ǂ��𒍎��_�Ƃ��邩
	static Vec3 target;
	static Vec3 targetStartPosition;
	static Vec3 targetGoalPosition;
	//�J�����̌����H
	static Vec3 up;
	static Vec3 cameraUp;
	//�����Ă�������̒P�ʃx�N�g��
	static XMFLOAT3 direction;
	//�r���[�s��̍쐬
	static XMMATRIX matView;
	//�r���{�[�h
	static XMMATRIX matBillboard;
	//Y���r���{�[�h
	static XMMATRIX matBillboardY;

	static XMMATRIX rotM;

	static XMVECTOR vv0;

	static float cameraRightAngle;
	static float cameraUpAngle;


	static float rotX;
	static float rotY;
	static float rotZ;

	static float cameraAngleHorizonal;
	static float cameraAngleVertical;

	static void Init();
	static void Update();
	static void EyeMove(Vec3 moveAmount);
	static void EyeAndTargetMove(XMFLOAT3 moveAmount);
	static void TargetMove(XMFLOAT3 moveAmount);
	static void FPSTargetMove(XMFLOAT3 moveAmount, float R = 100.0f);
	static float TargetLength();
	static XMFLOAT3 GetTargetDirection();
	static XMFLOAT3 GetTargetDirectionSideR();
	static XMFLOAT3 GetTargetDirectionSideL();
	static void setEyePositionTPS(XMFLOAT3 Amount);
	static void setEyeTargetTPS(XMFLOAT3 Amount);
	static void eyeMoveCircleHorizonal(XMFLOAT3 Amount);
	static void eyeMoveCircleVertical(XMFLOAT3 Amount);
	static void setEye(XMFLOAT3 Amount);
	static void SetTarget(const XMFLOAT3& target);
	static void QuaternionRotation(const float &RightAngle = NULL, const float &UpAngle = NULL);
	static void ShakeUpdate();
	static void SetShake(float shakePower);

	static XMFLOAT3 GetMousePosition();
	static XMFLOAT3 EyeToMouseVec();
	static XMFLOAT3 MousePosition(float z);
};