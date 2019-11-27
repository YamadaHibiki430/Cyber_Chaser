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
	 Model.cpp
				���f���N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Model.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CModel::CModel(ID3DXMesh* pMesh, const DWORD inMeshes)
	: m_pBaseMesh(NULL), m_pMesh(pMesh), m_pPMesh(NULL), m_pD3DDevice(NULL),
	  m_Meshes(inMeshes), m_Material(inMeshes), m_Texture(inMeshes, NULL),
	  m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f, 1.0f)
{
	assert(m_pMesh != NULL);
	m_pMesh->AddRef();

	m_pMesh->GetDevice(&m_pD3DDevice);
	assert(m_pD3DDevice != NULL);

	for(DWORD i = 0; i < m_Meshes; i++)
		::ZeroMemory(&m_Material[i], sizeof(m_Material[i]));

	m_pBaseMesh = m_pMesh;
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CModel::~CModel()
{
	// �G�t�F�N�g�e�N�X�`�����
	std::list<EFFECTMAP>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		SafeRelease((*it).pTexture);

	// �e�N�X�`�����
	for(unsigned int i = 0; i < m_Texture.size(); i++)
		SafeRelease(m_Texture[i]);

	m_pD3DDevice->Release();

	m_pBaseMesh = NULL;

	SafeRelease(m_pPMesh);
	SafeRelease(m_pMesh );
}

//------------------------------------------------------------------------------
//	���b�V���œK��
//------------------------------------------------------------------------------
bool CModel::Optimize()
{
	if(m_pMesh != NULL) {
		// �ʏ탂�[�h
		try {
			// �אږʐ���
			const DWORD    FACE = m_pMesh->GetNumFaces();
			DWORD*   pAdjacency = new DWORD[FACE * 3 * sizeof(DWORD)];
			if(m_pMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency) != D3D_OK)
				throw TEXT("*** Error - �אږʐ������s(CModel_Optimize)\n");

			// �œK��
			HRESULT   hr = m_pMesh->OptimizeInplace(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE,
													pAdjacency, NULL, NULL, NULL);
			delete[] pAdjacency;
			if(hr != D3D_OK)
				throw TEXT("*** Error - ���b�V���œK�����s(CModel_Optimize)\n");
		} catch(LPCTSTR ErrorString) {
			::OutputDebugString(ErrorString);
			return false;
		}
	} else {
		// �v���O���b�V�u���[�h
		m_pPMesh->OptimizeBaseLOD(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, NULL);
	}

	return true;
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CModel::Draw()
{
	// ���[���h�ϊ��s��ݒ�
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// �`��
	for(DWORD i = 0; i < m_Meshes; i++) {
		m_pD3DDevice->SetMaterial(  &m_Material[i]);	// �}�e���A���ݒ�
		m_pD3DDevice->SetTexture (0, m_Texture [i]);	// �e�N�X�`���ݒ�
		m_pBaseMesh->DrawSubset(i);
	}
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CModel::DrawPure()
{
	// �`��
	for(DWORD i = 0; i < m_Meshes; i++)
		m_pBaseMesh->DrawSubset(i);
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CModel::Draw(IEffect*& pEffect)
{
	const UINT   pass = pEffect->Begin();
	for(UINT ps = 0; ps < pass; ps++) {
		pEffect->BeginPass(ps);

		// ���b�V���`��
		for(DWORD i = 0; i < m_Meshes; i++) {
			pEffect->SetMaterial(m_Material[i], m_Texture[i]);
			m_pBaseMesh->DrawSubset(i);
		}

		pEffect->EndPass();
	}
	pEffect->End();
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CModel::Draw(IEffect*& pEffect, const UINT inPass)
{
	pEffect->Begin();
	DrawPass(pEffect, inPass);
	pEffect->End();
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CModel::DrawPass(IEffect*& pEffect, const UINT inPass)
{
	pEffect->BeginPass(inPass);

	for(DWORD i = 0; i < m_Meshes; i++) {
		pEffect->SetMaterial(m_Material[i], m_Texture[i]);
		m_pBaseMesh->DrawSubset(i);
	}

	pEffect->EndPass();
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
/*
void CModel::Draw(IEffect* EffectArray[])
{
	// ���b�V���`��
	for(DWORD i = 0; i < m_Meshes; i++) {
		if(EffectArray[i] == NULL) {
			// T&L�Œ�@�\�ŕ`��
			m_pD3DDevice->SetMaterial(  &m_Material[i]);
			m_pD3DDevice->SetTexture (0, m_Texture [i]);
			m_pBaseMesh->DrawSubset(i);
		} else {
			// �G�t�F�N�g�ŕ`��
			const UINT   pass = EffectArray[i]->Begin();
			for(UINT ps = 0; ps < pass; ps++) {
				EffectArray[i]->BeginPass(ps);

				EffectArray[i]->SetMaterial(m_Material[i], m_Texture[i]);
				m_pBaseMesh->DrawSubset(i);

				EffectArray[i]->EndPass();
			}
			EffectArray[i]->End();
		}
	}
}
*/

//------------------------------------------------------------------------------
//	�������`��
//------------------------------------------------------------------------------
void CModel::DrawAlpha(const float inAlpha, const DWORD inDraw)
{
	if(inAlpha <= FLT_EPSILON)
		return;

	m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// �}�e���A���ݒ�
	std::vector<float>   Diffuse(m_Meshes);
	for(DWORD i = 0; i < m_Meshes; i++) {
		Diffuse[i] = m_Material[i].Diffuse.a;
		m_Material[i].Diffuse.a *= inAlpha;
	}

	// �J���[�o�͕ۑ�
	DWORD   colorwrite_enable;
	m_pD3DDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite_enable);

	// �w��
	if((inDraw & DXGDRAWA_BACKFACE) != 0) {
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

		// ���ݒ�
		if((inDraw & DXGDRAWA_BACKFACE_ZDISABLE) == 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			// ���o�͐ݒ�
			if((inDraw & (DXGDRAWA_BACKFACE_ZWRDEFER | DXGDRAWA_BACKFACE_ZWRDISABLE)) == 0)
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			else
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		} else {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		}

		for(DWORD i = 0; i < m_Meshes; i++) {
			m_pD3DDevice->SetMaterial(  &m_Material[i]);
			m_pD3DDevice->SetTexture (0, m_Texture [i]);
			m_pBaseMesh ->DrawSubset (i);
		}

		// ���x���o��
		if((inDraw & DXGDRAWA_BACKFACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			for(DWORD i = 0; i < m_Meshes; i++)
				m_pBaseMesh->DrawSubset(i);

			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
		}
	}

	// �\��
	if((inDraw & DXGDRAWA_FACE) != 0) {
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		// ���ݒ�
		if((inDraw & DXGDRAWA_FACE_ZDISABLE) == 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			// ���o�͐ݒ�
			if((inDraw & (DXGDRAWA_FACE_ZWRDEFER | DXGDRAWA_FACE_ZWRDISABLE)) == 0)
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			else
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		} else {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		}

		for(DWORD i = 0; i < m_Meshes; i++) {
			m_pD3DDevice->SetMaterial(  &m_Material[i]);
			m_pD3DDevice->SetTexture (0, m_Texture [i]);
			m_pBaseMesh ->DrawSubset (i);
		}

		// ���x���o��
		if((inDraw & DXGDRAWA_FACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			for(DWORD i = 0; i < m_Meshes; i++)
				m_pBaseMesh->DrawSubset(i);

			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
		}
	}

	// �J�����O�Ȃ�
	if((inDraw & DXGDRAWA_CULLNONE) != 0) {
		m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		// ���ݒ�
		if((inDraw & DXGDRAWA_CULLNONE_ZDISABLE) == 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

			// ���o�͐ݒ�
			if((inDraw & (DXGDRAWA_CULLNONE_ZWRDEFER | DXGDRAWA_CULLNONE_ZWRDISABLE)) == 0)
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			else
				m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		} else {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		}

		for(DWORD i = 0; i < m_Meshes; i++) {
			m_pD3DDevice->SetMaterial(  &m_Material[i]);
			m_pD3DDevice->SetTexture (0, m_Texture [i]);
			m_pBaseMesh ->DrawSubset (i);
		}

		// ���x���o��
		if((inDraw & DXGDRAWA_CULLNONE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			for(DWORD i = 0; i < m_Meshes; i++)
				m_pBaseMesh->DrawSubset(i);

			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
		}
	}

	// ������
	const DWORD   ZWRITE = inDraw & 0x3000;
	if(ZWRITE != 0) {
		if(ZWRITE == DXGDRAWA_ZWRITE_FACE)
			m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		// �\��
		else if(ZWRITE == DXGDRAWA_ZWRITE_CULLNONE)
			m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);		// �J�����O�Ȃ�
		else
			m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);		// ����

		m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
		m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

		for(DWORD i = 0; i < m_Meshes; i++)
			m_pBaseMesh->DrawSubset(i);

		m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
	}

	// �}�e���A������
	for(DWORD i = 0; i < m_Meshes; i++)
		m_Material[i].Diffuse.a = Diffuse[i];
}

//------------------------------------------------------------------------------
//	�e�`��
//------------------------------------------------------------------------------
void CModel::DrawShadow(const D3DXMATRIX& inShadowMatrix, const float inDepth)
{
	// ���[���h�ϊ��s��ݒ�
	D3DXMATRIX   ShadowMatrix = GetWorldMatrix() * inShadowMatrix;
	m_pD3DDevice->SetTransform(D3DTS_WORLD, &ShadowMatrix);

	// �}�e���A���ݒ�
	D3DMATERIAL9   material;
	::ZeroMemory(&material, sizeof(material));
	material.Diffuse.a = inDepth;

	m_pD3DDevice->SetMaterial(&material);
	m_pD3DDevice->SetTexture (0, NULL);

	// �`��
	for(DWORD i = 0; i < m_Meshes; i++)
		m_pBaseMesh->DrawSubset(i);
}

//------------------------------------------------------------------------------
//	�}�e���A���ݒ�
//------------------------------------------------------------------------------
void CModel::SetMaterial(const D3DMATERIAL9& inMaterial, const DWORD inMeshIdx)
{
	m_Material[inMeshIdx] = inMaterial;
}

//------------------------------------------------------------------------------
//	�}�e���A���ݒ�
//------------------------------------------------------------------------------
void CModel::SetMaterial(const D3DMATERIAL9& inMaterial)
{
	for(DWORD i = 0; i < m_Meshes; i++)
		m_Material[i] = inMaterial;
}

//------------------------------------------------------------------------------
//	�e�N�X�`���ݒ�
//------------------------------------------------------------------------------
void CModel::SetTexture(LPCTSTR inFileName, const DWORD inMeshIdx)
{
	SafeRelease(m_Texture[inMeshIdx]);

	// �e�N�X�`������
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTextureFromFile(m_pD3DDevice, inFileName, &pTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CModel_SetTexture)\n"));
		pTexture = NULL;
	}

	m_Texture[inMeshIdx] = pTexture;
}

//------------------------------------------------------------------------------
//	�L���[�u�e�N�X�`���ݒ�
//------------------------------------------------------------------------------
void CModel::SetCubeTexture(LPCTSTR inFileName, const DWORD inMeshIdx)
{
	SafeRelease(m_Texture[inMeshIdx]);

	// �e�N�X�`������
	IDirect3DCubeTexture9*   pCubeTexture;
	if(::D3DXCreateCubeTextureFromFile(m_pD3DDevice, inFileName, &pCubeTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CModel_SetTexture)\n"));
		pCubeTexture = NULL;
	}

	m_Texture[inMeshIdx] = pCubeTexture;
}

//------------------------------------------------------------------------------
//	�e�N�X�`���ݒ�
//------------------------------------------------------------------------------
void CModel::SetTexture(IDirect3DBaseTexture9* pTexture, const DWORD inMeshIdx)
{
	SafeRelease(m_Texture[inMeshIdx]);

	if(pTexture != NULL) {
		m_Texture[inMeshIdx] = pTexture;
		m_Texture[inMeshIdx]->AddRef();
	}
}

//------------------------------------------------------------------------------
//	�p�x�ݒ�
//------------------------------------------------------------------------------
void CModel::SetDirection(const float inX, const float inY, const float inZ)
{
	::D3DXQuaternionIdentity(&m_Rotation);

	// �p���̃N�H�[�^�j�I�������߂�
	D3DXQUATERNION   qt;

	// ���[��
	if(inZ != 0.0f) {
		qt.x = 0.0f;
		qt.y = 0.0f;
		qt.z = ::sinf(D3DXToRadian(inZ) / 2.0f);
		qt.w = ::cosf(D3DXToRadian(inZ) / 2.0f);
		m_Rotation *= qt;
	}

	// �s�b�`
	if(inX != 0.0f) {
		qt.x = ::sinf(D3DXToRadian(inX) / 2.0f);
		qt.y = 0.0f;
		qt.z = 0.0f;
		qt.w = ::cosf(D3DXToRadian(inX) / 2.0f);
		m_Rotation *= qt;
	}

	// ���[
	if(inY != 0.0f) {
		qt.x = 0.0f;
		qt.y = ::sinf(D3DXToRadian(inY) / 2.0f);
		qt.z = 0.0f;
		qt.w = ::cosf(D3DXToRadian(inY) / 2.0f);
		m_Rotation *= qt;
	}

/*
	// ��]�N�H�[�^�j�I�������߂�
	::D3DXQuaternionRotationYawPitchRoll(&m_Rotation, D3DXToRadian(inY),
													  D3DXToRadian(inX),
													  D3DXToRadian(inZ));
*/
}

//------------------------------------------------------------------------------
//	�ړ�
//------------------------------------------------------------------------------
void CModel::Move(const float inRight, const float inUp, const float inFront)
{
	// ��]�s�񐶐�
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationQuaternion(&rotation, &m_Rotation);

	// �����x�N�g�������o��
	D3DXVECTOR3*   right = (D3DXVECTOR3*)rotation.m[0];
	D3DXVECTOR3*   up    = (D3DXVECTOR3*)rotation.m[1];
	D3DXVECTOR3*   front = (D3DXVECTOR3*)rotation.m[2];

	// �ړ�
	*right     *= inRight;
	*up        *= inUp;
	*front     *= inFront;
	m_Position += *front + *up + *right;
}

//------------------------------------------------------------------------------
//	��]
//------------------------------------------------------------------------------
void CModel::Rotation(const float inX, const float inY, const float inZ)
{
	// �N�H�[�^�j�I�����玲�����߂�
	D3DXMATRIX   axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXQUATERNION   qt;

	// ���[
	if (inY != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[1], D3DXToRadian(inY));
		m_Rotation *= qt;
	}

	// �s�b�`
	if (inX != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[0], D3DXToRadian(inX));
		m_Rotation *= qt;
	}

	// ���[��
	if (inZ != 0.0f) {
		::D3DXQuaternionRotationAxis(&qt, (D3DXVECTOR3*)axis.m[2], D3DXToRadian(inZ));
		m_Rotation *= qt;
	}
}

//------------------------------------------------------------------------------
//	���_�̐��ʂ������悤�ɉ�]
//------------------------------------------------------------------------------
void CModel::SetRotationBillboard(const D3DXVECTOR3& inEye, const D3DXQUATERNION& inAngle)
{
	m_Rotation = -inAngle;

	// ���_�̕����֌�����
	D3DXVECTOR3   dir = m_Position - inEye;
	float   angleY = ::atan2f(dir.z, dir.x);

	D3DXQUATERNION   qt;
	qt.x = 0.0f;
	qt.y = ::sinf(angleY / 2.0f);
	qt.z = 0.0f;
	qt.w = ::cosf(angleY / 2.0f);

	m_Rotation *= qt;
}

//------------------------------------------------------------------------------
//	���_�̐��ʂ������悤��Y������]������
//------------------------------------------------------------------------------
void CModel::SetRotationBillboardY(const D3DXVECTOR3& inEye, const D3DXVECTOR3& inFront)
{
	D3DXMATRIX    axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXVECTOR3*   x_axis = (D3DXVECTOR3*)axis.m[0];
	D3DXVECTOR3*   y_axis = (D3DXVECTOR3*)axis.m[1];
	D3DXVECTOR3*   z_axis = (D3DXVECTOR3*)axis.m[2];

	D3DXVECTOR3   dir;

	// �����������_�x�N�g���ƈ�v������
	// �����ݒ�
	dir = -inFront;
	::D3DXVec3Normalize(z_axis, &dir);

	// �����ݒ�
	::D3DXVec3Cross(x_axis, y_axis, z_axis);

	// ��]�N�H�[�^�j�I���ɕϊ�
	::D3DXQuaternionRotationMatrix(&m_Rotation, &axis);

	// ���_�̕����֌�����
	dir = m_Position - inEye;
	float   angleY = ::atan2f(dir.z, dir.x);

	D3DXQUATERNION   qt;
	qt.x = 0.0f;
	qt.y = ::sinf(angleY / 2.0f);
	qt.z = 0.0f;
	qt.w = ::cosf(angleY / 2.0f);

	m_Rotation *= qt;
}

//------------------------------------------------------------------------------
//	�������w�肳�ꂽ���̌X���ɍ��킹��
//------------------------------------------------------------------------------
void CModel::AlignmentAxisY(const D3DXVECTOR3& inAxisY)
{
	D3DXMATRIX    axis;
	::D3DXMatrixRotationQuaternion(&axis, &m_Rotation);

	D3DXVECTOR3*   x_axis = (D3DXVECTOR3*)axis.m[0];
	D3DXVECTOR3*   y_axis = (D3DXVECTOR3*)axis.m[1];
	D3DXVECTOR3*   z_axis = (D3DXVECTOR3*)axis.m[2];

	// �����ݒ�
	::D3DXVec3Normalize(y_axis, &inAxisY);

	// �����ݒ�
	::D3DXVec3Cross(x_axis, y_axis, z_axis);

	// ������]�N�H�[�^�j�I���ɕϊ�
	::D3DXQuaternionRotationMatrix(&m_Rotation, &axis);
}

//------------------------------------------------------------------------------
//	��]�p�擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel::GetDirection() const
{
	D3DXVECTOR3   rotation = GetDirectionRadian();

	rotation.x = D3DXToDegree(rotation.x);
	rotation.y = D3DXToDegree(rotation.y);
	rotation.z = D3DXToDegree(rotation.z);

	return rotation;
}

//------------------------------------------------------------------------------
//	��]�p�擾(���W�A��)
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel::GetDirectionRadian() const
{
	D3DXVECTOR3   rotation;

	rotation.x = ::asinf(m_Rotation.x) * 2.0f;
	rotation.y = ::asinf(m_Rotation.y) * 2.0f;
	rotation.z = ::asinf(m_Rotation.z) * 2.0f;

	return rotation;
/*
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    m;
	::D3DXMatrixRotationQuaternion(&m, &m_Rotation);

	// ��, y���擾
	D3DXVECTOR3   x_axis(m._11, m._21, m._31);
	D3DXVECTOR3   y_axis(m._12, m._22, m._32);

	// ��]�p�擾
	D3DXVECTOR3   rotation;

	// ����]�p
	rotation.z = ::atan2f(-y_axis.x, y_axis.y);

	// ����]�p
	const float  cos_z = ::cosf(rotation.z);
	rotation.x = ::atan2f(cos_z * -y_axis.z, ::fabs(y_axis.y));

	// ����]�p
	const float  sin_z = ::sinf(rotation.z);
	const float  cos_x = ::cosf(rotation.x);
	rotation.y = ::atan2(cos_x * -(sin_z * y_axis.y - cos_x * y_axis.x), y_axis.z);

	return rotation;
*/
}

//------------------------------------------------------------------------------
//	���[���h�ϊ��s�񐶐�
//------------------------------------------------------------------------------
D3DXMATRIX CModel::GetWorldMatrix() const
{
	// ���[���h�s��ݒ�
	D3DXMATRIX   world;

	// ��]�s�񐶐�
	::D3DXMatrixRotationQuaternion(&world, &m_Rotation);

	// �X�P�[�����O
	world._11 *= m_Scale.x;
	world._12 *= m_Scale.x;
	world._13 *= m_Scale.x;

	world._21 *= m_Scale.y;
	world._22 *= m_Scale.y;
	world._23 *= m_Scale.y;

	world._31 *= m_Scale.z;
	world._32 *= m_Scale.z;
	world._33 *= m_Scale.z;

	// �ړ�
	world._41  = m_Position.x;
	world._42  = m_Position.y;
	world._43  = m_Position.z;

	world._44  = 1.0f;

	return world;
}

//------------------------------------------------------------------------------
//	�O���x�N�g���擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel::GetFrontVector() const
{
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[2];
}

//------------------------------------------------------------------------------
//	�E���x�N�g���擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel::GetRightVector() const
{
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[0];
}

//------------------------------------------------------------------------------
//	����x�N�g���擾
//------------------------------------------------------------------------------
D3DXVECTOR3 CModel::GetUpVector() const
{
	// �N�H�[�^�j�I������]�s��
	D3DXMATRIX    rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);

	return *(D3DXVECTOR3*)rot.m[1];
}

//------------------------------------------------------------------------------
//	�Փ˔���
//------------------------------------------------------------------------------
BOOL CModel::IntersectRay(D3DXVECTOR3& inPos, D3DXVECTOR3& inDir,
						  float* pDist, D3DXVECTOR3* pNormal)
{
	// ���[���h�s��̋t�s��𐶐�
	D3DXMATRIXA16   world_inv;
	::D3DXMatrixInverse(&world_inv, NULL, &GetWorldMatrix());

	// ���C�����f���̃��[�J���֍��W�֕ϊ�
	D3DXVECTOR3   pos;
	::D3DXVec3TransformCoord (&pos, &inPos, &world_inv);

	// ���������f���̃��[�J�����W�֕ϊ�
	D3DXVECTOR3   dir;
	::D3DXVec3TransformNormal(&dir, &inDir, &world_inv);

	BOOL   hit = FALSE;
	if(pNormal == NULL) {
		// �@���s�v
		::D3DXIntersect(m_pBaseMesh, &pos, &dir, &hit, NULL, NULL, NULL, pDist, NULL, NULL);
	} else {
		// �@���K�v
		DWORD   face_index;
		::D3DXIntersect(m_pBaseMesh, &pos, &dir, &hit, &face_index, NULL, NULL, pDist, NULL, NULL);
		if(hit != FALSE) {
			// �C���f�b�N�X�o�b�t�@���b�N
			WORD*   pIndices;
			if(m_pBaseMesh->LockIndexBuffer(D3DLOCK_READONLY, (LPVOID*)&pIndices) != D3D_OK) {
				::OutputDebugString(TEXT("*** Error - �C���f�b�N�X�o�b�t�@���b�N���s(CModel_IntersectRay)\n"));
				*pNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				return hit;
			}

			// �Փ˖ʃC���f�b�N�X�擾
			WORD   index[3];
			index[0] = pIndices[face_index * 3 + 0];
			index[1] = pIndices[face_index * 3 + 1];
			index[2] = pIndices[face_index * 3 + 2];

			// �C���f�b�N�X�o�b�t�@�A�����b�N
			m_pBaseMesh->UnlockIndexBuffer();

			// �X�g���C�h�ݒ�
			const DWORD   STRIDE = m_pBaseMesh->GetNumBytesPerVertex();

			// ���_�o�b�t�@���b�N
			BYTE*   pVertices;
			if(m_pBaseMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&pVertices) != D3D_OK) {
				::OutputDebugString(TEXT("*** Error - ���_�o�b�t�@���b�N���s(CModel_IntersectRay)\n"));
				*pNormal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				return hit;
			}

			// �Փ˖ʒ��_���W�擾
			D3DXVECTOR3   face[3];
			::CopyMemory(&face[0], pVertices + index[0] * STRIDE, sizeof(D3DXVECTOR3));
			::CopyMemory(&face[1], pVertices + index[1] * STRIDE, sizeof(D3DXVECTOR3));
			::CopyMemory(&face[2], pVertices + index[2] * STRIDE, sizeof(D3DXVECTOR3));

			// ���_�o�b�t�@�A�����b�N
			m_pBaseMesh->UnlockVertexBuffer();

			// �@���ݒ�
			::D3DXVec3Cross(pNormal, &D3DXVECTOR3(face[1] - face[0]), &D3DXVECTOR3(face[2] - face[0]));
			::D3DXVec3Normalize(pNormal, pNormal);
		}
	}

	return hit;
}

//------------------------------------------------------------------------------
//	���E������
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CModel::GetBoundingSphere()
{
	DXGBOUNDINGSPHERE   shpere(GetBaseBoundingSphere());

	float   scale = m_Scale.x;
	if(scale < m_Scale.y)
		scale = m_Scale.y;
	if(scale < m_Scale.z)
		scale = m_Scale.z;

	shpere.Center += m_Position;
	shpere.Radius *= scale;

	return shpere;
}

//------------------------------------------------------------------------------
//	��{���E������
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CModel::GetBaseBoundingSphere()
{
	DXGBOUNDINGSPHERE   shpere;

	// ���_�o�b�t�@���b�N
	void*   pVertices;
	if(m_pBaseMesh->LockVertexBuffer(0, &pVertices) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���_�o�b�t�@���b�N���s(CModel_GetBaseBoundingSphere)\n"));
		return shpere;
	}

	// ���E������
	::D3DXComputeBoundingSphere((D3DXVECTOR3*)pVertices, m_pMesh->GetNumVertices(),
								::D3DXGetFVFVertexSize(m_pMesh->GetFVF()),
								&shpere.Center, &shpere.Radius);

	m_pBaseMesh->UnlockVertexBuffer();

	return shpere;
}

//------------------------------------------------------------------------------
//	OBB����
//------------------------------------------------------------------------------
DXGOBB CModel::GetOBB()
{
	DXGOBB   obb(GetBaseOBB());

	// ���S���W�ݒ�
	obb.Center += m_Position;

	// �e���̌X���ݒ�
	D3DXMATRIX   rot;
	::D3DXMatrixRotationQuaternion(&rot, &m_Rotation);
	obb.Axis[0] = D3DXVECTOR3(rot._11, rot._12, rot._13);
	obb.Axis[1] = D3DXVECTOR3(rot._21, rot._22, rot._23);
	obb.Axis[2] = D3DXVECTOR3(rot._31, rot._32, rot._33);

	// �e���̒����ݒ�
	obb.Radius.x *= m_Scale.x;
	obb.Radius.y *= m_Scale.y;
	obb.Radius.z *= m_Scale.z;

	return obb;
}

//------------------------------------------------------------------------------
//	��{OBB����
//------------------------------------------------------------------------------
DXGOBB CModel::GetBaseOBB()
{
	DXGOBB   obb;

	// ���_�o�b�t�@���b�N
	BYTE*   pVertices;
	if(m_pBaseMesh->LockVertexBuffer(0, (LPVOID*)&pVertices) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���_�o�b�t�@���b�N���s(CModel_GetOBB)\n"));
		return obb;
	}

	// �e���̍ő�A�ŏ����W�����߂�
	D3DXVECTOR3   pos_max(FLT_MIN, FLT_MIN, FLT_MIN);
	D3DXVECTOR3   pos_min(FLT_MAX, FLT_MAX, FLT_MAX);

	const int   NumVertices = m_pMesh->GetNumVertices();
	const int   STRIDE      = ::D3DXGetFVFVertexSize(m_pMesh->GetFVF());
	for(int i = 0; i < NumVertices; i++) {
		D3DXVECTOR3   pos = *(D3DXVECTOR3*)pVertices;
		if(pos.x < pos_min.x)	pos_min.x = pos.x;
		if(pos.x > pos_max.x)	pos_max.x = pos.x;
		if(pos.y < pos_min.y)	pos_min.y = pos.y;
		if(pos.y > pos_max.y)	pos_max.y = pos.y;
		if(pos.z < pos_min.z)	pos_min.z = pos.z;
		if(pos.z > pos_max.z)	pos_max.z = pos.z;

		pVertices += STRIDE;
	}

	m_pBaseMesh->UnlockVertexBuffer();

	// ���S�_�ݒ�
	obb.Center = (pos_max + pos_min) * 0.5f;

	// �e���̒����̔�����ݒ�
	obb.Radius.x = ::fabsf(pos_max.x - pos_min.x) * 0.5f;
	obb.Radius.y = ::fabsf(pos_max.y - pos_min.y) * 0.5f;
	obb.Radius.z = ::fabsf(pos_max.z - pos_min.z) * 0.5f;

	return obb;
}

//------------------------------------------------------------------------------
//	�I�N���[�W�����N�G���擾
//------------------------------------------------------------------------------
DWORD CModel::GetOcclusionQuery()
{
	IDirect3DQuery9*   pQuery;
	const HRESULT   hr = m_pD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &pQuery);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �I�N���[�W�����N�G���擾���s(CModel_GetOcclusionQuery)\n"));
		return 0xffffffff;
	}

	pQuery->Issue(D3DISSUE_BEGIN);
	Draw();
	pQuery->Issue(D3DISSUE_END  );

	// Force the driver to execute the commands from the command buffer.
	// Empty the command buffer and wait until the GPU is idle.
	DWORD   drawn = 0xffffffff;
	while(pQuery->GetData(&drawn, sizeof(DWORD), D3DGETDATA_FLUSH) == S_FALSE)
		;	// none

	pQuery->Release();

	return drawn;
}

//------------------------------------------------------------------------------
//	�I�N���[�W�����N�G���擾
//------------------------------------------------------------------------------
DWORD CModel::GetOcclusionQuery(IEffect*& pEffect)
{
	IDirect3DQuery9*   pQuery;
	const HRESULT   hr = m_pD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION, &pQuery);
	if(hr != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �I�N���[�W�����N�G���擾���s(CModel_GetOcclusionQuery)\n"));
		return 0xffffffff;
	}

	pQuery->Issue(D3DISSUE_BEGIN);
	Draw(pEffect);
	pQuery->Issue(D3DISSUE_END  );

	// Force the driver to execute the commands from the command buffer.
	// Empty the command buffer and wait until the GPU is idle.
	DWORD   drawn = 0xffffffff;
	while(pQuery->GetData(&drawn, sizeof(DWORD), D3DGETDATA_FLUSH) == S_FALSE)
		;	// none

	pQuery->Release();

	return drawn;
}

//------------------------------------------------------------------------------
//	�v���O���b�V�u���[�h
//------------------------------------------------------------------------------
bool CModel::EnableProgressive()
{
	if(m_pPMesh != NULL) {
		::OutputDebugString(TEXT("*** Error - �v���O���b�V�u���[�h�ݒ�ς�(CModel_EnableProgressive)\n"));
		return true;
	}

	// �אږʐ���
	const DWORD    FACE = m_pMesh->GetNumFaces();
	DWORD*   pAdjacency = new DWORD[FACE * 3 * sizeof(DWORD)];
	if(m_pMesh->GenerateAdjacency(FLT_EPSILON, pAdjacency) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �אږʐ������s(CModel_EnableProgressive)\n"));
		delete[] pAdjacency;
		return false;
	}

	// ���b�V���N���[�j���O
	ID3DXMesh*   pCleanMesh;
	if(::D3DXCleanMesh(D3DXCLEAN_SIMPLIFICATION, m_pMesh, pAdjacency, &pCleanMesh, pAdjacency, NULL) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���b�V���N���[�j���O���s(CModel_EnableProgressive)\n"));
		delete[] pAdjacency;
		return false;
	}
	SafeRelease(m_pMesh);
	m_pMesh = pCleanMesh;

	// ���b�V������
//	::D3DXWeldVertices(m_pMesh, D3DXWELDEPSILONS_WELDALL, NULL, pAdjacency, pAdjacency, NULL, NULL);

	// �v���O���b�V�u���b�V������
	if(::D3DXGeneratePMesh(m_pMesh, pAdjacency, NULL, NULL, 1, D3DXMESHSIMP_FACE, &m_pPMesh) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �v���O���b�V�u���b�V���������s(CModel_EnableProgressive)\n"));
		delete[] pAdjacency;
		return false;
	}

	// �œK��
	m_pPMesh->SetNumFaces(m_pPMesh->GetMaxFaces());
	m_pPMesh->OptimizeBaseLOD(D3DXMESHOPT_COMPACT | D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE, NULL);

	m_pBaseMesh = m_pPMesh;

	delete[] pAdjacency;
	SafeRelease(m_pMesh);

	return true;
}

//------------------------------------------------------------------------------
//	�ڍ׃��x���ݒ�
//------------------------------------------------------------------------------
void CModel::SetLOD(const float inLOD)
{
	if(m_pPMesh == NULL) {
		::OutputDebugString(TEXT("*** Error - �v���O���b�V�u���b�V��������(CModel_SetLOD)\n"));
		return;
	}

	const DWORD   MaxFaces = m_pPMesh->GetMaxFaces();
	const DWORD   MinFaces = m_pPMesh->GetMinFaces();

	// �ڍ׃��x���ݒ�
	m_pPMesh->SetNumFaces((DWORD)(MinFaces + ((MaxFaces - MinFaces) * inLOD + 0.5f)));
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�}�b�v�ǂݍ���
//------------------------------------------------------------------------------
void CModel::LoadEffectMap(const DWORD inStage, LPCTSTR inFileName)
{
	// �L���[�u�e�N�X�`������
	IDirect3DTexture9*   pTexture;
	if(::D3DXCreateTextureFromFile(m_pD3DDevice, inFileName, &pTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CModel_LoadEffectMap)\n"));
		return;
	}

	// ���X�g�֒ǉ�
	m_EffectMap.push_back(EFFECTMAP(inStage, pTexture));
}

//------------------------------------------------------------------------------
//	�L���[�u�G�t�F�N�g�}�b�v�ǂݍ���
//------------------------------------------------------------------------------
void CModel::LoadCubeEffectMap(const DWORD inStage, LPCTSTR inFileName)
{
	// �L���[�u�e�N�X�`������
	IDirect3DCubeTexture9*   pCubeTexture;
	if(::D3DXCreateCubeTextureFromFile(m_pD3DDevice, inFileName, &pCubeTexture) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �L���[�u�e�N�X�`���������s(CModel_LoadCubeEffectMap)\n"));
		return;
	}

	// ���X�g�֒ǉ�
	m_EffectMap.push_back(EFFECTMAP(inStage, pCubeTexture));
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�}�b�v�K�p
//------------------------------------------------------------------------------
void CModel::ApplyEffectMap()
{
	// �G�t�F�N�g�e�N�X�`���ݒ�
	std::list<EFFECTMAP>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		m_pD3DDevice->SetTexture((*it).Stage, (*it).pTexture);
}

//------------------------------------------------------------------------------
//	�G�t�F�N�g�}�b�v����
//------------------------------------------------------------------------------
void CModel::RemoveEffectMap()
{
	// �G�t�F�N�g�e�N�X�`������
	std::list<EFFECTMAP>::iterator   it;
	for(it = m_EffectMap.begin(); it != m_EffectMap.end(); it++)
		m_pD3DDevice ->SetTexture((*it).Stage, NULL);
}
