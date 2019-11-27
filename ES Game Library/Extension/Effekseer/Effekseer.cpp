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
	 Effekseer.cpp
				Effekseer�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Effekseer.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#ifndef _DEBUG
	#pragma comment(lib, "Effekseer.lib")
	#pragma comment(lib, "EffekseerRendererDX9.lib")
#else
	#pragma comment(lib, "Effekseerd.lib")
	#pragma comment(lib, "EffekseerRendererDX9d.lib")
#endif

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//------------------------------------------------------------------------------
//	�f�t�H���g�R���X�g���N�^
//------------------------------------------------------------------------------
CEffekseer::CEffekseer() : m_manager(NULL), m_renderer(NULL)
{
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CEffekseer::~CEffekseer()
{
	Release();
}

//------------------------------------------------------------------------------
//	������
//------------------------------------------------------------------------------
bool CEffekseer::Initialize(IDirect3DDevice9* pD3DDevice, const int inMaxSprite)
{
	assert(pD3DDevice != NULL);
	Release();

	// �`��Ǘ��C���X�^���X����
	m_renderer = EffekseerRendererDX9::Renderer::Create(pD3DDevice, inMaxSprite);

	// �G�t�F�N�g�Ǘ��C���X�^���X����
	m_manager  = Effekseer::Manager::Create(inMaxSprite);

	// �`����@�ݒ�
	m_manager->SetSpriteRenderer  (m_renderer->CreateSpriteRenderer());	// �X�v���C�g
	m_manager->SetRibbonRenderer  (m_renderer->CreateRibbonRenderer());	// ���{��
	m_manager->SetRingRenderer    (m_renderer->CreateRingRenderer  ());	// �����O
	m_manager->SetTrackRenderer   (m_renderer->CreateTrackRenderer ());	// �O��
	m_manager->SetModelRenderer   (m_renderer->CreateModelRenderer ());	// ���f��

	m_manager->SetModelLoader     (m_renderer->CreateModelLoader   ());	// ���f�����[�_
	m_manager->SetTextureLoader   (m_renderer->CreateTextureLoader ());	// �e�N�X�`�����[�_

	m_manager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH   );	// ���W�n

	// �J�����ݒ�
	D3DMATRIX             d3dmat;
	Effekseer::Matrix44   matrix;

	// �r���[
	pD3DDevice->GetTransform(D3DTS_VIEW, &d3dmat);
	::CopyMemory(&matrix.Values[0][0], &d3dmat.m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetCameraMatrix(matrix);

	// �v���W�F�N�V����
	pD3DDevice->GetTransform(D3DTS_PROJECTION, &d3dmat);
	::CopyMemory(&matrix.Values[0][0], &d3dmat.m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetProjectionMatrix(matrix);

	return true;
}

//------------------------------------------------------------------------------
//	���
//------------------------------------------------------------------------------
void CEffekseer::Release()
{
	m_protected_resource.clear();
	ReleaseAllEffects();

	if(m_manager != NULL) {
		m_manager->Destroy();
		m_manager = NULL;
	}

	if(m_renderer != NULL) {
		m_renderer->Destory();
		m_renderer = NULL;
	}
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g����
//------------------------------------------------------------------------------
IEffekseerEffect* CEffekseer::CreateEffectFromFile(LPCWSTR inFileName)
{
#ifdef _DEBUG
	if(m_manager == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer��������(CEffekseer_CreateEffectFromFile)\n"));
		return NULL;
	}
#endif

	IEffekseerEffect*    pEffekseerEffect;
	Effekseer::Effect*   effect = Effekseer::Effect::Create(m_manager, (const EFK_CHAR*)inFileName);
	if(effect != NULL) {
		pEffekseerEffect = new CEffekseerEffect(m_manager, effect);
	} else {
		pEffekseerEffect = new CNullEffekseerEffect();
	}
	m_effect_list.push_back(pEffekseerEffect);

	return pEffekseerEffect;
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g���
//------------------------------------------------------------------------------
void CEffekseer::ReleaseEffect(IEffekseerEffect*& pEffect)
{
	m_protected_resource.erase(pEffect);
	m_effect_list.remove(pEffect);

	delete pEffect;
	pEffect = NULL;
}

//------------------------------------------------------------------------------
//	�S�G�t�F�N�g���
//------------------------------------------------------------------------------
void CEffekseer::ReleaseAllEffects()
{
	std::list<IEffekseerEffect*>::iterator   list_it = m_effect_list.begin();
	if(m_protected_resource.empty()) {
		while(list_it != m_effect_list.end()) {
			delete *list_it;
			list_it++;
		}
		m_effect_list.clear();
	} else {
		std::set<void*>::iterator   set_it;
		std::set<void*>::iterator   set_end = m_protected_resource.end();
		while(list_it != m_effect_list.end()) {
			set_it = m_protected_resource.find(*list_it);
			if(set_it == set_end) {
				// �v���e�N�g�ΏۊO
				delete *list_it;
				list_it = m_effect_list.erase(list_it);
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
void CEffekseer::SetResourceProtect(void* pResource, const bool inProtect)
{
	if(inProtect) {
		m_protected_resource.insert(pResource);		// �v���e�N�g�ݒ�
	} else {
		m_protected_resource.erase (pResource);		// �v���e�N�g����
	}
}

/*
//------------------------------------------------------------------------------
//	�r���[�ݒ�
//------------------------------------------------------------------------------
void CEffekseer::SetView(const Effekseer::Matrix44& inMatrix)
{
#ifdef _DEBUG
	if(m_renderer == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer��������(CEffekseer_SetView)\n"));
		return;
	}
#endif

	m_renderer->SetCameraMatrix(inMatrix);
}

//------------------------------------------------------------------------------
//	�v���W�F�N�V�����ݒ�
//------------------------------------------------------------------------------
void CEffekseer::SetProjection(const Effekseer::Matrix44& inMatrix)
{
#ifdef _DEBUG
	if(m_renderer == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer��������(CEffekseer_SetProjection)\n"));
		return;
	}
#endif

	m_renderer->SetProjectionMatrix(inMatrix);
}
*/

//------------------------------------------------------------------------------
//	�r���[�ݒ�
//------------------------------------------------------------------------------
void CEffekseer::SetView(D3DMATRIX& inMatrix)
{
#ifdef _DEBUG
	if(m_renderer == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer��������(CEffekseer_SetView)\n"));
		return;
	}
#endif

	Effekseer::Matrix44   matrix;
	::CopyMemory(&matrix.Values[0][0], &inMatrix.m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetCameraMatrix(matrix);
}

//------------------------------------------------------------------------------
//	�v���W�F�N�V�����ݒ�
//------------------------------------------------------------------------------
void CEffekseer::SetProjection(D3DMATRIX& inMatrix)
{
#ifdef _DEBUG
	if(m_renderer == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer��������(CEffekseer_SetProjection)\n"));
		return;
	}
#endif

	Effekseer::Matrix44   matrix;
	::CopyMemory(&matrix.Values[0][0], &inMatrix.m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetProjectionMatrix(matrix);
}

//------------------------------------------------------------------------------
//	�J�����ݒ�
//------------------------------------------------------------------------------
void CEffekseer::SetCamera(CCamera& inCamera)
{
#ifdef _DEBUG
	if(m_renderer == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer��������(CEffekseer_SetCamera)\n"));
		return;
	}
#endif

	Effekseer::Matrix44   matrix;

	::CopyMemory(&matrix.Values[0][0], &inCamera.GetViewMatrix().m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetCameraMatrix(matrix);

	::CopyMemory(&matrix.Values[0][0], &inCamera.GetProjectionMatrix().m[0][0], sizeof(float) * 4 * 4);
	m_renderer->SetProjectionMatrix(matrix);
}

//------------------------------------------------------------------------------
//	�X�V
//------------------------------------------------------------------------------
void CEffekseer::Update()
{
#ifdef _DEBUG
	if(m_manager == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer��������(CEffekseer_Update)\n"));
		return;
	}
#endif

	m_manager->Update();
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CEffekseer::Draw()
{
#ifdef _DEBUG
	if(m_manager == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer��������(CEffekseer_Draw)\n"));
		return;
	}
#endif

	m_renderer->BeginRendering();
	m_manager ->Draw();
	m_renderer->EndRendering();
}

//------------------------------------------------------------------------------
//	�J�����O�̈搶��
//------------------------------------------------------------------------------
void CEffekseer::CreateCullingWorld(const float inWidth, const float inHeight,
									const float inDepth, const int inLayerCount)
{
#ifdef _DEBUG
	if(m_manager == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer��������(CEffekseer_CreateCullingWorld)\n"));
		return;
	}
#endif

	m_manager->CreateCullingWorld(inWidth, inHeight, inDepth, inLayerCount);
}

//------------------------------------------------------------------------------
//	�J�����O���Z
//------------------------------------------------------------------------------
void CEffekseer::CalculateCulling()
{
#ifdef _DEBUG
	if(m_manager == NULL) {
		::OutputDebugString(TEXT("*** Error - Effekseer��������(CEffekseer_CalculateCulling)\n"));
		return;
	}
#endif

	m_manager->CalcCulling(m_renderer->GetCameraProjectionMatrix(), false);
}

//------------------------------------------------------------------------------
//	�X�N���[�����W��Effekseer���[���h���W�ϊ�
//------------------------------------------------------------------------------
D3DXVECTOR3 CEffekseer::ScreenToWorld(const float inScreenX, const float inScreenY,
									  const float inEffectPosZ)
{
	// �r���[�|�[�g�t�s��
	D3DVIEWPORT9   vp;
	m_renderer->GetDevice()->GetViewport(&vp);

	D3DXMATRIX   viewport;
	::ZeroMemory(viewport.m, sizeof(float) * 4 * 4);
	viewport.m[0][0] =  float(vp.Width ) / 2.0f;
	viewport.m[1][1] = -float(vp.Height) / 2.0f;
	viewport.m[2][2] =  vp.MaxZ - vp.MinZ;
	viewport.m[3][0] =  float(vp.X + vp.Width ) / 2.0f;
	viewport.m[3][1] =  float(vp.Y + vp.Height) / 2.0f;
	viewport.m[3][2] =  vp.MinZ;
	viewport.m[3][3] =  1.0f;
	::D3DXMatrixInverse(&viewport, NULL, &viewport);

	Effekseer::Matrix44   matrix;

	// �v���W�F�N�V�����t�s��
	matrix = m_renderer->GetProjectionMatrix();
	D3DXMATRIX   proj;
	::CopyMemory(&proj.m[0][0], &matrix.Values[0][0], sizeof(float) * 4 * 4);
	::D3DXMatrixInverse(&proj, NULL, &proj);

	// �r���[�t�s��
	matrix = m_renderer->GetCameraMatrix();
	D3DXMATRIX   view;
	::CopyMemory(&view.m[0][0], &matrix.Values[0][0], sizeof(float) * 4 * 4);
	::D3DXMatrixInverse(&view, NULL, &view);

	// �X�N���[�����W�����[���h���W�ϊ�
	D3DXVECTOR3   world(inScreenX, inScreenY, 0.0f);
	::D3DXVec3TransformCoord(&world, &world, &viewport);
	::D3DXVec3TransformCoord(&world, &world, &proj);
	::D3DXVec3TransformCoord(&world, &world, &view);

	// ���[���h���W�␳
	world.x = (world.x - view.m[3][0]) * (inEffectPosZ - world.z) + view.m[3][0];
	world.y = (world.y - view.m[3][1]) * (inEffectPosZ - world.z) + view.m[3][1];
	world.z = inEffectPosZ;

	return world;

/*
	D3DVIEWPORT9   viewport;
	m_renderer->GetDevice()->GetViewport(&viewport);

	Effekseer::Matrix44   matrix;

	matrix = m_renderer->GetProjectionMatrix();
	D3DXMATRIX   proj;
	::CopyMemory(&proj.m[0][0], &matrix.Values[0][0], sizeof(float) * 4 * 4);

	matrix = m_renderer->GetCameraMatrix();
	D3DXMATRIX   view;
	::CopyMemory(&view.m[0][0], &matrix.Values[0][0], sizeof(float) * 4 * 4);

	// ���[���h�ϊ��s�񐶐�(�P�ʍs��)
	D3DXMATRIX   world;
	::D3DXMatrixIdentity(&world);

	D3DXVECTOR3   screen(inScreenX, inScreenY, 0.0f);
	D3DXVECTOR3   effekseer_world;
	::D3DXVec3Unproject(&effekseer_world, &screen, &viewport, &proj, &view, &world);

	effekseer_world.x = (effekseer_world.x - inCameraX) * (inEffectPosZ - effekseer_world.z) + inCameraX;
	effekseer_world.y = (effekseer_world.y - inCameraY) * (inEffectPosZ - effekseer_world.z) + inCameraY;
	effekseer_world.z = inEffectPosZ;

	return effekseer_world;
*/
}
