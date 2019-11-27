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
				メディアクラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable:4996)

#include <streams.h>
#include <atlcomcli.h>

#pragma warning(pop)

#include <d3d9.h>

//------------------------------------------------------------------------------
//	GUID定義
//------------------------------------------------------------------------------
struct __declspec(uuid("{7A168DBA-32F8-4e07-86E9-76AA3802F83C}")) CLSID_D3DTexture9Renderer;

//------------------------------------------------------------------------------
//	Direct3DTexture9レンダラー定義
//------------------------------------------------------------------------------
class CD3DTexture9Renderer : public CBaseVideoRenderer {
public:
	CD3DTexture9Renderer(IDirect3DDevice9* pD3DDevice, HRESULT* pHR);
	virtual ~CD3DTexture9Renderer();

	virtual HRESULT CheckMediaType(const CMediaType* pMediaFormat);
	virtual HRESULT SetMediaType  (const CMediaType* pMediaFormat);
	virtual HRESULT DoRenderSample(IMediaSample* pMediaSample);
	virtual void OnReceiveFirstSample(IMediaSample* pMediaSample);

	IDirect3DTexture9* GetTexture() const { return m_pTexture[m_Primary]; }

	int GetWidth () const { return m_Width;  }
	int GetHeight() const { return m_Height; }

private:
	IDirect3DDevice9*    m_pD3DDevice;

	IDirect3DTexture9*   m_pTexture[2];
	D3DFORMAT            m_Format  [2];
	int                  m_Primary;

	int                  m_Width;
	int                  m_Height;
	int                  m_Pitch;

	template <class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; } }
};

//------------------------------------------------------------------------------
//	メディアインタフェース定義
//------------------------------------------------------------------------------
class IMedia {
public:
	virtual ~IMedia() {}

	virtual void Play  () = 0;
	virtual void Stop  () = 0;
	virtual void Pause () = 0;
	virtual void Replay() = 0;

	virtual OAFilterState GetState() = 0;
	virtual bool IsComplete() = 0;

	virtual int GetWidth () const = 0;
	virtual int GetHeight() const = 0;

	virtual LONGLONG GetLength    () = 0;
	virtual LONGLONG GetPosition  () = 0;
	virtual double   GetRate      () = 0;
	virtual GUID     GetTimeFormat() = 0;

	virtual long GetVolume () = 0;
	virtual long GetBalance() = 0;

	virtual void SetPosition  (LONGLONG     inPosition) = 0;
	virtual void SetRate      (const double inRate    ) = 0;
	virtual void SetTimeFormat(const GUID&  inFormat  ) = 0;

	virtual void SetRelativePosition(LONGLONG inPosition) = 0;

	virtual void SetVolume (long inVolume ) = 0;
	virtual void SetBalance(long inBalance) = 0;

	virtual void Suspend() = 0;
	virtual void Resume () = 0;

	// オペレータ
	virtual operator IDirect3DTexture9*() const = 0;
};

//------------------------------------------------------------------------------
//	メディアクラス定義
//------------------------------------------------------------------------------
class CMedia : public IMedia {
public:
	CMedia(IDirect3DDevice9* pD3Device);
	virtual ~CMedia();

	bool LoadFromFile(LPCTSTR inFileName);

	virtual void Play  () { m_pMediaControl->Run  (); }
	virtual void Stop  () { m_pMediaControl->Stop (); }
	virtual void Pause () { m_pMediaControl->Pause(); }
	virtual void Replay();

	virtual OAFilterState GetState();
	virtual bool IsComplete();

	virtual int GetWidth () const { return m_pD3DTxtRenderer->GetWidth (); }
	virtual int GetHeight() const { return m_pD3DTxtRenderer->GetHeight(); }

	virtual LONGLONG GetLength    ();
	virtual LONGLONG GetPosition  ();
	virtual double   GetRate      ();
	virtual GUID     GetTimeFormat();

	virtual long GetVolume ();
	virtual long GetBalance();

	virtual void SetPosition  (LONGLONG     inPosition);
	virtual void SetRate      (const double inRate    );
	virtual void SetTimeFormat(const GUID&  inFormat  );

	virtual void SetRelativePosition(LONGLONG inPosition);

	virtual void SetVolume (long inVolume );
	virtual void SetBalance(long inBalance);

	virtual void Suspend();
	virtual void Resume ();

	virtual operator IDirect3DTexture9*() const
	{ return m_pD3DTxtRenderer->GetTexture(); }

private:
	void Release();

	template <class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x.Release(); x = NULL; } }

private:
	IDirect3DDevice9*        m_pD3DDevice;

	CComPtr<IGraphBuilder>   m_pGraphBuilder;

	CD3DTexture9Renderer*    m_pD3DTxtRenderer;
	CComPtr<IBaseFilter  >   m_pVideoRenderer;
	CComPtr<IBaseFilter  >   m_pSoundRenderer;

	CComPtr<IMediaControl>   m_pMediaControl;
	CComPtr<IMediaSeeking>   m_pMediaSeeking;

	CComPtr<IBasicAudio>     m_pBasicAudio;

	bool                     m_Suspend;
};

//------------------------------------------------------------------------------
//	NULLメディアクラス定義
//------------------------------------------------------------------------------
class CNullMedia : public IMedia {
public:
	CNullMedia() {}
	virtual ~CNullMedia() {}

	virtual void Play  () {}
	virtual void Stop  () {}
	virtual void Pause () {}
	virtual void Replay() {}

	virtual OAFilterState GetState() { return State_Stopped; }
	virtual bool IsComplete()        { return false; }

	virtual int GetWidth () const    { return 0;   }
	virtual int GetHeight() const    { return 0;   }

	virtual LONGLONG GetLength    () { return 0;   }
	virtual LONGLONG GetPosition  () { return 0;   }
	virtual double   GetRate      () { return 0.0; }
	virtual GUID     GetTimeFormat() { return TIME_FORMAT_NONE; }

	virtual long GetVolume () { return 0; }
	virtual long GetBalance() { return 0; }

	virtual void SetPosition  (LONGLONG     inPosition) {}
	virtual void SetRate      (const double inRate    ) {}
	virtual void SetTimeFormat(const GUID&  inFormat  ) {}

	virtual void SetRelativePosition(LONGLONG inPosition) {}

	virtual void SetVolume (long inVolume ) {}
	virtual void SetBalance(long inBalance) {}

	virtual void Suspend() {}
	virtual void Resume () {}

	virtual operator IDirect3DTexture9*() const { return NULL; }
};
