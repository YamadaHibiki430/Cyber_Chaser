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
	 DShow.cpp
				DirectShow�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "DShow.hpp"
#include "../Common/ComLibrary.hpp"

#include <cassert>

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CDShow::CDShow() : m_pD3DDevice(NULL)
{
	CComLibrary::Initialize();		// COM���C�u����������
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CDShow::~CDShow()
{
	Release();
	CComLibrary::Uninitialize();	// COM���C�u�������
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CDShow::Initialize(IDirect3DDevice9* pD3DDevice)
{
	assert(pD3DDevice != NULL);

	if(m_pD3DDevice == pD3DDevice)
		return true;

	Release();
	m_pD3DDevice = pD3DDevice;
	m_pD3DDevice->AddRef();

	return true;
}

//------------------------------------------------------------------------------
//	���
//------------------------------------------------------------------------------
void CDShow::Release()
{
	m_ProtectedResource.clear();
	ReleaseAllMedia();
	SafeRelease(m_pD3DDevice);
}

//------------------------------------------------------------------------------
//	�T�X�y���h����
//------------------------------------------------------------------------------
void CDShow::Suspend()
{
	if(m_pD3DDevice == NULL)
		return;

	for(std::list<IMedia*>::iterator it = m_MediaList.begin(); it != m_MediaList.end(); it++)
		(*it)->Suspend();
}

//------------------------------------------------------------------------------
//	���W���[������
//------------------------------------------------------------------------------
void CDShow::Resume()
{
	for(std::list<IMedia*>::iterator it = m_MediaList.begin(); it != m_MediaList.end(); it++)
		(*it)->Resume();
}

//------------------------------------------------------------------------------
//	���f�B�A����
//------------------------------------------------------------------------------
IMedia* CDShow::CreateMediaFromFile(LPTSTR inFileName)
{
#ifdef _DEBUG
	if(m_pD3DDevice == NULL) {
		::OutputDebugString(TEXT("*** Error - Direct3DDevice9��������(CDShow_CreateMediaFromFile)\n"));
		return NULL;
	}
#endif

	CMedia*   pMediaLoader = new CMedia(m_pD3DDevice);
	if(pMediaLoader == NULL) {
		::OutputDebugString(TEXT("*** Error - ���f�B�A�C���X�^���X�������s(CDShow_CreateMediaFromFile)\n"));
		return NULL;
	}

	// �t�@�C���ǂݍ���
	IMedia*   pMedia = pMediaLoader;
	if(pMediaLoader->LoadFromFile(inFileName) == false) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���ǂݍ��ݎ��s(CDShow_CreateMediaFromFile)\n"));
		delete pMediaLoader;
		pMedia = new CNullMedia();
	}
	m_MediaList.push_back(pMedia);

	return pMedia;
}

//------------------------------------------------------------------------------
//	���f�B�A���
//------------------------------------------------------------------------------
void CDShow::ReleaseMedia(IMedia*& pMedia)
{
	m_ProtectedResource.erase(pMedia);		// �v���e�N�g����

	m_MediaList.remove(pMedia);
	delete pMedia;
	pMedia = NULL;
}

//------------------------------------------------------------------------------
//	�S���f�B�A���
//------------------------------------------------------------------------------
void CDShow::ReleaseAllMedia()
{
	std::list<IMedia*>::iterator   list_it = m_MediaList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_MediaList.end()) {
			delete *list_it;
			list_it++;
		}
		m_MediaList.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_MediaList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_MediaList.erase(list_it);
			} else {
				// �v���e�N�g�Ώ�
				list_it++;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	���\�[�X�v���e�N�g�ݒ�
//------------------------------------------------------------------------------
void CDShow::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// �v���e�N�g�ݒ�
	} else {
		m_ProtectedResource.erase (pResource);		// �v���e�N�g����
	}
}
