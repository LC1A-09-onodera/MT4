#include "ImguiControl.h"
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx12.h"
#include "../imgui/imgui_impl_win32.h"
#include "../BaseDirectX/BaseDirectX.h"
//#include "../BaseDirectX/GameScene.h"

ComPtr<ID3D12DescriptorHeap> Imgui::imguiDescHeap;
ComPtr<ID3D12DescriptorHeap> Imgui::heapForImgui;
bool Imgui::spotLight1;
float Imgui::lightColor[3];
float Imgui::dethLine = 25.0f;

float Imgui::v1 = 1.0f;
float Imgui::v2 = -0.4f;

float Imgui::m1 = 1.5f;
float Imgui::m2 = 1.0f;

ComPtr<ID3D12DescriptorHeap> Imgui::CreateDescrriptorHeapForImgui()
{
    ComPtr<ID3D12DescriptorHeap> ret;
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    desc.NodeMask = 0;
    desc.NumDescriptors = 1;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    BaseDirectX::dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(ret.ReleaseAndGetAddressOf()));
    return ret;
}

ComPtr<ID3D12DescriptorHeap> Imgui::GetHeapForImgui()
{
    return imguiDescHeap;
}

void Imgui::DrawImGui()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("Space->time = 0");//ウィンドウの名前
    ImGui::SetWindowSize(ImVec2(200, 100), ImGuiCond_::ImGuiCond_FirstUseEver);

    //imguiここにいろいろ書く
    /*ImGui::Checkbox("spotLight1", &spotLight1);
    ImGui::SliderFloat3("ligthColor", lightColor, 0.0f, 1.0f);*/
    //ImGui::SliderFloat("dethLine", &dethLine, 0.0f, 50.0f);
    ImGui::SliderFloat("v1", &v1, 0.0f, 2.0f);
    ImGui::SliderFloat("v2", &v2, -2.0f, 0.0f);

    ImGui::SliderFloat("m1", &m1, 0.0f, 5.0f);
    ImGui::SliderFloat("m2", &m2, 0.0f, 5.0f);
    //------------

    ImGui::End();

    ImGui::Render();
    BaseDirectX::cmdList->SetDescriptorHeaps(1, GetHeapForImgui().GetAddressOf());
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), BaseDirectX::cmdList.Get());
}

void Imgui::Init()
{
    //imgui
    imguiDescHeap = CreateDescrriptorHeapForImgui();
    if (imguiDescHeap == nullptr)
    {
        return;
    }
    if (ImGui::CreateContext() == nullptr)
    {
        assert(0);
        return;
    }
    //[ImGui::]
    bool blnResult = ImGui_ImplWin32_Init(WindowsAPI::hwnd);
    if (!blnResult)
    {
        assert(0);
        return;
    }
    blnResult = ImGui_ImplDX12_Init(BaseDirectX::dev.Get(), 3, DXGI_FORMAT_R8G8B8A8_UNORM, GetHeapForImgui().Get(), GetHeapForImgui()->GetCPUDescriptorHandleForHeapStart(), GetHeapForImgui()->GetGPUDescriptorHandleForHeapStart());
}

void Imgui::ChangeInfo(int& intOriginal, int& imguiInfo)
{
    intOriginal = imguiInfo;
    return;
}

void Imgui::ChangeInfo(float& floatOriginal, float& imguiInfo)
{
    floatOriginal = imguiInfo;
    return;
}

void Imgui::ChangeInfo(XMFLOAT3& xmfloat3Original, XMFLOAT3& imguiInfo)
{
    xmfloat3Original = imguiInfo;
    return;
}

void Imgui::GetAndDrawInfo(const int &intOriginal, int& imguiInfo)
{
    imguiInfo = intOriginal;
    return;
}

void Imgui::GetAndDrawInfo(const float &floatOriginal, float& imguiInfo)
{
    imguiInfo = floatOriginal;
    return;
}

void Imgui::GetAndDrawInfo(const XMFLOAT3& xmfloat3Original, XMFLOAT3& imguiInfo)
{
    imguiInfo = xmfloat3Original;
    return;
}
