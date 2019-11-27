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
	 VertexBuffer.hpp
				���_�o�b�t�@�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "VertexBuffer.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CVertexBuffer::CVertexBuffer(IDirect3DVertexBuffer9* pVertexBuffer,
							 const DWORD inFVF, const UINT inStride, const bool inWriteOnly)
	: m_pVertexBuffer(pVertexBuffer), m_pD3DDevice(NULL), m_pTexture(NULL),
	  m_FVF(inFVF), m_Stride(inStride),
	  m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f)
{
	assert(m_pVertexBuffer != NULL);
	m_pVertexBuffer->AddRef();

	m_pVertexBuffer->GetDevice(&m_pD3DDevice);
	assert(m_pD3DDevice != NULL);

	// �X�g���C�h�ݒ�
	if(m_Stride == 0)
		m_Stride = ::D3DXGetFVFVertexSize(m_FVF);

	//���b�N�t���O�ݒ�
	if(inWriteOnly == false)
		m_LockFlags = 0;
	else
		m_LockFlags = D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE;

	::ZeroMemory(&m_Material, sizeof(m_Material));
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CVertexBuffer::~CVertexBuffer()
{
	SafeRelease(m_pTexture);

	m_pD3DDevice   ->Release();
	m_pVertexBuffer->Release();
}

//------------------------------------------------------------------------------
//	���b�N
//------------------------------------------------------------------------------
LPVOID CVertexBuffer::Lock()
{
	LPVOID	pBuffer;
	if(m_pVertexBuffer->Lock(0, 0, &pBuffer, m_LockFlags) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���b�N���s(CVertexBuffer_Lock)\n"));
		return NULL;
	}

	return pBuffer;
}

//------------------------------------------------------------------------------
//	���b�N����
//------------------------------------------------------------------------------
void CVertexBuffer::Unlock()
{
	m_pVertexBuffer->Unlock();
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CVertexBuffer::Draw(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount)
{
	// �V�F�[�_�E�t�H�[�}�b�g�ݒ�
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// ���[���h�ϊ��s��ݒ�
	if((m_FVF & D3DFVF_XYZ) != 0)
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// �}�e���A���ݒ�
	if((m_FVF & D3DFVF_NORMAL) != 0)
		m_pD3DDevice->SetMaterial(&m_Material);

	// �e�N�X�`���ݒ�
	if((m_FVF & D3DFVF_TEX1) != 0)
		m_pD3DDevice->SetTexture(0, m_pTexture);

	// �`��
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);
	m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CVertexBuffer::Draw(IEffect*& pEffect, const D3DPRIMITIVETYPE inType,
						 const UINT inStart, const UINT inCount)
{
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);

	pEffect->SetMaterial(m_Material, m_pTexture);

	const UINT   pass = pEffect->Begin();
	for(UINT i = 0; i < pass; i++) {
		pEffect->BeginPass(i);

		m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

		pEffect->EndPass();
	}
	pEffect->End();
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CVertexBuffer::Draw(IEffect*& pEffect, const UINT inPass, const D3DPRIMITIVETYPE inType,
						 const UINT inStart, const UINT inCount)
{
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);
	pEffect->SetMaterial(m_Material, m_pTexture);

	pEffect->Begin();
	pEffect->BeginPass(inPass);

	m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

	pEffect->EndPass();
	pEffect->End();
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CVertexBuffer::DrawPass(IEffect*& pEffect, const UINT inPass, const D3DPRIMITIVETYPE inType,
							 const UINT inStart, const UINT inCount)
{
	pEffect->SetMaterial(m_Material, m_pTexture);

	pEffect->BeginPass(inPass);

	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);
	m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

	pEffect->EndPass();
}

//------------------------------------------------------------------------------
//	�`��
//------------------------------------------------------------------------------
void CVertexBuffer::DrawPure(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount)
{
	m_pD3DDevice->SetFVF(m_FVF);
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);
	m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);
}

//------------------------------------------------------------------------------
//	�������`��
//------------------------------------------------------------------------------
void CVertexBuffer::DrawAlpha(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							  const float inAlpha, const DWORD inDraw)
{
	if(inAlpha <= FLT_EPSILON)
		return;

	// �X�g���[������
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);

	// �V�F�[�_�E�t�H�[�}�b�g�ݒ�
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// ���[���h�ϊ��s��ݒ�
	if((m_FVF & D3DFVF_XYZ) != 0)
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// �}�e���A���ݒ�
	const float   ALPHA = m_Material.Diffuse.a;
	if((m_FVF & D3DFVF_NORMAL) != 0) {
		m_Material.Diffuse.a *= inAlpha;
		m_pD3DDevice->SetMaterial(&m_Material);
	}

	// �e�N�X�`���ݒ�
	if((m_FVF & D3DFVF_TEX1) != 0)
		m_pD3DDevice->SetTexture(0, m_pTexture);

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

		m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

		// ���x���o��
		if((inDraw & DXGDRAWA_BACKFACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

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

		m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

		// ���x���o��
		if((inDraw & DXGDRAWA_FACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

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

		m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

		// ���x���o��
		if((inDraw & DXGDRAWA_CULLNONE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

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

		m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);

		m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
	}

	// �}�e���A������
	m_Material.Diffuse.a = ALPHA;
}

//------------------------------------------------------------------------------
//	�e�`��
//------------------------------------------------------------------------------
void CVertexBuffer::DrawShadow(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount,
							   const D3DXMATRIX& inShadowMatrix, const float inDepth)
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

	// �V�F�[�_�E�t�H�[�}�b�g�ݒ�
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// �`��
	m_pD3DDevice->SetStreamSource(0, m_pVertexBuffer, 0, m_Stride);
	m_pD3DDevice->DrawPrimitive(inType, inStart, inCount);
}

//------------------------------------------------------------------------------
//	FVF�ݒ�
//------------------------------------------------------------------------------
void CVertexBuffer::SetFVF(const DWORD inFVF, const UINT inStride)
{
	m_FVF = inFVF;

	if(m_Stride != 0)
		m_Stride = inStride;
	else
		m_Stride = ::D3DXGetFVFVertexSize(m_FVF);
}

//------------------------------------------------------------------------------
//	���[���h�ϊ��s�񐶐�
//------------------------------------------------------------------------------
D3DXMATRIX CVertexBuffer::GetWorldMatrix() const
{
	// �N�H�[�^�j�I������
	D3DXQUATERNION   rotation;
	::D3DXQuaternionRotationYawPitchRoll(&rotation, D3DXToRadian(m_Rotation.y), 
													D3DXToRadian(m_Rotation.x),
													D3DXToRadian(m_Rotation.z));

	// ���[���h�s��ݒ�
	D3DXMATRIX   world;
	::D3DXMatrixTransformation(&world, NULL, NULL, &m_Scale, NULL, &rotation, &m_Position);

	return world;
}

//------------------------------------------------------------------------------
//	�e�N�X�`���ݒ�
//------------------------------------------------------------------------------
void CVertexBuffer::SetTexture(LPCTSTR inFileName)
{
	SafeRelease(m_pTexture);	// �Â��e�N�X�`���̉��

	//�e�N�X�`������
	if(::D3DXCreateTextureFromFile(m_pD3DDevice, inFileName, &m_pTexture) != D3D_OK)
		::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CVertexBuffer_SetTexture)\n"));
}

//------------------------------------------------------------------------------
//	�e�N�X�`���ݒ�
//------------------------------------------------------------------------------
void CVertexBuffer::SetTexture(IDirect3DTexture9* pTexture)
{
	SafeRelease(m_pTexture);	// �Â��e�N�X�`���̉��

	if(pTexture != NULL) {
		m_pTexture = pTexture;
		m_pTexture->AddRef();
	}
}

#ifdef UNICODE
//------------------------------------------------------------------------------
//	�}�e���A���ݒ�
//------------------------------------------------------------------------------
void CVertexBuffer::SetMaterial(const D3DXMATERIAL& inMaterial)
{
	// �t�@�C�����ϊ�
	WCHAR   wzFileName[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, inMaterial.pTextureFilename, -1, wzFileName, MAX_PATH);

	SetMaterial(inMaterial.MatD3D);
	SetTexture(wzFileName);
}
#endif

//------------------------------------------------------------------------------
//	�ړ�
//------------------------------------------------------------------------------
void CVertexBuffer::Move(const float inRight, const float inUp, const float inFront)
{
	// ��]�s�񐶐�
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationYawPitchRoll(&rotation, D3DXToRadian(m_Rotation.y), 
												D3DXToRadian(m_Rotation.x),
												D3DXToRadian(m_Rotation.z));

	// �����x�N�g�������o��
	D3DXVECTOR3   right(rotation._11, rotation._12, rotation._13);
	D3DXVECTOR3   up   (rotation._21, rotation._22, rotation._23);
	D3DXVECTOR3   front(rotation._31, rotation._32, rotation._33);

	// �ړ�
	right      *= inRight;
	up         *= inUp;
	front      *= inFront;
	m_Position += front + up + right;
}

//------------------------------------------------------------------------------
//	��]
//------------------------------------------------------------------------------
void CVertexBuffer::Rotation(const float inX, const float inY, const float inZ)
{
	m_Rotation.x += inX;
	m_Rotation.y += inY;
	m_Rotation.z += inZ;
}

//------------------------------------------------------------------------------
//	���E������
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CVertexBuffer::GetBoundingSphere()
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
DXGBOUNDINGSPHERE CVertexBuffer::GetBaseBoundingSphere()
{
	DXGBOUNDINGSPHERE   shpere;

	// ���_���擾
	D3DVERTEXBUFFER_DESC   desc;
	if(m_pVertexBuffer->GetDesc(&desc) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���_���擾���s(VertexBuffer_GetBaseBoundingSphere)\n"));
		return shpere;
	}

	// ���_�o�b�t�@���b�N
	void*   pVertices;
	if(m_pVertexBuffer->Lock(0, 0, &pVertices, 0) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���_�o�b�t�@���b�N���s(VertexBuffer_GetBaseBoundingSphere)\n"));
		return shpere;
	}

	// ���E������
	::D3DXComputeBoundingSphere((D3DXVECTOR3*)pVertices,
								desc.Size / m_Stride, m_Stride,
								&shpere.Center, &shpere.Radius);

	m_pVertexBuffer->Unlock();

	return shpere;
}

//------------------------------------------------------------------------------
//	OBB����
//------------------------------------------------------------------------------
DXGOBB CVertexBuffer::GetOBB()
{
	DXGOBB   obb(GetBaseOBB());

	// ���S���W�ݒ�
	obb.Center += m_Position;

	// �e���̌X���ݒ�
	D3DXMATRIX   rot;
	::D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
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
DXGOBB CVertexBuffer::GetBaseOBB()
{
	DXGOBB   obb;

	// ���_���擾
	D3DVERTEXBUFFER_DESC   desc;
	if(m_pVertexBuffer->GetDesc(&desc) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���_���擾���s(VertexBuffer_GetOBB)\n"));
		return obb;
	}

	// ���_�o�b�t�@���b�N
	BYTE*   pVertices;
	if(m_pVertexBuffer->Lock(0, 0, (LPVOID*)&pVertices, 0) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - ���_�o�b�t�@���b�N���s(VertexBuffer_GetOBB)\n"));
		return obb;
	}

	// �e���̍ő�A�ŏ����W�����߂�
	D3DXVECTOR3   pos_max(FLT_MIN, FLT_MIN, FLT_MIN);
	D3DXVECTOR3   pos_min(FLT_MAX, FLT_MAX, FLT_MAX);

	const int   NumVertices = desc.Size / m_Stride;
	for(int i = 0; i < NumVertices; i++) {
		D3DXVECTOR3   pos = *(D3DXVECTOR3*)pVertices;
		if(pos.x < pos_min.x)	pos_min.x = pos.x;
		if(pos.x > pos_max.x)	pos_max.x = pos.x;
		if(pos.y < pos_min.y)	pos_min.y = pos.y;
		if(pos.y > pos_max.y)	pos_max.y = pos.y;
		if(pos.z < pos_min.z)	pos_min.z = pos.z;
		if(pos.z > pos_max.z)	pos_max.z = pos.z;

		pVertices += m_Stride;
	}

	m_pVertexBuffer->Unlock();

	// ���S�_�ݒ�
	obb.Center = (pos_max + pos_min) * 0.5f;

	// �e���̒����̔�����ݒ�
	obb.Radius.x = ::fabsf(pos_max.x - pos_min.x) * 0.5f;
	obb.Radius.y = ::fabsf(pos_max.y - pos_min.y) * 0.5f;
	obb.Radius.z = ::fabsf(pos_max.z - pos_min.z) * 0.5f;

	return obb;
}

//------------------------------------------------------------------------------
//	�R���X�g���N�^�|���[�U�[�|�C���^
//------------------------------------------------------------------------------
CVertexBufferUP::CVertexBufferUP(IDirect3DDevice9* pD3DDevice,
								 void* pVertexBuffer, const DWORD inBufferSize,
								 const DWORD inFVF,   const UINT  inStride)
	: m_pVertexBuffer(pVertexBuffer), m_pD3DDevice(pD3DDevice), m_pTexture(NULL),
	  m_BufferSize(inBufferSize), m_FVF(inFVF), m_Stride(inStride),
	  m_Position(0.0f, 0.0f, 0.0f), m_Scale(1.0f, 1.0f, 1.0f), m_Rotation(0.0f, 0.0f, 0.0f)
{
	assert(m_pVertexBuffer != NULL && m_BufferSize > 0);
	assert(m_pD3DDevice    != NULL);

	m_pD3DDevice->AddRef();

	::ZeroMemory(m_pVertexBuffer, m_BufferSize);
	::ZeroMemory(&m_Material, sizeof(m_Material));

	// �X�g���C�h�ݒ�
	if(m_Stride == 0)
		m_Stride = ::D3DXGetFVFVertexSize(m_FVF);
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^�|���[�U�[�|�C���^
//------------------------------------------------------------------------------
CVertexBufferUP::~CVertexBufferUP()
{
	delete[] m_pVertexBuffer;

	SafeRelease(m_pTexture);
	m_pD3DDevice->Release();
}

//------------------------------------------------------------------------------
//	�`��|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::Draw(const D3DPRIMITIVETYPE inType, const UINT inStart, const UINT inCount)
{
	// �V�F�[�_�E�t�H�[�}�b�g�ݒ�
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// ���[���h�ϊ��s��ݒ�
	if((m_FVF & D3DFVF_XYZ) != 0)
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// �}�e���A���ݒ�
	if((m_FVF & D3DFVF_NORMAL) != 0)
		m_pD3DDevice->SetMaterial(&m_Material);

	// �e�N�X�`���ݒ�
	if((m_FVF & D3DFVF_TEX1) != 0)
		m_pD3DDevice->SetTexture(0, m_pTexture);

	// �`��
	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);
}

//------------------------------------------------------------------------------
//	�`��|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::Draw(IEffect*& pEffect, const D3DPRIMITIVETYPE inType,
						   const UINT inStart, const UINT inCount)
{
	pEffect->SetMaterial(m_Material, m_pTexture);

	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	const UINT    pass   = pEffect->Begin();
	for(UINT i = 0; i < pass; i++) {
		pEffect->BeginPass(i);

		m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

		pEffect->EndPass();
	}
	pEffect->End();
}

//------------------------------------------------------------------------------
//	�`��|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::Draw(IEffect*& pEffect, const UINT inPass,
						   const D3DPRIMITIVETYPE inType,
						   const UINT inStart, const UINT inCount)
{
	pEffect->SetMaterial(m_Material, m_pTexture);

	pEffect->Begin();
	pEffect->BeginPass(inPass);

	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

	pEffect->EndPass();
	pEffect->End();
}

//------------------------------------------------------------------------------
//	�`��|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::DrawPass(IEffect*& pEffect, const UINT inPass,
							   const D3DPRIMITIVETYPE inType,
							   const UINT inStart, const UINT inCount)
{
	pEffect->SetMaterial(m_Material, m_pTexture);

	pEffect->BeginPass(inPass);

	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

	pEffect->EndPass();
}

//------------------------------------------------------------------------------
//	�`��|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::DrawPure(const D3DPRIMITIVETYPE inType,
							   const UINT inStart, const UINT inCount)
{
	m_pD3DDevice->SetFVF(m_FVF);

	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);
}

//------------------------------------------------------------------------------
//	�������`��|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::DrawAlpha(const D3DPRIMITIVETYPE inType,
								const UINT  inStart, const UINT  inCount,
								const float inAlpha, const DWORD inDraw)
{
	if(inAlpha <= FLT_EPSILON)
		return;

	// �V�F�[�_�E�t�H�[�}�b�g�ݒ�
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// ���[���h�ϊ��s��ݒ�
	if((m_FVF & D3DFVF_XYZ) != 0)
		m_pD3DDevice->SetTransform(D3DTS_WORLD, &GetWorldMatrix());

	// �}�e���A���ݒ�
	const float   ALPHA = m_Material.Diffuse.a;
	if((m_FVF & D3DFVF_NORMAL) != 0) {
		m_Material.Diffuse.a *= inAlpha;
		m_pD3DDevice->SetMaterial(&m_Material);
	}

	// �e�N�X�`���ݒ�
	if((m_FVF & D3DFVF_TEX1) != 0)
		m_pD3DDevice->SetTexture(0, m_pTexture);

	// �J���[�o�͕ۑ�
	DWORD   colorwrite_enable;
	m_pD3DDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite_enable);

	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;

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

		m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

		// ���x���o��
		if((inDraw & DXGDRAWA_BACKFACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

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

		m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

		// ���x���o��
		if((inDraw & DXGDRAWA_FACE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

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

		m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

		// ���x���o��
		if((inDraw & DXGDRAWA_CULLNONE_ZWRDEFER) != 0) {
			m_pD3DDevice->SetRenderState(D3DRS_ZENABLE,       TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE,  TRUE);
			m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

			m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

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

		m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);

		m_pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite_enable);
	}

	// �}�e���A������
	m_Material.Diffuse.a = ALPHA;
}

//------------------------------------------------------------------------------
//	�e�`��|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::DrawShadow(const D3DPRIMITIVETYPE inType,
								 const UINT inStart, const UINT inCount,
								 const D3DXMATRIX& inShadowMatrix, const float inDepth)
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

	// �V�F�[�_�E�t�H�[�}�b�g�ݒ�
	m_pD3DDevice->SetVertexShader(NULL);
	m_pD3DDevice->SetPixelShader (NULL);
	m_pD3DDevice->SetFVF(m_FVF);

	// �`��
	const void*   buffer = (BYTE*)m_pVertexBuffer + m_Stride * inStart;
	m_pD3DDevice->DrawPrimitiveUP(inType, inCount, buffer, m_Stride);
}

//------------------------------------------------------------------------------
//	FVF�ݒ�|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::SetFVF(const DWORD inFVF, const UINT inStride)
{
	m_FVF = inFVF;

	if(m_Stride != 0)
		m_Stride = inStride;
	else
		m_Stride = ::D3DXGetFVFVertexSize(m_FVF);
}

//------------------------------------------------------------------------------
//	���[���h�ϊ��s�񐶐��|���[�U�[�|�C���^
//------------------------------------------------------------------------------
D3DXMATRIX CVertexBufferUP::GetWorldMatrix() const
{
	// �N�H�[�^�j�I������
	D3DXQUATERNION   rotation;
	::D3DXQuaternionRotationYawPitchRoll(&rotation, D3DXToRadian(m_Rotation.y), 
													D3DXToRadian(m_Rotation.x),
													D3DXToRadian(m_Rotation.z));

	// ���[���h�s��ݒ�
	D3DXMATRIX   world;
	::D3DXMatrixTransformation(&world, NULL, NULL, &m_Scale, NULL, &rotation, &m_Position);

	return world;
}

//------------------------------------------------------------------------------
//	�e�N�X�`���ݒ�|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::SetTexture(LPCTSTR inFileName)
{
	SafeRelease(m_pTexture);	// �Â��e�N�X�`���̉��

	//�e�N�X�`������
	if(::D3DXCreateTextureFromFile(m_pD3DDevice, inFileName, &m_pTexture) != D3D_OK)
		::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CVertexBufferUP_SetTexture)\n"));
}

//------------------------------------------------------------------------------
//	�e�N�X�`���ݒ�|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::SetTexture(IDirect3DTexture9* pTexture)
{
	SafeRelease(m_pTexture);	// �Â��e�N�X�`���̉��

	if(pTexture != NULL) {
		m_pTexture = pTexture;
		m_pTexture->AddRef();
	}
}

#ifdef UNICODE
//------------------------------------------------------------------------------
//	�}�e���A���ݒ�|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::SetMaterial(const D3DXMATERIAL& inMaterial)
{
	// �t�@�C�����ϊ�
	WCHAR   wzFileName[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, inMaterial.pTextureFilename, -1, wzFileName, MAX_PATH);

	SetMaterial(inMaterial.MatD3D);
	SetTexture(wzFileName);
}
#endif

//------------------------------------------------------------------------------
//	�ړ��|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::Move(const float inRight, const float inUp, const float inFront)
{
	// ��]�s�񐶐�
	D3DXMATRIX   rotation;
	::D3DXMatrixRotationYawPitchRoll(&rotation, D3DXToRadian(m_Rotation.y), 
												D3DXToRadian(m_Rotation.x),
												D3DXToRadian(m_Rotation.z));

	// �����x�N�g�������o��
	D3DXVECTOR3   right(rotation._11, rotation._12, rotation._13);
	D3DXVECTOR3   up   (rotation._21, rotation._22, rotation._23);
	D3DXVECTOR3   front(rotation._31, rotation._32, rotation._33);

	// �ړ�
	right      *= inRight;
	up         *= inUp;
	front      *= inFront;
	m_Position += front + up + right;
}

//------------------------------------------------------------------------------
//	��]�|���[�U�[�|�C���^
//------------------------------------------------------------------------------
void CVertexBufferUP::Rotation(const float inX, const float inY, const float inZ)
{
	m_Rotation.x += inX;
	m_Rotation.y += inY;
	m_Rotation.z += inZ;
}

//------------------------------------------------------------------------------
//	���E�������|���[�U�[�|�C���^
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CVertexBufferUP::GetBoundingSphere()
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
//	��{���E�������|���[�U�[�|�C���^
//------------------------------------------------------------------------------
DXGBOUNDINGSPHERE CVertexBufferUP::GetBaseBoundingSphere()
{
	DXGBOUNDINGSPHERE   shpere;

	// ���E������
	::D3DXComputeBoundingSphere((D3DXVECTOR3*)m_pVertexBuffer,
								m_BufferSize / m_Stride, m_Stride,
								&shpere.Center, &shpere.Radius);

	return shpere;
}

//------------------------------------------------------------------------------
//	OBB�����|���[�U�[�|�C���^
//------------------------------------------------------------------------------
DXGOBB CVertexBufferUP::GetOBB()
{
	DXGOBB   obb(GetBaseOBB());

	// ���S���W�ݒ�
	obb.Center += m_Position;

	// �e���̌X���ݒ�
	D3DXMATRIX   rot;
	::D3DXMatrixRotationYawPitchRoll(&rot, m_Rotation.y, m_Rotation.x, m_Rotation.z);
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
//	��{OBB�����|���[�U�[�|�C���^
//------------------------------------------------------------------------------
DXGOBB CVertexBufferUP::GetBaseOBB()
{
	DXGOBB   obb;
	BYTE*    pVertices = (BYTE*)m_pVertexBuffer;

	// �e���̍ő�A�ŏ����W�����߂�
	D3DXVECTOR3   pos_max(FLT_MIN, FLT_MIN, FLT_MIN);
	D3DXVECTOR3   pos_min(FLT_MAX, FLT_MAX, FLT_MAX);

	const int   NumVertices = m_BufferSize / m_Stride;
	for(int i = 0; i < NumVertices; i++) {
		D3DXVECTOR3   pos = *(D3DXVECTOR3*)pVertices;
		if(pos.x < pos_min.x)	pos_min.x = pos.x;
		if(pos.x > pos_max.x)	pos_max.x = pos.x;
		if(pos.y < pos_min.y)	pos_min.y = pos.y;
		if(pos.y > pos_max.y)	pos_max.y = pos.y;
		if(pos.z < pos_min.z)	pos_min.z = pos.z;
		if(pos.z > pos_max.z)	pos_max.z = pos.z;

		pVertices += m_Stride;
	}

	// ���S�_�ݒ�
	obb.Center = (pos_max + pos_min) * 0.5f;

	// �e���̒����̔�����ݒ�
	obb.Radius.x = ::fabsf(pos_max.x - pos_min.x) * 0.5f;
	obb.Radius.y = ::fabsf(pos_max.y - pos_min.y) * 0.5f;
	obb.Radius.z = ::fabsf(pos_max.z - pos_min.z) * 0.5f;

	return obb;
}
