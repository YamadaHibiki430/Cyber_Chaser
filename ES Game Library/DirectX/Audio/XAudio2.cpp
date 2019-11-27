/*
================================================================================
					 オブジェクト指向＆＆ゲームプログラミング
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 【対象ＯＳ】
     Microsoft WindowsXP以降

 【コンパイラ】
     Microsoft VisualC++ 2008

 【プログラム】
	 XAudio2.cpp
				XAudio2クラス

 【バージョン】
	 * Version    2014.03.00      2014/03/14  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "XAudio2.hpp"
#include "../Common/ComLibrary.hpp"

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#ifdef NDEBUG
#pragma comment(lib, "XAPOBase.lib")
#else
#pragma comment(lib, "XAPOBased.lib")
#endif

//------------------------------------------------------------------------------
//	デフォルトコンストラクタ
//------------------------------------------------------------------------------
CXAudio2::CXAudio2() : m_pXAudio(NULL), m_pMasterVoice(NULL)
{
	CComLibrary::Initialize();		// COMライブラリ初期化
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CXAudio2::~CXAudio2()
{
	Release();
	CComLibrary::Uninitialize();	// COMライブラリ解放
}

//------------------------------------------------------------------------------
//	初期化
//------------------------------------------------------------------------------
bool CXAudio2::Initialize()
{
	Release();

	// XAudio2インタフェース生成
#ifdef NDEBUG
	if(::XAudio2Create(&m_pXAudio) != S_OK) {
#else
	if(::XAudio2Create(&m_pXAudio, XAUDIO2_DEBUG_ENGINE) != S_OK) {
		::OutputDebugString(TEXT("*** Error - XAudio2インタフェース生成(CXAudio2_Initialize)\n"));
#endif
		return false;
	}

	// マスターボイス生成
	if(m_pXAudio->CreateMasteringVoice(&m_pMasterVoice) != S_OK) {
		::OutputDebugString(TEXT("*** Error - マスターボイス生成(CXAudio2_Initialize)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	解放
//------------------------------------------------------------------------------
void CXAudio2::Release()
{
	// マスターボイス解放
	if(m_pMasterVoice != NULL) {
		m_pMasterVoice->DestroyVoice();
		m_pMasterVoice = NULL;
	}

	SafeRelease(m_pXAudio);
}

//------------------------------------------------------------------------------
//	マスターボリューム設定
//------------------------------------------------------------------------------
void CXAudio2::SetMasterVolume(const float inVolume)
{
	if(m_pMasterVoice == NULL) {
		::OutputDebugString(TEXT("*** Error - マスターボイス未初期化(CXAudio2_SetMasterVolume)\n"));
		return;
	}

	m_pMasterVoice->SetVolume(inVolume);
}

//------------------------------------------------------------------------------
//	マスターボリューム取得
//------------------------------------------------------------------------------
float CXAudio2::GetMasterVolume() const
{
	if(m_pMasterVoice == NULL) {
		::OutputDebugString(TEXT("*** Error - マスターボイス未初期化(CXAudio2_GetMasterVolume)\n"));
		return 0.0f;
	}

	float   volume = 0.0f;
	m_pMasterVoice->GetVolume(&volume);

	return volume;
}

//------------------------------------------------------------------------------
//	サウンドボイス生成
//------------------------------------------------------------------------------
ISoundVoice* CXAudio2::CreateVoiceFromFile(LPTSTR inFileName)
{
	HANDLE   hFile    = INVALID_HANDLE_VALUE;
	HANDLE   hMapFile = NULL;
	void*    pMemFile = NULL;
	try {
		if(m_pXAudio == NULL)
			throw TEXT("*** Error - XAudio2未初期化(CXAudio2_CreateVoiceFromFile)\n");

		// ファイルオープン
		hFile = ::CreateFile(inFileName, GENERIC_READ,
							 FILE_SHARE_READ, NULL, OPEN_EXISTING,
							 FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			throw TEXT("*** Error - ファイルオープン失敗(CXAudio2_CreateVoiceFromFile)\n");

		// ファイルマッピングオブジェクト生成
		hMapFile = ::CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if(hMapFile == NULL)
			throw TEXT("*** Error - ファイルマッピングオブジェクト生成失敗(CXAudio2_CreateVoiceFromFile)\n");

		// ファイルマッピング
		pMemFile = ::MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
		if(pMemFile == NULL)
			throw TEXT("*** Error - ファイルマッピング失敗(CXAudio2_CreateVoiceFromFile)\n");

		// WAVEファイル解析
		BYTE*   pWave = (BYTE*)pMemFile;

		// RIFF
		if(::memcmp(pWave, "RIFF", 4) != 0)
			throw TEXT("*** Error - RIFFヘッダーエラー(CXAudio2_CreateVoiceFromFile)\n");

		// データサイズ
		const DWORD   WAVE_SIZE = *(DWORD*)pWave;
		pWave += 4;

		// WAVE
		if(::memcmp(pWave, "WAVE", 4) != 0)
			throw TEXT("*** Error - WAVEチャンクエラー(CXAudio2_CreateVoiceFromFile)\n");

		// サブチャンク
	} catch(LPCTSTR err_str) {
		::OutputDebugString(err_str);

		// NULL
	}

	if(pMemFile != NULL)
		::UnmapViewOfFile(pMemFile);
	if(hMapFile != NULL)
		::CloseHandle(hMapFile);
	if(hFile != INVALID_HANDLE_VALUE)
		::CloseHandle(hMapFile);

	return NULL;
}

/*
//------------------------------------------------------------------------------
//	サウンドボイス生成
//------------------------------------------------------------------------------
ISoundVoice* CXAudio2::CreateVoice(WAVEFORMATEX& inFormat)
{
	IXAudio2SourceVoice*   pSrcVoice;
	const HRESULT   hr = m_pXAudio->CreateSourceVoice(&pSrcVoice, &inFormat);

	ISourceVoice*   pVoice;
	if(hr == S_OK) {
		pVoice = new CSourceVoice(pSrcVoice);
		pSrcVoice->DestroyVoice();
	} else {
		::OutputDebugString(TEXT("*** Error - ソースボイス生成失敗(CXAudio2_CreateVoice)\n"));
		pVoice = new CNullSourceVoice();
	}

	// ボイスリストへ追加
	;

	return pVoice;
}

//------------------------------------------------------------------------------
//	サウンドボイス生成
//------------------------------------------------------------------------------
ISoundVoice* CXAudio2::CreateVoice(const int inSamplesPerSec,
									const int inBitsPerSample,
									const int inChannels)
{
	WAVEFORMATEX   wav_fmt;
	wav_fmt.wFormatTag      = WAVE_FORMAT_PCM;
	wav_fmt.nChannels       = inChannels;
	wav_fmt.nSamplesPerSec  = inSamplesPerSec;
	wav_fmt.wBitsPerSample  = inBitsPerSample;
	wav_fmt.nBlockAlign     = (wav_fmt.wBitsPerSample * wav_fmt.nChannels) / 8;
	wav_fmt.nAvgBytesPerSec =  wav_fmt.nSamplesPerSec * wav_fmt.nBlockAlign;
	wav_fmt.cbSize          = 0;

	return CreateVoice(wav_fmt);
}
*/