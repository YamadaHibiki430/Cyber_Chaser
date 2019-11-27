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
	 DXAduio8.hpp
				DirectX Audioクラスヘッダ

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "MusicSegment.hpp"
#include "SoundBuffer.hpp"
#include "SoundListener.hpp"

#include <list>
#include <set>

//------------------------------------------------------------------------------
//	DirectX Audio8クラス定義
//------------------------------------------------------------------------------
class CDXAudio8 {
public:
	virtual ~CDXAudio8();

	// シングルトンインスタンスの取得
	static CDXAudio8& GetInstance() { static CDXAudio8 theDXAudio; return theDXAudio; }

	bool Initialize(const HWND hWnd);
	void Release();

	void Suspend();
	void Resume ()  { SetMasterVolume(m_SuspendPrimary); SetSegmentVolume(m_SuspendSegment); }

	LONG GetMasterVolume();
	void SetMasterVolume(LONG inVolume);

	int  GetSegmentVolume();
	void SetSegmentVolume(int inVolume);

	ISoundListener* GetListener() const { return m_pListener; }

	// サウンドバッファ
	ISoundBuffer* CreateBufferFromFile(LPTSTR inFileName, const bool in3DBuffer = false);
	DWORD CreateSharedBufferFromFile(LPTSTR inFileName,
									 ISoundBuffer* pSoundBufferArray[], const DWORD inArrayCount,
									 const bool in3DBuffer = false);

	void ReleaseBuffer(ISoundBuffer*& pBuffer);
	void ReleaseAllBuffers();

	// ミュージックセグメント
	IMusicSegment* CreateSegmentFromFile(LPTSTR inFileName);
	void ReleaseSegment(IMusicSegment*& pSegment);
	void ReleaseAllSegments();

	// 別名
	inline int  GetMusicVolume() { return GetSegmentVolume(); }
	inline void SetMusicVolume(int inVolume) { SetSegmentVolume(inVolume); }

	inline ISoundBuffer* CreateSoundFromFile(LPTSTR inFileName, const bool in3D = false)
	{ return CreateBufferFromFile(inFileName, in3D); }
	inline DWORD CreateSharedSoundFromFile(LPTSTR inFileName,
										   ISoundBuffer* pSBArray[], const DWORD inSBAryCnt,
										   const bool in3D)
	{ return CreateSharedBufferFromFile(inFileName, pSBArray, inSBAryCnt, in3D); }
	inline void ReleaseSound(ISoundBuffer*& pSound) { ReleaseBuffer(pSound); }
	inline void ReleaseAllSounds() { ReleaseAllBuffers(); }

	inline IMusicSegment* CreateMusicFromFile(LPTSTR inFileName)
	{ return CreateSegmentFromFile(inFileName); }
	inline void ReleaseMusic(IMusicSegment*& pMusic) { ReleaseSegment(pMusic); }
	inline void ReleaseAllMusics() { ReleaseAllSegments(); }

	// リソースプロテクト設定
	void SetResourceProtect(void* pResource, const bool inProtect);

private:
#ifdef DXA_OGG_OPT
	bool CreateMusicSegmentFromOggFile(LPTSTR inFileName,
									   IDirectMusicSegment8*& pDMSegment, BYTE*& pWaveImage);
	bool DecodeOggFile(LPTSTR inFileName, BYTE*& pWaveImage, DWORD& outImageSize);
#endif

	IDirectSoundBuffer8* CreateDirectSoundBufferFromFile(LPTSTR inFileName,
														 const bool inShared, const bool in3DBuffer);

	template <class T> inline void SafeRelease(T& x) { if(x != NULL) { x->Release(); x = NULL; }}

	// メンバ変数
	IDirectSound8*              m_pDSound;				// DirectSoundインタフェース
	IDirectSoundBuffer*         m_pDSBPrimary;			// プライマリバッファ
	ISoundListener*             m_pListener;			// リスナ
	std::list<ISoundBuffer*>    m_BufferList;			// バッファリスト

	IDirectMusicLoader8*        m_pDMLoader;			// ローダ
	IDirectMusicPerformance8*   m_pDMPerformance;		// パフォーマンス
	std::list<IMusicSegment*>   m_SegmentList;			// セグメントリスト

	std::set <void*>            m_ProtectedResource;	// リソースプロテクト設定

	LONG                        m_SuspendPrimary;		// サスペンド時プライマリボリューム
	int                         m_SuspendSegment;		// サスペンド時セグメントボリューム

	CDXAudio8();
	CDXAudio8(const CDXAudio8&);
	CDXAudio8& operator=(const CDXAudio8&);
};

//------------------------------------------------------------------------------
//	アクセス関数
//------------------------------------------------------------------------------
inline CDXAudio8& DXAudio() { return CDXAudio8::GetInstance(); }
