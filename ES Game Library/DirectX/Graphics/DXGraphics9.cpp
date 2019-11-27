/*
================================================================================
					 �I�u�W�F�N�g�w�������Q�[���v���O���~���O
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 �y�Ώۂn�r�z
     Microsoft Windows 7�ȍ~

 �y�R���p�C���z
     Microsoft VisualC++ 2013

 �y�v���O�����z
	 DXGraphics9.cpp
				DirectX Graphics9�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "DXGraphics9.hpp"

#pragma warning(push)
#pragma warning(disable:4005)
#include <D2D1.h>
#include <wincodec.h>
#pragma warning(pop)

#include <tchar.h>
#include <vector>

// #include <olectl.h>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "windowscodecs.lib")

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CDXGraphics9::CDXGraphics9() : m_pD3D(NULL),			    m_pD3DDevice(NULL),
							   m_pD3DXSprite(NULL),         m_pD3DXEffect(NULL),
							   m_Width(0),                  m_Height(0),
							   m_hBackBufferDC(NULL),
							   m_pBackBufferSurface(NULL),  m_pDepthStencilSurface(NULL),
							   m_pRenderTargetVertex(NULL),
							   m_RenderTargetWidth(0),      m_RenderTargetHeight(0)
{
	::ZeroMemory(&m_PresentParams, sizeof(m_PresentParams));
	::ZeroMemory(&m_DisplayMode,   sizeof(m_DisplayMode  ));
	::ZeroMemory(&m_AlphaState,    sizeof(m_AlphaState   ));

	m_pPresentRect[0] = NULL;
	m_pPresentRect[1] = NULL;
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CDXGraphics9::~CDXGraphics9()
{
	Release();
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CDXGraphics9::Initialize(const HWND hWnd, const UINT inWidth, UINT inHeight,
							  const bool inWindowed)
{
	Release();

	// Direct3D9�I�u�W�F�N�g����
	m_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION);
	if(m_pD3D == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3D9�I�u�W�F�N�g�������s(CDXGraphics9_Initialize)\n")); 
		return false;
	}

	m_Width  = inWidth;
	m_Height = inHeight;

	// �𑜓x����
	D3DDISPLAYMODE   dpm;
	D3DFORMAT        format[3] = {D3DFMT_R5G6B5, D3DFMT_X1R5G5B5, D3DFMT_X8R8G8B8};

	m_DisplayMode.Format = D3DFMT_UNKNOWN;
	for(UINT fmt = 0; fmt < 3; fmt++) {
		const UINT   AdapterModeCount = m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, format[fmt]);
		for(UINT amc = 0; amc < AdapterModeCount; amc++) {
			if(m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, format[fmt], amc, &dpm) != D3D_OK)
				continue;
			if(m_Width <= dpm.Width && m_Height <= dpm.Height) {
				m_DisplayMode.Width  = dpm.Width;
				m_DisplayMode.Height = dpm.Height;
				m_DisplayMode.Format = dpm.Format;
				break;
			}	// if
		}	// for(amc)
	}	// for(fmt)

	if(m_DisplayMode.Format == D3DFMT_UNKNOWN) {
		::OutputDebugString(TEXT("*** Error - �f�B�X�v���C���Ή�(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	// �v���[���e�[�V�����p�����[�^ �| ���ʕ��ݒ�
	m_PresentParams.BackBufferCount            = 1;										// �o�b�N�o�b�t�@��
	m_PresentParams.MultiSampleType            = D3DMULTISAMPLE_NONE;					// �}���`�T���v���^�C�v
	m_PresentParams.MultiSampleQuality         = 0;										// �}���`�T���v���i��
	m_PresentParams.hDeviceWindow              = hWnd;									// �^�[�Q�b�g�E�B���h�E
	m_PresentParams.EnableAutoDepthStencil     = TRUE;									// �[�x�o�b�t�@
	m_PresentParams.AutoDepthStencilFormat     = D3DFMT_D24S8;							// �[�x�o�b�t�@�t�H�[�}�b�g
	m_PresentParams.Flags                      = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;	// �t���O
	m_PresentParams.FullScreen_RefreshRateInHz = 0;										// ���t���b�V�����[�g

	// �v���[���e�[�V�����p�����[�^ �| �񋤒ʕ��ݒ�
	SetPresentParams(inWindowed);

	// Direct3DDevice9�I�u�W�F�N�g����
	const DWORD   Behavior[3] = {D3DCREATE_MIXED_VERTEXPROCESSING    | D3DCREATE_MULTITHREADED,
								 D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
								 D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED};
	HRESULT       hr;
	for(int i = 0; i < 3; i++) {
		hr = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
								  Behavior[i], &m_PresentParams, &m_pD3DDevice);
		if(hr == D3D_OK)
			break;
	}
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9�I�u�W�F�N�g�������s(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	// ��ʏ���
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);

	// �X�v���C�g�C���^�t�F�[�X����
	if(::D3DXCreateSprite(m_pD3DDevice, &m_pD3DXSprite) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �X�v���C�g�C���^�t�F�[�X�������s(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	// �X�v���C�g�o�b�`������
	if(DXGSpriteBatch().Initialize(m_pD3DXSprite) == false) {
//		::OutputDebugString(TEXT("*** Error - �X�v���C�g�o�b�`���������s(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	// �o�b�N�o�b�t�@�T�[�t�F�X�擾
	if(m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBufferSurface) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �o�b�N�o�b�t�@�T�[�t�F�X�擾���s(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	// �f�v�X�X�e���V���T�[�t�F�X�擾
	if(m_pD3DDevice->GetDepthStencilSurface(&m_pDepthStencilSurface) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �f�v�X�X�e���V���T�[�t�F�X�擾���s(CDXGraphics9_Initialize)\n"));
		Release();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	���
//------------------------------------------------------------------------------
void CDXGraphics9::Release()
{
	m_ProtectedResource.clear();

	ReleaseAllRenderTargets();				// �S�����_�����O�^�[�Q�b�g���
	ReleaseAllStateBlocks();				// �S�X�e�[�g�u���b�N���
	ReleaseAllFonts();						// �S�t�H���g���
	ReleaseAllSprites();					// �S�X�v���C�g���
	ReleaseAllAnimationModels();			// �S�A�j���[�V�������f�����
	ReleaseAllModels();						// �S���f�����
	ReleaseAllVertexBuffers();				// �S�o�[�e�b�N�X�o�b�t�@���
	ReleaseAllEffects();					// �S�G�t�F�N�g���

	ReleaseDC();

	SafeRelease(m_pDepthStencilSurface);
	SafeRelease(m_pBackBufferSurface);

	DXGSpriteBatch().Release();

	SafeRelease(m_pD3DXEffect);
	SafeRelease(m_pD3DXSprite);
	SafeRelease(m_pD3DDevice);
	SafeRelease(m_pD3D);

	delete m_pPresentRect[1];
	m_pPresentRect[1] = NULL;

	delete m_pPresentRect[0];
	m_pPresentRect[0] = NULL;

	::ZeroMemory(&m_DisplayMode,   sizeof(m_DisplayMode  ));
	::ZeroMemory(&m_PresentParams, sizeof(m_PresentParams));

	m_RenderTargetHeight = 0;
	m_RenderTargetWidth  = 0;

	m_Height = 0;
	m_Width  = 0;
}

//------------------------------------------------------------------------------
//	�f�o�C�X���Z�b�g
//------------------------------------------------------------------------------
bool CDXGraphics9::ResetDevice(const bool inTestDevice)
{
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_ResetDevice)\n"));
		return false;
	}

	// �f�o�C�X�����X�g���Ă��邩�H
	if(inTestDevice) {
		if(m_pD3DDevice->TestCooperativeLevel() == D3D_OK)
			return true;
	}

	// ���C�g�ۑ�
	UINT   idx = 0;
	std::vector<DXGLIGHT>       LightData(m_LightIndex.size());
	std::set<DWORD>::iterator   light_it;
	for(light_it = m_LightIndex.begin(); light_it != m_LightIndex.end(); light_it++) {
		LightData[idx].Index = *light_it;
		m_pD3DDevice->GetLight      (*light_it, &LightData[idx].Light);
		m_pD3DDevice->GetLightEnable(*light_it, &LightData[idx].Enable);
	}

	// �����_�[�^�[�Q�b�g���Z�b�g�O����
	std::list<IRenderTarget*>::iterator     render_it;
	for(render_it = m_RenderTarget.begin(); render_it != m_RenderTarget.end(); render_it++)
		(*render_it)->OnLostDevice();

	// �t�H���g���Z�b�g�O����
	std::list<IDXGFont*>::iterator     font_it;
	for(font_it = m_Font.begin(); font_it != m_Font.end(); font_it++)
		(*font_it)->OnLostDevice();

	// �G�t�F�N�g���Z�b�g�O����
	std::list<IEffect*>::iterator   effect_it;
	for(effect_it = m_Effect.begin(); effect_it != m_Effect.end(); effect_it++)
		(*effect_it)->OnLostDevice();

	// �X�v���C�g�o�b�`���Z�b�g�O����
	m_pD3DXSprite->OnLostDevice();

	// �o�b�N�o�b�t�@���Z�b�g�O����
	SafeRelease(m_pDepthStencilSurface);
	SafeRelease(m_pBackBufferSurface  );

	// �f�o�C�X���Z�b�g
	if(m_pD3DDevice->Reset(&m_PresentParams) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �f�o�C�X���Z�b�g���s(CDXGraphics9_ResetDevice)\n"));
		return false;
	}
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f, 0);

	// �o�b�N�o�b�t�@���Z�b�g�㏈��
	m_pD3DDevice ->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_pBackBufferSurface);
	m_pD3DDevice ->GetDepthStencilSurface(&m_pDepthStencilSurface);

	// �X�v���C�g�o�b�`���Z�b�g�㏈��
	m_pD3DXSprite->OnResetDevice();

	// �G�t�F�N�g���Z�b�g�㏈��
	for(effect_it = m_Effect.begin(); effect_it != m_Effect.end(); effect_it++)
		(*effect_it)->OnResetDevice();

	// �t�H���g���Z�b�g�㏈��
	for(font_it = m_Font.begin(); font_it != m_Font.end(); font_it++)
		(*font_it)->OnResetDevice();

	// �����_�[�^�[�Q�b�g���Z�b�g�㏈��
	for(render_it = m_RenderTarget.begin(); render_it != m_RenderTarget.end(); render_it++)
		(*render_it)->OnResetDevice();

	// �����_�����O�X�e�[�g����
	std::map<D3DRENDERSTATETYPE, DXGSAVESTATE>::iterator   state_it;
	for(state_it = m_RenderState.begin(); state_it != m_RenderState.end(); state_it++) {
		if(state_it->second.Value != state_it->second.DefValue)
			m_pD3DDevice->SetRenderState(state_it->first, state_it->second.Value);
	}

	// ���C�g����
	for(idx = 0; idx < LightData.size(); idx++){
		m_pD3DDevice->SetLight   (LightData[idx].Index,	&LightData[idx].Light);
		m_pD3DDevice->LightEnable(LightData[idx].Index,	 LightData[idx].Enable);
	}

	return true;
}

//------------------------------------------------------------------------------
//	���[�h�ύX
//------------------------------------------------------------------------------
bool CDXGraphics9::ChangeMode(const bool inWindowed)
{
	if(m_PresentParams.Windowed == static_cast<BOOL>(inWindowed))
		return true;

	m_PresentParams.Windowed = inWindowed;
	SetPresentParams(m_PresentParams.Windowed);		// �V�����p�����[�^��ݒ肷��
	return ResetDevice(false);						// �V�����p�����[�^�Ńf�o�C�X�����Z�b�g����
}

//------------------------------------------------------------------------------
//	WM_PAINT���b�Z�[�W����
//------------------------------------------------------------------------------
void CDXGraphics9::OnPaint(const PAINTSTRUCT& inPaint)
{
	if(m_pD3DDevice == NULL)
		return;

	HDC   hSrcDC = GetDC();

	// �ĕ`�悵�Ȃ���΂Ȃ�Ȃ��̈���N���C�A���g�̈�ɓ]������
	::BitBlt(inPaint.hdc, inPaint.rcPaint.left, inPaint.rcPaint.top,
			 inPaint.rcPaint.right - inPaint.rcPaint.left, inPaint.rcPaint.bottom - inPaint.rcPaint.top,
			 hSrcDC, inPaint.rcPaint.left, inPaint.rcPaint.top, SRCCOPY);

	ReleaseDC();
}

//------------------------------------------------------------------------------
//	�v���[���e�[�V�����p�����[�^�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetPresentParams(const BOOL inWindowed)
{
	if(inWindowed != FALSE) {
		// �E�B���h�E���[�h
		m_PresentParams.Windowed             = TRUE;								// ���샂�[�h
		m_PresentParams.BackBufferWidth      = 0;									// �o�b�N�o�b�t�@��
		m_PresentParams.BackBufferHeight     = 0;									// �o�b�N�o�b�t�@����
		m_PresentParams.BackBufferFormat     = D3DFMT_UNKNOWN;						// �o�b�N�o�b�t�@�t�H�[�}�b�g
		m_PresentParams.SwapEffect           = D3DSWAPEFFECT_DISCARD;				// �X���b�v�G�t�F�N�g
//		m_PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;		// �X���b�v�C���^�[�o��
		m_PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;				// �X���b�v�C���^�[�o��

		delete m_pPresentRect[1];
		m_pPresentRect[1] = NULL;

		delete m_pPresentRect[0];
		m_pPresentRect[0] = NULL;
	} else {
		// �t���X�N���[�����[�h
		m_PresentParams.Windowed             = FALSE;								// ���샂�[�h
		m_PresentParams.BackBufferWidth      = m_DisplayMode.Width;					// �o�b�N�o�b�t�@��
		m_PresentParams.BackBufferHeight     = m_DisplayMode.Height;				// �o�b�N�o�b�t�@����
		m_PresentParams.BackBufferFormat     = m_DisplayMode.Format;				// �o�b�N�o�b�t�@�t�H�[�}�b�g
		m_PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE;				// �X���b�v�C���^�[�o��

		// �X���b�v�G�t�F�N�g
		if(m_DisplayMode.Width == m_Width && m_DisplayMode.Height == m_Height) {
			m_PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
		} else {
			m_PresentParams.SwapEffect = D3DSWAPEFFECT_COPY;

			// �`��̈�]�����ݒ�
			m_pPresentRect[0] = new RECT;
			m_pPresentRect[0]->left   = 0;
			m_pPresentRect[0]->top    = 0;
			m_pPresentRect[0]->right  = m_pPresentRect[0]->left + m_Width;
			m_pPresentRect[0]->bottom = m_pPresentRect[0]->top  + m_Height;

			// �`��̈�]����ݒ�
			m_pPresentRect[1] = new RECT;
			m_pPresentRect[1]->left   = (m_DisplayMode.Width  - m_Width ) / 2;
			m_pPresentRect[1]->top    = (m_DisplayMode.Height - m_Height) / 2;
			m_pPresentRect[1]->right  = m_pPresentRect[1]->left + m_Width;
			m_pPresentRect[1]->bottom = m_pPresentRect[1]->top  + m_Height;
		}
	}
}

//------------------------------------------------------------------------------
//	�r���[�|�[�g�N���A
//------------------------------------------------------------------------------
void CDXGraphics9::Clear(const D3DCOLOR inColor, const DWORD inFlags,
						 const D3DRECT* inRect , const DWORD inRectCount)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_Clear)\n"));
		return;
	}
#endif

	m_pD3DDevice->Clear(inRectCount, inRect, inFlags, inColor, 1.0f, 0);
}

//------------------------------------------------------------------------------
//	�t���[���X�V
//------------------------------------------------------------------------------
//void CDXGraphics9::UpdateFrame(const bool inWaitVSync)
void CDXGraphics9::UpdateFrame()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_UpdateFrame)\n"));
		return;
	}
#endif
/*
	// �����A����҂�
	if(inWaitVSync == true) {
		D3DRASTER_STATUS   rs;
		if(m_PresentParams.Windowed == TRUE) {
			// �E�B���h�E���[�h
			// �N���C�A���g�̈���擾���A�X�N���[�����W�֕ϊ�
			RECT    window;
			::GetClientRect(m_PresentParams.hDeviceWindow, &window);
			POINT   pt = {0, 0};
			::ClientToScreen(m_PresentParams.hDeviceWindow, &pt);
			::OffsetRect(&window, pt.x, pt.y);

			while(true) {
				if(m_pD3DDevice->GetRasterStatus(0, &rs) != D3D_OK)
					break;
				if(rs.InVBlank == TRUE)
					break;			// �����A����
				if(m_PresentParams.Windowed == TRUE) {
					if(static_cast<int>(rs.ScanLine) < window.top || static_cast<int>(rs.ScanLine) >= window.bottom)
						break;		// �������̈ʒu�I�Ƀe�A�����O���Ȃ�
				}
				::Sleep(0);
			}	// while(true)
		} else {
			// �t���X�N���[�����[�h
			while(true) {
				if(m_pD3DDevice->GetRasterStatus(0, &rs) != D3D_OK)
					break;
				if(rs.InVBlank == TRUE)
					break;			// �����A����
				::Sleep(0);
			}	// while(true)
		}	// if(m_PresentParams.Windowed)
	}	// if(inWaitVSync)
*/
	if(m_pD3DDevice->Present(m_pPresentRect[0], m_pPresentRect[1], NULL, NULL) != D3D_OK) {
		if(m_pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice(false);
	}
}

//------------------------------------------------------------------------------
//	�r���[�|�[�g���Z�b�g
//------------------------------------------------------------------------------
void CDXGraphics9::ResetViewport()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_ResetViewport)\n"));
		return;
	}
#endif

	D3DVIEWPORT9   view;
	view.X      = 0;
	view.Y      = 0;
	view.Width  = m_Width;
	view.Height = m_Height;
	view.MinZ   = 0.0f;
	view.MaxZ   = 1.0f;

	if(m_pD3DDevice->SetViewport(&view) != D3D_OK)
		::OutputDebugString(TEXT("*** Error - �r���[�|�[�g�ݒ莸�s(CDXGraphics9_ResetViewport)\n"));
}

//------------------------------------------------------------------------------
//	�r���[�|�[�g�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetViewport(const D3DVIEWPORT9& view)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetViewport)\n"));
		return;
	}
#endif

	if(m_pD3DDevice->SetViewport(&view) != D3D_OK)
		::OutputDebugString(TEXT("*** Error - �r���[�|�[�g�ݒ莸�s(CDXGraphics9_SetViewport)\n"));
}

//------------------------------------------------------------------------------
//	�r���[�|�[�g���擾
//------------------------------------------------------------------------------
D3DVIEWPORT9 CDXGraphics9::GetViewport()
{
	D3DVIEWPORT9   view;
	::ZeroMemory(&view, sizeof(view));

#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_GetViewport)\n"));
		return view;
	}
#endif

	if(m_pD3DDevice->GetViewport(&view) != D3D_OK)
		::OutputDebugString(TEXT("*** Error - �r���[�|�[�g�ݒ莸�s(CDXGraphics9_GetViewport)\n"));

	return view;
}

//------------------------------------------------------------------------------
//	�r���[�|�[�g�ϊ��s��擾
//------------------------------------------------------------------------------
D3DXMATRIX CDXGraphics9::GetViewportMatrix()
{
	D3DVIEWPORT9   viewport = GetViewport();

	D3DXMATRIX     mat;
	::ZeroMemory(&mat, sizeof(mat));

	mat._11 = viewport.Width / 2.0f;
	mat._22 = -static_cast<float>(viewport.Height) / 2.0f;
	mat._33 = viewport.MaxZ - viewport.MinZ;
	mat._41 = viewport.X + viewport.Width  / 2.0f;
	mat._42 = viewport.Y + viewport.Height / 2.0f;
	mat._43 = viewport.MinZ;
	mat._44 = 1.0f;

	return mat;
}

//------------------------------------------------------------------------------
//	�ϊ��s��擾
//------------------------------------------------------------------------------
D3DXMATRIX CDXGraphics9::GetTransform(const D3DTRANSFORMSTATETYPE inTransType)
{
	D3DXMATRIX   transform;

#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_GetTransform)\n"));
		::ZeroMemory(&transform, sizeof(transform));
		return transform;
	}
#endif

	if(m_pD3DDevice->GetTransform(inTransType, &transform) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �ϊ��s��擾���s(CDXGraphics9_GetTransform)\n"));
		::D3DXMatrixIdentity(&transform);
	}

	return transform;
}

//------------------------------------------------------------------------------
//	���[���h�ϊ��s��ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetTransform(const D3DTRANSFORMSTATETYPE inTransType,
								const D3DXMATRIX& inTransform)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetTransform)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetTransform(inTransType, &inTransform);
}

//------------------------------------------------------------------------------
//	���[���h���W���X�N���[�����W
//------------------------------------------------------------------------------
D3DXVECTOR3 CDXGraphics9::WorldToScreen(const D3DXVECTOR3& inWorld)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_WorldToScreen)\n"));
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
#endif

	// �r���[�s��擾
	D3DXMATRIX   view;
	m_pD3DDevice->GetTransform(D3DTS_VIEW, &view);

	// �v���W�F�N�V�����s��擾
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);

	// ���[���h���W���r���[�|�[�g(�X�N���[��)���W
	D3DXVECTOR3   screen = inWorld;
	::D3DXVec3TransformCoord(&screen, &screen, &view);
	::D3DXVec3TransformCoord(&screen, &screen, &proj);
	::D3DXVec3TransformCoord(&screen, &screen, &GetViewportMatrix());

	return screen;
}

//------------------------------------------------------------------------------
//	�X�N���[�����W�����[���h���W
//------------------------------------------------------------------------------
D3DXVECTOR3 CDXGraphics9::ScreenToWorld(const float inScreenX, const float inScreenY,
										const float inWorldPosZ)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_ScreenToWorld)\n"));
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
#endif

	// �r���[�|�[�g�t�s��
	D3DVIEWPORT9   vp;
	m_pD3DDevice->GetViewport(&vp);

	D3DXMATRIX   viewport;
	::ZeroMemory(viewport.m, sizeof(float) * 4 * 4);
	viewport.m[0][0] =  float(vp.Width ) / 2.0f;
	viewport.m[1][1] = -float(vp.Height) / 2.0f;
	viewport.m[2][2] =  vp.MaxZ - vp.MinZ;
	viewport.m[3][0] =  float(vp.X + vp.Width ) / 2.0f;
	viewport.m[3][1] =  float(vp.Y + vp.Height) / 2.0f;
	viewport.m[3][2] =  vp.MinZ;
	viewport.m[3][3] =  1.0f;
	::D3DXMatrixInverse(&viewport, NULL, &viewport);

	// �v���W�F�N�V�����t�s��
	D3DXMATRIX   proj;
	m_pD3DDevice->GetTransform(D3DTS_PROJECTION, &proj);
	::D3DXMatrixInverse(&proj, NULL, &proj);

	// �r���[�t�s��
	D3DXMATRIX   view;
	m_pD3DDevice->GetTransform(D3DTS_VIEW, &view);
	::D3DXMatrixInverse(&view, NULL, &view);

	// �X�N���[�����W�����[���h���W�ϊ�
	D3DXVECTOR3   world(inScreenX, inScreenY, 0.0f);
	::D3DXVec3TransformCoord(&world, &world, &viewport);
	::D3DXVec3TransformCoord(&world, &world, &proj);
	::D3DXVec3TransformCoord(&world, &world, &view);

	// ���[���h���W�␳
	world.x = (world.x - view.m[3][0]) * (inWorldPosZ - world.z) + view.m[3][0];
	world.y = (world.y - view.m[3][1]) * (inWorldPosZ - world.z) + view.m[3][1];
	world.z = inWorldPosZ;

	return world;
}

//------------------------------------------------------------------------------
//	�V�[���J�n
//------------------------------------------------------------------------------
bool CDXGraphics9::BeginScene()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_BeginScene)\n"));
		return false;
	}
	if(m_pD3DDevice->BeginScene() != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �V�[���J�n���s(CDXGraphics9_BeginScene)\n"));
		return false;
	}
#else
	m_pD3DDevice->BeginScene();
#endif

	return true;
}

//------------------------------------------------------------------------------
//	�V�[���I��
//------------------------------------------------------------------------------
void CDXGraphics9::EndScene()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_EndScene)\n"));
		return;
	}
#endif

	m_pD3DDevice->EndScene();
}

//------------------------------------------------------------------------------
//	�v���~�e�B�u�`��
//------------------------------------------------------------------------------
void CDXGraphics9::DrawUserPrimitives(const D3DPRIMITIVETYPE inType, const void* pData,
									  const UINT inCount, const DWORD inFVF, const UINT inStride)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_DrawPrimitive)\n"));
		return;
	}
#endif

	const UINT   STRIDE = inStride == 0 ? ::D3DXGetFVFVertexSize(inFVF) : inStride;

	m_pD3DDevice->SetFVF(inFVF);
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, pData, STRIDE);
}

//------------------------------------------------------------------------------
//	���C�g�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetLight(const D3DLIGHT9& inLight, const DWORD inIndex, const BOOL inEnable)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetLight)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetLight   (inIndex, &inLight);
	m_pD3DDevice->LightEnable(inIndex, inEnable);

	m_LightIndex.insert(inIndex);
}

//------------------------------------------------------------------------------
//	���C�g�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::EnableLight(const DWORD inIndex, const BOOL inEnable)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_EnableLight)\n"));
		return;
	}
#endif

	m_pD3DDevice->LightEnable(inIndex, inEnable);
}

//------------------------------------------------------------------------------
//	���C�g�N���A
//------------------------------------------------------------------------------
void CDXGraphics9::ClearLight()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL)
		return;
#endif

	std::set<DWORD>::iterator   it;
	for(it = m_LightIndex.begin(); it != m_LightIndex.end(); it++)
		m_pD3DDevice->LightEnable(*it, FALSE);

	m_LightIndex.clear();
}

//------------------------------------------------------------------------------
//	���C�g�擾
//------------------------------------------------------------------------------
D3DLIGHT9 CDXGraphics9::GetLight(const DWORD inIndex)
{
	D3DLIGHT9   light;
	::ZeroMemory(&light, sizeof(light));

	m_pD3DDevice->GetLight(inIndex, &light);

	return light;
}

//------------------------------------------------------------------------------
//	�����_�����O�X�e�[�g�擾
//------------------------------------------------------------------------------
DWORD CDXGraphics9::GetRenderState(const D3DRENDERSTATETYPE inState)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_GetRenderState)\n"));
		return 0;
	}
#endif

	DWORD   value;
	if(m_pD3DDevice->GetRenderState(inState, &value) != D3D_OK)
		return 0;

	return value;
}

//------------------------------------------------------------------------------
//	�����_�����O�X�e�[�g�擾
//------------------------------------------------------------------------------
float CDXGraphics9::GetRenderStateF(const D3DRENDERSTATETYPE inState)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_GetRenderState)\n"));
		return 0;
	}
#endif

	const DWORD   dwValue = GetRenderState(inState);

	float   fValue;
	::CopyMemory(&fValue, &dwValue, 4);

	return fValue;
}

/*
//------------------------------------------------------------------------------
//	�����_�����O�X�e�[�g�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetRenderState)\n"));
		return;
	}
#endif

	// �X�e�[�g���}�b�v�ɓo�^����Ă��邩���ׂ�
	std::map<D3DRENDERSTATETYPE, DXGSAVESTATE>::iterator it;
	it = m_RenderState.find(inState);
	if(it == m_RenderState.end()) {
		// �o�^����Ă��Ȃ��ꍇ�A�f�t�H���g�l��ۑ�
		DXGSAVESTATE	SaveState;
		SaveState.Value = inValue;
		m_pD3DDevice->GetRenderState(inState, &SaveState.DefValue);
		if(inValue == SaveState.DefValue)
			return;							// �ݒ�l���f�t�H���g�l�̏ꍇ�͏I��

		// �}�b�v�ɓo�^����
		m_RenderState.insert(std::pair<D3DRENDERSTATETYPE, DXGSAVESTATE>(inState, SaveState));
	} else{
		// �o�^�ς݂̏ꍇ
		if(it->second.DefValue != inValue) {
			if(it->second.Value == inValue)
				return;						// �f�t�H���g�l�łȂ��A�o�^�ς݂Ɠ����l�̏ꍇ�͉������Ȃ�
			it->second.Value = inValue;		// �ݒ�l�̍X�V
		} else {
			m_RenderState.erase(it);		// �f�t�H���g�l�ɖ߂�ꍇ�́A�}�b�v����폜
		}
	}

	//�����_�����O�X�e�[�g�ݒ�
	m_pD3DDevice->SetRenderState(inState, inValue);
}
*/

//------------------------------------------------------------------------------
//	�����_�����O�X�e�[�g�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetRenderState)\n"));
		return;
	}
#endif

	// �X�e�[�g���}�b�v�ɓo�^����Ă��邩���ׂ�
	std::map<D3DRENDERSTATETYPE, DXGSAVESTATE>::iterator it;
	it = m_RenderState.find(inState);
	if(it == m_RenderState.end()) {
		// �o�^����Ă��Ȃ��ꍇ�A�f�t�H���g�l��ۑ�
		DXGSAVESTATE	SaveState;
		SaveState.Value = inValue;
		m_pD3DDevice->GetRenderState(inState, &SaveState.DefValue);

		// �}�b�v�ɓo�^����
		m_RenderState.insert(std::pair<D3DRENDERSTATETYPE, DXGSAVESTATE>(inState, SaveState));

		// �f�t�H���g�l�̏ꍇ�́A�X�e�[�g�ɕύX���Ȃ��̂ŏI��
		if(inValue == SaveState.DefValue)
			return;
	} else{
		// �o�^�ς݂̏ꍇ
		if(it->second.Value == inValue)
			return;						// �o�^�ς݂̒l�Ɠ����l�̏ꍇ�͉������Ȃ�

		it->second.Value = inValue;		// �ݒ�l�̍X�V
	}

	//�����_�����O�X�e�[�g�ݒ�
	m_pD3DDevice->SetRenderState(inState, inValue);
}

//------------------------------------------------------------------------------
//	�����_�����O�X�e�[�g���Z�b�g
//------------------------------------------------------------------------------
void CDXGraphics9::ResetRenderState()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_ResetRenderState)\n"));
		return;
	}
#endif

	// �����_�����O�X�e�[�g���f�t�H���g�ɖ߂�
	std::map<D3DRENDERSTATETYPE, DXGSAVESTATE>::iterator   it;
	for(it = m_RenderState.begin(); it != m_RenderState.end(); it++)
		m_pD3DDevice->SetRenderState(it->first, it->second.DefValue);

	// �}�b�v�폜
	m_RenderState.clear();
}

//------------------------------------------------------------------------------
//	�}�e���A���ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetMaterial(const D3DMATERIAL9& inMaterial)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetMaterial)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetMaterial(&inMaterial);
}

//------------------------------------------------------------------------------
//	�e�N�X�`���ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetTexture(const DWORD inStage, IDirect3DBaseTexture9* pTexture)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetTexture)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetTexture(inStage, pTexture);
}

//------------------------------------------------------------------------------
//	�e�N�X�`���X�e�[�W�X�e�[�g�擾
//------------------------------------------------------------------------------
DWORD CDXGraphics9::GetTextureStageState(const DWORD inStage, const D3DTEXTURESTAGESTATETYPE inType)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_GetTextureStageState)\n"));
		return 0;
	}
#endif

	DWORD   value;
	if(m_pD3DDevice->GetTextureStageState(inStage, inType, &value) != D3D_OK)
		return 0;

	return value;
}

//------------------------------------------------------------------------------
//	�e�N�X�`���X�e�[�W�X�e�[�g�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetTextureStageState(const DWORD inStage,
										const D3DTEXTURESTAGESTATETYPE inType, const DWORD inValue)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetTextureStageState)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetTextureStageState(inStage, inType, inValue);
}

//------------------------------------------------------------------------------
//	�e�N�X�`���T���v���[�X�e�[�g�擾
//------------------------------------------------------------------------------
DWORD CDXGraphics9::GetSamplerState(const DWORD inSampler, const D3DSAMPLERSTATETYPE inType)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_GetSamplerState)\n"));
		return 0;
	}
#endif

	DWORD   value;
	if(m_pD3DDevice->GetSamplerState(inSampler, inType, &value) != D3D_OK)
		return 0;

	return value;
}

//------------------------------------------------------------------------------
//	�e�N�X�`���T���v���[�X�e�[�g�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetSamplerState(const DWORD inSampler, const D3DSAMPLERSTATETYPE inType, const DWORD inValue)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetSamplerState)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetSamplerState(inSampler, inType, inValue);
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g����
//------------------------------------------------------------------------------
IEffect* CDXGraphics9::CreateEffectFromFile(LPCTSTR inFileName)
{
	IEffect*   pEffect;

	try {
		if(m_pD3DDevice == NULL)
			throw TEXT("DirectX Graphics��������");

		// �G�t�F�N�g�ǂݍ���
		ID3DXEffect*   pD3DXEffect;
//		DWORD   flag = inCompiled ? 0 : D3DXSHADER_SKIPVALIDATION;
		if(::D3DXCreateEffectFromFile(m_pD3DDevice, inFileName, NULL, NULL, D3DXFX_NOT_CLONEABLE, 
									  NULL, &pD3DXEffect, NULL)
		   != D3D_OK)
			   throw TEXT("�G�t�F�N�g�ǂݍ��ݎ��s");

		pEffect = new CEffect(pD3DXEffect);
		pD3DXEffect->Release();
	} catch(LPCTSTR ErrorString) {
		// �G���[���b�Z�[�W����
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CDXGraphics9_CreateEffectFromFile)\n"), ErrorString);
		::OutputDebugString(msg);

		pEffect = new CNullEffect();
	}

	m_Effect.push_back(pEffect);

	return pEffect;
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseEffect(IEffect*& pEffect)
{
	m_ProtectedResource.erase(pEffect);		// �v���e�N�g����

	m_Effect.remove(pEffect);
	delete pEffect;
	pEffect = NULL;
}

//------------------------------------------------------------------------------
//	�S�G�t�F�N�g���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllEffects()
{
	std::list<IEffect*>::iterator   list_it = m_Effect.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_Effect.end()) {
			delete *list_it;
			list_it++;
		}
		m_Effect.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_Effect.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_Effect.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	���_�o�b�t�@����
//------------------------------------------------------------------------------
IVertexBuffer* CDXGraphics9::CreateVertexBuffer(const UINT inSize, const DWORD inFVF,
												const UINT inStride, const bool inWriteOnly)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_CreateVertexBuffer)\n"));
		return NULL;
	}
#endif

	// �X�g���C�h�ݒ�
	const UINT   STRIDE = inStride == 0 ? ::D3DXGetFVFVertexSize(inFVF) : inStride;

	// ���_�o�b�t�@����
	IVertexBuffer*            pVertex;
	IDirect3DVertexBuffer9*   pBuffer;
	if(m_pD3DDevice->CreateVertexBuffer(inSize, 0, inFVF, D3DPOOL_MANAGED, &pBuffer, NULL) == D3D_OK) {
		pVertex = new CVertexBuffer(pBuffer, inFVF, STRIDE, inWriteOnly);
	} else {
		::OutputDebugString(TEXT("*** Error - ���_�o�b�t�@�������s(CDXGraphics9_CreateVertexBuffer)\n"));
		pVertex = new CNullVertexBuffer();
	}

	m_VertexBuffer.push_back(pVertex);
	SafeRelease(pBuffer);

	return pVertex;
}

//------------------------------------------------------------------------------
//	���_�o�b�t�@����
//------------------------------------------------------------------------------
IVertexBuffer* CDXGraphics9::CreateVertexBufferUP(const UINT inSize, const DWORD inFVF,
												  const UINT inStride)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_CreateVertexBufferUP)\n"));
		return NULL;
	}
#endif

	// �X�g���C�h�ݒ�
	const UINT   STRIDE = inStride == 0 ? ::D3DXGetFVFVertexSize(inFVF) : inStride;

	// ���_�o�b�t�@����
	IVertexBuffer*   pVertex;
	void*            pBuffer = new BYTE[inSize];
	if(pBuffer != NULL) {
		pVertex = new CVertexBufferUP(m_pD3DDevice, pBuffer, inSize, inFVF, STRIDE);
	} else {
		::OutputDebugString(TEXT("*** Error - ���_�o�b�t�@�������s(CDXGraphics9_CreateVertexBufferUP)\n"));
		pVertex = new CNullVertexBuffer();
	}

	m_VertexBuffer.push_back(pVertex);

	return pVertex;
}

//------------------------------------------------------------------------------
//	���_�o�b�t�@���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseVertexBuffer(IVertexBuffer*& pVertex)
{
	m_ProtectedResource.erase(pVertex);		// �v���e�N�g����

	m_VertexBuffer.remove(pVertex);
	delete pVertex;
	pVertex = NULL;
}

//------------------------------------------------------------------------------
//	�S���_�o�b�t�@���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllVertexBuffers()
{
	std::list<IVertexBuffer*>::iterator   list_it = m_VertexBuffer.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_VertexBuffer.end()) {
			delete *list_it;
			list_it++;
		}
		m_VertexBuffer.clear();
	} else {
		std::set<void*>   ::iterator    set_it;
		std::set<void*>   ::iterator    set_end = m_ProtectedResource.end();
		while(list_it != m_VertexBuffer.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_VertexBuffer.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	�V���v���V�F�C�v����
//------------------------------------------------------------------------------
IModel* CDXGraphics9::CreateModelFromSimpleShape(const DXGSIMPLESHAPE&  inShape,
												 const DXGCOMPUTENORMAL inCompute)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL){
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_CreateSimpleShape)\n"));
		return NULL;
	}
#endif

	// �V���v���V�F�C�v����
	ID3DXMesh*	pMesh = NULL;
	HRESULT		hr	  = D3DXERR_INVALIDDATA;
	switch(inShape.Type){
		case DXGSHAPE_BOX:
			hr = ::D3DXCreateBox(m_pD3DDevice, inShape.Width, inShape.Height, inShape.Depth,
								 &pMesh, NULL);
			break;

		case DXGSHAPE_CYLINDER:
			hr = ::D3DXCreateCylinder(m_pD3DDevice, inShape.Radius1, inShape.Radius2, inShape.Length,
									  inShape.Slices, inShape.Stacks, &pMesh, NULL);
			break;

		case DXGSHAPE_POLYGON:
			hr = ::D3DXCreatePolygon(m_pD3DDevice, inShape.Length, inShape.Sides, &pMesh, NULL);
			break;

		case DXGSHAPE_SPHERE:
			hr = ::D3DXCreateSphere(m_pD3DDevice, inShape.Radius, inShape.Slices, inShape.Stacks,
									&pMesh, NULL);
			break;

		case DXGSHAPE_TORUS:
			hr = ::D3DXCreateTorus(m_pD3DDevice, inShape.InnerRadius, inShape.OuterRadius,
								   inShape.Sides, inShape.Rings, &pMesh, NULL);
			break;

		case DXGSHAPE_TEAPOT:
			hr = ::D3DXCreateTeapot(m_pD3DDevice, &pMesh, NULL);
			break;
	};

	// ���b�V���I�u�W�F�N�g����
	IModel*   pShape;
	if(hr == D3D_OK){
		// ���b�V�����`
		ID3DXMesh*   pNewMesh = ModelingMesh(pMesh, inCompute);
		pMesh->Release();

		// ���b�V���I�u�W�F�N�g����
		pShape = new CModel(pNewMesh, 1);
		pShape->Optimize();

		pNewMesh->Release();
	} else{
		// NULL���b�V���I�u�W�F�N�g����
		::OutputDebugString(TEXT("*** Error - �V���v���V�F�C�v�������s(CDXGraphics9_CreateModelFromSimpleShape)\n"));
		pShape = new CNullModel();
	}

	m_Model.push_back(pShape);		// ���b�V�����X�g�֒ǉ�

	return pShape;
}

//------------------------------------------------------------------------------
//	���f������
//------------------------------------------------------------------------------
IModel* CDXGraphics9::CreateModelFromFile(LPCTSTR inFileName,
										  const DXGCOMPUTENORMAL inCompute,
										  const bool inUseSysMem)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_CreateModelFromFile)\n"));
		return NULL;
	}
#endif

	// ���t�@�C���ǂݍ���
	IModel*        pModel;
	ID3DXMesh*     pMesh;
	ID3DXBuffer*   pMatBuf;
	DWORD          SubsetCount;
	const DWORD    FLAG = inUseSysMem == false ? D3DXMESH_MANAGED : D3DXMESH_SYSTEMMEM;
	if(::D3DXLoadMeshFromX(inFileName, FLAG, m_pD3DDevice, NULL, &pMatBuf, NULL, &SubsetCount, &pMesh) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���t�@�C���ǂݍ��ݎ��s(CDXGraphics9_CreateModelFromFile)\n"));
		pModel = new CNullModel();
		m_Model.push_back(pModel);
		return pModel;
	}

	// ���b�V�����`
	ID3DXMesh*   pNewMesh = ModelingMesh(pMesh, inCompute);
	pMesh->Release();

	pModel = new CModel(pNewMesh, SubsetCount);
	pModel->Optimize();				// �œK��
	m_Model.push_back(pModel);		// ���b�V�����X�g�֒ǉ�
	pNewMesh->Release();

	// �t���p�X�擾
	TCHAR   FullPath[MAX_PATH + 1];
	::_tfullpath(FullPath, inFileName, MAX_PATH + 1);

	// �t�@�C���p�X�擾
	TCHAR   drive[_MAX_DRIVE + 1];
	TCHAR   dir  [_MAX_DIR   + 1];
	::_tsplitpath_s(FullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);

	D3DXMATERIAL*   pMaterial = (D3DXMATERIAL*)pMatBuf->GetBufferPointer();
	for(DWORD i = 0; i < SubsetCount; i++) {
		// �}�e���A���ݒ�
		pModel->SetMaterial((pMaterial + i)->MatD3D, i);

		// �e�N�X�`���ݒ�
		if((pMaterial + i)->pTextureFilename != NULL) {
			TCHAR   FileName[MAX_PATH + 1];
			::MultiByteToWideChar(CP_ACP, 0, (pMaterial + i)->pTextureFilename, -1, FileName, MAX_PATH + 1);
			::wsprintf(FullPath, TEXT("%s%s%s"), drive, dir, FileName);
			pModel->SetTexture(FullPath, i);
		}
	}

	pMatBuf->Release();

	return pModel;
}

//------------------------------------------------------------------------------
//	���f������
//------------------------------------------------------------------------------
IModel* CDXGraphics9::CreateModelFromText(LPCTSTR inText,
										  const LOGFONT& inLogFont, const float inExtrusion,
										  const DXGCOMPUTENORMAL inCompute)
{
	// �݊��f�o�C�X�R���e�L�X�g����
	HDC     hdc      = ::CreateCompatibleDC(NULL);

	// �t�H���g����
	HFONT   hFont    = ::CreateFontIndirect(&inLogFont);
	HFONT   hOldFont = (HFONT)::SelectObject(hdc, hFont);

	// ���b�V���I�u�W�F�N�g����
	ID3DXMesh*	 pMesh = NULL;
	const HRESULT   hr = ::D3DXCreateText(m_pD3DDevice, hdc, inText, 0.0f, inExtrusion, &pMesh, NULL, NULL);

	// ���\�[�X���
    ::SelectObject(hdc, hOldFont);
    ::DeleteObject(hFont);
    ::DeleteDC(hdc);

	IModel*   pModel;
	if(hr == D3D_OK){
		// ���b�V�����`
		ID3DXMesh*   pNewMesh = ModelingMesh(pMesh, inCompute);
		pMesh->Release();

		// ���b�V���I�u�W�F�N�g����
		pModel = new CModel(pNewMesh, 1);
		pModel->Optimize();

		pNewMesh->Release();
	} else{
		// NULL���b�V���I�u�W�F�N�g����
		::OutputDebugString(TEXT("*** Error - �e�L�X�g���o�鐶�����s(CDXGraphics9_CreateModelFromText)\n"));
		pModel = new CNullModel();
	}

	m_Model.push_back(pModel);		// ���b�V�����X�g�֒ǉ�

	return pModel;
}

//------------------------------------------------------------------------------
//	���f������
//------------------------------------------------------------------------------
IModel* CDXGraphics9::CreateModelFromText(LPCTSTR inText,
										  LPCTSTR inFontName, const int inSize,
										  const float inExtrusion,
										  const DXGCOMPUTENORMAL inCompute)
{
	LOGFONT   lf;
	::ZeroMemory(&lf, sizeof(lf));

	lf.lfHeight         = inSize;
	lf.lfWidth          = 0;
	lf.lfWeight         = 0;
	lf.lfEscapement     = 0;
	lf.lfOrientation    = 0;
	lf.lfItalic         = 0;
	lf.lfUnderline      = 0;
	lf.lfStrikeOut      = 0;
#ifdef UNICODE
	lf.lfCharSet        = DEFAULT_CHARSET;
#else
	lf.lfCharSet        = SHIFTJIS_CHARSET;
#endif
	lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lf.lfQuality        = PROOF_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	return CreateModelFromText(inText, lf, inExtrusion, inCompute);
}

//------------------------------------------------------------------------------
//	���f�����`
//------------------------------------------------------------------------------
ID3DXMesh* CDXGraphics9::ModelingMesh(ID3DXMesh* pBaseMesh, const DXGCOMPUTENORMAL inCompute)
{
	if(inCompute == DXGCOMPUTE_DONOT_INPLACE) {
		pBaseMesh->AddRef();
		return pBaseMesh;
	}

	// ���_�f�[�^���C�A�E�g�ݒ�
	D3DVERTEXELEMENT9   VertexDecl[6] = {
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END(),
		D3DDECL_END()
	};

	UINT    decl_idx =  1;	// ���W�ݒ�ς݂̂���
	WORD    offset   = 12;	// float x 3 = 12byte

	// �@��
	DWORD   NormalSemantic = D3DX_DEFAULT;
	if((inCompute & DXGCOMPUTE_NORMAL) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT3;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_NORMAL;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 12;	// float x 3 = 12byte

		NormalSemantic = D3DDECLUSAGE_NORMAL;
	}

	// �ڐ�
	DWORD   TangentSemantic = D3DX_DEFAULT;
	if((inCompute & DXGCOMPUTE_TANGENT) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT3;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_TANGENT;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 12;	// float x 3 = 12byte

		TangentSemantic = D3DDECLUSAGE_TANGENT;
	}

	// �]�@��
	DWORD   BinormalSemantic = D3DX_DEFAULT;
	if((inCompute & DXGCOMPUTE_BINORMAL) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT3;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_BINORMAL;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 12;	// float x 3 = 12byte

		BinormalSemantic = D3DDECLUSAGE_BINORMAL;
	}

	// �e�N�X�`�����W
	DWORD   TextureSemantic = D3DX_DEFAULT;
	if((pBaseMesh->GetFVF() & D3DFVF_TEX1) != 0) {
		VertexDecl[decl_idx].Stream     = 0;
		VertexDecl[decl_idx].Offset     = offset;
		VertexDecl[decl_idx].Type       = D3DDECLTYPE_FLOAT2;
		VertexDecl[decl_idx].Method     = D3DDECLMETHOD_DEFAULT;
		VertexDecl[decl_idx].Usage      = D3DDECLUSAGE_TEXCOORD;
		VertexDecl[decl_idx].UsageIndex = 0;

		decl_idx++;
		offset += 8;	// float x 2 = 8byte

		TextureSemantic = D3DDECLUSAGE_TEXCOORD;
	}

	// ���b�V������
	ID3DXMesh*   pNewMesh;
	if(pBaseMesh->CloneMesh(pBaseMesh->GetOptions(), VertexDecl, m_pD3DDevice, &pNewMesh) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���b�V���������s(CDXGraphics9_ModelingMesh)\n"));
		pBaseMesh->AddRef();
		return pBaseMesh;
	}

	// �@���E�ڐ��E�]�@���v�Z�s�v
	if(inCompute == DXGCOMPUTE_NONE   ||
	   inCompute == DXGCOMPUTE_NORMAL && (pBaseMesh->GetFVF() & D3DFVF_NORMAL) != 0)
		return pNewMesh;

	// �@���E�ڐ��E�]�@���v�Z
	if(::D3DXComputeTangentFrameEx(pNewMesh,
								   TextureSemantic , 0,
								   TangentSemantic , 0,
								   BinormalSemantic, 0,
								   NormalSemantic  , 0,
								   D3DXTANGENT_GENERATE_IN_PLACE, NULL,
								   -1.0f, 0.0f, -1.0f,
								   NULL, NULL)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �@���E�ڐ��E�]�@���v�Z���s(CDXGraphics9_ModelingMesh)\n"));
	}

	return pNewMesh;
}

//------------------------------------------------------------------------------
//	���L���f������
//------------------------------------------------------------------------------
bool CDXGraphics9::CreateSharedModelFromFile(LPTSTR inFileName,
											 IModel* pModelArray[],
											 const DWORD inArrayCount,
											 const DXGCOMPUTENORMAL inCompute)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_CreateSharedModelFromFile)\n"));
		return false;
	}
#endif

	// ���t�@�C���ǂݍ���
	ID3DXMesh*     pMesh;
	ID3DXBuffer*   pMatBuf;
	DWORD          SubsetCount;
	if(::D3DXLoadMeshFromX(inFileName, D3DXMESH_MANAGED, m_pD3DDevice, NULL, &pMatBuf, NULL, &SubsetCount, &pMesh) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���t�@�C���ǂݍ��ݎ��s(CDXGraphics9_CreateSharedModelFromFile)\n"));
		// NULL���f������
		for(DWORD i = 0; i < inArrayCount; i++) {
			pModelArray[i] = new CNullModel();
			m_Model.push_back(pModelArray[i]);
		}

		return false;
	}

	// ���b�V�����`
	ID3DXMesh*   pNewMesh = ModelingMesh(pMesh, inCompute);
	pMesh->Release();

	// ���b�V���œK��
	// �אږʐ���
	const DWORD    FACE = pNewMesh->GetNumFaces();
	DWORD*   pAdjacency = new DWORD[FACE * 3 * sizeof(DWORD)];
	if(pNewMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency) == D3D_OK) {
		// �œK��
		if(pNewMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
									pAdjacency, NULL, NULL, NULL) != D3D_OK) {
			delete[] pAdjacency;
			::OutputDebugString(TEXT("*** Error - ���b�V���œK�����s(CDXGraphics9_CreateSharedModelFromFile)\n"));
		}
	}

	// �}�e���A���|�C���^�[�擾
	D3DXMATERIAL*   pMaterial = (D3DXMATERIAL*)pMatBuf->GetBufferPointer();

	// �e�N�X�`���ǂݍ��ݐݒ�
	// �t���p�X�擾
	TCHAR   FullPath[MAX_PATH + 1];
	::_tfullpath(FullPath, inFileName, MAX_PATH + 1);

	// �t�@�C���p�X�擾
	TCHAR   drive[_MAX_DRIVE + 1];
	TCHAR   dir  [_MAX_DIR   + 1];
	::_tsplitpath_s(FullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, NULL, 0, NULL, 0);

	// �e�N�X�`���ǂݍ���
	std::vector<IDirect3DTexture9*>   textures(SubsetCount, NULL);
	for(DWORD i = 0; i < SubsetCount; i++) {
		if((pMaterial + i)->pTextureFilename != NULL) {
			if(::D3DXCreateTextureFromFileA(m_pD3DDevice, (pMaterial + i)->pTextureFilename, &textures[i])
			   != D3D_OK)
				::OutputDebugString(TEXT("*** Error - �e�N�X�`���ǂݍ��ݎ��s(CDXGraphics9_CreateSharedModelFromFile)\n"));
		}
	}

	// ���f������
	for(DWORD moi = 0; moi < inArrayCount; moi++) {
		pModelArray[moi] = new CModel(pNewMesh, SubsetCount);
		m_Model.push_back(pModelArray[moi]);

		// �}�e���A���ݒ�
		for(DWORD mai = 0; mai < SubsetCount; mai++) {
			pModelArray[moi]->SetMaterial((pMaterial + mai)->MatD3D, mai);
			pModelArray[moi]->SetTexture (textures[mai], mai);
		}
	}

	pNewMesh->Release();
	pMatBuf ->Release();

	return true;
}

//------------------------------------------------------------------------------
//	���f�����
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseModel(IModel*& pModel)
{
	m_ProtectedResource.erase(pModel);		// �v���e�N�g����

	m_Model.remove(pModel);
	delete pModel;
	pModel = NULL;
}

//------------------------------------------------------------------------------
//	�S���f�����
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllModels()
{
	std::list<IModel*>::iterator  list_it = m_Model.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_Model.end()) {
			delete *list_it;
			list_it++;
		}
		m_Model.clear();
	}else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_Model.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_Model.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	�A�j���[�V�������f������
//------------------------------------------------------------------------------
IAnimationModel* CDXGraphics9::CreateAnimationModelFromFile(LPCTSTR inFileName,
															const DXGCOMPUTENORMAL   inCompute,
															const DXGSKINNING_METHOD inSkinMethod)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_CreateAnimationModelFromX)\n"));
		return NULL;
	}
#endif

	CAnimationModel*   pAnimeModel = new CAnimationModel(m_pD3DDevice);
	if(pAnimeModel == NULL) {
		::OutputDebugString(TEXT("*** Error - �A�j���[�V�������f���������s(CDXGraphics9_CreateAnimationModelFromX)\n"));
		return NULL;
	}

	// ���t�@�C���ǂݍ���
	IAnimationModel*   pModel = pAnimeModel;
	if(pAnimeModel->LoadFromX(inFileName, inCompute, inSkinMethod) == false) {
		::OutputDebugString(TEXT("*** Error - ���t�@�C���ǂݍ��ݎ��s(CDXGraphics9_CreateAnimationModelFromX)\n"));
		delete pAnimeModel;
		pModel = new CNullAnimationModel();
	}
	m_AnimationModel.push_back(pModel);

	return pModel;
}

//------------------------------------------------------------------------------
//	�A�j���[�V�������f�����
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAnimationModel(IAnimationModel*& pAnimeModel)
{
	m_ProtectedResource.erase(pAnimeModel);		// �v���e�N�g����

	m_AnimationModel.remove(pAnimeModel);
	delete pAnimeModel;
	pAnimeModel = NULL;
}

//------------------------------------------------------------------------------
//	�S�A�j���[�V�������f�����
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllAnimationModels()
{
	std::list<IAnimationModel*>::iterator   list_it = m_AnimationModel.begin();
	if(m_AnimationModel.empty()) {
		while(list_it != m_AnimationModel.end()) {
			delete *list_it;
			list_it++;
		}
		m_AnimationModel.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_AnimationModel.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_AnimationModel.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	�X�v���C�g����
//------------------------------------------------------------------------------
ISprite* CDXGraphics9::CreateSprite(const UINT inWidth, const UINT inHeight, const D3DFORMAT inFormat)
{
	ISprite*   pSprite;

	try {
#ifdef _DEBUG
		if(m_pD3DDevice == NULL)
			throw TEXT("DirectX Graphics��������");
#endif

		// �e�N�X�`������
		IDirect3DTexture9*   pTexture = NULL;
		if(::D3DXCreateTexture(m_pD3DDevice, inWidth, inHeight, 1, 0, inFormat, D3DPOOL_MANAGED, &pTexture) != D3D_OK)
			throw TEXT("�e�N�X�`���������s");

		pSprite = new CSprite(pTexture);
		pTexture->Release();
	} catch(LPCTSTR ErrorString) {
		// �G���[���b�Z�[�W����
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CDXGraphics9_CreateSrpite)\n"), ErrorString);
		::OutputDebugString(msg);

		// ��O�����������ꍇ�́ANULL�I�u�W�F�N�g�𐶐�
		pSprite = new CNullSprite();
	}

	m_Sprite.push_back(pSprite);	// �X�v���C�g���X�g�֒ǉ�

	return pSprite;
}

//------------------------------------------------------------------------------
//	�X�v���C�g����
//------------------------------------------------------------------------------
ISprite* CDXGraphics9::CreateSpriteFromFile(LPCTSTR inFileName,
											const D3DFORMAT inFormat,
											const D3DCOLOR  inColorKey)
{
	ISprite*   pSprite;

	try {
#ifdef _DEBUG
		if(m_pD3DDevice == NULL)
			throw TEXT("DirectX Graphics��������");
#endif

		// �e�N�X�`������
		IDirect3DTexture9*   pTexture;
		if(::D3DXCreateTextureFromFileEx(m_pD3DDevice, inFileName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, inFormat,
//										 D3DPOOL_MANAGED, D3DX_DEFAULT,     D3DX_DEFAULT, inColorKey, NULL, NULL, &pTexture)
										 D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT, inColorKey, NULL, NULL, &pTexture)
		   != D3D_OK) {
			// WIC�ɂ��t�@�C���ǂݍ���
			pTexture = CreateTextureFromWIC(inFileName, inFormat, inColorKey);
			// OLE�ɂ��t�@�C���ǂݍ���
//			pTexture = CreateTextureFromPicture(inFileName, inFormat, inColorKey);
			if(pTexture == NULL)
				throw TEXT("�t�@�C���ǂݍ��ݎ��s");
		}

		pSprite = new CSprite(pTexture);
		pTexture->Release();
	} catch(LPCTSTR ErrorString) {
		// �G���[���b�Z�[�W����
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CDXGraphics9_CreateSrpiteFromFile)\n"), ErrorString);
		::OutputDebugString(msg);

		// ��O�����������ꍇ�́ANULL�I�u�W�F�N�g�𐶐�
		pSprite = new CNullSprite();
	}

	m_Sprite.push_back(pSprite);	// �X�v���C�g���X�g�֒ǉ�

	return pSprite;
}

//------------------------------------------------------------------------------
//	�e�N�X�`������
//------------------------------------------------------------------------------
IDirect3DTexture9* CDXGraphics9::CreateTextureFromWIC(LPCTSTR inFileName,
													  const D3DFORMAT inFormat,
													  const D3DCOLOR  inColorKey)
{
	IDirect3DTexture9*       pTexture      = NULL;

	IWICImagingFactory*      pWICFactory   = NULL;
	IWICBitmapDecoder*       pWICDecoder   = NULL;
	IWICBitmapFrameDecode*   pWICFrameDec  = NULL;
	IWICFormatConverter*     pWICConverter = NULL;

	IDirect3DTexture9*       pMemTexture   = NULL;

	try {
		// WIC�t�@�N�g���[����
		if(::CoCreateInstance(CLSID_WICImagingFactory, NULL,
							  CLSCTX_INPROC_SERVER, IID_IWICImagingFactory,
							  (LPVOID*)&pWICFactory)
		   != S_OK)
			throw TEXT("WIC�t�@�N�g���[�������s");

		// WIC�f�R�[�_�[����
		if(pWICFactory->CreateDecoderFromFilename(inFileName, NULL, GENERIC_READ,
												  WICDecodeMetadataCacheOnLoad, &pWICDecoder)
		   != S_OK)
			throw TEXT("WIC�f�R�[�_�[�������s");

		// WIC�t���[���f�R�[�_�[����
		if(pWICDecoder->GetFrame(0, &pWICFrameDec) != S_OK)
			throw TEXT("WIC�t���[���f�R�[�_�[�������s");

		// WIC�R���o�[�^�[����
		if(pWICFactory->CreateFormatConverter(&pWICConverter) != S_OK)
			throw TEXT("WIC�R���o�[�^�[�������s");

		// WIC�R���o�[�^�[������
		if(pWICConverter->Initialize(pWICFrameDec, GUID_WICPixelFormat32bppPBGRA,
									 WICBitmapDitherTypeNone, NULL, 0.0f,
									 WICBitmapPaletteTypeMedianCut)
		   != S_OK)
			throw TEXT("WIC�R���o�[�^�[���������s");

		// �T�C�Y�擾
		UINT   wic_width, wic_height;
		pWICConverter->GetSize(&wic_width, &wic_height);

		// �V�X�e���������e�N�X�`������
		if(::D3DXCreateTexture(m_pD3DDevice, wic_width, wic_height, 1, 0, D3DFMT_A8R8G8B8,
							   D3DPOOL_SYSTEMMEM, &pMemTexture)
		   != D3D_OK)
			throw TEXT("�V�X�e���������e�N�X�`���������s");

		// �V�X�e���������e�N�X�`�����擾
		D3DSURFACE_DESC   desc;
		pMemTexture->GetLevelDesc(0, &desc);

		// �R�s�[�̈�ݒ�
		WICRect   copy_rect;
		copy_rect.X = 0;
		copy_rect.Y = 0;

		if(wic_width <= desc.Width)
			copy_rect.Width = wic_width;
		else
			copy_rect.Width = desc.Width;

		if(wic_width <= desc.Height)
			copy_rect.Height = wic_height;
		else
			copy_rect.Height = desc.Height;

		// �s�N�Z���擾
		D3DLOCKED_RECT   lock_rect;
		if(pMemTexture->LockRect(0, &lock_rect, NULL, 0) != D3D_OK)
			throw TEXT("�V�X�e���������e�N�X�`�����b�N���s");

		const HRESULT   hr = pWICConverter->CopyPixels(&copy_rect, lock_rect.Pitch,
													   lock_rect.Pitch * desc.Height,
													   (BYTE*)lock_rect.pBits);

		pMemTexture->UnlockRect(0);

		if(hr != S_OK)
			throw TEXT("�s�N�Z���擾���s");

		pWICConverter->Release();
		pWICFrameDec ->Release();
		pWICDecoder  ->Release();
		pWICFactory  ->Release();

		// �e�N�X�`������
		if(::D3DXCreateTexture(m_pD3DDevice, wic_width, wic_height, 1, 0,
							   inFormat, D3DPOOL_MANAGED, &pTexture)
		   != D3D_OK)
			throw  TEXT("�e�N�X�`���������s");

		IDirect3DSurface9*   pMemSurface;
		pMemTexture->GetSurfaceLevel(0, &pMemSurface);

		IDirect3DSurface9*   pSurface;
		pTexture->GetSurfaceLevel(0, &pSurface);

		// �V�X�e���������e�N�X�`���]��
		::D3DXLoadSurfaceFromSurface(pSurface, NULL, NULL, pMemSurface, NULL, NULL, D3DX_FILTER_POINT, inColorKey);

		pSurface   ->Release();
		pMemSurface->Release();
		pMemTexture->Release();
	} catch(LPCTSTR err_str) {
#ifdef _DEBUG
		// �G���[���b�Z�[�W����
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CDXGraphics9_CreateTextureFromWIC)\n"), err_str);
		::OutputDebugString(msg);
#endif
		SafeRelease(pMemTexture);

		SafeRelease(pWICConverter);
		SafeRelease(pWICFrameDec);
		SafeRelease(pWICDecoder);
		SafeRelease(pWICFactory);
	}

	return pTexture;
}

/*
//------------------------------------------------------------------------------
//	�e�N�X�`������
//------------------------------------------------------------------------------
IDirect3DTexture9* CDXGraphics9::CreateTextureFromPicture(LPCTSTR inFileName,
														  const D3DFORMAT inFormat,
														  const D3DCOLOR  inColorKey)
{
	// �t�@�C���I�[�v��
	HANDLE   hFile = ::CreateFile(inFileName, GENERIC_READ, 0, NULL, 
								  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���I�[�v�����s(CDXGraphics9_CreateTextureFromPicture)\n"));
		return NULL;
	}

	// �O���[�o���������m��
	const DWORD   FILE_SIZE = ::GetFileSize(hFile, NULL);
	HGLOBAL       hGlobal   = ::GlobalAlloc(GPTR, FILE_SIZE);
	if(hGlobal == NULL) {
		::OutputDebugString(TEXT("*** Error - �O���[�o���������m�ێ��s(CDXGraphics9_CreateTextureFromPicture)\n"));
		::CloseHandle(hFile);
		return NULL;
	}

	// �t�@�C���ǂݍ���
	DWORD   actual;
	::ReadFile(hFile, hGlobal, FILE_SIZE, &actual, NULL);
	::CloseHandle(hFile);

	// �X�g���[������
	IStream*   pStream;
	if(::CreateStreamOnHGlobal(hGlobal, FALSE, &pStream) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �X�g���[���������s(CDXGraphics9_CreateTextureFromPicture)\n"));
		::GlobalFree(hGlobal);
		return NULL;
	}

	// �s�N�`���[����
	IPicture*   pPicture;
	if(::OleLoadPicture(pStream, FILE_SIZE, TRUE, IID_IPicture, (LPVOID*)&pPicture) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �s�N�`���[�������s(CDXGraphics9_CreateTextureFromPicture)\n"));
		pStream->Release();
		::GlobalFree(hGlobal);
		return NULL;
	}

	// �s�N�`���[�T�C�Y�擾
	OLE_XSIZE_HIMETRIC   pic_width;
	pPicture->get_Width (&pic_width);

	OLE_YSIZE_HIMETRIC   pic_height;
	pPicture->get_Height(&pic_height);

	// �P�ʕϊ� - HiMetric(1/100mm)����s�N�Z��(2540 = HiMetric / Inch)
	const long   DEST_WIDTH  = ::MulDiv(pic_width , 96, 2540);
	const long   DEST_HEIGHT = ::MulDiv(pic_height, 96, 2540);

	// �J���[�L�[�ݒ�
	D3DCOLOR   color_key = inColorKey;
	if(color_key == 0) {
		DWORD   attribute;
		pPicture->get_Attributes(&attribute);
		if(attribute == PICTURE_TRANSPARENT)
			color_key = 0xFF000000;
	}

	// �V�X�e���������e�N�X�`������
	IDirect3DTexture9*   pMemTexture;
	if(::D3DXCreateTexture(m_pD3DDevice, DEST_WIDTH, DEST_HEIGHT, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM, &pMemTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �V�X�e���������e�N�X�`���������s(CDXGraphics9_CreateTextureFromPicture)\n"));
		pPicture->Release();
		pStream ->Release();
		::GlobalFree(hGlobal);
		return NULL;
	}

	IDirect3DSurface9*   pMemSurface;
	pMemTexture->GetSurfaceLevel(0, &pMemSurface);

	// �s�N�`���[�]��
	HDC   hDC;
	pMemSurface->GetDC(&hDC);
	pPicture->Render(hDC, 0, 0, DEST_WIDTH, DEST_HEIGHT, 0, pic_height, pic_width, -pic_height, NULL);

	// ���
	pMemSurface->ReleaseDC(hDC);

	pPicture->Release();
	pStream ->Release();
	::GlobalFree(hGlobal);

	// �e�N�X�`������
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTexture(m_pD3DDevice, DEST_WIDTH, DEST_HEIGHT, 1, 0, inFormat, D3DPOOL_MANAGED, &pTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CDXGraphics9_CreateTextureFromPicture)\n"));
		pMemSurface->Release();
		pMemTexture->Release();
		return NULL;
	}

	IDirect3DSurface9*   pSurface;
	pTexture->GetSurfaceLevel(0, &pSurface);

	// �V�X�e���������e�N�X�`���]��
	::D3DXLoadSurfaceFromSurface(pSurface, NULL, NULL, pMemSurface, NULL, NULL, D3DX_FILTER_POINT, color_key);

	pSurface->Release();

	pMemSurface->Release();
	pMemTexture->Release();

	return pTexture;
}
*/

//------------------------------------------------------------------------------
//	�X�v���C�g���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseSprite(ISprite*& pSprite)
{
	m_ProtectedResource.erase(pSprite);		// �v���e�N�g����

	m_Sprite.remove(pSprite);
	delete pSprite;
	pSprite = NULL;
}

//------------------------------------------------------------------------------
//	�S�X�v���C�g���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllSprites()
{
	std::list<ISprite*>::iterator   list_it = m_Sprite.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_Sprite.end()) {
			delete *list_it;
			list_it++;
		}
		m_Sprite.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_Sprite.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_Sprite.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	�t�H���g����
//------------------------------------------------------------------------------
IDXGFont* CDXGraphics9::CreateSpriteFont(LPCTSTR inFontName, const int inSize)
{
	D3DXFONT_DESC   font_desc;
	font_desc.Height          = inSize;
	font_desc.Width           = 0;
	font_desc.Weight          = FW_NORMAL;
	font_desc.MipLevels       = 1;
	font_desc.Italic          = FALSE;
#ifdef UNICODE
	font_desc.CharSet         = DEFAULT_CHARSET;
#else
	font_desc.CharSet         = SHIFTJIS_CHARSET;
#endif
	font_desc.OutputPrecision = OUT_DEFAULT_PRECIS;
	font_desc.Quality         = PROOF_QUALITY;
	font_desc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;

	::lstrcpyn(font_desc.FaceName, inFontName, LF_FACESIZE - 1);

	return CreateSpriteFont(font_desc);
}

//------------------------------------------------------------------------------
//	�t�H���g����
//------------------------------------------------------------------------------
IDXGFont* CDXGraphics9::CreateSpriteFont(const D3DXFONT_DESC& inFontDesc)
{
	IDXGFont*    pFont;
	ID3DXFont*   pD3DXFont;
	if(::D3DXCreateFontIndirect(m_pD3DDevice, &inFontDesc, &pD3DXFont) == D3D_OK) {
		pFont = new CDXGFont(pD3DXFont, m_pD3DXSprite);
		pD3DXFont->Release();
	} else {
		::OutputDebugString(TEXT("*** Error - �t�H���g�������s(CDXGraphics9_CreateFont)\n"));
		pFont = new CDXGNullFont();
	}

	m_Font.push_back(pFont);

	return pFont;
}

//------------------------------------------------------------------------------
//	�f�t�H���g�t�H���g����
//------------------------------------------------------------------------------
IDXGFont* CDXGraphics9::CreateDefaultFont()
{
	// �V�X�e���t�H���g���擾
	LOGFONT   lf;
	::GetObject((HFONT)::GetStockObject(SYSTEM_FONT), sizeof(lf), &lf);

	// �t�H���g���ݒ�
	D3DXFONT_DESC   font_desc;
	font_desc.Height          = lf.lfHeight;
	font_desc.Width           = lf.lfWidth;
	font_desc.Weight          = lf.lfWeight;
	font_desc.MipLevels       = 1;
	font_desc.Italic          = lf.lfItalic;
	font_desc.CharSet         = lf.lfCharSet;
	font_desc.OutputPrecision = lf.lfOutPrecision;
	font_desc.Quality         = lf.lfQuality;
	font_desc.PitchAndFamily  = lf.lfPitchAndFamily;

	::lstrcpyn(font_desc.FaceName, lf.lfFaceName, LF_FACESIZE - 1);

	IDXGFont*   pFont = CreateSpriteFont(font_desc);
	pFont->PreloadCharacters(0x20, 0x7e);
//	pFont->PreloadCharacters(0xa1, 0xdf);

	return pFont;
}

//------------------------------------------------------------------------------
//	�t�H���g���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseFont(IDXGFont*& pFont)
{
	m_ProtectedResource.erase(pFont);		// �v���e�N�g����

	m_Font.remove(pFont);
	delete pFont;
	pFont = NULL;
}

//------------------------------------------------------------------------------
//	�S�t�H���g���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllFonts()
{
	std::list<IDXGFont*>::iterator   list_it = m_Font.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_Font.end()) {
			delete *list_it;
			list_it++;
		}
		m_Font.clear();
	} else {
		std::set<void*>::iterator    set_it;
		std::set<void*>::iterator    set_end = m_ProtectedResource.end();
		while(list_it != m_Font.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_Font.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	�X�e�[�g�u���b�N����
//------------------------------------------------------------------------------
IStateBlock* CDXGraphics9::CreateStateBlock()
{
	IStateBlock*   pStateBlock = new CStateBlock(m_pD3DDevice);
	m_StateBlock.push_back(pStateBlock);

	return pStateBlock;
}

//------------------------------------------------------------------------------
//	�X�e�[�g�u���b�N���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseStateBlock(IStateBlock*& pStateBlock)
{
	m_ProtectedResource.erase(pStateBlock);		// �v���e�N�g����

	m_StateBlock.remove(pStateBlock);
	delete pStateBlock;
	pStateBlock = NULL;
}

//------------------------------------------------------------------------------
//	�S�X�e�[�g�u���b�N���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllStateBlocks()
{
	std::list<IStateBlock*>::iterator   list_it  = m_StateBlock.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_StateBlock.end()) {
			delete *list_it;
			list_it++;
		}
		m_StateBlock.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_StateBlock.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_StateBlock.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	�����_�����O�^�[�Q�b�g����
//------------------------------------------------------------------------------
IRenderTarget* CDXGraphics9::CreateRenderTarget(const UINT inWidth, const UINT inHeight,
												const D3DFORMAT inFormat,
												const D3DFORMAT inDepthFormat)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_CreateRenderTarget)\n"));
		return NULL;
	}
#endif

	// ���A�����ݒ�
	UINT   width  = inWidth;
	UINT   height = inHeight;

	// �e�N�X�`������
	IDirect3DTexture9*   pD3DTexture = NULL;
	if(inFormat != D3DFMT_UNKNOWN) {
		if(::D3DXCreateTexture(m_pD3DDevice, width, height, 1,
							   D3DUSAGE_RENDERTARGET, inFormat, D3DPOOL_DEFAULT,
							  &pD3DTexture)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CDXGraphics9_CreateRenderTarget)\n"));
		}

		// ���A�����ݒ�
		D3DSURFACE_DESC   desc;
		pD3DTexture->GetLevelDesc(0, &desc);
		width  = desc.Width;
		height = desc.Height;
	}

	// �f�v�X�X�e���V������
	IDirect3DSurface9*   pDepthStencil = NULL;
	if(inDepthFormat != D3DFMT_UNKNOWN) {
		if(m_pD3DDevice->CreateDepthStencilSurface(width, height, inDepthFormat,
												   D3DMULTISAMPLE_NONE, 0, FALSE,
												  &pDepthStencil, NULL)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - �f�v�X�X�e���V���������s(CDXGraphics9_CreateRenderTarget)\n"));
		}
	}

	// �����_�[�^�[�Q�b�g����
	IRenderTarget*   pRenderTarget;
	if(pD3DTexture != NULL || pDepthStencil != NULL) {
		pRenderTarget = new CRenderTarget(pD3DTexture, pDepthStencil, inWidth, inHeight);

		SafeRelease(pDepthStencil);
		SafeRelease(pD3DTexture);
	} else {
		pRenderTarget = new CNullRenderTarget();
	}

	m_RenderTarget.push_back(pRenderTarget);

	if(m_pRenderTargetVertex == NULL) {
		// ���_�o�b�t�@����
		m_pRenderTargetVertex = CreateVertexBuffer(sizeof(DXGTLVERTEX) * 4, DXGFVF_TLVERTEX, sizeof(DXGTLVERTEX), true);
		m_VertexBuffer.remove(m_pRenderTargetVertex);	// �Ǘ����X�g����O��

		// ���_���𒸓_�o�b�t�@�֓]��
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx != NULL) {
			// ���W
			vtx[0].z  = 0.0f;	vtx[0].rhw = 1.0f;
			vtx[1].z  = 0.0f;	vtx[1].rhw = 1.0f;
			vtx[2].z  = 0.0f;	vtx[2].rhw = 1.0f;
			vtx[3].z  = 0.0f;	vtx[3].rhw = 1.0f;

			// UV���W
			vtx[0].tu = 0.0f;	vtx[0].tv  = 1.0f;
			vtx[1].tu = 0.0f;	vtx[1].tv  = 0.0f;
			vtx[2].tu = 1.0f;	vtx[2].tv  = 1.0f;
			vtx[3].tu = 1.0f;	vtx[3].tv  = 0.0f;

			// ���_�J���[
			vtx[0].color = 0xffffffff;
			vtx[1].color = 0xffffffff;
			vtx[2].color = 0xffffffff;
			vtx[3].color = 0xffffffff;

			m_pRenderTargetVertex->Unlock();
		}
	}

	return pRenderTarget;
}

//------------------------------------------------------------------------------
//	�L���[�u�����_�����O�^�[�Q�b�g����
//------------------------------------------------------------------------------
IRenderTarget* CDXGraphics9::CreateCubeRenderTarget(const UINT inSize,
													const D3DFORMAT inFormat,
													const D3DFORMAT inDepthFormat)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_CreateRenderTarget)\n"));
		return NULL;
	}
#endif

	// �L���[�u�e�N�X�`������
	IDirect3DCubeTexture9*   pCubeTexture = NULL;
	if(::D3DXCreateCubeTexture(m_pD3DDevice, inSize, 1,
							   D3DUSAGE_RENDERTARGET, inFormat, D3DPOOL_DEFAULT,
							  &pCubeTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �L���[�u�e�N�X�`���������s(CDXGraphics9_CreateCubeRenderTarget)\n"));
	}

	// �f�v�X�X�e���V������
	IDirect3DSurface9*   pDepthStencil = NULL;
	if(inDepthFormat != D3DFMT_UNKNOWN) {
		D3DSURFACE_DESC   desc;
		pCubeTexture->GetLevelDesc(0, &desc);
		if(m_pD3DDevice->CreateDepthStencilSurface(desc.Width, desc.Height, inDepthFormat,
												   D3DMULTISAMPLE_NONE, 0, FALSE,
												  &pDepthStencil, NULL)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - �f�v�X�X�e���V���������s(CDXGraphics9_CreateCubeRenderTarget)\n"));
		}
	}

	// �����_�[�^�[�Q�b�g����
	IRenderTarget*   pRenderTarget;
	if(pCubeTexture != NULL) {
		pRenderTarget = new CRenderTarget(pCubeTexture, pDepthStencil, inSize);

		SafeRelease(pDepthStencil);
		SafeRelease(pCubeTexture);
	} else {
		pRenderTarget = new CNullRenderTarget();
	}

	m_RenderTarget.push_back(pRenderTarget);

	if(m_pRenderTargetVertex == NULL) {
		// ���_�o�b�t�@����
		m_pRenderTargetVertex = CreateVertexBuffer(sizeof(DXGTLVERTEX) * 4, DXGFVF_TLVERTEX, sizeof(DXGTLVERTEX), true);
		m_VertexBuffer.remove(m_pRenderTargetVertex);	// �Ǘ����X�g����O��

		// ���_���𒸓_�o�b�t�@�֓]��
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx != NULL) {
			// ���W
			vtx[0].z  = 0.0f;	vtx[0].rhw = 1.0f;
			vtx[1].z  = 0.0f;	vtx[1].rhw = 1.0f;
			vtx[2].z  = 0.0f;	vtx[2].rhw = 1.0f;
			vtx[3].z  = 0.0f;	vtx[3].rhw = 1.0f;

			// UV���W
			vtx[0].tu = 0.0f;	vtx[0].tv  = 1.0f;
			vtx[1].tu = 0.0f;	vtx[1].tv  = 0.0f;
			vtx[2].tu = 1.0f;	vtx[2].tv  = 1.0f;
			vtx[3].tu = 1.0f;	vtx[3].tv  = 0.0f;

			// ���_�J���[
			vtx[0].color = 0xffffffff;
			vtx[1].color = 0xffffffff;
			vtx[2].color = 0xffffffff;
			vtx[3].color = 0xffffffff;

			m_pRenderTargetVertex->Unlock();
		}
	}

	return pRenderTarget;
}

//------------------------------------------------------------------------------
//	HDR�����_�����O�^�[�Q�b�g����
//------------------------------------------------------------------------------
IRenderTarget* CDXGraphics9::CreateHDRRenderTarget(const UINT inWidth, const UINT inHeight,
												   const D3DFORMAT inDepthFormat)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_CreateHDRRenderTarget)\n"));
		return NULL;
	}
#endif

	HRESULT              hr;
	IDirect3DTexture9*   pD3DTexture;

	// �e�N�X�`������
	const D3DFORMAT   FORMAT[] = {D3DFMT_A16B16G16R16F, D3DFMT_A32B32G32R32F, D3DFMT_A16B16G16R16};
	for(UINT i = 0; i < sizeof(FORMAT) / sizeof(FORMAT[0]); i++) {
		hr = ::D3DXCreateTexture(m_pD3DDevice, inWidth, inHeight, 1,
								 D3DUSAGE_RENDERTARGET, FORMAT[i], D3DPOOL_DEFAULT,
								&pD3DTexture);
		if(hr == D3D_OK)
			break;
	}

	// �f�v�X�X�e���V������
	IDirect3DSurface9*   pDepthStencil = NULL;
	if(inDepthFormat != D3DFMT_UNKNOWN) {
		D3DSURFACE_DESC   desc;
		pD3DTexture->GetLevelDesc(0, &desc);
		if(m_pD3DDevice->CreateDepthStencilSurface(desc.Width, desc.Height, inDepthFormat,
												   D3DMULTISAMPLE_NONE, 0, FALSE,
												  &pDepthStencil, NULL)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - �f�v�X�X�e���V���������s(CDXGraphics9_CreateHDRRenderTarget)\n"));
		}
	}

	// �����_�[�^�[�Q�b�g����
	IRenderTarget*   pRenderTarget;
	if(hr == D3D_OK) {
		pRenderTarget = new CRenderTarget(pD3DTexture, pDepthStencil, inWidth, inHeight);
		SafeRelease(pDepthStencil);
		SafeRelease(pD3DTexture  );
	} else {
		::OutputDebugString(TEXT("*** Error - HDR�e�N�X�`���������s(CDXGraphics9_CreateHDRRenderTarget)\n"));
		pRenderTarget = new CNullRenderTarget();
	}

	m_RenderTarget.push_back(pRenderTarget);

	if(m_pRenderTargetVertex == NULL) {
		// ���_�o�b�t�@����
		m_pRenderTargetVertex = CreateVertexBuffer(sizeof(DXGTLVERTEX) * 4, DXGFVF_TLVERTEX, sizeof(DXGTLVERTEX), true);
		m_VertexBuffer.remove(m_pRenderTargetVertex);	// �Ǘ����X�g����O��

		// ���_���𒸓_�o�b�t�@�֓]��
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx != NULL) {
			// ���W
			vtx[0].z  = 0.0f;	vtx[0].rhw = 1.0f;
			vtx[1].z  = 0.0f;	vtx[1].rhw = 1.0f;
			vtx[2].z  = 0.0f;	vtx[2].rhw = 1.0f;
			vtx[3].z  = 0.0f;	vtx[3].rhw = 1.0f;

			// UV���W
			vtx[0].tu = 0.0f;	vtx[0].tv  = 1.0f;
			vtx[1].tu = 0.0f;	vtx[1].tv  = 0.0f;
			vtx[2].tu = 1.0f;	vtx[2].tv  = 1.0f;
			vtx[3].tu = 1.0f;	vtx[3].tv  = 0.0f;

			// ���_�J���[
			vtx[0].color = 0xffffffff;
			vtx[1].color = 0xffffffff;
			vtx[2].color = 0xffffffff;
			vtx[3].color = 0xffffffff;

			m_pRenderTargetVertex->Unlock();
		}
	}

	return pRenderTarget;
}

//------------------------------------------------------------------------------
//	�����_�����O�^�[�Q�b�g���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseRenderTarget(IRenderTarget*& pRenderTarget)
{
	m_ProtectedResource.erase(pRenderTarget);	// �v���e�N�g����

	m_RenderTarget.remove(pRenderTarget);
	delete pRenderTarget;
	pRenderTarget = NULL;

	if(m_RenderTarget.empty()) {
		// �����_�����O�^�[�Q�b�g�p�o�[�e�b�N�X�o�b�t�@���
		delete m_pRenderTargetVertex;
		m_pRenderTargetVertex = NULL;
	}
}

//------------------------------------------------------------------------------
//	�S�����_�����O�^�[�Q�b�g���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseAllRenderTargets()
{
	std::list<IRenderTarget*>::iterator   list_it = m_RenderTarget.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_RenderTarget.end()) {
			delete *list_it;
			list_it++;
		}
		m_RenderTarget.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_RenderTarget.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_RenderTarget.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}

	// �����_�����O�^�[�Q�b�g�p�o�[�e�b�N�X�o�b�t�@���
	delete m_pRenderTargetVertex;
	m_pRenderTargetVertex = NULL;
}

//------------------------------------------------------------------------------
//	�f�t�H���g�����_�[�^�[�Q�b�g�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetDefaultRenderTarget()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetDefaultRenderTarget)\n"));
		return;
	}
#endif

	m_pD3DDevice->SetRenderTarget(0, m_pBackBufferSurface);
	m_pD3DDevice->SetDepthStencilSurface(m_pDepthStencilSurface);

	ResetViewport();
}

//------------------------------------------------------------------------------
//	�����_�[�^�[�Q�b�g���o�b�N�o�b�t�@
//------------------------------------------------------------------------------
void CDXGraphics9::RenderTargetToBackBuffer(const RECT* pDestRect,
											IRenderTarget*& pSrcTarget, const RECT* pSrcRect,
											const D3DTEXTUREFILTERTYPE inFilter)
{
	SetDefaultRenderTarget();
	StretchRect(*pSrcTarget, pSrcRect, m_pBackBufferSurface, pDestRect, inFilter);
/*
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_RenderTargetToBackBuffer)\n"));
		return;
	}
#endif

	// �����_�����O�^�[�Q�b�g���f�t�H���g�ɖ߂�
	SetDefaultRenderTarget();

	// �����_�[�^�[�Q�b�g�T�[�t�F�X�擾
	IDirect3DSurface9*   pSrcSurface = pRenderTarget->GetSurface();
	if(pSrcSurface == NULL)
		return;

	// �����_�[�^�[�Q�b�g���o�b�N�o�b�t�@
	m_pD3DDevice->StretchRect(pSrcSurface, pSrcRect, m_pBackBufferSurface, pDestRect, inFilter);
*/
}

//------------------------------------------------------------------------------
//	�����_�[�^�[�Q�b�g���o�b�N�o�b�t�@
//------------------------------------------------------------------------------
void CDXGraphics9::RenderTargetToBackBuffer(IRenderTarget*& pRenderTarget, IEffect*& pEffect)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_RenderTargetToBackBuffer)\n"));
		return;
	}
#endif

	// �����_�����O�^�[�Q�b�g���f�t�H���g�ɖ߂�
	SetDefaultRenderTarget();

	// �����_�����O�^�[�Q�b�g�T�C�Y�m�F
	const UINT   WIDTH  = pRenderTarget->GetWidth ();
	const UINT   HEIGHT = pRenderTarget->GetHeight();
	if(WIDTH == 0 || HEIGHT == 0)
		return;

	// ���_�o�b�t�@�ݒ�
	if(m_RenderTargetWidth != WIDTH || m_RenderTargetHeight != HEIGHT) {
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx == NULL)
			return;

		// ���_���W
		vtx[0].x = 0.0f         - 0.5f;	vtx[0].y = (float)HEIGHT - 0.5f;
		vtx[1].x = 0.0f         - 0.5f;	vtx[1].y = 0.0f          - 0.5f;
		vtx[2].x = (float)WIDTH - 0.5f;	vtx[2].y = (float)HEIGHT - 0.5f;
		vtx[3].x = (float)WIDTH - 0.5f;	vtx[3].y = 0.0f          - 0.5f;

		m_pRenderTargetVertex->Unlock();

		m_RenderTargetWidth  = WIDTH;
		m_RenderTargetHeight = HEIGHT;
	}

	// �t�H�O����
	DWORD   fogEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_FOGENABLE, &fogEnable);
	if(fogEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// ���o�b�t�@����
	DWORD   zEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_ZENABLE, &zEnable);
	if(zEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	// ���C�e�B���O����
	DWORD   lightEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_LIGHTING, &lightEnable);
	if(lightEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �G�t�F�N�g���g���ăo�b�N�o�b�t�@�փ����_�����O
	m_pD3DDevice->SetTexture(0, pRenderTarget->GetTexture());

	const UINT   pass = pEffect->Begin();
	for(UINT i = 0; i < pass; i++) {
		pEffect->BeginPass(i);

		m_pRenderTargetVertex->DrawPure(D3DPT_TRIANGLESTRIP, 2);

		pEffect->EndPass();
	}
	pEffect->End();

	// �X�e�[�g����
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING , lightEnable);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE  , zEnable);
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, fogEnable);
}

//------------------------------------------------------------------------------
//	�����_�[�^�[�Q�b�g���o�b�N�o�b�t�@
//------------------------------------------------------------------------------
void CDXGraphics9::RenderTargetToBackBuffer(IRenderTarget*& pRenderTarget,
											IEffect*& pEffect, const UINT inPass)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_RenderTargetToBackBuffer)\n"));
		return;
	}
#endif

	// �����_�����O�^�[�Q�b�g���f�t�H���g�ɖ߂�
	SetDefaultRenderTarget();

	// �����_�����O�^�[�Q�b�g�T�C�Y�m�F
	const UINT   WIDTH  = pRenderTarget->GetWidth ();
	const UINT   HEIGHT = pRenderTarget->GetHeight();
	if(WIDTH == 0 || HEIGHT == 0)
		return;

	// ���_�o�b�t�@�ݒ�
	if(m_RenderTargetWidth != WIDTH || m_RenderTargetHeight != HEIGHT) {
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx == NULL)
			return;

		// ���_���W
		vtx[0].x = 0.0f         - 0.5f;	vtx[0].y = (float)HEIGHT - 0.5f;
		vtx[1].x = 0.0f         - 0.5f;	vtx[1].y = 0.0f          - 0.5f;
		vtx[2].x = (float)WIDTH - 0.5f;	vtx[2].y = (float)HEIGHT - 0.5f;
		vtx[3].x = (float)WIDTH - 0.5f;	vtx[3].y = 0.0f          - 0.5f;

		m_pRenderTargetVertex->Unlock();

		m_RenderTargetWidth  = WIDTH;
		m_RenderTargetHeight = HEIGHT;
	}

	// �t�H�O����
	DWORD   fogEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_FOGENABLE, &fogEnable);
	if(fogEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// ���o�b�t�@����
	DWORD   zEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_ZENABLE, &zEnable);
	if(zEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	// ���C�e�B���O����
	DWORD   lightEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_LIGHTING, &lightEnable);
	if(lightEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �G�t�F�N�g���g���ăo�b�N�o�b�t�@�փ����_�����O
	m_pD3DDevice->SetTexture(0, pRenderTarget->GetTexture());

	pEffect->Begin();
	pEffect->BeginPass(inPass);

	m_pRenderTargetVertex->DrawPure(D3DPT_TRIANGLESTRIP, 2);

	pEffect->EndPass();
	pEffect->End();

	// �X�e�[�g����
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING , lightEnable);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE  , zEnable);
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, fogEnable);
}

//------------------------------------------------------------------------------
//	�����_�[�^�[�Q�b�g�������_�[�^�[�Q�b�g
//------------------------------------------------------------------------------
void CDXGraphics9::RenderTargetToRenderTarget(IRenderTarget*& pDestRenderTarget,
											  IRenderTarget*&  pSrcRenderTarget,
											  IEffect*&        pEffect)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_RenderTargetToRenderTarget)\n"));
		return;
	}
#endif

	// �����_�����O�^�[�Q�b�g��]���惌���_�[�^�[�Q�b�g�ɂ���
	pDestRenderTarget->Use();

	// �����_�����O�^�[�Q�b�g�T�C�Y�m�F
	const UINT   WIDTH  = pSrcRenderTarget->GetWidth ();
	const UINT   HEIGHT = pSrcRenderTarget->GetHeight();
	if(WIDTH == 0 || HEIGHT == 0)
		return;

	// ���_�o�b�t�@�ݒ�
	if(m_RenderTargetWidth != WIDTH || m_RenderTargetHeight != HEIGHT) {
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx == NULL)
			return;

		// ���_���W
		vtx[0].x = 0.0f         - 0.5f;	vtx[0].y = (float)HEIGHT - 0.5f;
		vtx[1].x = 0.0f         - 0.5f;	vtx[1].y = 0.0f          - 0.5f;
		vtx[2].x = (float)WIDTH - 0.5f;	vtx[2].y = (float)HEIGHT - 0.5f;
		vtx[3].x = (float)WIDTH - 0.5f;	vtx[3].y = 0.0f          - 0.5f;

		m_pRenderTargetVertex->Unlock();

		m_RenderTargetWidth  = WIDTH;
		m_RenderTargetHeight = HEIGHT;
	}

	// �t�H�O����
	DWORD   fogEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_FOGENABLE, &fogEnable);
	if(fogEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// ���o�b�t�@����
	DWORD   zEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_ZENABLE, &zEnable);
	if(zEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	// ���C�e�B���O����
	DWORD   lightEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_LIGHTING, &lightEnable);
	if(lightEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �G�t�F�N�g���g���ē]���惌���_�[�^�[�Q�b�g�փ����_�����O
	m_pD3DDevice->SetTexture(0, pSrcRenderTarget->GetTexture());

	const UINT   pass = pEffect->Begin();
	for(UINT i = 0; i < pass; i++) {
		pEffect->BeginPass(i);

		m_pRenderTargetVertex->DrawPure(D3DPT_TRIANGLESTRIP, 2);

		pEffect->EndPass();
	}
	pEffect->End();

	// �X�e�[�g����
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING , lightEnable);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE  , zEnable);
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, fogEnable);
}

//------------------------------------------------------------------------------
//	�����_�[�^�[�Q�b�g�������_�[�^�[�Q�b�g
//------------------------------------------------------------------------------
void CDXGraphics9::RenderTargetToRenderTarget(IRenderTarget*& pDestRenderTarget,
											  IRenderTarget*&  pSrcRenderTarget,
											  IEffect*& pEffect, const UINT inPass)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_RenderTargetToRenderTarget)\n"));
		return;
	}
#endif

	// �����_�����O�^�[�Q�b�g��]���惌���_�[�^�[�Q�b�g�ɂ���
	pDestRenderTarget->Use();

	// �����_�����O�^�[�Q�b�g�T�C�Y�m�F
	const UINT   WIDTH  = pSrcRenderTarget->GetWidth ();
	const UINT   HEIGHT = pSrcRenderTarget->GetHeight();
	if(WIDTH == 0 || HEIGHT == 0)
		return;

	// ���_�o�b�t�@�ݒ�
	if(m_RenderTargetWidth != WIDTH || m_RenderTargetHeight != HEIGHT) {
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx == NULL)
			return;

		// ���_���W
		vtx[0].x = 0.0f         - 0.5f;	vtx[0].y = (float)HEIGHT - 0.5f;
		vtx[1].x = 0.0f         - 0.5f;	vtx[1].y = 0.0f          - 0.5f;
		vtx[2].x = (float)WIDTH - 0.5f;	vtx[2].y = (float)HEIGHT - 0.5f;
		vtx[3].x = (float)WIDTH - 0.5f;	vtx[3].y = 0.0f          - 0.5f;

		m_pRenderTargetVertex->Unlock();

		m_RenderTargetWidth  = WIDTH;
		m_RenderTargetHeight = HEIGHT;
	}

	// �t�H�O����
	DWORD   fogEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_FOGENABLE, &fogEnable);
	if(fogEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// ���o�b�t�@����
	DWORD   zEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_ZENABLE, &zEnable);
	if(zEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	// ���C�e�B���O����
	DWORD   lightEnable = TRUE;
	m_pD3DDevice->GetRenderState(D3DRS_LIGHTING, &lightEnable);
	if(lightEnable != FALSE)
		m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// �G�t�F�N�g���g���ē]���惌���_�[�^�[�Q�b�g�փ����_�����O
	m_pD3DDevice->SetTexture(0, pSrcRenderTarget->GetTexture());

	pEffect->Begin();
	pEffect->BeginPass(inPass);

	m_pRenderTargetVertex->DrawPure(D3DPT_TRIANGLESTRIP, 2);

	pEffect->EndPass();
	pEffect->End();

	// �X�e�[�g��߂�
	m_pD3DDevice->SetRenderState(D3DRS_LIGHTING , lightEnable);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE  , zEnable);
	m_pD3DDevice->SetRenderState(D3DRS_FOGENABLE, fogEnable);
}

//------------------------------------------------------------------------------
//	�T�[�t�F�X�]��
//------------------------------------------------------------------------------
void CDXGraphics9::StretchRect(IDirect3DSurface9* pDestSurface, const RECT* pDestRect,
							   IDirect3DSurface9* pSrcSurface,  const RECT* pSrcRect,
							   const D3DTEXTUREFILTERTYPE inFilter)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_StretchRect)\n"));
		return;
	}
#endif

	m_pD3DDevice->StretchRect(pSrcSurface, pSrcRect, pDestSurface, pDestRect, inFilter);
}

//------------------------------------------------------------------------------
//	�T�[�t�F�X���[�h
//------------------------------------------------------------------------------
void CDXGraphics9::LoadSurfaceFromSurface(IDirect3DSurface9* pDestSurface,
										  const        RECT* pDestRect,
										  IDirect3DSurface9* pSrcSurface,
										  const        RECT* pSrcRect,
										  const DWORD inFilter , const D3DCOLOR inColorKey)
{
	::D3DXLoadSurfaceFromSurface(pDestSurface, NULL, pDestRect,
								 pSrcSurface,  NULL, pSrcRect,
								 inFilter, inColorKey);
}

//------------------------------------------------------------------------------
//	�X�v���C�g3D���C���[����
//------------------------------------------------------------------------------
ISprite3DLayer* CDXGraphics9::CreateSprite3DLayer(const UINT      inWidth , 
												  const UINT      inHeight,
												  const D3DFORMAT inFormat,
												  const D3DFORMAT inDepthFormat)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_CreateSprite3DLayer)\n"));
		return NULL;
	}
#endif

	// ���A�����ݒ�
	UINT   width  = inWidth;
	UINT   height = inHeight;

	// �e�N�X�`������
	IDirect3DTexture9*   pD3DTexture = NULL;
	if(inFormat != D3DFMT_UNKNOWN) {
		if(::D3DXCreateTexture(m_pD3DDevice, width, height, 1,
							   D3DUSAGE_RENDERTARGET, inFormat, D3DPOOL_DEFAULT,
							  &pD3DTexture)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CDXGraphics9_CreateSprite3DLayer)\n"));
		}

		// ���A�����ݒ�
		D3DSURFACE_DESC   desc;
		pD3DTexture->GetLevelDesc(0, &desc);
		width  = desc.Width;
		height = desc.Height;
	}

	// �f�v�X�X�e���V������
	IDirect3DSurface9*   pDepthStencil = NULL;
	if(inDepthFormat != D3DFMT_UNKNOWN) {
		if(m_pD3DDevice->CreateDepthStencilSurface(width, height, inDepthFormat,
												   D3DMULTISAMPLE_NONE, 0, FALSE,
												  &pDepthStencil, NULL)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - �f�v�X�X�e���V���������s(CDXGraphics9_CreateSprite3DLayer)\n"));
		}
	}

	// �����_�[�^�[�Q�b�g����
	ISprite3DLayer*   pSprite3DLayer;
	if(pD3DTexture != NULL || pDepthStencil != NULL) {
		pSprite3DLayer = new CSprite3DLayer(pD3DTexture, pDepthStencil, inWidth, inHeight);

		SafeRelease(pDepthStencil);
		SafeRelease(pD3DTexture);
	} else {
		pSprite3DLayer = new CNullSprite3DLayer();
	}

	m_RenderTarget.push_back(pSprite3DLayer);

	if(m_pRenderTargetVertex == NULL) {
		// ���_�o�b�t�@����
		m_pRenderTargetVertex = CreateVertexBuffer(sizeof(DXGTLVERTEX) * 4, DXGFVF_TLVERTEX, sizeof(DXGTLVERTEX), true);
		m_VertexBuffer.remove(m_pRenderTargetVertex);	// �Ǘ����X�g����O��

		// ���_���𒸓_�o�b�t�@�֓]��
		DXGTLVERTEX*   vtx = (DXGTLVERTEX*)m_pRenderTargetVertex->Lock();
		if(vtx != NULL) {
			// ���W
			vtx[0].z  = 0.0f;	vtx[0].rhw = 1.0f;
			vtx[1].z  = 0.0f;	vtx[1].rhw = 1.0f;
			vtx[2].z  = 0.0f;	vtx[2].rhw = 1.0f;
			vtx[3].z  = 0.0f;	vtx[3].rhw = 1.0f;

			// UV���W
			vtx[0].tu = 0.0f;	vtx[0].tv  = 1.0f;
			vtx[1].tu = 0.0f;	vtx[1].tv  = 0.0f;
			vtx[2].tu = 1.0f;	vtx[2].tv  = 1.0f;
			vtx[3].tu = 1.0f;	vtx[3].tv  = 0.0f;

			// ���_�J���[
			vtx[0].color = 0xffffffff;
			vtx[1].color = 0xffffffff;
			vtx[2].color = 0xffffffff;
			vtx[3].color = 0xffffffff;

			m_pRenderTargetVertex->Unlock();
		}
	}

	return pSprite3DLayer;
}

//------------------------------------------------------------------------------
//	���\�[�X�v���e�N�g�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// �v���e�N�g�ݒ�
	} else {
		m_ProtectedResource.erase (pResource);		// �v���e�N�g����
	}
}

//------------------------------------------------------------------------------
//	�u�����f�B���O���[�h�J�n
//------------------------------------------------------------------------------
bool CDXGraphics9::BeginAlphaBlend(const DXGBLENDMODE inBlendMode)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_BeginAlphaBlend)\n"));
		return false;
	}
#endif
	// �����_�����O�X�e�[�g�ۑ�
	m_pD3DDevice->GetRenderState(D3DRS_CULLMODE,              &m_AlphaState.CullMode);

	m_pD3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,      &m_AlphaState.AlphaEnable);
	m_pD3DDevice->GetRenderState(D3DRS_BLENDOP,               &m_AlphaState.BlendOP);
	m_pD3DDevice->GetRenderState(D3DRS_DESTBLEND,             &m_AlphaState.DestBlend);
	m_pD3DDevice->GetRenderState(D3DRS_SRCBLEND,              &m_AlphaState.SrcBlend);
	m_pD3DDevice->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &m_AlphaState.Diffuse);

	m_pD3DDevice->GetRenderState(D3DRS_ZENABLE,               &m_AlphaState.ZEnable);
	m_pD3DDevice->GetRenderState(D3DRS_ZWRITEENABLE,          &m_AlphaState.ZWriteEnable);

	m_pD3DDevice->GetTextureStageState(0, D3DTSS_ALPHAOP,     &m_AlphaState.AlphaOP);

	// �����_�����O�X�e�[�g�ݒ�
	// �A���t�@�u�����h�ݒ�
	SetBlendMode(inBlendMode);

	m_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,      TRUE);

	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,     D3DTOP_MODULATE);

	return true;
}

//------------------------------------------------------------------------------
//	�u�����f�B���O���[�h�ݒ�
//------------------------------------------------------------------------------
void CDXGraphics9::SetBlendMode(const DXGBLENDMODE inBlendMode)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_SetBlendMode)\n"));
		return;
	}
#endif

	// �u�����h���[�h�ݒ�
	switch(inBlendMode) {
	  // ��Z����
	  case DXGBLEND_MODULATE:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;

	  // ���Z����
	  case DXGBLEND_ADD:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;

	  // ���Z����
	  case DXGBLEND_SUBTRACT:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_SUBTRACT);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;

	  case DXGBLEND_SRCCOLOR:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
		break;

	  case DXGBLEND_ZEROCOLOR:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
/*
	  // �u�����f�B���O�Ȃ�
	  case DXGBLEND_NONE:
	  default:
		m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,   D3DBLENDOP_ADD);
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_ZERO);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
*/
	}
}

//------------------------------------------------------------------------------
//	�A���t�@�u�����h�I��
//------------------------------------------------------------------------------
void CDXGraphics9::EndAlphaBlend()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_EndAlphaBlend)\n"));
		return;
	}
#endif

	// �����_�����O�X�e�[�g����
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,     m_AlphaState.AlphaOP);

	m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,          m_AlphaState.ZWriteEnable);
	m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,               m_AlphaState.ZEnable);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,      m_AlphaState.AlphaEnable);
	m_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, m_AlphaState.Diffuse);
	m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,               m_AlphaState.BlendOP);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,             m_AlphaState.DestBlend);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,              m_AlphaState.SrcBlend);

	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,              m_AlphaState.CullMode);
}

//------------------------------------------------------------------------------
//	�V���h�E�����_�����O�J�n
//------------------------------------------------------------------------------
bool CDXGraphics9::BeginShadowRendering()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_BeginShadowRendering)\n"));
		return false;
	}
#endif
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 1.0f, 0);

	// �����_�����O�X�e�[�g�ۑ�
	m_pD3DDevice->GetRenderState(D3DRS_CULLMODE,  &m_AlphaState.CullMode);
	m_pD3DDevice->GetRenderState(D3DRS_SHADEMODE, &m_AlphaState.ShadeMode);

	m_pD3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE,      &m_AlphaState.AlphaEnable);
	m_pD3DDevice->GetRenderState(D3DRS_BLENDOP,               &m_AlphaState.BlendOP);
	m_pD3DDevice->GetRenderState(D3DRS_DESTBLEND,             &m_AlphaState.DestBlend);
	m_pD3DDevice->GetRenderState(D3DRS_SRCBLEND,              &m_AlphaState.SrcBlend);
	m_pD3DDevice->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &m_AlphaState.Diffuse);

	m_pD3DDevice->GetRenderState(D3DRS_STENCILENABLE, &m_AlphaState.StencilEnable);
	m_pD3DDevice->GetRenderState(D3DRS_STENCILFUNC,   &m_AlphaState.StencilFunc);
	m_pD3DDevice->GetRenderState(D3DRS_STENCILPASS,   &m_AlphaState.StencilPass);
	m_pD3DDevice->GetRenderState(D3DRS_STENCILFAIL,   &m_AlphaState.StencilFail);
	m_pD3DDevice->GetRenderState(D3DRS_STENCILZFAIL,  &m_AlphaState.StencilZFail);
	m_pD3DDevice->GetRenderState(D3DRS_STENCILREF,    &m_AlphaState.StencilRef);

	// �����_�����O�X�e�[�g�ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,  D3DCULL_NONE);
	m_pD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);

	// �A���t�@�u�����h�ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,               D3DBLENDOP_ADD);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,             D3DBLEND_INVSRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,              D3DBLEND_ZERO);
//	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,              D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,      TRUE);

	// �X�e���V���ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,   D3DCMP_NOTEQUAL);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS,   D3DSTENCILOP_REPLACE);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILFAIL,   D3DSTENCILOP_KEEP);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILZFAIL,  D3DSTENCILOP_KEEP);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILREF,    1);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

	return true;
}

//------------------------------------------------------------------------------
//	�V���h�E�����_�����O�I��
//------------------------------------------------------------------------------
void CDXGraphics9::EndShadowRendering()
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDXGraphics9_EndShadowRendering)\n"));
		return;
	}
#endif

	// �����_�����O�X�e�[�g����
	m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, m_AlphaState.StencilEnable);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILFUNC,   m_AlphaState.StencilFunc);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILPASS,   m_AlphaState.StencilPass);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILFAIL,   m_AlphaState.StencilFail);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILZFAIL,  m_AlphaState.StencilZFail);
	m_pD3DDevice->SetRenderState(D3DRS_STENCILREF,    m_AlphaState.StencilRef);

	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,      m_AlphaState.AlphaEnable);
	m_pD3DDevice->SetRenderState(D3DRS_BLENDOP,               m_AlphaState.BlendOP);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,             m_AlphaState.DestBlend);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,              m_AlphaState.SrcBlend);
	m_pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, m_AlphaState.Diffuse);

	m_pD3DDevice->SetRenderState(D3DRS_SHADEMODE, m_AlphaState.ShadeMode);
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE,  m_AlphaState.CullMode);
}

//------------------------------------------------------------------------------
//	�f�o�C�X�R���e�L�X�g�擾
//------------------------------------------------------------------------------
HDC CDXGraphics9::GetDC()
{
#ifdef _DEBUG
	if(m_pBackBufferSurface == NULL) {
		::OutputDebugString(TEXT("*** Error - �o�b�N�o�b�t�@���擾(CDXGraphics9_GetDC)\n"));
		return NULL;
	}
#endif
	if(m_hBackBufferDC != NULL)
		return m_hBackBufferDC;

	m_pBackBufferSurface->GetDC(&m_hBackBufferDC);

	return m_hBackBufferDC;
}

//------------------------------------------------------------------------------
//	�f�o�C�X�R���e�L�X�g���
//------------------------------------------------------------------------------
void CDXGraphics9::ReleaseDC()
{
	// �o�b�N�o�b�t�@���
	if(m_hBackBufferDC != NULL) {
		m_pBackBufferSurface->ReleaseDC(m_hBackBufferDC);
		m_hBackBufferDC = NULL;
	}
}
