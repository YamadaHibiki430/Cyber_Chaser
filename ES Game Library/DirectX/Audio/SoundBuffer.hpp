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
	 SoundBuffer.hpp
				サウンドバッファクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Sound3DBuffer.hpp"

//------------------------------------------------------------------------------
//	エフェクトフラグ
//------------------------------------------------------------------------------
enum DXAFX {
	DXAFX_CHORUS      = 1 << 0,		// コーラス
	DXAFX_COMPRESSOR  = 1 << 1,		// コンプレッション
	DXAFX_DISTORTION  = 1 << 2,		// ディストーション
	DXAFX_ECHO        = 1 << 3,		// エコー
	DXAFX_FLANGER     = 1 << 4,		// フランジ
	DXAFX_GARGLE      = 1 << 5,		// ガーグル
	DXAFX_ENVREVERB   = 1 << 6,		// 環境リバーブ
	DXAFX_PARAMEQ     = 1 << 7,		// パラメトリックイコライザ
	DXAFX_WAVESREVERB = 1 << 8,		// ミュージックリバーブ
	DXAFX_MAX         = 9
};

//------------------------------------------------------------------------------
//	サウンドバッファインタフェース定義
//------------------------------------------------------------------------------
class ISoundBuffer {
public:
	virtual ~ISoundBuffer() {}

	virtual bool Play(const DWORD inPriority = 0, const DWORD inFlags = 0) = 0;
	virtual bool PlayLooping() = 0;
	virtual void Stop() = 0;

	virtual void SetVolume(LONG inVolume) = 0;
	virtual void SetPan(LONG inPan) = 0;
	virtual void SetFrequency(DWORD inFrequency) = 0;
	virtual bool SetFX(const DWORD inFXFlags) = 0;

	inline void SetVolume(const float inVolume)
	{ SetVolume(DSBVOLUME_MAX - LONG( (DSBVOLUME_MAX - DSBVOLUME_MIN) * (1.0f - inVolume) )); }

	inline void SetPan(const float inPan)
	{ SetPan( LONG(DSBPAN_RIGHT * inPan) ); }

	virtual LONG  GetVolume() = 0;
	virtual LONG  GetPan() = 0;
	virtual DWORD GetFrequency() = 0;
	virtual DWORD GetState() = 0;

	virtual bool IsPlaying() = 0;
	virtual bool IsLooping() = 0;

	virtual ISound3DBuffer* GetSound3D() = 0;
};

//------------------------------------------------------------------------------
//	サウンドバッファクラス定義
//------------------------------------------------------------------------------
class CSoundBuffer : public ISoundBuffer {
public:
	CSoundBuffer(IDirectSoundBuffer8* pDSBuffer, LPCTSTR inFileName);
	virtual ~CSoundBuffer() { delete m_p3DBuffer; m_pDSBuffer->Release(); }

	virtual bool Play(const DWORD inPriority, const DWORD inFlags);
	virtual bool PlayLooping() { return Play(0, DSBPLAY_LOOPING); }
	virtual void Stop() { m_pDSBuffer->Stop(); }

	virtual void SetVolume   (LONG  inVolume)    { m_pDSBuffer->SetVolume(inVolume); }
	virtual void SetPan      (LONG  inPan)       { m_pDSBuffer->SetPan(inPan); }
	virtual void SetFrequency(DWORD inFrequency) { m_pDSBuffer->SetFrequency(inFrequency); }
	virtual bool SetFX       (const DWORD inFXFlags);

	virtual LONG  GetVolume();
	virtual LONG  GetPan();
	virtual DWORD GetFrequency();
	virtual DWORD GetState();

	virtual bool IsPlaying() { return (GetState() & DSBSTATUS_PLAYING) != 0; }
	virtual bool IsLooping() { return (GetState() & DSBSTATUS_LOOPING) != 0; }

	virtual ISound3DBuffer* GetSound3D() { return m_p3DBuffer; }

private:
	void Restore();

	IDirectSoundBuffer8*   m_pDSBuffer;
	ISound3DBuffer*        m_p3DBuffer;

	TCHAR                  m_FileName[MAX_PATH + 1];
};

//------------------------------------------------------------------------------
//	NULLサウンドバッファクラス定義
//------------------------------------------------------------------------------
class CNullSoundBuffer : public ISoundBuffer {
public:
	CNullSoundBuffer() {}
	virtual ~CNullSoundBuffer() {}

	virtual bool Play(const DWORD inPriority, const DWORD inFlags) { return false; }
	virtual bool PlayLooping() { return Play(0, DSBPLAY_LOOPING); }
	virtual void Stop() {}

	virtual void SetVolume(LONG inVolume) {}
	virtual void SetPan(LONG inPan) {}
	virtual void SetFrequency(DWORD inFrequency) {}
	virtual bool SetFX(const DWORD inFXFlags) { return false; }

	virtual LONG  GetVolume()    { return DSBVOLUME_MIN; }
	virtual LONG  GetPan()       { return DSBPAN_CENTER; }
	virtual DWORD GetFrequency() { return 0; }
	virtual DWORD GetState()     { return 0; }

	virtual bool IsPlaying() { return false; }
	virtual bool IsLooping() { return false; }

	virtual ISound3DBuffer* GetSound3D() { return &m_3DBuffer; }

private:
	CNullSound3DBuffer   m_3DBuffer;
};
