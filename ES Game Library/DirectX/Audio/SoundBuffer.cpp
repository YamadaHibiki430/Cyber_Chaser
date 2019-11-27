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
	 SoundBuffer.cpp
				サウンドバッファクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "SoundBuffer.hpp"
#include <mmreg.h>
#include <msacm.h>
#include <cassert>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "msacm32.lib")

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CSoundBuffer::CSoundBuffer(IDirectSoundBuffer8* pDSBuffer, LPCTSTR inFileName)
{
	assert(pDSBuffer != NULL);

	m_pDSBuffer = pDSBuffer;
	m_pDSBuffer->AddRef();		// 参照カウンタインクリメント

	::ZeroMemory(m_FileName, sizeof(m_FileName));
	if(inFileName != NULL)
		::lstrcpy(m_FileName, inFileName);

	// ３Ｄバッファ設定
	IDirectSound3DBuffer8*   pDS3DBuffer;
	if(m_pDSBuffer->QueryInterface(IID_IDirectSound3DBuffer8, (LPVOID*)&pDS3DBuffer) == S_OK) {
		m_p3DBuffer = new CSound3DBuffer(pDS3DBuffer);
		pDS3DBuffer->Release();
	} else {
		m_p3DBuffer = new CNullSound3DBuffer();
	}
}

//------------------------------------------------------------------------------
//	再生
//------------------------------------------------------------------------------
bool CSoundBuffer::Play(const DWORD inPriority, const DWORD inFlags)
{
	m_pDSBuffer->SetCurrentPosition(0);

	// 再生
	HRESULT   hr = m_pDSBuffer->Play(0, inPriority, inFlags);
	if(hr == DS_OK)
		return true;
	if(hr != DSERR_BUFFERLOST) {
		::OutputDebugString(TEXT("*** Error - バッファ再生失敗(CSoundBuffer_Play)\n"));
		return false;
	}

	// バッファを復元し、再度再生を試みる
	Restore();
	if(m_pDSBuffer->Play(0, inPriority, inFlags) != DS_OK) {
		::OutputDebugString(TEXT("*** Error - バッファ再生失敗(CSoundBuffer_Play)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	エフェクト設定
//------------------------------------------------------------------------------
bool CSoundBuffer::SetFX(const DWORD inFXFlags)
{
	Stop();

	// エフェクト削除
	m_pDSBuffer->SetFX(0, NULL, NULL);
	if(inFXFlags == 0)
		return true;

	// エフェクト設定
	int            FXCnt = 0;
	DSEFFECTDESC   EffectDesc[DXAFX_MAX];
	::ZeroMemory(EffectDesc, sizeof(EffectDesc));
	for(int i = 0; i < DXAFX_MAX; i++) {
		EffectDesc[i].dwSize        = sizeof(DSEFFECTDESC);
		EffectDesc[i].guidDSFXClass = GUID_NULL;
	}

	// コーラス
	if((inFXFlags & DXAFX_CHORUS) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_CHORUS;
		FXCnt++;
	}
	// コンプレッション
	if((inFXFlags & DXAFX_COMPRESSOR) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_COMPRESSOR;
		FXCnt++;
	}
	// ディストーション
	if((inFXFlags & DXAFX_DISTORTION) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_DISTORTION;
		FXCnt++;
	}
	// エコー
	if((inFXFlags & DXAFX_ECHO) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_ECHO;
		FXCnt++;
	}
	// フランジ
	if((inFXFlags & DXAFX_FLANGER) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_FLANGER;
		FXCnt++;
	}
	// ガーグル
	if((inFXFlags & DXAFX_GARGLE) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_GARGLE;
		FXCnt++;
	}
	// 環境リバーブ
	if((inFXFlags & DXAFX_ENVREVERB) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_I3DL2REVERB;
		FXCnt++;
	}
	// パラメトリックイコライザ
	if((inFXFlags & DXAFX_PARAMEQ) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_STANDARD_PARAMEQ;
		FXCnt++;
	}
	// ミュージックリバーブ
	if((inFXFlags & DXAFX_WAVESREVERB) != 0) {
		EffectDesc[FXCnt].guidDSFXClass = GUID_DSFX_WAVES_REVERB;
		FXCnt++;
	}

	if(FAILED(m_pDSBuffer->SetFX(FXCnt, EffectDesc, NULL))) {
		::OutputDebugString(TEXT("*** Error - エフェクト設定失敗(CSoundBuffer_SetFX)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	ボリューム取得
//------------------------------------------------------------------------------
LONG CSoundBuffer::GetVolume()
{
	LONG   volume;
	if(m_pDSBuffer->GetVolume(&volume) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ボリューム取得失敗(CSoundBuffer_GetVolume)\n"));
		return DSBVOLUME_MAX;
	}

	return volume;
}

//------------------------------------------------------------------------------
//	パン取得
//------------------------------------------------------------------------------
LONG CSoundBuffer::GetPan()
{
	LONG   pan;
	if(m_pDSBuffer->GetPan(&pan) != S_OK) {
		::OutputDebugString(TEXT("*** Error - パン取得失敗(CSoundBuffer_GetPan)\n"));
		return DSBPAN_CENTER;
	}

	return pan;
}

//------------------------------------------------------------------------------
//	周波数取得
//------------------------------------------------------------------------------
DWORD CSoundBuffer::GetFrequency()
{
	DWORD   freq;
	if(m_pDSBuffer->GetFrequency(&freq) != S_OK) {
		::OutputDebugString(TEXT("*** Error - 周波数取得失敗(CSoundBuffer_GetFrequency)\n"));
		return 0;
	}

	return freq;
}

//------------------------------------------------------------------------------
//	ステータス取得
//------------------------------------------------------------------------------
DWORD CSoundBuffer::GetState()
{
	DWORD   status;
	if(m_pDSBuffer->GetStatus(&status) != S_OK) {
		::OutputDebugString(TEXT("*** Error - ステータス取得失敗(CSoundBuffer_GetState)\n"));
		return 0;
	}

	return status;
}

//------------------------------------------------------------------------------
//	バッファ復元
//------------------------------------------------------------------------------
void CSoundBuffer::Restore()
{
	// バッファ復元
	if(m_pDSBuffer->Restore() != DS_OK) {
		::OutputDebugString(TEXT("*** Error - ステータス取得失敗(CSoundBuffer_Restore)\n"));
		return;
	}

	// 共有メモリは復元しない
	if(m_FileName[0] == '\0')
		return;

	// イメージ再読込
	HMMIO                 hFile      = NULL;
	HACMSTREAM            hACMStream = NULL;	// ACMストリーム
	LPVOID                pFormat    = NULL;	// フォーマット
	LPVOID                pImage     = NULL;	// イメージ

	try {
		// ファイルオープン
		hFile = ::mmioOpen(m_FileName, NULL, MMIO_READ | MMIO_ALLOCBUF);
		if(hFile == NULL)
			throw TEXT("*** Error - ファイルオープン失敗(CSoundBuffer_Restore)\n");

		MMCKINFO   ParentChunk, ChildChunk;
		::ZeroMemory(&ParentChunk, sizeof(ParentChunk));
		::ZeroMemory(&ChildChunk,  sizeof(ChildChunk ));

		// 'WAVE'チャンクへ進入
		ParentChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		if(::mmioDescend(hFile, &ParentChunk, NULL, MMIO_FINDRIFF) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'WAVE'チャンクが見つかりません(CSoundBuffer_Restore)\n");

		// 'WAVE'チャンクの'fmt 'チャンクへ進入
		ChildChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'fmt 'チャンクが見つかりません(CSoundBuffer_Restore)\n");

		// フォーマット情報取得
		pFormat = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pFormat, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - フォーマット情報取得失敗(CSoundBuffer_Restore)\n");

		::mmioAscend(hFile, &ChildChunk, 0);	// 'fmt 'チャンク退出

		// 'WAVE'チャンクの'data'チャンクへ進入
		ChildChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
		if(::mmioDescend(hFile, &ChildChunk, &ParentChunk, MMIO_FINDCHUNK) != MMSYSERR_NOERROR)
			throw TEXT("*** Error - 'data'チャンクが見つかりません(CSoundBuffer_Restore)\n");

		// リニアPCMに変換
		WAVEFORMATEX   NewFormat;
		::ZeroMemory(&NewFormat, sizeof(NewFormat));

		// 推奨フォーマット取得
		NewFormat.wFormatTag = WAVE_FORMAT_PCM;
		if(::acmFormatSuggest(NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, sizeof(NewFormat),
							  ACM_FORMATSUGGESTF_WFORMATTAG) != 0)
			throw TEXT("*** Error - 推奨フォーマット取得失敗(CSoundBuffer_Restore)\n");

		// ACMストリームオープン
		if(::acmStreamOpen(&hACMStream, NULL, (LPWAVEFORMATEX)pFormat, &NewFormat, NULL, 0, 0,
						   ACM_STREAMOPENF_NONREALTIME) != 0)
			throw TEXT("*** Error - ACMストリームオープン失敗(CSoundBuffer_Restore)\n");

		// フォーマット変換後取得
		DWORD   NewSize;
		if(::acmStreamSize(hACMStream, ChildChunk.cksize, &NewSize, ACM_STREAMSIZEF_SOURCE) != 0)
			throw TEXT("*** Error - デコードサイズ取得失敗(CSoundBuffer_Restore)\n");

		// 変換元イメージ読み込み
		pImage = new BYTE[ChildChunk.cksize];
		if((DWORD)::mmioRead(hFile, (HPSTR)pImage, ChildChunk.cksize) != ChildChunk.cksize)
			throw TEXT("*** Error - 変換元イメージ読み込み失敗(CSoundBuffer_Restore)\n");

		// バッファのロック
		LPVOID    pBuf[2];
		DWORD     BufBytes[2];
		if(m_pDSBuffer->Lock(0, 0, &pBuf[0], &BufBytes[0], &pBuf[1], &BufBytes[1], DSBLOCK_ENTIREBUFFER)
		   != DS_OK)
			throw TEXT("*** Error - バッファロック失敗(CSoundBuffer_Restore)\n");

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
			m_pDSBuffer->Unlock(pBuf[0], 0, pBuf[1], 0);
			throw TEXT("*** Error - ACMストリームヘッダ設定失敗(CSoundBuffer_Restore)\n");
		}

		// 変換しながらバッファへ転送
		const MMRESULT   mmr = ::acmStreamConvert(hACMStream, &acmsh, 0);

		::acmStreamUnprepareHeader(hACMStream, &acmsh, 0);
		m_pDSBuffer->Unlock(pBuf[0], BufBytes[0], pBuf[1], 0);
		if(mmr != 0)
			throw TEXT("*** Error - イメージ変換失敗(CSoundBuffer_Restore)\n");
	} catch(LPCTSTR ErrorString) {
		::OutputDebugString(ErrorString);
	}

	delete[] pImage;
	delete[] pFormat;
	if(hACMStream != NULL)	::acmStreamClose(hACMStream, 0);;
	if(hFile      != NULL)	::mmioClose(hFile, 0);
}
