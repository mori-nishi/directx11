#include <Windows.h>
#include <tchar.h>
#include <d3d11.h>
#include "vs.h"
#include "ps.h"
#pragma once
#pragma comment(lib, "d3d11.lib")
\

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

//�܂��̓E�B���h�E�̃R�[���o�b�N�֐����L�q
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_CLOSE:
		PostMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hWnd, message, wParam, lParam));
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	//�E�B���h�E�N���X��o�^����
	TCHAR szWindowClass[] = "3DDISPPG";
	WNDCLASS wcex;
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL;
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	RegisterClass(&wcex);

	//�E�B���h�E���N���G�C�g
	HWND hWnd;
	hWnd = CreateWindow(szWindowClass,
		"3D Disp Pg",
		WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0,
		0,
		1920,
		1080,
		NULL,
		NULL,
		hInstance,
		NULL);

	//��ŏ������C�Ƃ肠�����E�B���h�E���o���Ȃ��Ƃ킩��h���̂�
	//�E�B���h�E���N���G�C�g�̒��ォ��
	//�f�o�C�X�̐���
	HRESULT hr;
	ID3D11Device* hpDevice = NULL;
	ID3D11DeviceContext* hpDeviceContext = NULL;
	hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&hpDevice,
		NULL,
		&hpDeviceContext);
	if FAILED(hr)
	{
		MessageBox(hWnd, _T("D3D11CreateDevice"), _T("Err"), MB_ICONSTOP);
		/*goto End;*/
	}

	//�C���^�[�t�F�[�X�擾
	IDXGIDevice1* hpDXGI = NULL;
	if (FAILED(hpDevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&hpDXGI))) {
		MessageBox(hWnd, _T("QueryInterface"), _T("Err"), MB_ICONSTOP);
		/*goto End;*/
	}

	//�A�_�v�^�[�擾
	IDXGIAdapter* hpAdapter = NULL;
	if (FAILED(hpDXGI->GetAdapter(&hpAdapter))) {
		MessageBox(hWnd, _T("GetAdapter"), _T("Err"), MB_ICONSTOP);
		/*goto End;*/
	}

	//�t�@�N�g���[�擾
	IDXGIFactory* hpDXGIFactory = NULL;
	hpAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&hpDXGIFactory);
	if (hpDXGIFactory == NULL) {
		MessageBox(hWnd, _T("GetParent"), _T("Err"), MB_ICONSTOP);
		/*goto End;*/
	}

	//ALT+Enter�Ńt���X�N���[����������
	if (FAILED(hpDXGIFactory->MakeWindowAssociation(hWnd, 0))) {
		MessageBox(hWnd, _T("MakeWindowAssociation"), _T("Err"), MB_ICONSTOP);
		/*goto End;*/
	}

	//�X���b�v�`�F�C���쐬
	IDXGISwapChain* hpDXGISwpChain = NULL;
	DXGI_SWAP_CHAIN_DESC hDXGISwapChainDesc;
	hDXGISwapChainDesc.BufferDesc.Width = 1980;
	hDXGISwapChainDesc.BufferDesc.Height = 1080;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	hDXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	hDXGISwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	hDXGISwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	hDXGISwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	hDXGISwapChainDesc.SampleDesc.Count = 1;
	hDXGISwapChainDesc.SampleDesc.Quality = 0;
	hDXGISwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	hDXGISwapChainDesc.BufferCount = 1;
	hDXGISwapChainDesc.OutputWindow = hWnd;
	hDXGISwapChainDesc.Windowed = TRUE;
	hDXGISwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hDXGISwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	if (FAILED(hpDXGIFactory->CreateSwapChain(hpDevice, &hDXGISwapChainDesc, &hpDXGISwpChain))) {
		MessageBox(hWnd, _T("CreateSwapChain"), _T("Err"), MB_ICONSTOP);
		/*goto End;*/
	}

	//���̃X���b�v�`�F�C���̃o�b�N�o�b�t�@�擾
	ID3D11Texture2D* hpBackBuffer = NULL;
	if (FAILED(hpDXGISwpChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&hpBackBuffer))) {
		MessageBox(hWnd, _T("SwpChain GetBuffer"), _T("Err"), MB_ICONSTOP);
		/*goto End;*/
	}

	//���̃o�b�N�o�b�t�@����`��^�[�Q�b�g����
	ID3D11RenderTargetView *hpRenderTargetView = NULL;
	if (FAILED(hpDevice->CreateRenderTargetView(hpBackBuffer, NULL, &hpRenderTargetView))) {
		MessageBox(hWnd, _T("CreateRenderTargetView"), _T("Err"), MB_ICONSTOP);
		/*goto End;*/
	}

	//�X�ɂ��̕`��^�[�Q�b�g���R���e�L�X�g�ɐݒ�
	hpDeviceContext->OMSetRenderTargets(1, &hpRenderTargetView, NULL);

	//�r���[�|�[�g�ݒ�
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = 1920;
	vp.Height = 1080;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	hpDeviceContext->RSSetViewports(1, &vp);
	//�|���S�����_�\����
	struct Vertex3D {
		float pos[3];	//x-y-z
		float col[4];	//r-g-b-a
	};

	const int TYOUTEN = 3;	//�|���S���̒��_��

							//���_�f�[�^(�O�p�|���S��1��)
	Vertex3D hVectorData[TYOUTEN] = {
		{ { +0.0f, +0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } },
	{ { +0.5f, -0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } },
	{ { -0.5f, -0.5f, +0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f } }
	};

	//���_���C�A�E�g
	//�T�Ԗڂ̃p�����[�^�͐擪����̃o�C�g���Ȃ̂ŁCCOLOR�ɂ�POSITION��float�^4�o�C�g�~3���L�q
	D3D11_INPUT_ELEMENT_DESC hInElementDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//���_�o�b�t�@�쐬
	D3D11_BUFFER_DESC hBufferDesc;
	hBufferDesc.ByteWidth = sizeof(Vertex3D) * TYOUTEN;
	hBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	hBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	hBufferDesc.CPUAccessFlags = 0;
	hBufferDesc.MiscFlags = 0;
	hBufferDesc.StructureByteStride = sizeof(float);

	D3D11_SUBRESOURCE_DATA hSubResourceData;
	hSubResourceData.pSysMem = hVectorData;
	hSubResourceData.SysMemPitch = 0;
	hSubResourceData.SysMemSlicePitch = 0;

	ID3D11Buffer* hpBuffer;
	if (FAILED(hpDevice->CreateBuffer(&hBufferDesc, &hSubResourceData, &hpBuffer))) {
		MessageBox(hWnd, _T("CreateBuffer"), _T("Err"), MB_ICONSTOP);
	/*	goto End;*/
	}

	//���̒��_�o�b�t�@���R���e�L�X�g�ɐݒ�
	UINT hStrides = sizeof(Vertex3D);
	UINT hOffsets = 0;
	hpDeviceContext->IASetVertexBuffers(0, 1, &hpBuffer, &hStrides, &hOffsets);

	//�v���~�e�B�u(�|���S���̌`��)���R���e�L�X�g�ɐݒ�
	hpDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//���_���C�A�E�g�쐬
	ID3D11InputLayout* hpInputLayout = NULL;
	if (FAILED(hpDevice->CreateInputLayout(hInElementDesc, ARRAYSIZE(hInElementDesc), &g_vs_main, sizeof(g_vs_main), &hpInputLayout))) {
		MessageBox(hWnd, _T("CreateInputLayout"), _T("Err"), MB_ICONSTOP);
		goto End;
	}

	//���_���C�A�E�g���R���e�L�X�g�ɐݒ�
	hpDeviceContext->IASetInputLayout(hpInputLayout);

	//���_�V�F�[�_�[����
	ID3D11VertexShader* hpVertexShader;
	if (FAILED(hpDevice->CreateVertexShader(&g_vs_main, sizeof(g_vs_main), NULL, &hpVertexShader))) {
		MessageBox(hWnd, _T("CreateVertexShader"), _T("Err"), MB_ICONSTOP);
		goto End;
	}

	//���_�V�F�[�_�[���R���e�L�X�g�ɐݒ�
	hpDeviceContext->VSSetShader(hpVertexShader, NULL, 0);

	//�s�N�Z���V�F�[�_�[����
	ID3D11PixelShader* hpPixelShader;
	if (FAILED(hpDevice->CreatePixelShader(&g_ps_main, sizeof(g_ps_main), NULL, &hpPixelShader))) {
		MessageBox(hWnd, _T("CreateVertexShader"), _T("Err"), MB_ICONSTOP);
		goto End;
	}

	//�s�N�Z���V�F�[�_�[���R���e�L�X�g�ɐݒ�
	hpDeviceContext->PSSetShader(hpPixelShader, NULL, 0);

	//���C�����[�v
	MSG hMsg;
	while (true) {
		while (PeekMessageW(&hMsg, NULL, 0, 0, PM_REMOVE)) {
			if (hMsg.message == WM_QUIT) {
				goto End;
			}
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}

		//�`��
		hpDeviceContext->Draw(TYOUTEN, 0);

		//�o�b�N�o�b�t�@���X���b�v
		hpDXGISwpChain->Present(0, 0);
	}
	//���C�����[�v
	/*MSG hMsg;*/
	while (true) {
		while (PeekMessageW(&hMsg, NULL, 0, 0, PM_REMOVE)) {
			if (hMsg.message == WM_QUIT) {
			/*	goto End;*/
			}
			TranslateMessage(&hMsg);
			DispatchMessage(&hMsg);
		}

		float ClearColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
		hpDeviceContext->ClearRenderTargetView(hpRenderTargetView, ClearColor);
		hpDXGISwpChain->Present(0, 0);
	}

End:
	//�|�C���^�Ő����������̂̓����[�X����K�v������
	//�ǂ̕ϐ������ʂ��Ղ��悤�ɁChp�Ŏn�߂Ă���
	SAFE_RELEASE(hpRenderTargetView);
	SAFE_RELEASE(hpBackBuffer);
	SAFE_RELEASE(hpDXGISwpChain);
	SAFE_RELEASE(hpDXGIFactory);
	SAFE_RELEASE(hpAdapter);
	SAFE_RELEASE(hpDXGI);
	SAFE_RELEASE(hpDeviceContext);
	SAFE_RELEASE(hpDevice);

	return 0;
}