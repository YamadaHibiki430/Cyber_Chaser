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
	 NUI.hpp
				NUI�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#ifndef _INC_NUI
#define _INC_NUI

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Kinect.hpp"

#include <d3d9.h>

#include <list>
#include <set>

//------------------------------------------------------------------------------
//	NUI�N���X��`
//------------------------------------------------------------------------------
class CNUI {
public:
	virtual ~CNUI();

	// �V���O���g���C���X�^���X�̎擾
	static CNUI& GetInstance() { static CNUI theNUI; return theNUI; }

	bool Initialize(IDirect3DDevice9* pD3DDevice);
	void Release();

	inline bool Attach(IDirect3DDevice9* pD3DDevice)
	{ return Initialize(pD3DDevice); }

	IKinect* CreateKinect(const UINT  inIndex,
						  const DWORD inFlags =   NUI_INITIALIZE_FLAG_USES_COLOR
											    | NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX
											    | NUI_INITIALIZE_FLAG_USES_SKELETON
											  /*| NUI_INITIALIZE_FLAG_USES_AUDIO*/);
	void ReleaseKinect(IKinect*& pKinect);
	void ReleaseAllKinects();

	// ���\�[�X�v���e�N�g�ݒ�
	void SetResourceProtect(void* pResource, const bool inProtect);

private:
	IDirect3DDevice9*      m_pD3DDevice;

	std::list<IKinect*>    m_KinectList;
	std::set <void*   >    m_ProtectedResource;		// ���\�[�X�v���e�N�g�ݒ�

private:
	CNUI();

	template <class T> inline void SafeRelease(T& x) {if(x != NULL) { x->Release(); x = NULL; }}

	CNUI(const CNUI&);
	CNUI& operator=(const CNUI&);
};

//------------------------------------------------------------------------------
//	�C�����C���֐���`
//------------------------------------------------------------------------------
inline CNUI& Nui() { return CNUI::GetInstance(); }

#endif /* _INC_NUI */