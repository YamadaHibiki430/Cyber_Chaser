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
	 DShow.hpp
				DirectShow�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Media.hpp"

#include <d3d9.h>

#include <list>
#include <set>

//------------------------------------------------------------------------------
//	DirectShow�N���X��`
//------------------------------------------------------------------------------
class CDShow {
public:
	virtual ~CDShow();

	// �V���O���g���C���X�^���X�̎擾
	static CDShow& GetInstance() { static CDShow theDShow; return theDShow; }

	bool Initialize(IDirect3DDevice9* pD3DDevice);
	void Release();

	inline bool Attach(IDirect3DDevice9* pD3DDevice)
	{ return Initialize(pD3DDevice); }

	void Suspend();
	void Resume ();

	IMedia* CreateMediaFromFile(LPTSTR inFileName);

	void ReleaseMedia(IMedia*& pMedia);
	void ReleaseAllMedia();

	// ���\�[�X�v���e�N�g�ݒ�
	void SetResourceProtect(void* pResource, const bool inProtect);

private:
	CDShow();

	IDirect3DDevice9*    m_pD3DDevice;

	std::list<IMedia*>   m_MediaList;
	std::set <void*  >   m_ProtectedResource;	// ���\�[�X�v���e�N�g�ݒ�

	template <class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; } }

	CDShow(const CDShow&);
	CDShow& operator=(const CDShow&);
};

//------------------------------------------------------------------------------
//	�C�����C���֐���`
//------------------------------------------------------------------------------
inline CDShow& DShow() { return CDShow::GetInstance(); }
