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
	 Media.hpp
				���f�B�A�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Media.hpp"

#include <d3dx9.h>
#include <cassert>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#ifdef NDEBUG
#pragma comment(lib, "strmbase.lib")
#else
#pragma comment(lib, "strmbasd.lib")
#endif
#pragma comment(lib, "strmiids.lib")

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CMedia::CMedia(IDirect3DDevice9* pD3Device) : m_pD3DDevice(pD3Device), m_Suspend(false)
{
	assert(m_pD3DDevice != NULL);
	m_pD3DDevice->AddRef();
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CMedia::~CMedia()
{
	Release();
	m_pD3DDevice->Release();
}

//------------------------------------------------------------------------------
//	���
//------------------------------------------------------------------------------
void CMedia::Release()
{
	SafeRelease(m_pBasicAudio);

	if(m_pMediaControl != NULL) {
		m_pMediaControl->Stop();
		m_pMediaControl.Release();
		m_pMediaControl = NULL;
	}

	SafeRelease(m_pMediaSeeking );
	SafeRelease(m_pGraphBuilder );

	SafeRelease(m_pSoundRenderer);
	SafeRelease(m_pVideoRenderer);

	m_pD3DTxtRenderer = NULL;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���
//------------------------------------------------------------------------------
bool CMedia::LoadFromFile(LPCTSTR inFileName)
{
	Release();

	// �O���t�r���_�[����
	if(m_pGraphBuilder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �O���t�r���_�[�������s(CMedia_CreateMediaFromFile)\n"));
		return false;
	}

	HRESULT   hr;

	// �e�N�X�`�������_���[����
	m_pD3DTxtRenderer = new CD3DTexture9Renderer(m_pD3DDevice, &hr);
	if(hr != S_OK) {
		::OutputDebugString(TEXT("*** Error - �e�N�X�`�������_���[�������s(CMedia_CreateMediaFromFile)\n"));

		delete m_pD3DTxtRenderer;
		m_pD3DTxtRenderer = NULL;
		SafeRelease(m_pGraphBuilder);

		return false;
	}
	m_pVideoRenderer = m_pD3DTxtRenderer;

	// �r�f�I�����_���[�ǉ�
	CComPtr<IBaseFilter>   pBaseFilter;
	m_pVideoRenderer.QueryInterface(&pBaseFilter);
	if(FAILED(m_pGraphBuilder->AddFilter(pBaseFilter, L"D3DTexture9Renderer"))) {
		::OutputDebugString(TEXT("*** Error - �e�N�X�`�������_���[�ǉ����s(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	// DirectSound�����_���[����
	if(m_pSoundRenderer.CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC) != S_OK) {
		::OutputDebugString(TEXT("*** Error - DirectSound�����_���[�������s(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	// DirectSound�����_���[�ǉ�
	if(FAILED(m_pGraphBuilder->AddFilter(m_pSoundRenderer, L"DSoundRenderer"))) {
		::OutputDebugString(TEXT("*** Error - DirectSound�����_���[�ǉ����s(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	// �t�@�C���ǂݍ���
#ifdef UNICODE
	m_pGraphBuilder->RenderFile(inFileName, NULL);
#else
	// �t�@�C������UNICODE�֕ϊ�
	WCHAR   wzFileName[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, inFileName, -1, wzFileName, MAX_PATH);
	m_pGraphBuilder->RenderFile(wzFileName);
#endif

	// ���f�B�A�R���g���[���擾
	if(m_pGraphBuilder.QueryInterface(&m_pMediaControl) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ���f�B�A�R���g���[���擾���s(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	// ���f�B�A�V�[�N�擾
	if(m_pGraphBuilder.QueryInterface(&m_pMediaSeeking) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ���f�B�A�V�[�N�擾���s(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	// �x�[�V�b�N�I�[�f�B�I�擾
	if(m_pGraphBuilder.QueryInterface(&m_pBasicAudio) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �x�[�V�b�N�I�[�f�B�I�擾���s(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�擪����Đ�
//------------------------------------------------------------------------------
void CMedia::Replay()
{
	LONGLONG   pos = 0;
	m_pMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning,
								  NULL, AM_SEEKING_NoPositioning);
	m_pMediaControl->Run();
}

//------------------------------------------------------------------------------
//	�X�e�[�^�X�擾
//------------------------------------------------------------------------------
OAFilterState CMedia::GetState()
{
	OAFilterState  state;
	const HRESULT   hr = m_pMediaControl->GetState(INFINITE, &state);
	if(hr == E_FAIL)
		return State_Stopped;

	return state;
}

//------------------------------------------------------------------------------
//	�Đ�����
//------------------------------------------------------------------------------
bool CMedia::IsComplete()
{
	LONGLONG   duration = 0;
	m_pMediaSeeking->GetDuration(&duration);

	LONGLONG   pos = 0;
	m_pMediaSeeking->GetCurrentPosition(&pos);

	if(pos < duration)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	�����擾
//------------------------------------------------------------------------------
LONGLONG CMedia::GetLength()
{
	LONGLONG   duration = 0;
	m_pMediaSeeking->GetDuration(&duration);

	return duration;
}

//------------------------------------------------------------------------------
//	�ʒu�擾
//------------------------------------------------------------------------------
LONGLONG CMedia::GetPosition()
{
	LONGLONG   pos = 0;
	m_pMediaSeeking->GetCurrentPosition(&pos);

	return pos;
}

//------------------------------------------------------------------------------
//	���[�g�擾
//------------------------------------------------------------------------------
double CMedia::GetRate()
{
	double   rate = 0.0;
	m_pMediaSeeking->GetRate(&rate);

	return rate;
}

//------------------------------------------------------------------------------
//	�^�C���t�H�[�}�b�g�擾
//------------------------------------------------------------------------------
GUID CMedia::GetTimeFormat()
{
	GUID   format;
	m_pMediaSeeking->GetTimeFormat(&format);

	return format;
}

//------------------------------------------------------------------------------
//	���ʎ擾
//------------------------------------------------------------------------------
long CMedia::GetVolume()
{
	long   volume = -10000;
	m_pBasicAudio->get_Volume(&volume);

	return volume;
}

//------------------------------------------------------------------------------
//	���E�o�����X�擾
//------------------------------------------------------------------------------
long CMedia::GetBalance()
{
	long   balance = 0;
	m_pBasicAudio->get_Balance(&balance);

	return balance;
}

//------------------------------------------------------------------------------
//	�ʒu�ݒ�
//------------------------------------------------------------------------------
void CMedia::SetPosition(LONGLONG inPosition)
{
	m_pMediaSeeking->SetPositions(&inPosition, AM_SEEKING_AbsolutePositioning,
								  NULL,        AM_SEEKING_NoPositioning);
}

//------------------------------------------------------------------------------
//	���[�g�ݒ�
//------------------------------------------------------------------------------
void CMedia::SetRate(const double inRate)
{
	m_pMediaSeeking->SetRate(inRate);
}

//------------------------------------------------------------------------------
//	�^�C���t�H�[�}�b�g�ݒ�
//------------------------------------------------------------------------------
void CMedia::SetTimeFormat(const GUID& inFormat)
{
	m_pMediaSeeking->SetTimeFormat(&inFormat);
}

//------------------------------------------------------------------------------
//	���Έʒu�ݒ�
//------------------------------------------------------------------------------
void CMedia::SetRelativePosition(LONGLONG inPosition)
{
	m_pMediaSeeking->SetPositions(&inPosition, AM_SEEKING_RelativePositioning,
								  NULL,        AM_SEEKING_NoPositioning);
}

//------------------------------------------------------------------------------
//	���ʐݒ�(0�c�ő�, -10000�c����)
//------------------------------------------------------------------------------
void CMedia::SetVolume(long inVolume)
{
	m_pBasicAudio->put_Volume(inVolume);
}

//------------------------------------------------------------------------------
//	���E�o�����X�ݒ�(-10000�c�E�`�����l������, 10000�c���`�����l������)
//------------------------------------------------------------------------------
void CMedia::SetBalance(long inBalance)
{
	m_pBasicAudio->put_Balance(inBalance);
}

//------------------------------------------------------------------------------
//	�T�X�y���h
//------------------------------------------------------------------------------
void CMedia::Suspend()
{
	OAFilterState  state = State_Stopped;
	m_pMediaControl->GetState(0, &state);
	if(state == State_Running) {
		m_Suspend = true;
		m_pMediaControl->Stop();
	}
}

//------------------------------------------------------------------------------
//	���W���[��
//------------------------------------------------------------------------------
void CMedia::Resume()
{
	if(m_Suspend != false) {
		m_Suspend = false;
		m_pMediaControl->Run();
	}
}

//------------------------------------------------------------------------------
//	�e�N�X�`�������_���[�|�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CD3DTexture9Renderer::CD3DTexture9Renderer(IDirect3DDevice9* pD3DDevice, HRESULT* pHR)
	: CBaseVideoRenderer(__uuidof(CLSID_D3DTexture9Renderer),
						 NAME("D3DTexture9Renderer"), NULL, pHR),
	  m_pD3DDevice(pD3DDevice), m_Primary(0), m_Width(0), m_Height(0), m_Pitch(0)
{
	assert(m_pD3DDevice != NULL);
	m_pD3DDevice->AddRef();

	m_pTexture[0] = NULL;
	m_pTexture[1] = NULL;

	m_Format  [0] = D3DFMT_UNKNOWN;
	m_Format  [1] = D3DFMT_UNKNOWN;

    if(pHR != NULL)
        *pHR = S_OK;
}

//------------------------------------------------------------------------------
//	�e�N�X�`�������_���[�|�f�X�g���N�^
//------------------------------------------------------------------------------
CD3DTexture9Renderer::~CD3DTexture9Renderer()
{
	SafeRelease(m_pTexture[1]);
	SafeRelease(m_pTexture[0]);
	SafeRelease(m_pD3DDevice );
}

//------------------------------------------------------------------------------
//	�e�N�X�`�������_���[�|���f�B�A���ݒ�
//------------------------------------------------------------------------------
HRESULT CD3DTexture9Renderer::CheckMediaType(const CMediaType* pMediaFormat)
{
	if(pMediaFormat == NULL)
		return E_POINTER;

	if(::IsEqualGUID(*pMediaFormat->FormatType(), FORMAT_VideoInfo  ) == false)
		return VFW_E_NO_TYPES;

	if(::IsEqualGUID(*pMediaFormat->Type()      , MEDIATYPE_Video   ) == false)
		return VFW_E_INVALIDMEDIATYPE;

	if(::IsEqualGUID(*pMediaFormat->Subtype()   , MEDIASUBTYPE_RGB24) == false)
		return VFW_E_INVALIDSUBTYPE;

	return S_OK;	
}

//------------------------------------------------------------------------------
//	�e�N�X�`�������_���[�|���f�B�A���ݒ�
//------------------------------------------------------------------------------
HRESULT CD3DTexture9Renderer::SetMediaType(const CMediaType* pMediaFormat)
{
	if(pMediaFormat == NULL)
		return E_POINTER;

	// �r�f�I���擾
	const VIDEOINFOHEADER*    pVideoInfoHeader  =  (VIDEOINFOHEADER*)pMediaFormat->Format();
	const BITMAPINFOHEADER*   pBitmapInfoHeader = &(pVideoInfoHeader->bmiHeader);

	m_Width  = pBitmapInfoHeader->biWidth;
	m_Height = pBitmapInfoHeader->biHeight;
	m_Pitch  = (int)((((size_t)m_Width * pBitmapInfoHeader->biBitCount / 8) + 0x03) & ~0x03);

	// �e�N�X�`������
	int   height = m_Height;
	if(height < 0)
		height *= -1;

	HRESULT   hr;
	hr = ::D3DXCreateTexture(m_pD3DDevice, (UINT)m_Width, (UINT)height, 1, 0, D3DFMT_X8R8G8B8,
							 D3DPOOL_MANAGED, &m_pTexture[0]);
	if(hr != D3D_OK)
		return hr;
	hr = ::D3DXCreateTexture(m_pD3DDevice, (UINT)m_Width, (UINT)height, 1, 0, D3DFMT_X8R8G8B8,
							 D3DPOOL_MANAGED, &m_pTexture[1]);
	if(hr != D3D_OK) {
		SafeRelease(m_pTexture[0]);
		return hr;
	}

	// �e�N�X�`���t�H�[�}�b�g�`�F�b�N
	for(int i = 0; i < 2; i++) {
		D3DSURFACE_DESC   ddsd;
		::ZeroMemory(&ddsd, sizeof(ddsd));

		m_pTexture[i]->GetLevelDesc(0, &ddsd);
		if(ddsd.Format != D3DFMT_X8R8G8B8 && ddsd.Format != D3DFMT_A1R5G5B5) {
			SafeRelease(m_pTexture[1]);
			SafeRelease(m_pTexture[0]);
			return VFW_E_TYPE_NOT_ACCEPTED;
		}
		m_Format[i] = ddsd.Format;
	}	// for

	return S_OK;
}

//------------------------------------------------------------------------------
//	�e�N�X�`�������_���[�|�T���v�������_�����O
//------------------------------------------------------------------------------
HRESULT CD3DTexture9Renderer::DoRenderSample(IMediaSample* pMediaSample)
{
	if(pMediaSample == NULL)
		return E_POINTER;

	// �v���C�}���^�o�b�N�o�b�t�@�؂芷��
	const int   BACKBUFFER = m_Primary ^ 0x01;

	// �o�b�N�o�b�t�@�e�N�X�`���|�C���^�擾
	D3DLOCKED_RECT   d3dlr;
	if(m_pTexture[BACKBUFFER]->LockRect(0, &d3dlr, 0, 0) != D3D_OK)
		return E_FAIL;

	BYTE*       pTxtImage = (BYTE*)d3dlr.pBits;
	const INT   TXT_PITCH = d3dlr.Pitch;

	// �r�b�g�}�b�v�|�C���^�擾
	BYTE*   pBmpImage;
	pMediaSample->GetPointer(&pBmpImage);

	// �C���[�W�R�s�[
	if(m_Format[BACKBUFFER] == D3DFMT_X8R8G8B8) {
		// �t���J���[
		const int   WIDTH = m_Width / 4;
		LPDWORD     pTXT, pBMP;
		LPBYTE      pRMN;
		int         bmp_pitch, height;
		int         x, y;

		if(m_Height >= 0) {
			// �{�g���A�b�v
			pBmpImage +=  m_Pitch * (m_Height - 1);
			bmp_pitch  = -m_Pitch;
			height     =  m_Height;
		} else {
			// �g�b�v�_�E��
			bmp_pitch  =  m_Pitch;
			height     = -m_Height;
		}

		// �C���[�W�R�s�[
		for(y = 0; y < height; y++) {
			pTXT = (LPDWORD)pTxtImage;
			pBMP = (LPDWORD)pBmpImage;

			for(x = 0; x < WIDTH; x++ ) {
				pTXT[0] =   pBMP[0]        | 0xFF000000;
				pTXT[1] = ((pBMP[1] <<  8) | 0xFF000000) | (pBMP[0] >> 24);
				pTXT[2] = ((pBMP[2] << 16) | 0xFF000000) | (pBMP[1] >> 16);
				pTXT[3] =                    0xFF000000  | (pBMP[2] >>  8);
				pTXT +=4;
				pBMP +=3;
			}

			// we might have remaining (misaligned) bytes here
			pRMN = (LPBYTE)pBMP;
			for(x = 0; x < m_Width % 4; x++) {
				*pTXT =  0xFF000000     |
						(pRMN[2] << 16) | (pRMN[1] << 8) | pRMN[0];
				pTXT++;
				pRMN += 3;           
			}

			pTxtImage += TXT_PITCH;
			pBmpImage += bmp_pitch;
		}	// for(y)
	} else {
		// �n�C�J���[
		WORD*   pTXT;
		BYTE*   pBMP;
		int     bmp_pitch, height;
		int     x, y;

		if(m_Height >= 0) {
			// �{�g���A�b�v
			pBmpImage +=  m_Pitch * (m_Height - 1);
			bmp_pitch  = -m_Pitch;
			height     =  m_Height;
		} else {
			// �g�b�v�_�E��
			bmp_pitch  =  m_Pitch;
			height     = -m_Height;
		}

		// �C���[�W�R�s�[
		for(y = 0; y < height; y++ ) {
			pTXT = (WORD*)pTxtImage;
			pBMP = (BYTE*)pBmpImage;

			for (x = 0; x < m_Width; x++) {
				*pTXT = (WORD)(0x8000                |
							 ((pBMP[2] & 0xF8) << 7) |
							 ((pBMP[1] & 0xF8) << 2) |
							  (pBMP[0]         >> 3));

				pTXT++;
				pBMP += 3;
			}

			pTxtImage += TXT_PITCH;
			pBmpImage += bmp_pitch;
		}	// for(y)
	}	// if(m_Format)

	m_pTexture[BACKBUFFER]->UnlockRect(0);
	m_Primary = BACKBUFFER;

    return S_OK;
}

//------------------------------------------------------------------------------
//	�e�N�X�`�������_���[�|OnReceiveFirstSample����
//------------------------------------------------------------------------------
void CD3DTexture9Renderer::OnReceiveFirstSample(IMediaSample* pMediaSample)
{
	DoRenderSample(pMediaSample);
}
