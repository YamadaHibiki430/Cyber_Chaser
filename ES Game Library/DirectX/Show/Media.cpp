/*
================================================================================
					 オブジェクト指向＆＆ゲームプログラミング
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 【対象ＯＳ】
     Microsoft Windows 7以降

 【コンパイラ】
     Microsoft VisualC++ 2013

 【プログラム】
	 Media.hpp
				メディアクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Media.hpp"

#include <d3dx9.h>
#include <cassert>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#ifdef NDEBUG
#pragma comment(lib, "strmbase.lib")
#else
#pragma comment(lib, "strmbasd.lib")
#endif
#pragma comment(lib, "strmiids.lib")

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CMedia::CMedia(IDirect3DDevice9* pD3Device) : m_pD3DDevice(pD3Device), m_Suspend(false)
{
	assert(m_pD3DDevice != NULL);
	m_pD3DDevice->AddRef();
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CMedia::~CMedia()
{
	Release();
	m_pD3DDevice->Release();
}

//------------------------------------------------------------------------------
//	解放
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
//	ファイル読み込み
//------------------------------------------------------------------------------
bool CMedia::LoadFromFile(LPCTSTR inFileName)
{
	Release();

	// グラフビルダー生成
	if(m_pGraphBuilder.CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC) != S_OK) {
		::OutputDebugString(TEXT("*** Error - グラフビルダー生成失敗(CMedia_CreateMediaFromFile)\n"));
		return false;
	}

	HRESULT   hr;

	// テクスチャレンダラー生成
	m_pD3DTxtRenderer = new CD3DTexture9Renderer(m_pD3DDevice, &hr);
	if(hr != S_OK) {
		::OutputDebugString(TEXT("*** Error - テクスチャレンダラー生成失敗(CMedia_CreateMediaFromFile)\n"));

		delete m_pD3DTxtRenderer;
		m_pD3DTxtRenderer = NULL;
		SafeRelease(m_pGraphBuilder);

		return false;
	}
	m_pVideoRenderer = m_pD3DTxtRenderer;

	// ビデオレンダラー追加
	CComPtr<IBaseFilter>   pBaseFilter;
	m_pVideoRenderer.QueryInterface(&pBaseFilter);
	if(FAILED(m_pGraphBuilder->AddFilter(pBaseFilter, L"D3DTexture9Renderer"))) {
		::OutputDebugString(TEXT("*** Error - テクスチャレンダラー追加失敗(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	// DirectSoundレンダラー生成
	if(m_pSoundRenderer.CoCreateInstance(CLSID_DSoundRender, NULL, CLSCTX_INPROC) != S_OK) {
		::OutputDebugString(TEXT("*** Error - DirectSoundレンダラー生成失敗(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	// DirectSoundレンダラー追加
	if(FAILED(m_pGraphBuilder->AddFilter(m_pSoundRenderer, L"DSoundRenderer"))) {
		::OutputDebugString(TEXT("*** Error - DirectSoundレンダラー追加失敗(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	// ファイル読み込み
#ifdef UNICODE
	m_pGraphBuilder->RenderFile(inFileName, NULL);
#else
	// ファイル名をUNICODEへ変換
	WCHAR   wzFileName[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, inFileName, -1, wzFileName, MAX_PATH);
	m_pGraphBuilder->RenderFile(wzFileName);
#endif

	// メディアコントロール取得
	if(m_pGraphBuilder.QueryInterface(&m_pMediaControl) != S_OK) {
		::OutputDebugString(TEXT("*** Error - メディアコントロール取得失敗(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	// メディアシーク取得
	if(m_pGraphBuilder.QueryInterface(&m_pMediaSeeking) != S_OK) {
		::OutputDebugString(TEXT("*** Error - メディアシーク取得失敗(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	// ベーシックオーディオ取得
	if(m_pGraphBuilder.QueryInterface(&m_pBasicAudio) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ベーシックオーディオ取得失敗(CMedia_CreateMediaFromFile)\n"));
		Release();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	先頭から再生
//------------------------------------------------------------------------------
void CMedia::Replay()
{
	LONGLONG   pos = 0;
	m_pMediaSeeking->SetPositions(&pos, AM_SEEKING_AbsolutePositioning,
								  NULL, AM_SEEKING_NoPositioning);
	m_pMediaControl->Run();
}

//------------------------------------------------------------------------------
//	ステータス取得
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
//	再生完了
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
//	長さ取得
//------------------------------------------------------------------------------
LONGLONG CMedia::GetLength()
{
	LONGLONG   duration = 0;
	m_pMediaSeeking->GetDuration(&duration);

	return duration;
}

//------------------------------------------------------------------------------
//	位置取得
//------------------------------------------------------------------------------
LONGLONG CMedia::GetPosition()
{
	LONGLONG   pos = 0;
	m_pMediaSeeking->GetCurrentPosition(&pos);

	return pos;
}

//------------------------------------------------------------------------------
//	レート取得
//------------------------------------------------------------------------------
double CMedia::GetRate()
{
	double   rate = 0.0;
	m_pMediaSeeking->GetRate(&rate);

	return rate;
}

//------------------------------------------------------------------------------
//	タイムフォーマット取得
//------------------------------------------------------------------------------
GUID CMedia::GetTimeFormat()
{
	GUID   format;
	m_pMediaSeeking->GetTimeFormat(&format);

	return format;
}

//------------------------------------------------------------------------------
//	音量取得
//------------------------------------------------------------------------------
long CMedia::GetVolume()
{
	long   volume = -10000;
	m_pBasicAudio->get_Volume(&volume);

	return volume;
}

//------------------------------------------------------------------------------
//	左右バランス取得
//------------------------------------------------------------------------------
long CMedia::GetBalance()
{
	long   balance = 0;
	m_pBasicAudio->get_Balance(&balance);

	return balance;
}

//------------------------------------------------------------------------------
//	位置設定
//------------------------------------------------------------------------------
void CMedia::SetPosition(LONGLONG inPosition)
{
	m_pMediaSeeking->SetPositions(&inPosition, AM_SEEKING_AbsolutePositioning,
								  NULL,        AM_SEEKING_NoPositioning);
}

//------------------------------------------------------------------------------
//	レート設定
//------------------------------------------------------------------------------
void CMedia::SetRate(const double inRate)
{
	m_pMediaSeeking->SetRate(inRate);
}

//------------------------------------------------------------------------------
//	タイムフォーマット設定
//------------------------------------------------------------------------------
void CMedia::SetTimeFormat(const GUID& inFormat)
{
	m_pMediaSeeking->SetTimeFormat(&inFormat);
}

//------------------------------------------------------------------------------
//	相対位置設定
//------------------------------------------------------------------------------
void CMedia::SetRelativePosition(LONGLONG inPosition)
{
	m_pMediaSeeking->SetPositions(&inPosition, AM_SEEKING_RelativePositioning,
								  NULL,        AM_SEEKING_NoPositioning);
}

//------------------------------------------------------------------------------
//	音量設定(0…最大, -10000…無音)
//------------------------------------------------------------------------------
void CMedia::SetVolume(long inVolume)
{
	m_pBasicAudio->put_Volume(inVolume);
}

//------------------------------------------------------------------------------
//	左右バランス設定(-10000…右チャンネル無音, 10000…左チャンネル無音)
//------------------------------------------------------------------------------
void CMedia::SetBalance(long inBalance)
{
	m_pBasicAudio->put_Balance(inBalance);
}

//------------------------------------------------------------------------------
//	サスペンド
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
//	レジューム
//------------------------------------------------------------------------------
void CMedia::Resume()
{
	if(m_Suspend != false) {
		m_Suspend = false;
		m_pMediaControl->Run();
	}
}

//------------------------------------------------------------------------------
//	テクスチャレンダラー－デフォルトコンストラクタ
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
//	テクスチャレンダラー－デストラクタ
//------------------------------------------------------------------------------
CD3DTexture9Renderer::~CD3DTexture9Renderer()
{
	SafeRelease(m_pTexture[1]);
	SafeRelease(m_pTexture[0]);
	SafeRelease(m_pD3DDevice );
}

//------------------------------------------------------------------------------
//	テクスチャレンダラー－メディア情報設定
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
//	テクスチャレンダラー－メディア情報設定
//------------------------------------------------------------------------------
HRESULT CD3DTexture9Renderer::SetMediaType(const CMediaType* pMediaFormat)
{
	if(pMediaFormat == NULL)
		return E_POINTER;

	// ビデオ情報取得
	const VIDEOINFOHEADER*    pVideoInfoHeader  =  (VIDEOINFOHEADER*)pMediaFormat->Format();
	const BITMAPINFOHEADER*   pBitmapInfoHeader = &(pVideoInfoHeader->bmiHeader);

	m_Width  = pBitmapInfoHeader->biWidth;
	m_Height = pBitmapInfoHeader->biHeight;
	m_Pitch  = (int)((((size_t)m_Width * pBitmapInfoHeader->biBitCount / 8) + 0x03) & ~0x03);

	// テクスチャ生成
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

	// テクスチャフォーマットチェック
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
//	テクスチャレンダラー－サンプルレンダリング
//------------------------------------------------------------------------------
HRESULT CD3DTexture9Renderer::DoRenderSample(IMediaSample* pMediaSample)
{
	if(pMediaSample == NULL)
		return E_POINTER;

	// プライマリ／バックバッファ切り換え
	const int   BACKBUFFER = m_Primary ^ 0x01;

	// バックバッファテクスチャポインタ取得
	D3DLOCKED_RECT   d3dlr;
	if(m_pTexture[BACKBUFFER]->LockRect(0, &d3dlr, 0, 0) != D3D_OK)
		return E_FAIL;

	BYTE*       pTxtImage = (BYTE*)d3dlr.pBits;
	const INT   TXT_PITCH = d3dlr.Pitch;

	// ビットマップポインタ取得
	BYTE*   pBmpImage;
	pMediaSample->GetPointer(&pBmpImage);

	// イメージコピー
	if(m_Format[BACKBUFFER] == D3DFMT_X8R8G8B8) {
		// フルカラー
		const int   WIDTH = m_Width / 4;
		LPDWORD     pTXT, pBMP;
		LPBYTE      pRMN;
		int         bmp_pitch, height;
		int         x, y;

		if(m_Height >= 0) {
			// ボトムアップ
			pBmpImage +=  m_Pitch * (m_Height - 1);
			bmp_pitch  = -m_Pitch;
			height     =  m_Height;
		} else {
			// トップダウン
			bmp_pitch  =  m_Pitch;
			height     = -m_Height;
		}

		// イメージコピー
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
		// ハイカラー
		WORD*   pTXT;
		BYTE*   pBMP;
		int     bmp_pitch, height;
		int     x, y;

		if(m_Height >= 0) {
			// ボトムアップ
			pBmpImage +=  m_Pitch * (m_Height - 1);
			bmp_pitch  = -m_Pitch;
			height     =  m_Height;
		} else {
			// トップダウン
			bmp_pitch  =  m_Pitch;
			height     = -m_Height;
		}

		// イメージコピー
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
//	テクスチャレンダラー－OnReceiveFirstSample処理
//------------------------------------------------------------------------------
void CD3DTexture9Renderer::OnReceiveFirstSample(IMediaSample* pMediaSample)
{
	DoRenderSample(pMediaSample);
}
