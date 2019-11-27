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
	 NUI.cpp
				NUI�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "NUI.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "Kinect10.lib")

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CNUI::CNUI() : m_pD3DDevice(NULL)
{
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CNUI::~CNUI()
{
	Release();
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CNUI::Initialize(IDirect3DDevice9* pD3DDevice)
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
void CNUI::Release()
{
	m_ProtectedResource.clear();

	ReleaseAllKinects();
	SafeRelease(m_pD3DDevice);
}

//------------------------------------------------------------------------------
//	Kinect����
//------------------------------------------------------------------------------
IKinect* CNUI::CreateKinect(const UINT inIndex, const DWORD inFlags)
{
	IKinect*      pKinect;
	INuiSensor*   pSensor = NULL;

	try {
		// Kinect����
		if(::NuiCreateSensorByIndex(inIndex, &pSensor) != S_OK)
			throw TEXT("Kinect�������s");

		// Kinect������
		if(pSensor->NuiInitialize(inFlags) != S_OK)
			throw TEXT("Kinect���������s");

		// Kinect�C���X�^���X����
		pKinect = new CKinect(pSensor, m_pD3DDevice);
	} catch(LPCTSTR err_str) {
		// �G���[���b�Z�[�W����
		TCHAR   msg[128];
		::wsprintf(msg, TEXT("*** Error - %s(CNUI_CreateKinect)\n"), err_str);
		::OutputDebugString(msg);

		// ��O�����������ꍇ�́ANULL�I�u�W�F�N�g�𐶐�
		pKinect = new CNullKinect();
	}
	SafeRelease(pSensor);

	// ���X�g�֒ǉ�
	m_KinectList.push_back(pKinect);

	return pKinect;
}

//------------------------------------------------------------------------------
//	Kinect���
//------------------------------------------------------------------------------
void CNUI::ReleaseKinect(IKinect*& pKinect)
{
	m_ProtectedResource.erase(pKinect);		// �v���e�N�g����

	m_KinectList.remove(pKinect);
	delete pKinect;
	pKinect = NULL;
}

//------------------------------------------------------------------------------
//	�SKinect���
//------------------------------------------------------------------------------
void CNUI::ReleaseAllKinects()
{
	std::list<IKinect*>::iterator   list_it = m_KinectList.begin();
	if(m_ProtectedResource.empty()) {
		while(list_it != m_KinectList.end()) {
			delete *list_it;
			list_it++;
		}
		m_KinectList.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_ProtectedResource.end();
		while(list_it != m_KinectList.end()) {
			set_it = m_ProtectedResource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_KinectList.erase(list_it);
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
void CNUI::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_ProtectedResource.insert(pResource);		// �v���e�N�g�ݒ�
	} else {
		m_ProtectedResource.erase (pResource);		// �v���e�N�g����
	}
}
