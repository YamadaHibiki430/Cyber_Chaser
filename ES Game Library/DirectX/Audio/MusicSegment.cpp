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
	 MusicSegment.cpp
				�~���[�W�b�N�Z�O�����g�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "MusicSegment.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CMusicSegment::CMusicSegment(IDirectMusicLoader8*      pDMLoader,
							 IDirectMusicPerformance8* pDMPerformance,
							 IDirectMusicSegment8*     pDMSegment,
							 BYTE*                     pWaveData)
{
	assert(pDMLoader != NULL && pDMPerformance != NULL && pDMSegment != NULL);

	m_pDMLoader      = pDMLoader;		m_pDMLoader     ->AddRef();		// �Q�ƃJ�E���^�C���N�������g
	m_pDMPerformance = pDMPerformance;	m_pDMPerformance->AddRef();
	m_pDMSegment     = pDMSegment;		m_pDMSegment    ->AddRef();
	m_pWaveData      = pWaveData;
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CMusicSegment::~CMusicSegment()
{
	m_pDMPerformance->StopEx(m_pDMSegment, 0, 0);		// ��~
	m_pDMSegment    ->Unload(m_pDMPerformance);			// �A�����[�h

	m_pDMLoader->ReleaseObjectByUnknown(m_pDMSegment);	// �L���b�V���폜
	m_pDMLoader->CollectGarbage();						// �N���[���A�b�v

	m_pDMSegment    ->Release();						// �Z�O�����g���
	m_pDMPerformance->Release();						// �p�t�H�[�}���X�Q�Ɖ���
	m_pDMLoader     ->Release();						// ���[�_�Q�Ɖ���

	delete[] m_pWaveData;
}

//------------------------------------------------------------------------------
//	�Đ�
//------------------------------------------------------------------------------
bool CMusicSegment::Play(const DWORD inFlag, const int inLoop)
{
	Stop();

	// ���[�v�ݒ�
	if(inLoop >= 0)
		m_pDMSegment->SetRepeats(inLoop);
	else
		m_pDMSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);

	// �Đ�
	if(m_pDMPerformance->PlaySegmentEx(m_pDMSegment, NULL, NULL, inFlag, 0, NULL, NULL, NULL) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �Đ����s(CMusicSegment_Play)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�Đ��������ׂ�
//------------------------------------------------------------------------------
bool CMusicSegment::IsPlaying()
{
	if(m_pDMPerformance->IsPlaying(m_pDMSegment, NULL) != S_OK)
		return false;

	return true;
}
