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
	 DXAudio8.cpp
				DirectX Audio8クラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "DXAudio8.hpp"
#include "../Common/ComLibrary.hpp"

#include <mmreg.h>
#include <msacm.h>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "msacm32.lib")

//------------------------------------------------------------------------------
//	OGG使用時
//------------------------------------------------------------------------------
#ifdef DXA_OGG_OPT
	#include <vorbis/codec.h>
	#include <vorbis/vorbisfile.h>

	#pragma comment(lib, "ogg_static.lib")
	#pragma comment(lib, "vorbis_static.lib")
	#pragma comment(lib, "vorbisfile_static.lib")
#endif

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CDXAudio8::CDXAudio8() : m_pDSound  (NULL), m_pDSBPrimary   (NULL), m_pListener(NULL),
						 m_pDMLoader(NULL), m_pDMPerformance(NULL),
						 m_SuspendPrimary(DSBVOLUME_MAX), m_SuspendSegment(0)
{
	CComLibrary::Initialize();		// COMライブラリ初期化
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CDXAudio8::~CDXAudio8()
{
	Release();
	CComLibrary::Uninitialize();	// COMライブラリ解放
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CDXAudio8::Initialize(const HWND hWnd)
{
	Release();

	try {
		// DirectSoundオブジェクトの生成とインタフェースの取得
		if(::CoCreateInstance(CLSID_DirectSound8, NULL, CLSCTX_INPROC_SERVER,
							  IID_IDirectSound8, (LPVOID*)&m_pDSound) != S_OK)
			throw TEXT("*** Error - DirectSoundオブジェクト生成失敗(CDXAudio8_Initialize)\n");

		// DirectSoundオブジェクト初期化
		if(m_pDSound->Initialize(NULL) != DS_OK)
			throw TEXT("*** Error - DirectSoundオブジェクト初期化失敗(CDXAudio8_Initialize)\n");

		// DirectSound協調レベル設定
		if(m_pDSound->SetCooperativeLevel(hWnd, DSSCL_PRIORITY) != DS_OK)
			throw TEXT("*** Error - DirectSound協調レベル設定失敗(CDXAudio8_Initialize)\n");

		// DirectMusicLoaderオブジェクトの生成とインタフェースの取得
		if(::CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC_SERVER,
							  IID_IDirectMusicLoader8, (LPVOID*)&m_pDMLoader) != S_OK)
			throw TEXT("*** Error - ローダオブジェクト生成失敗(CDXAudio8_Initialize)\n");

		// DirectMusicPerformanceオブジェクトの生成とインタフェースの取得
		if(::CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC_SERVER,
							  IID_IDirectMusicPerformance8, (LPVOID*)&m_pDMPerformance) != S_OK)
			throw TEXT("*** Error - パフォーマンスオブジェクト生成失敗(CDXAudio8_Initialize)\n");

		// パフォーマンスの初期化とデフォルトオーディオパスのセットアップ
		if(m_pDMPerformance->InitAudio(NULL, (LPDIRECTSOUND*)&m_pDSound, NULL,
									   DMUS_APATH_DYNAMIC_STEREO, 64, DMUS_AUDIOF_ALL, NULL) != S_OK)
			throw TEXT("*** Error - パフォーマンス初期化失敗(CDXAudio8_Initialize)\n");

		// プライマリバッファ取得
		DSBUFFERDESC   dsbd;
		::ZeroMemory(&dsbd, sizeof(dsbd));
		dsbd.dwSize        = sizeof(dsbd);
		dsbd.dwFlags       = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
		dsbd.dwBufferBytes = 0;
		dsbd.lpwfxFormat   = NULL;
		if(FAILED(m_pDSound->CreateSoundBuffer(&dsbd, &m_pDSBPrimary, NULL)))
			throw TEXT("*** Error - プライマリバッファ取得失敗(CDXAudio8_Initialize)\n");

		// リスナ生成
		IDirectSound3DListener8*   pDS3DListener;
		if(m_pDSBPrimary->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&pDS3DListener) == S_OK) {
			m_pListener = new CSoundListener(pDS3DListener);
			pDS3DListener->Release();
		} else {
			::OutputDebugString(TEXT("*** Error - リスナ取得失敗(CDXAudio8_Initialize)\n"));
			m_pListener = new CNullSoundListener();
		}
	} catch(LPCTSTR ErrorString) {
		::OutputDebugString(ErrorString);
		Release();

		// 初期化に失敗した場合は、リスナにNULLオブジェクトを設定
		m_pListener = new CNullSoundListener();

		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CDXAudio8::Release()
{
	m_ProtectedResource.clear();

	// リスナ解放
	delete m_pListener;
	m_pListener = NULL;

	// プライマリバッファ解放
	if(m_pDSBPrimary != NULL) {
		m_pDSBPrimary->Release();
		m_pDSBPrimary = NULL;
	}

	// セグメント解放
	ReleaseAllSegments();

	// パフォーマンスオブジェクト解放
	if(m_pDMPerformance != NULL) {
		m_pDMPerformance->CloseDown();
		m_pDMPerformance->Release();
		m_pDMPerformance = NULL;
	}

	// ローダオブジェクト解放
	if(m_pDMLoader != NULL) {
		m_pDMLoader->Release();
		m_pDMLoader = NULL;
	}

	// バッファ解放
	ReleaseAllBuffers();

	// DirectSoundオブジェクト解放
	if(m_pDSound != NULL) {
		m_pDSound->Release();
		m_pDSound = NULL;
	}
}

//------------------------------------------------------------------------------
//	サスペンド処理
//------------------------------------------------------------------------------
void CDXAudio8::Suspend()
{ 
	m_SuspendPrimary = GetMasterVolume();	SetMasterVolume(DSBVOLUME_MIN);
	m_SuspendSegment = GetSegmentVolume();	SetSegmentVolume(-10000);
}

//------------------------------------------------------------------------------
//	マスターボリューム取得
//------------------------------------------------------------------------------
LONG CDXAudio8::GetMasterVolume()
{
	if(m_pDSBPrimary == NULL) {
		::OutputDebugString(TEXT("*** Error - プライマリバッファ未生成(CDXAudio8_GetMasterVolume)\n"));
		return DSBVOLUME_MIN;
	}

	LONG   volume;
	if(m_pDSBPrimary->GetVolume(&volume) != DS_OK)
		return DSBVOLUME_MAX;
	
	return volume;
}

//------------------------------------------------------------------------------
//	マスターボリューム設定
//------------------------------------------------------------------------------
void CDXAudio8::SetMasterVolume(LONG inVolume)
{
	if(m_pDSBPrimary == NULL) {
		::OutputDebugString(TEXT("*** Error - プライマリバッファ未生成(CDXAudio8_SetMasterVolume)\n"));
		return;
	}
	m_pDSBPrimary->SetVolume(inVolume);
}

//------------------------------------------------------------------------------
//	セグメントマスターボリューム取得
//------------------------------------------------------------------------------
int CDXAudio8::GetSegmentVolume()
{
	if(m_pDMPerformance == NULL) {
		::OutputDebugString(TEXT("*** Error - パフォーマンス未生成(CDXAudio8_GetSegmentVolume)\n"));
		return 0;
	}

	int   volume;
	if(m_pDMPerformance->GetGlobalParam(GUID_PerfMasterVolume, (LPVOID)&volume, sizeof(volume)) != S_OK)
		return 0;

	return volume;
}

//------------------------------------------------------------------------------
//	セグメントマスターボリューム設定
//------------------------------------------------------------------------------
void CDXAudio8::SetSegmentVolume(int inVolume)
{
	if(m_pDMPerformance == NULL) {
		::OutputDebugString(TEXT("*** Error - パフォーマンス未生成(CDXAudio8_GetSegmentVolume)\n"));
		return;
	}
	m_pDMPerformance->SetGlobalParam(GUID_PerfMasterVolume, (LPVOID)&inVolume, sizeof(inVolume));
}

//------------------------------------------------------------------------------
//	セグメント生成
//------------------------------------------------------------------------------
IMusicSegment* CDXAudio8::CreateSegmentFromFile(LPTSTR inFileName)
{
	// MusicSegmentオブジェクト生成
	IMusicSegment*          pSegment   = NULL;
	IDirectMusicSegment8*   pDMSegment = NULL;
	BYTE*                   pWaveData  = NULL;
	try {
		if(m_pDMLoader == NULL)
			throw TEXT("*** Error - DirectX Audio未初期化(CDXAudio8_CreateSegmentFromFile)\n");

		// DirectMusicSegmentオブジェクト生成
		HRESULT   hr;
#ifdef UNICODE
		// ファイル読み込み
		hr = m_pDMLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8,
											 inFileName, (LPVOID*)&pDMSegment);
#else
		// ファイル名をUNICODEへ変換
		WCHAR   wzFileName[MAX_PATH];
		::MultiByteToWideChar(CP_ACP, 0, inFileName, -1, wzFileName, MAX_PATH);

		// ファイル読み込み
		hr = m_pDMLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8,
											 wzFileName, (LPVOID*)&pDMSegment);
#endif

#ifndef DXA_OGG_OPT
		if(FAILED(hr))
			throw TEXT("*** Error - ファイル読み込み失敗(CDXAudio8_CreateSegmentFromFile)\n");
#else
		// OGGファイルとして開く
		if(FAILED(hr)) {
			if(CreateMusicSegmentFromOggFile(inFileName, pDMSegment, pWaveData) == false)
				throw TEXT("*** Error - ファイル読み込み失敗(CDXAudio8_CreateSegmentFromFile)\n");
		}
#endif

		// 標準MIDIファイル設定
		MUSIC_TIME   mt;
		pDMSegment->GetLength(&mt);
		if(mt != 1)
			pDMSegment->SetParam(GUID_StandardMIDIFile, 0xFFFFFFFF, 0, 0, NULL);

		// パフォーマンスにダウンロード
		if(pDMSegment->Download(m_pDMPerformance) != S_OK)
			throw TEXT("*** Error - ダウンロード失敗(CDXAudio8_CreateSegmentFromFile)\n");

		pSegment = new CMusicSegment(m_pDMLoader, m_pDMPerformance, pDMSegment, pWaveData);
	} catch(LPCTSTR ErrorString) {
		::OutputDebugString(ErrorString);

		delete[] pWaveData;
		pSegment = new CNullMusicSegment();
	}
	SafeRelease(pDMSegment);

	// セグメントリストへ追加
	m_SegmentList.push_back(pSegment);

	return pSegment;
}

//------------------------------------------------------------------------------
//	セグメント解放
//------------------------------------------------------------------------------
void CDXAudio8::ReleaseSegment(IMusicSegment*& pSegment)
{
	m_ProtectedResource.erase(pSegment);	// プロテクト解除

	m_SegmentList.remove(pSegment);
	delete pSegment;
	pSegment = NULL;
}

//------------------------------------------------------------------------------
//	全セグメント解放
//------------------------------------------------------------------------------
void CDXAudio8::ReleaseAllSegments()
{
	std::list<IMusicSegment*>::iterator   list_it = m_SegmentList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_SegmentList.end()) {
			delete *list_it;
			list_it++;
		}
		m_SegmentList.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_SegmentList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_SegmentList.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	バッファ生成
//------------------------------------------------------------------------------
ISoundBuffer* CDXAudio8::CreateBufferFromFile(LPTSTR inFileName, const bool in3DBuffer)
{
/*
	HMMIO                 hFile      = NULL;
	HACMSTREAM            hACMStream = NULL;	// ACMストリーム
	LPVOID                pFormat    = NULL;	// フォーマット
	LPVOID                pImage     = NULL;	// イメージ

	ISoundBuffer*         pBuffer    = NULL;
	IDirectSoundBuffer8*  pDSBuffer  = NULL;

	try {
		if(m_pDSound == NULL)
			throw TEXT("*** Error - DirectX Audio未初期化(CDXAudio8_CreateBufferFromFile)\n");

		// ファイルオープン
		hFile = ::mmioOpen(inFileName, NULL, MMIO_READ | MMIO_ALLOCBUF);
		if(hFile == NULL)
			throw TEXT("*** Error - ファイルオープン失敗(CDXAudio8_CreateBufferFromFile)\n");

		MMCKINFO   ParentChunk, ChildChunk;
		::ZeroMemory(&ParentChunk, sizeof(ParentChunk));
		::ZeroMemory(&ChildChunk,  sizeof(ChildChunk ));

		// 'WAVE'チャンクへ進入
		ParentChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if(::mmioDescend(hFile, &ParentChunk, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'WAVE'チャンクが見つかりません(CDXAudio8_CreateBufferFromFile)\n");

		// 'WAVE'チャンクの'fmt 'チャンクへ進入
		ChildChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'fmt 'チャンクが見つかりません(CDXAudio8_CreateBufferFromFile)\n");

		// フォーマット情報取得
		pFormat = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pFormat, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - フォーマット情報取得失敗(CDXAudio8_CreateBufferFromFile)\n");

		::mmioAscend(hFile, &ChildChunk, 0);	// 'fmt 'チャンク退出

		// 'WAVE'チャンクの'data'チャンクへ進入
		ChildChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'data'チャンクが見つかりません(CDXAudio8_CreateBufferFromFile)\n");

		// リニアPCMに変換
		WAVEFORMATEX   NewFormat;
		::ZeroMemory(&NewFormat, sizeof(NewFormat));

		// 推奨フォーマット取得
		NewFormat.wFormatTag = WAVE_FORMAT_PCM;
		if(::acmFormatSuggest(NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, sizeof(NewFormat),
								ACM_FORMATSUGGESTF_WFORMATTAG) != 0)
			throw TEXT("*** Error - 推奨フォーマット取得失敗(CDXAudio8_CreateBufferFromFile)\n");

		// ACMストリームオープン
		if(::acmStreamOpen(&hACMStream, NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, NULL, 0, 0,
							ACM_STREAMOPENF_NONREALTIME) != 0)
			throw TEXT("*** Error - ACMストリームオープン失敗(CDXAudio8_CreateBufferFromFile)\n");

		// フォーマット変換後取得
		DWORD   NewSize;
		if(::acmStreamSize(hACMStream, ChildChunk.cksize, &NewSize, ACM_STREAMSIZEF_SOURCE) != 0)
			throw TEXT("*** Error - デコードサイズ取得失敗(CDXAudio8_CreateBufferFromFile)\n");

		// 変換元イメージ読み込み
		pImage = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pImage, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - 変換元イメージ読み込み失敗(CDXAudio8_CreateBufferFromFile)\n");

		// セカンダリバッファ生成
		DSBUFFERDESC   dsbd;
		::ZeroMemory(&dsbd, sizeof(dsbd));
		dsbd.dwSize          = sizeof(dsbd);
		dsbd.dwBufferBytes   = ChildChunk.cksize;
		dsbd.dwReserved      = 0;
		dsbd.lpwfxFormat     = (LPWAVEFORMATEX)pFormat;
		dsbd.dwFlags         = DSBCAPS_LOCDEFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
							   DSBCAPS_CTRLFX   | DSBCAPS_GETCURRENTPOSITION2;
		if(in3DBuffer == false) {
			dsbd.dwFlags        |= DSBCAPS_CTRLPAN;
			dsbd.guid3DAlgorithm = GUID_NULL;
		} else {
			dsbd.dwFlags        |= DSBCAPS_CTRL3D;
			dsbd.guid3DAlgorithm = DS3DALG_HRTF_LIGHT;
		}

		// DirectSoundBuffer生成
		IDirectSoundBuffer*   pdsb;
		if(FAILED(m_pDSound->CreateSoundBuffer(&dsbd, &pdsb, NULL)))
			throw TEXT("*** Error - DirectSoundBuffer生成失敗(CDXAudio8_CreateBufferFromFile)\n");

		// DirectSoundBuffer8取得
		const HRESULT   hr = pdsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pDSBuffer);
		pdsb->Release();
		if(hr != S_OK)
			throw TEXT("*** Error - DirectSoundBuffer8取得失敗(CDXAudio8_CreateBufferFromFile)\n");

		// バッファのロック
		LPVOID    pBuf[2];
		DWORD     BufBytes[2];
		if(pDSBuffer->Lock(0, 0, &pBuf[0], &BufBytes[0], &pBuf[1], &BufBytes[1], DSBLOCK_ENTIREBUFFER)
			!= DS_OK)
			throw TEXT("*** Error - バッファロック失敗(CDXAudio8_CreateBufferFromFile)\n");

		// ACMストリームヘッダ設定
		ACMSTREAMHEADER   acmsh;
		::ZeroMemory(&acmsh, sizeof(acmsh));
		acmsh.cbStruct    = sizeof(acmsh);
		acmsh.fdwStatus   = 0;
		acmsh.pbSrc       = (LPBYTE)pImage;			// 変換元イメージ領域
		acmsh.cbSrcLength = ChildChunk.cksize;		// 変換元イメージ領域のサイズ
		acmsh.pbDst       = (LPBYTE)pBuf[0];		// 変換後イメージ領域
		acmsh.cbDstLength = BufBytes[0];			// 変換後イメージ領域のサイズ
		if(::acmStreamPrepareHeader(hACMStream, &acmsh, 0) != 0) {
			pDSBuffer->Unlock(pBuf[0], 0, pBuf[1], 0);
			throw TEXT("*** Error - ACMストリームヘッダ設定失敗(CDXAudio8_CreateBufferFromFile)\n");
		}

		// 変換しながらバッファへ転送
		const MMRESULT   mmr = ::acmStreamConvert(hACMStream, &acmsh, 0);

		::acmStreamUnprepareHeader(hACMStream, &acmsh, 0);
		pDSBuffer->Unlock(pBuf[0], BufBytes[0], pBuf[1], 0);
		if(mmr != 0)
			throw TEXT("*** Error - イメージ変換失敗(CDXAudio8_CreateBufferFromFile)\n");

		pBuffer = new CSoundBuffer(pDSBuffer, inFileName);
	} catch(LPCTSTR ErrorString) {
		::OutputDebugString(ErrorString);
		// 例外が発生した場合は、NULLサウンドバッファを生成
		pBuffer = new CNullSoundBuffer();
	}

	SafeRelease(pDSBuffer);
	delete[] pImage;
	delete[] pFormat;
	if(hACMStream != NULL)	::acmStreamClose(hACMStream, 0);;
	if(hFile      != NULL)	::mmioClose(hFile, 0);
*/

	// DirectSoundBuffer8生成
	IDirectSoundBuffer8*  pDSBuffer = CreateDirectSoundBufferFromFile(inFileName, false, in3DBuffer);
	ISoundBuffer*         pBuffer;
	if(pDSBuffer != NULL)
		pBuffer = new CSoundBuffer(pDSBuffer, inFileName);
	else
		pBuffer = new CNullSoundBuffer();

	// バッファリストへ追加
	m_BufferList.push_back(pBuffer);
	SafeRelease(pDSBuffer);

	return pBuffer;
}

//------------------------------------------------------------------------------
//	共有バッファ生成
//------------------------------------------------------------------------------
DWORD CDXAudio8::CreateSharedBufferFromFile(LPTSTR inFileName,
											ISoundBuffer* pSoundBufferArray[],
											const DWORD inArrayCount,
											const bool  in3DBuffer)
{
#ifdef _DEBUG
	if(inArrayCount == 0) {
		::OutputDebugString(TEXT("*** Error - 配列要素数ゼロ(CDXAudio8_CreateSharedBufferFromFile)\n"));
		return 0;
	}
#endif

	ISoundBuffer*   pBuffer;
	DWORD           actual = 0;	// 実際に生成できた数
	DWORD           index  = 0;	// 配列インデックス

	// DirectSoundBuffer8生成
	IDirectSoundBuffer8*  pDSBuffer = CreateDirectSoundBufferFromFile(inFileName, true, in3DBuffer);
	if(pDSBuffer != NULL) {
		pBuffer = new CSoundBuffer(pDSBuffer, inFileName);
		actual++;
	} else {
		pBuffer = new CNullSoundBuffer();
	}

	// バッファリストへ追加
	m_BufferList.push_back(pBuffer);
	pSoundBufferArray[index] = pBuffer;
	index++;

	// 共有バッファ生成
	IDirectSoundBuffer*    pdsb  = NULL;
	IDirectSoundBuffer8*   pdsb8 = NULL;
	HRESULT                hr;
	while(index < inArrayCount) {
		try {
			if(m_pDSound == NULL)
				throw TEXT("*** Error - DirectX Audio未初期化(CDXAudio8_CreateSharedBufferFromFile)\n");
			if(pDSBuffer == NULL)
				throw TEXT("*** Error - DirectSoundBuffer8生成失敗(CDXAudio8_CreateSharedBufferFromFile)\n");

			// 共有バッファ生成
			if(m_pDSound->DuplicateSoundBuffer(pDSBuffer, &pdsb) != DS_OK)
				throw TEXT("*** Error - 共有バッファ生成失敗(CDXAudio8_CreateSharedBufferFromFile)\n");
			// IDirectSoundBuffer8取得
			hr = pdsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pdsb8);
			pdsb->Release();
			if(hr != S_OK)
				throw TEXT("*** Error - DirectSoundBuffer8取得失敗(CDXAudio8_CreateSharedBufferFromFile)\n");

			pBuffer = new CSoundBuffer(pdsb8, TEXT(""));
			pdsb8->Release();
			actual++;
		} catch(LPCTSTR ErrorString) {
			::OutputDebugString(ErrorString);
			pBuffer = new CNullSoundBuffer();
		}

		// バッファリストへ追加
		m_BufferList.push_back(pBuffer);
		pSoundBufferArray[index] = pBuffer;
		index++;
	}
	SafeRelease(pDSBuffer);

	return actual;
}

//------------------------------------------------------------------------------
//	バッファ解放
//------------------------------------------------------------------------------
void CDXAudio8::ReleaseBuffer(ISoundBuffer*& pBuffer)
{
	m_ProtectedResource.erase(pBuffer);		// プロテクト解除

	m_BufferList.remove(pBuffer);
	delete pBuffer;
	pBuffer = NULL;
}

//------------------------------------------------------------------------------
//	全バッファ解放
//------------------------------------------------------------------------------
void CDXAudio8::ReleaseAllBuffers()
{
	std::list<ISoundBuffer*>::iterator   list_it = m_BufferList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_BufferList.end()) {
			delete *list_it;
			list_it++;
		}
		m_BufferList.clear();
	} else {
		std::set <void*>::iterator   set_it;
		std::set <void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_BufferList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// プロテクト対象外
				delete *list_it;
				list_it = m_BufferList.erase(list_it);
			} else {
				// プロテクト対象
				list_it++;
			}
		}
	}
}

#ifdef DXA_OGG_OPT
//------------------------------------------------------------------------------
//	OGGファイルからIDirectMusicSegment8インタフェース生成
//------------------------------------------------------------------------------
bool CDXAudio8::CreateMusicSegmentFromOggFile(LPTSTR inFileName,
											  IDirectMusicSegment8*& pDMSegment,
											  BYTE*& pWaveImage)
{
/*
	// OGGファイルオープン
	FILE*       pFile = NULL;
#if UNICODE
	char        file_name[MAX_PATH + 1];
	const int   len = ::WideCharToMultiByte(CP_THREAD_ACP, 0, inFileName, -1, file_name, 0, NULL, NULL);
	::WideCharToMultiByte(CP_THREAD_ACP, 0, inFileName, -1, file_name, len, NULL, NULL);
	::fopen_s(&pFile, file_name,  "rb");
#else
	::fopen_s(&pFile, inFileName, "rb);
#endif
	if(pFile == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイルオープン失敗(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));
		return false;
	}

	// OGGファイル情報取得
	OggVorbis_File   ovf;
	if(::ov_open(pFile, &ovf, NULL, 0) < 0) {
		::OutputDebugString(TEXT("*** Error - OGGファイル情報取得失敗(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));
		::fclose(pFile);
		return false;
	}

	// OGG情報取得
	vorbis_info*   vi = ov_info(&ovf, -1);
	if(vi == NULL) {
		::OutputDebugString(TEXT("*** Error - OGG情報取得失敗(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));
		::ov_clear(&ovf);
		return false;
	}

	// WAVEファイルヘッダーサイズ設定
	struct WAVEFILEHEADER {
		char         cRIFF[4];
		int          iSizeRIFF;
		char         cType[4];
		char         cFmt [4];
		int          iSizeFmt;
		WAVEFORMATEX WaveFmt;
		char         cData[4];
		int          iSizeData;
	} wh;
	const long   wh_size = sizeof(wh.cRIFF)    + sizeof(wh.iSizeRIFF) +
						   sizeof(wh.cType)    + sizeof(wh.cFmt)      +
						   sizeof(wh.iSizeFmt) + sizeof(wh.WaveFmt)   +
						   sizeof(wh.cData)    + sizeof(wh.iSizeData);

	// イメージサイズ設定
//	const long   data_size = static_cast<long>(::ceil(vi->channels * vi->rate   *
//													  ::ov_time_total(&ovf, -1) * sizeof(WORD)));
	const long   data_size = static_cast<long>(::ov_pcm_total(&ovf, -1)) * vi->channels * sizeof(WORD);

	// メモリ確保
	pWaveImage = new BYTE[wh_size + data_size];
	if(pWaveImage == NULL) {
		::OutputDebugString(TEXT("*** Error - メモリ確保失敗(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));
		::ov_clear(&ovf);
		return false;
	}

	// OGGデコード
	int    bitstream;
	long   read_size, write_size = 0;
	while(true) {
		// デコード
//		read_size = ov_read(&ovf, (char*)pWaveImage + wh_size + write_size,
//							data_size - write_size, 0, sizeof(WORD), 1, &bitstream);
		read_size = ov_read(&ovf, (char*)pWaveImage + wh_size + write_size,
							4096, 0, sizeof(WORD), 1, &bitstream);
		if(read_size == 0)
			break;	// デコード終了
		if(read_size < 0) {
			// デコード失敗
			::OutputDebugString(TEXT("*** Error - デコード失敗(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));

			delete[] pWaveImage;
			pWaveImage = NULL;
			::ov_clear(&ovf);

			return false;
		}

		write_size += read_size;
	}

	// ヘッダー初期化
	::CopyMemory(wh.cRIFF, "RIFF", 4);
	::CopyMemory(wh.cType, "WAVE", 4);
	::CopyMemory(wh.cFmt,  "fmt ", 4);
	::CopyMemory(wh.cData, "data", 4);

	wh.iSizeRIFF = wh_size + write_size - 8;
	wh.iSizeFmt  = sizeof(WAVEFORMATEX);
	wh.iSizeData = write_size;

	wh.WaveFmt.cbSize          = sizeof(WAVEFORMATEX);
	wh.WaveFmt.wFormatTag      = WAVE_FORMAT_PCM;
	wh.WaveFmt.nChannels       = vi->channels;
	wh.WaveFmt.nSamplesPerSec  = vi->rate;
	wh.WaveFmt.nAvgBytesPerSec = vi->rate * vi->channels * sizeof(WORD);
	wh.WaveFmt.nBlockAlign     = vi->channels * sizeof(WORD);
	wh.WaveFmt.wBitsPerSample  = sizeof(WORD) * 8;

	// 確保したメモリへ転送
	int  copy_size = 0;
	::CopyMemory(pWaveImage,              wh.cRIFF,     sizeof(wh.cRIFF    ));	copy_size += sizeof(wh.cRIFF    );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeRIFF, sizeof(wh.iSizeRIFF));	copy_size += sizeof(wh.iSizeRIFF);
	::CopyMemory(pWaveImage + copy_size,  wh.cType,     sizeof(wh.cType    ));	copy_size += sizeof(wh.cType    );
	::CopyMemory(pWaveImage + copy_size,  wh.cFmt,      sizeof(wh.cFmt     ));	copy_size += sizeof(wh.cFmt     );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeFmt,  sizeof(wh.iSizeFmt ));	copy_size += sizeof(wh.iSizeFmt );
	::CopyMemory(pWaveImage + copy_size, &wh.WaveFmt,   sizeof(wh.WaveFmt  ));	copy_size += sizeof(wh.WaveFmt  );
	::CopyMemory(pWaveImage + copy_size,  wh.cData,     sizeof(wh.cData    ));	copy_size += sizeof(wh.cData    );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeData, sizeof(wh.iSizeData));

	// OGGファイルクローズ
	::ov_clear(&ovf);
*/

	// OGGファイルデコード
	DWORD   image_size;
	if(DecodeOggFile(inFileName, pWaveImage, image_size) == false) {
		::OutputDebugString(TEXT("*** Error - OGGファイルデコード失敗(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));
		return false;
	}

	// セグメントパラメータ設定
	DMUS_OBJECTDESC  desc;
	::ZeroMemory(&desc, sizeof(desc));
	desc.dwSize      = sizeof(desc);
	desc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_MEMORY;
	desc.guidClass   = CLSID_DirectMusicSegment;
	desc.pbMemData   = pWaveImage;
	desc.llMemLength = image_size;

	// セグメント生成
	if(FAILED(m_pDMLoader->GetObject(&desc, IID_IDirectMusicSegment8, (LPVOID*)&pDMSegment))) {
		::OutputDebugString(TEXT("*** Error - セグメント生成失敗(CDXAudio8_CreateMusicSegmentFromOggFile)\n"));

		delete[] pWaveImage;
		pWaveImage = NULL;

		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	OGGファイルをメモリにデコード
//------------------------------------------------------------------------------
bool CDXAudio8::DecodeOggFile(LPTSTR inFileName, BYTE*& pWaveImage, DWORD& outImageSize)
{
	pWaveImage   = NULL;
	outImageSize = 0;

	// OGGファイルオープン
	FILE*       pFile = NULL;
#if UNICODE
	char        file_name[MAX_PATH + 1];
	const int   len = ::WideCharToMultiByte(CP_THREAD_ACP, 0, inFileName, -1, file_name, 0, NULL, NULL);
	::WideCharToMultiByte(CP_THREAD_ACP, 0, inFileName, -1, file_name, len, NULL, NULL);
	::fopen_s(&pFile, file_name,  "rb");
#else
	::fopen_s(&pFile, inFileName, "rb);
#endif
	if(pFile == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイルオープン失敗(CDXAudio8_DecodeOggFile)\n"));
		return false;
	}

	// OGGファイル情報取得
	OggVorbis_File   ovf;
	if(::ov_open(pFile, &ovf, NULL, 0) < 0) {
		::OutputDebugString(TEXT("*** Error - OGGファイル情報取得失敗(CDXAudio8_DecodeOggFile)\n"));
		::fclose(pFile);
		return false;
	}

	// OGG情報取得
	vorbis_info*   vi = ov_info(&ovf, -1);
	if(vi == NULL) {
		::OutputDebugString(TEXT("*** Error - OGG情報取得失敗(CDXAudio8_DecodeOggFile)\n"));
		::ov_clear(&ovf);
		return false;
	}

	// WAVEファイルヘッダーサイズ設定
	struct WAVEFILEHEADER {
		char         cRIFF[4];
		int          iSizeRIFF;
		char         cType[4];
		char         cFmt [4];
		int          iSizeFmt;
		WAVEFORMATEX WaveFmt;
		char         cData[4];
		int          iSizeData;
	} wh;
	const long   wh_size = sizeof(wh.cRIFF)    + sizeof(wh.iSizeRIFF) +
						   sizeof(wh.cType)    + sizeof(wh.cFmt)      +
						   sizeof(wh.iSizeFmt) + sizeof(wh.WaveFmt)   +
						   sizeof(wh.cData)    + sizeof(wh.iSizeData);

	// イメージサイズ設定
	const long   data_size = static_cast<long>(::ov_pcm_total(&ovf, -1)) * vi->channels * sizeof(WORD);

	// メモリ確保
	pWaveImage = new BYTE[wh_size + data_size];
	if(pWaveImage == NULL) {
		::OutputDebugString(TEXT("*** Error - メモリ確保失敗(CDXAudio8_DecodeOggFile)\n"));
		::ov_clear(&ovf);
		return false;
	}

	// OGGデコード
	int    bitstream;
	long   read_size, write_size = 0;
	while(true) {
		// デコード
		read_size = ov_read(&ovf, (char*)pWaveImage + wh_size + write_size,
							4096, 0, sizeof(WORD), 1, &bitstream);
		if(read_size == 0)
			break;	// デコード終了
		if(read_size < 0) {
			// デコード失敗
			::OutputDebugString(TEXT("*** Error - デコード失敗(CDXAudio8_DecodeOggFile)\n"));

			delete[] pWaveImage;
			pWaveImage = NULL;
			::ov_clear(&ovf);

			return false;
		}

		write_size += read_size;
	}

	// イメージサイズ設定
	outImageSize = wh_size + write_size;

	// ヘッダー初期化
	::CopyMemory(wh.cRIFF, "RIFF", 4);
	::CopyMemory(wh.cType, "WAVE", 4);
	::CopyMemory(wh.cFmt,  "fmt ", 4);
	::CopyMemory(wh.cData, "data", 4);

	wh.iSizeRIFF = outImageSize - 8;		// "RIFF"と"WAVE"の分を引く
	wh.iSizeFmt  = sizeof(WAVEFORMATEX);
	wh.iSizeData = write_size;

	wh.WaveFmt.cbSize          = sizeof(WAVEFORMATEX);
	wh.WaveFmt.wFormatTag      = WAVE_FORMAT_PCM;
	wh.WaveFmt.nChannels       = vi->channels;
	wh.WaveFmt.nSamplesPerSec  = vi->rate;
	wh.WaveFmt.nAvgBytesPerSec = vi->rate * vi->channels * sizeof(WORD);
	wh.WaveFmt.nBlockAlign     = vi->channels * static_cast<WORD>(sizeof(WORD));
	wh.WaveFmt.wBitsPerSample  = sizeof(WORD) * 8;

	// 確保したメモリへヘッダー情報を転送
	int  copy_size = 0;
	::CopyMemory(pWaveImage,              wh.cRIFF,     sizeof(wh.cRIFF    ));	copy_size += sizeof(wh.cRIFF    );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeRIFF, sizeof(wh.iSizeRIFF));	copy_size += sizeof(wh.iSizeRIFF);
	::CopyMemory(pWaveImage + copy_size,  wh.cType,     sizeof(wh.cType    ));	copy_size += sizeof(wh.cType    );
	::CopyMemory(pWaveImage + copy_size,  wh.cFmt,      sizeof(wh.cFmt     ));	copy_size += sizeof(wh.cFmt     );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeFmt,  sizeof(wh.iSizeFmt ));	copy_size += sizeof(wh.iSizeFmt );
	::CopyMemory(pWaveImage + copy_size, &wh.WaveFmt,   sizeof(wh.WaveFmt  ));	copy_size += sizeof(wh.WaveFmt  );
	::CopyMemory(pWaveImage + copy_size,  wh.cData,     sizeof(wh.cData    ));	copy_size += sizeof(wh.cData    );
	::CopyMemory(pWaveImage + copy_size, &wh.iSizeData, sizeof(wh.iSizeData));

	// OGGファイルクローズ
	::ov_clear(&ovf);

	return true;
}
#endif

//------------------------------------------------------------------------------
//	DirectSoundBuffer8インタフェース生成
//------------------------------------------------------------------------------
IDirectSoundBuffer8* CDXAudio8::CreateDirectSoundBufferFromFile(LPTSTR inFileName,
																const bool inShared, const bool in3DBuffer)
{
	IDirectSoundBuffer8*  pDSBuffer  = NULL;

	HMMIO                 hFile      = NULL;
	HACMSTREAM            hACMStream = NULL;	// ACMストリーム
	LPVOID                pFormat    = NULL;	// フォーマット
	LPVOID                pImage     = NULL;	// イメージ

	try {
		if(m_pDSound == NULL)
			throw TEXT("*** Error - DirectX Audio未初期化(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// ファイルオープン
		hFile = ::mmioOpen(inFileName, NULL, MMIO_READ | MMIO_ALLOCBUF);
		if(hFile == NULL)
			throw TEXT("*** Error - ファイルオープン失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		MMCKINFO   ParentChunk, ChildChunk;
		::ZeroMemory(&ParentChunk, sizeof(ParentChunk));
		::ZeroMemory(&ChildChunk,  sizeof(ChildChunk ));

		// 'WAVE'チャンクへ進入
		ParentChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if(::mmioDescend(hFile, &ParentChunk, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'WAVE'チャンクが見つかりません(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// 'WAVE'チャンクの'fmt 'チャンクへ進入
		ChildChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'fmt 'チャンクが見つかりません(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// フォーマット情報取得
		pFormat = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pFormat, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - フォーマット情報取得失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		::mmioAscend(hFile, &ChildChunk, 0);	// 'fmt 'チャンク退出

		// 'WAVE'チャンクの'data'チャンクへ進入
		ChildChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'data'チャンクが見つかりません(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// リニアPCMに変換
		WAVEFORMATEX   NewFormat;
		::ZeroMemory(&NewFormat, sizeof(NewFormat));

		// 推奨フォーマット取得
		NewFormat.wFormatTag = WAVE_FORMAT_PCM;
		if(::acmFormatSuggest(NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, sizeof(NewFormat),
							  ACM_FORMATSUGGESTF_WFORMATTAG) != 0)
			throw TEXT("*** Error - 推奨フォーマット取得失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// ACMストリームオープン
		if(::acmStreamOpen(&hACMStream, NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, NULL, 0, 0,
						   ACM_STREAMOPENF_NONREALTIME) != 0)
			throw TEXT("*** Error - ACMストリームオープン失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// フォーマット変換後取得
		DWORD   NewSize;
		if(::acmStreamSize(hACMStream, ChildChunk.cksize, &NewSize, ACM_STREAMSIZEF_SOURCE) != 0)
			throw TEXT("*** Error - デコードサイズ取得失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// 変換元イメージ読み込み
		pImage = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pImage, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - 変換元イメージ読み込み失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// セカンダリバッファ生成
		DSBUFFERDESC   dsbd;
		::ZeroMemory(&dsbd, sizeof(dsbd));
		dsbd.dwSize          = sizeof(dsbd);
		dsbd.dwBufferBytes   = NewSize;
		dsbd.dwReserved      = 0;
		dsbd.lpwfxFormat     = &NewFormat;
		dsbd.dwFlags         = DSBCAPS_LOCDEFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY |
							   DSBCAPS_GETCURRENTPOSITION2;
		if(inShared   == false) {
			dsbd.dwFlags        |= DSBCAPS_CTRLFX;
		}
		if(in3DBuffer == false) {
			dsbd.dwFlags        |= DSBCAPS_CTRLPAN;
			dsbd.guid3DAlgorithm = GUID_NULL;
		} else {
			dsbd.dwFlags        |= DSBCAPS_CTRL3D;
			dsbd.guid3DAlgorithm = DS3DALG_HRTF_LIGHT;
		}

		// DirectSoundBuffer生成
		IDirectSoundBuffer*   pdsb;
		if(FAILED(m_pDSound->CreateSoundBuffer(&dsbd, &pdsb, NULL)))
			throw TEXT("*** Error - DirectSoundBuffer生成失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// DirectSoundBuffer8取得
		const HRESULT   hr = pdsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&pDSBuffer);
		pdsb->Release();
		if(hr != S_OK)
			throw TEXT("*** Error - DirectSoundBuffer8取得失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// バッファロック
		LPVOID    pBuf[2];
		DWORD     BufBytes[2];
		if(pDSBuffer->Lock(0, 0, &pBuf[0], &BufBytes[0], &pBuf[1], &BufBytes[1], DSBLOCK_ENTIREBUFFER)
			!= DS_OK)
			throw TEXT("*** Error - バッファロック失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");

		// ACMストリームヘッダ設定
		ACMSTREAMHEADER   acmsh;
		::ZeroMemory(&acmsh, sizeof(acmsh));
		acmsh.cbStruct    = sizeof(acmsh);
		acmsh.fdwStatus   = 0;
		acmsh.pbSrc       = (LPBYTE)pImage;			// 変換元イメージ領域
		acmsh.cbSrcLength = ChildChunk.cksize;		// 変換元イメージ領域のサイズ
		acmsh.pbDst       = (LPBYTE)pBuf[0];		// 変換後イメージ領域
		acmsh.cbDstLength = BufBytes[0];			// 変換後イメージ領域のサイズ
		if(::acmStreamPrepareHeader(hACMStream, &acmsh, 0) != 0) {
			pDSBuffer->Unlock(pBuf[0], 0, pBuf[1], 0);
			throw TEXT("*** Error - ACMストリームヘッダ設定失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");
		}

		// 変換しながらバッファへ転送
		const MMRESULT   mmr = ::acmStreamConvert(hACMStream, &acmsh, 0);

		::acmStreamUnprepareHeader(hACMStream, &acmsh, 0);
		pDSBuffer->Unlock(pBuf[0], BufBytes[0], pBuf[1], 0);
		if(mmr != 0)
			throw TEXT("*** Error - イメージ変換失敗(CDXAudio8_CreateDirectSoundBufferFromFile)\n");
	} catch(LPCTSTR ErrorString) {
		::OutputDebugString(ErrorString);
		SafeRelease(pDSBuffer);
	}

	delete[] pImage;
	delete[] pFormat;
	if(hACMStream != NULL)	::acmStreamClose(hACMStream, 0);
	if(hFile      != NULL)	::mmioClose(hFile, 0);

	return pDSBuffer;
}

//------------------------------------------------------------------------------
//	リソースプロテクト設定
//------------------------------------------------------------------------------
void CDXAudio8::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// プロテクト設定
	} else {
		m_ProtectedResource.erase (pResource);		// プロテクト解除
	}
}
