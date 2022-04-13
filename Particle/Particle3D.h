#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include <forward_list>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "../BaseDirectX/BaseDirectX.h"

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

/// <summary>
/// �p�[�e�B�N��
/// </summary>
class ParticleManager
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	ParticleManager() = default;
	~ParticleManager() = delete;
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;

public:
	static const int vertexCount = 2048;
private: // �萔
	//std::forward_list<Particle> particles;

public: // �ÓI�����o�֐�
	/// <summary>
	/// �ÓI������
	/// </summary>
	static bool StaticInitialize(ID3D12Device *device,  int window_width, int window_height, XMFLOAT3 eye, XMFLOAT3 target, XMFLOAT3 up);

	/// <summary>
	/// ���_���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	static const XMFLOAT3 &GetEye() { return eye; }

	/// <summary>
	/// ���_���W�̐ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	static void SetEye(XMFLOAT3 eye);

	/// <summary>
	/// �����_���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	static const XMFLOAT3 &GetTarget() { return target; }

	/// <summary>
	/// �����_���W�̐ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	static void SetTarget(XMFLOAT3 target);

	/// <summary>
	/// �x�N�g���ɂ��ړ�
	/// </summary>
	/// <param name="move">�ړ���</param>
	static void CameraMoveVector(XMFLOAT3 move);

	/// <summary>
	/// �x�N�g���ɂ�鎋�_�ړ�
	/// </summary>
	static void CameraMoveEyeVector(XMFLOAT3 move);
	
	// �f�o�C�X
	static ID3D12Device *device;

public: // �ÓI�����o�ϐ�
	// �r���[�s��
	static XMMATRIX matView;
	// �ˉe�s��
	static XMMATRIX matProjection;
	// ���_���W
	static XMFLOAT3 eye;
	// �����_���W
	static XMFLOAT3 target;
	// ������x�N�g��
	static XMFLOAT3 up;
	// ���_�o�b�t�@�r���[
	static D3D12_VERTEX_BUFFER_VIEW vbView;
	// �C���f�b�N�X�o�b�t�@�r���[
	static D3D12_INDEX_BUFFER_VIEW ibView;
	// ���_�f�[�^�z��
	//static VertexPos vertices[vertexCount];
	// ���_�C���f�b�N�X�z��
	//�r���{�[�h
	static XMMATRIX matBillboard;
	static XMMATRIX matBillboardY;
private:// �ÓI�����o�֐�

	/// <summary>
	/// �J����������
	/// </summary>
	/// <param name="window_width">��ʉ���</param>
	/// <param name="window_height">��ʏc��</param>
	static void InitializeCamera(int window_width, int window_height, XMFLOAT3 eye, XMFLOAT3 target, XMFLOAT3 up);

	/// <summary>
	/// �J����������
	/// </summary>
	static void CameraUpdate();

public: // �����o�֐�
	/// <summary>
	/// �r���[�s����X�V
	/// </summary>
	static void UpdateViewMatrix(XMFLOAT3 eye, XMFLOAT3 target, XMFLOAT3 up, bool isBillbord = true);
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList *cmdList;

private: // �����o�ϐ�
	
	//�ŃX�N���v�^�̉��Ԗڂ̃e�N�X�`�����g������I������
	int textureNum = 0;
};

class ParticleIndi
{
	struct VertexPos
	{
		DirectX::XMFLOAT3 pos;
		float scale;
	};

	struct Particle
	{
		using XMFLOAT3 = DirectX::XMFLOAT3;
		XMFLOAT3 position = {};
		XMFLOAT3 velocity = {};
		XMFLOAT3 accel = {};
		int frame = 0;
		int num_frame = 0;
		//�X�P�[��
		float scale = 1.0f;
		//�X�P�[�������l
		float s_scale = 1.0f;
		//�ŏI�n
		float e_scale = 0.0f;
		
	};
	struct ConstBufferData
	{
		DirectX::XMMATRIX mat;
		DirectX::XMMATRIX matBillboard;
		float alpha;
	};
public:
	// ���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootsignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	ComPtr<ID3D12PipelineState> pipelinestate;
	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;
	// ���_�f�[�^�z��
	VertexPos vertices[ParticleManager::vertexCount];
	//�p�[�e�B�N���̃R���e�i
	std::forward_list<Particle> particles;
	// �f�X�N���v�^�T�C�Y
	UINT descriptorHandleIncrementSize;

	//���ł��Ȃ����߂̃t���O
	bool isUI = false;
	//�c����ύX����Ȃ�d�l
	bool isSize = false;
	float alpha = 1.0f;
	ComPtr<ID3D12Resource> constBuff; // �萔�o�b�t�@
	// �F
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	// ���[�J���X�P�[��
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	// X,Y,Z�����̃��[�J����]�p
	DirectX::XMFLOAT3 rotation = { 0,0,0 };
	// ���[�J�����W
	DirectX::XMFLOAT3 position = { 0,0,0 };
	// ���[�J�����[���h�ϊ��s��
	DirectX::XMMATRIX matWorld;
	// �e�I�u�W�F�N�g
	ParticleManager *parent = nullptr;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;
	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;
	/// <summary>
	/// ���W�̎擾
	/// </summary>
	/// <returns>���W</returns>
	const DirectX::XMFLOAT3 &GetPosition() { return position; }
	/// <summary>
	/// ���W�̐ݒ�
	/// </summary>
	/// <param name="position">���W</param>
	void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }
	/// <summary>
	/// 3D�I�u�W�F�N�g����
	/// </summary>
	/// <returns></returns>
	ParticleIndi *Create(const wchar_t *texName);

	void CreateModel();
	/// <summary>
	/// �f�X�N���v�^�q�[�v�̏�����
	/// </summary>
	bool InitializeDescriptorHeap();
	/// <summary>
	/// �O���t�B�b�N�p�C�v���C������
	/// </summary>
	bool InitializeGraphicsPipeline();

	bool Initialize(const wchar_t *texName);

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update(DirectX::XMFLOAT3 eye, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up , DirectX::XMFLOAT3 *pos = nullptr, bool isBilbord = true);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	/// <returns>����</returns>
	bool LoadTexture(const wchar_t *texName);

	/// <summary>
	/// �p�[�e�B�N���̒ǉ�
	/// </summary>
	void Add(int life, DirectX::XMFLOAT3 &position, DirectX::XMFLOAT3 &velocity, DirectX::XMFLOAT3 &accel, float start_scale = 1.0f, float end_scale = 0.0f);

	/// <summary>
	/// �p�[�e�B�N���̐���
	/// </summary>
	void StartParticle(const DirectX::XMFLOAT3 emitterPosition = { 0, 0, 0 }, float startSize = 1.0f, float endSize = 0.0f, int life = 60);

	/// <summary>
	/// �~�`�p�[�e�B�N��
	/// </summary>
	void CiycleParticle(const DirectX::XMFLOAT3 emitterPosition = {0, 0, 0}, float startSize = 1.0f, float endSize = 0.0f, int life = 60);

	void ContinueParticle(const DirectX::XMFLOAT3 emitterPosition = {0, 0, 0}, float startSize = 1.0f, float endSize = 0.0f, int life = 60);

	/// <summary>
	/// ���̂悤�ȃp�[�e�B�N��
	/// </summary>
	void FrameParticle(const DirectX::XMFLOAT3 emitterPosition = {0, 0, 0}, float startSize = 1.0f, float endSize = 0.0f, int life = 60, int count = 2);

	/// <summary>
	/// �W�����v���p�p�[�e�B�N��
	/// </summary>
	void JumpParticle(const DirectX::XMFLOAT3 emitterPosition = {0, 0, 0}, float startSize = 1.0f, float endSize = 0.0f, int life = 60);

	/// <summary>
	/// �|�W�e�B�u�ȃp�[�e�B�N��
	/// </summary>
	void LuckParticle(const DirectX::XMFLOAT3 emitterPosition = {0, 0, 0}, float startSize = 1.0f, float endSize = 0.0f, int life = 60);

	/// <summary>
	/// �l�K�e�B�u�ȃp�[�e�B�N��
	/// </summary>
	void DownParticle(const DirectX::XMFLOAT3 emitterPosition = { 0, 0, 0 }, float startSize = 1.0f, float endSize = 0.0f, int life = 60);

	/// <summary>
	/// �|���\��
	/// </summary>
	void UI(const DirectX::XMFLOAT3 emitterPosition = { 0, 0, 0 }, float startSize = 5.0f, float endSize = 5.0f, int life = 1);
	void UI2(const DirectX::XMFLOAT3 emitterPosition = { 0, 0, 0 }, float startSize = 130.0f, float endSize = 130.0f);
	/// <summary>
	/// �p�[�e�B�N�����������Ă���
	/// </summary>
	void UpParticle(const DirectX::XMFLOAT3 emitterPosition = { 0, 0, 0 }, float startSize = 0.0f, float endSize = 1.0f, int life = 30);

	/// <summary>
	/// �Jparticle
	/// </summary>
	void RainParticle(const DirectX::XMFLOAT3 emitterPosition = { 0, 100, 0 }, float startSize = 0.0f, float endSize = 1.0f, int life = 60, int count = 30);

	/// <summary>
	/// ����particle
	/// </summary>
	void BaffParticle(const DirectX::XMFLOAT3 emitterPosition = { 0, 0, 0 }, float startSize = 4.0f, float endSize = 0.0f, int life = 60, int count = 20);


	/// <summary>
	/// ���b�N�I��
	/// </summary>
	void RockOn(const DirectX::XMFLOAT3 emitterPosition = { 0, 0, 0 }, float startSize = 5.0f, float endSize = 5.0f);

	/// <summary>
	/// 
	/// </summary>
	void StarParticle(const DirectX::XMFLOAT3 emitterPosition = { 0, 0, 0 }, float startSize = 6.0f, float endSize = 3.0f, int life = 60, int count = 20);

	void BackParticle(const DirectX::XMFLOAT3 emitterPosition = { 0, 0, 0 }, float startSize = 2.0f, float endSize = 2.0f, int life = 180);

	void FlashParticle(const DirectX::XMFLOAT3 emitterPosition = { 0, 0, 0 }, float startSize = 2.0f, float endSize = 2.0f, int life = 60);
};
/// <summary>
	/// �`��
	/// </summary>
void ParticleDraw(ID3D12GraphicsCommandList *cmdList, const ParticleIndi *particle);
/// <summary>
/// �`��㏈��
/// </summary>
void PostDraw();

class ParticleControl
{
private:
	ParticleControl();
	~ParticleControl();
public:
	ParticleControl(const ParticleControl &obj) = delete;
	ParticleControl &operator=(const ParticleControl &obj) = delete;
	static ParticleIndi *attackEffect;
	static ParticleIndi *expEffect;
	static ParticleIndi *flashEffect;
	static ParticleIndi *rockOnEffect;
	static ParticleIndi *thunder1;
	static ParticleIndi *thunder2;
	static ParticleIndi *thunder3;
	static ParticleIndi *numbers[10];
	static void Update();
	static void Init();
	static void Draw();
};