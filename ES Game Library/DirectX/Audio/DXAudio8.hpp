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
	 DXAduio8.hpp
				DirectX Audio�N���X�w�b�_

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "MusicSegment.hpp"
#include "SoundBuffer.hpp"
#include "SoundListener.hpp"

#include <list>
#include <set>

//------------------------------------------------------------------------------
//	DirectX Audio8�N���X��`
//------------------------------------------------------------------------------
class CDXAudio8 {
public:
	virtual ~CDXAudio8();

	// �V���O���g���C���X�^���X�̎擾
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

	// �T�E���h�o�b�t�@
	ISoundBuffer* CreateBufferFromFile(LPTSTR inFileName, const bool in3DBuffer = false);
	DWORD CreateSharedBufferFromFile(LPTSTR inFileName,
									 ISoundBuffer* pSoundBufferArray[], const DWORD inArrayCount,
									 const bool in3DBuffer = false);

	void ReleaseBuffer(ISoundBuffer*& pBuffer);
	void ReleaseAllBuffers();

	// �~���[�W�b�N�Z�O�����g
	IMusicSegment* CreateSegmentFromFile(LPTSTR inFileName);
	void ReleaseSegment(IMusicSegment*& pSegment);
	void ReleaseAllSegments();

	// �ʖ�
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

	// ���\�[�X�v���e�N�g�ݒ�
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

	// �����o�ϐ�
	IDirectSound8*              m_pDSound;				// DirectSound�C���^�t�F�[�X
	IDirectSoundBuffer*         m_pDSBPrimary;			// �v���C�}���o�b�t�@
	ISoundListener*             m_pListener;			// ���X�i
	std::list<ISoundBuffer*>    m_BufferList;			// �o�b�t�@���X�g

	IDirectMusicLoader8*        m_pDMLoader;			// ���[�_
	IDirectMusicPerformance8*   m_pDMPerformance;		// �p�t�H�[�}���X
	std::list<IMusicSegment*>   m_SegmentList;			// �Z�O�����g���X�g

	std::set <void*>            m_ProtectedResource;	// ���\�[�X�v���e�N�g�ݒ�

	LONG                        m_SuspendPrimary;		// �T�X�y���h���v���C�}���{�����[��
	int                         m_SuspendSegment;		// �T�X�y���h���Z�O�����g�{�����[��

	CDXAudio8();
	CDXAudio8(const CDXAudio8&);
	CDXAudio8& operator=(const CDXAudio8&);
};

//------------------------------------------------------------------------------
//	�A�N�Z�X�֐�
//------------------------------------------------------------------------------
inline CDXAudio8& DXAudio() { return CDXAudio8::GetInstance(); }
