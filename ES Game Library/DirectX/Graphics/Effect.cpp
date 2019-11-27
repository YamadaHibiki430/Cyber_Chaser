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
	 Effect.cpp
				�G�t�F�N�g�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Effect.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CEffect::CEffect(ID3DXEffect* pD3DXEffect) : m_pD3DXEffect(NULL), m_pD3DDevice(NULL),
											 m_Material(), m_UseStage0(false)
{
	assert(pD3DXEffect != NULL);
	m_pD3DXEffect = pD3DXEffect;
	m_pD3DXEffect->AddRef();

	m_pD3DXEffect->GetDevice(&m_pD3DDevice);
	assert(m_pD3DDevice != NULL);
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CEffect::~CEffect()
{
	// �G�t�F�N�g�e�N�X�`�����
	std::list<DXGEFFECTMAPINFO>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		SafeRelease((*it).pTexture);

	m_pD3DDevice ->Release();
	m_pD3DXEffect->Release();
}

//------------------------------------------------------------------------------
//	�f�o�C�X��������
//------------------------------------------------------------------------------
void CEffect::OnLostDevice()
{
	m_pD3DXEffect->OnLostDevice();
}

//------------------------------------------------------------------------------
//	�f�o�C�X���Z�b�g����
//------------------------------------------------------------------------------
void CEffect::OnResetDevice()
{
	m_pD3DXEffect->OnResetDevice();
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�J�n
//------------------------------------------------------------------------------
UINT CEffect::Begin()
{
	// �G�t�F�N�g�e�N�X�`���ݒ�
	std::list<DXGEFFECTMAPINFO>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++) {
		if((*it).hParameter != NULL)
			m_pD3DXEffect->SetTexture((*it).hParameter,   (*it).pTexture);
		else
			m_pD3DDevice ->SetTexture((*it).TextureStage, (*it).pTexture);
	}

	UINT   pass;
	if(m_pD3DXEffect->Begin(&pass, 0) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �G�t�F�N�g�J�n���s(CEffect_Begin)\n"));
		return 0;
	}

	return pass;
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�I��
//------------------------------------------------------------------------------
void CEffect::End()
{
	m_pD3DXEffect->End();

	// �G�t�F�N�g�e�N�X�`������
	std::list<DXGEFFECTMAPINFO>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++) {
		if((*it).hParameter == NULL)
			m_pD3DDevice ->SetTexture((*it).TextureStage, NULL);
	}
}

//------------------------------------------------------------------------------
//	�p�X�J�n
//------------------------------------------------------------------------------
bool CEffect::BeginPass(const UINT inPass)
{
	if(m_pD3DXEffect->BeginPass(inPass) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �p�X�J�n���s(CEffect_BeginPass)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�p�X�I��
//------------------------------------------------------------------------------
void CEffect::EndPass()
{
	m_pD3DXEffect->EndPass();
}

//------------------------------------------------------------------------------
//	�e�N�j�b�N�ݒ�
//------------------------------------------------------------------------------
void CEffect::SetTechnique(LPCSTR inTechName)
{
	m_pD3DXEffect->SetTechnique(inTechName);
}

//------------------------------------------------------------------------------
//	�f�[�^�ݒ�
//------------------------------------------------------------------------------
void CEffect::SetData(const D3DXHANDLE inParamName, LPCVOID lpData, const UINT inBytes)
{
	m_pD3DXEffect->SetValue(inParamName, lpData, inBytes);
}

//------------------------------------------------------------------------------
//	�����z��ݒ�
//------------------------------------------------------------------------------
void CEffect::SetIntArray(const D3DXHANDLE inParamName, const int inIntArray[], UINT inCount)
{
	m_pD3DXEffect->SetIntArray(inParamName, inIntArray, inCount);
}

//------------------------------------------------------------------------------
//	�P���x���������z��ݒ�
//------------------------------------------------------------------------------
void CEffect::SetFloatArray(const D3DXHANDLE inParamName, const float inFloatArray[], UINT inCount)
{
	m_pD3DXEffect->SetFloatArray(inParamName, inFloatArray, inCount);
}

//------------------------------------------------------------------------------
//	�x�N�g���S�z��ݒ�
//------------------------------------------------------------------------------
void CEffect::SetVector4Array(const D3DXHANDLE inParamName, const D3DXVECTOR4 inVec4Array[], UINT inCount)
{
	m_pD3DXEffect->SetVectorArray(inParamName, inVec4Array, inCount);
}

//------------------------------------------------------------------------------
//	�s��z��ݒ�
//------------------------------------------------------------------------------
void CEffect::SetMatrixArray(const D3DXHANDLE inParamName, const D3DXMATRIX inMatrixArray[], UINT inCount)
{
	m_pD3DXEffect->SetMatrixArray(inParamName, inMatrixArray, inCount);
}

//------------------------------------------------------------------------------
//	�t�s��ݒ�
//------------------------------------------------------------------------------
void CEffect::SetMatrixInverse(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix)
{
	// �t�s�񐶐�
	D3DXMATRIX   inv;
	if(::D3DXMatrixInverse(&inv, NULL, &inMatrix) == NULL)
		::ZeroMemory(&inv, sizeof(inv));

	m_pD3DXEffect->SetMatrix(inParamName, &inv);
}

//------------------------------------------------------------------------------
//	�]�u�s��ݒ�
//------------------------------------------------------------------------------
void CEffect::SetMatrixTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix)
{
	m_pD3DXEffect->SetMatrixTranspose(inParamName, &inMatrix);
}

//------------------------------------------------------------------------------
//	�t�]�u�s��ݒ�
//------------------------------------------------------------------------------
void CEffect::SetMatrixInverseTranspose(const D3DXHANDLE inParamName, const D3DXMATRIX& inMatrix)
{
	// �t�s�񐶐�
	D3DXMATRIX   inv;
	if(::D3DXMatrixInverse(&inv, NULL, &inMatrix) == NULL)
		::ZeroMemory(&inv, sizeof(inv));

	m_pD3DXEffect->SetMatrixTranspose(inParamName, &inv);
}

//------------------------------------------------------------------------------
//	�e�N�X�`���ݒ�
//------------------------------------------------------------------------------
void CEffect::SetTexture(const D3DXHANDLE inParamName, IDirect3DBaseTexture9* pTexture)
{
	m_pD3DXEffect->SetTexture(inParamName, pTexture);
}

//------------------------------------------------------------------------------
//	�e�N�X�`���ݒ�
//------------------------------------------------------------------------------
void CEffect::SetTexture(const DWORD inTextureStage, IDirect3DBaseTexture9* pTexture)
{
	m_pD3DDevice->SetTexture(inTextureStage, pTexture);
}

//------------------------------------------------------------------------------
//	�}�e���A�����o�^
//------------------------------------------------------------------------------
void CEffect::RetisterMaterialByName(LPCSTR inDiffuse,  LPCSTR inAmbient, LPCSTR inSpecular,
									 LPCSTR inEmissive, LPCSTR inPower,   LPCSTR inTexture)
{
	D3DXVECTOR4   v(0.0f, 0.0f, 0.0f, 0.0f);
	float         f = 0.0f;

	if(inDiffuse  != NULL) {
		m_Material.Diffuse  = m_pD3DXEffect->GetParameterByName(NULL, inDiffuse );
		m_pD3DXEffect->SetVector (m_Material.Diffuse,  &v);
	}
	if(inAmbient  != NULL) {
		m_Material.Ambient  = m_pD3DXEffect->GetParameterByName(NULL, inAmbient );
		m_pD3DXEffect->SetVector (m_Material.Ambient,  &v);
	}
	if(inSpecular != NULL) {
		m_Material.Specular = m_pD3DXEffect->GetParameterByName(NULL, inSpecular);
		m_pD3DXEffect->SetVector (m_Material.Specular, &v);
	}
	if(inEmissive != NULL) {
		m_Material.Emissive = m_pD3DXEffect->GetParameterByName(NULL, inEmissive);
		m_pD3DXEffect->SetVector (m_Material.Emissive, &v);
	}
	if(inPower    != NULL) {
		m_Material.Power    = m_pD3DXEffect->GetParameterByName(NULL, inPower   );
		m_pD3DXEffect->SetFloat  (m_Material.Power,     f);
	}
	if(inTexture  != NULL) {
		m_Material.Texture  = m_pD3DXEffect->GetParameterByName(NULL, inTexture );
		m_pD3DXEffect->SetTexture(m_Material.Texture,  NULL);
	}
}

//------------------------------------------------------------------------------
//	�}�e���A���ݒ�
//------------------------------------------------------------------------------
bool CEffect::SetMaterial(D3DMATERIAL9& inMaterial, IDirect3DBaseTexture9* pTexture)
{
	bool   change = false;

	// �}�e���A���ݒ�
	if(m_Material.Diffuse  != NULL) {
		m_pD3DXEffect->SetVector(m_Material.Diffuse,  (D3DXVECTOR4*)&inMaterial.Diffuse );
		change = true;
	}
	if(m_Material.Ambient  != NULL) {
		m_pD3DXEffect->SetVector(m_Material.Ambient,  (D3DXVECTOR4*)&inMaterial.Ambient );
		change = true;
	}
	if(m_Material.Specular != NULL) {
		m_pD3DXEffect->SetVector(m_Material.Specular, (D3DXVECTOR4*)&inMaterial.Specular);
		change = true;
	}
	if(m_Material.Emissive != NULL) {
		m_pD3DXEffect->SetVector(m_Material.Emissive, (D3DXVECTOR4*)&inMaterial.Emissive);
		change = true;
	}
	if(m_Material.Power    != NULL) {
		m_pD3DXEffect->SetFloat (m_Material.Power,    inMaterial.Power);
		change = true;
	}

	// �e�N�X�`���ݒ�
	if(m_UseStage0 == false) {
		if(m_Material.Texture != NULL) {
			m_pD3DXEffect->SetTexture(m_Material.Texture, pTexture);
			change = true;
		} else {
			m_pD3DDevice ->SetTexture(0, pTexture);
		}
	}

	if(change)
		m_pD3DXEffect->CommitChanges();

	return change;
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�}�b�s���O�ݒ�
//------------------------------------------------------------------------------
void CEffect::SetEffectMap(LPCSTR inParamName, LPCTSTR inFileName, const D3DFORMAT inFormat)
{
	// �L���ȃp�����[�^�������ׂ�
	D3DXHANDLE   hParameter = m_pD3DXEffect->GetParameterByName(NULL, inParamName);
	if(hParameter == NULL) {
		::OutputDebugString(TEXT("*** Error - �p�����[�^������`(CEffect_SetEffectMap)\n"));
		return;
	}

	// �e�N�X�`������
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTextureFromFileEx(m_pD3DDevice, inFileName, 0, 0, 0, 0,
									 inFormat, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
									 0, NULL, NULL, &pTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CEffect_SetEffectMap)\n"));
		return;
	}

	// ���X�g�֒ǉ�
	m_EffectMap.push_back(DXGEFFECTMAPINFO(hParameter, 0, pTexture));
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�}�b�s���O�ݒ�
//------------------------------------------------------------------------------
void CEffect::SetEffectMap(const DWORD inTextureStage, LPCTSTR inFileName, const D3DFORMAT inFormat)
{
	// �e�N�X�`������
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTextureFromFileEx(m_pD3DDevice, inFileName, 0, 0, 0, 0,
									 inFormat, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT,
									 0, NULL, NULL, &pTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CEffect_SetEffectMap)\n"));
		return;
	}

	// ���X�g�֒ǉ�
	m_EffectMap.push_back(DXGEFFECTMAPINFO(NULL, inTextureStage, pTexture));

	// �X�e�[�W�O�g�p�ݒ�
	if(inTextureStage == 0)
		m_UseStage0 = true;
}

//------------------------------------------------------------------------------
//	�L���[�u�}�b�s���O�ݒ�
//------------------------------------------------------------------------------
void CEffect::SetCubeMap(LPCSTR inParamName, LPCTSTR inFileName)
{
	// �L���ȃp�����[�^�������ׂ�
	D3DXHANDLE   hParameter = m_pD3DXEffect->GetParameterByName(NULL, inParamName);
	if(hParameter == NULL) {
		::OutputDebugString(TEXT("*** Error - �p�����[�^���ُ�(CEffect_SetEffectMap)\n"));
		return;
	}

	// �L���[�u�e�N�X�`������
	IDirect3DCubeTexture9*   pCubeTexture;
	if(::D3DXCreateCubeTextureFromFile(m_pD3DDevice, inFileName, &pCubeTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �L���[�u�e�N�X�`���������s(CEffect_SetCubeMap)\n"));
		return;
	}

	// ���X�g�֒ǉ�
	m_EffectMap.push_back(DXGEFFECTMAPINFO(hParameter, 0, pCubeTexture));
}

//------------------------------------------------------------------------------
//	�L���[�u�}�b�s���O�ݒ�
//------------------------------------------------------------------------------
void CEffect::SetCubeMap(const DWORD inTextureStage, LPCTSTR inFileName)
{
	// �L���[�u�e�N�X�`������
	IDirect3DCubeTexture9*   pCubeTexture;
	if(::D3DXCreateCubeTextureFromFile(m_pD3DDevice, inFileName, &pCubeTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �L���[�u�e�N�X�`���������s(CEffect_SetCubeMap)\n"));
		return;
	}

	// ���X�g�֒ǉ�
	m_EffectMap.push_back(DXGEFFECTMAPINFO(NULL, inTextureStage, pCubeTexture));

	// �X�e�[�W�O�g�p�ݒ�
	if(inTextureStage == 0)
		m_UseStage0 = true;
}
