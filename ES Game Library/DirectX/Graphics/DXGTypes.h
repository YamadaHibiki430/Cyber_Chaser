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
	 DXGTypes.h
				DirectX Graphics�^��`

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <d3dx9.h>

//------------------------------------------------------------------------------
//	�萔
//------------------------------------------------------------------------------
// FVF�t���O
enum { DXGFVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1,
	   DXGFVF_LVERTEX  = D3DFVF_XYZ    | D3DFVF_DIFFUSE | D3DFVF_TEX1,
	   DXGFVF_VERTEX   = D3DFVF_XYZ    | D3DFVF_NORMAL  | D3DFVF_TEX1 };

// �u�����f�B���O���[�h
enum DXGBLENDMODE {
	DXGBLEND_MODULATE,		// ��Z����
	DXGBLEND_ADD,			// ���Z����
	DXGBLEND_SUBTRACT,		// ���Z����
	DXGBLEND_SRCCOLOR,
	DXGBLEND_ZEROCOLOR,
	DXGBLEND_NORMAL = DXGBLEND_MODULATE,
	DXGBLEND_NONE   = -1	// �u�����f�B���O�Ȃ�
};

//------------------------------------------------------------------------------
//	�\���̒�`
//------------------------------------------------------------------------------
// �g�����X�t�H�[���ς݃��C�e�B���O�ς݂̒��_
struct DXGTLVERTEX {
	float      x, y, z;			// ���_���W
	float      rhw;				// RHW
	D3DCOLOR   color;			// ���_�F
	float      tu, tv;			// �e�N�X�`�����W

	DXGTLVERTEX() : x(0.0f), y(0.0f), z(0.0f), rhw(1.0f), color(0), tu(0.0f), tv(0.0f) {}
	static inline DWORD FVF() { return DXGFVF_TLVERTEX; }
};

// ���g�����X�t�H�[�����C�e�B���O�ς݂̒��_
struct DXGLVERTEX {
	float      x, y, z;			// ���_���W
	D3DCOLOR   color;			// ���_�F
	float      tu, tv;			// �e�N�X�`�����W

	DXGLVERTEX() : x(0.0f), y(0.0f), z(0.0f), color(0), tu(0.0f), tv(0.0f) {}
	static inline DWORD FVF() { return DXGFVF_LVERTEX; }
};

// ���g�����X�t�H�[�������C�e�B���O�̒��_
struct DXGVERTEX {
	float      x, y, z;			// ���_���W
	float      nx, ny, nz;		// �@���x�N�g��
	float      tu, tv;			// �e�N�X�`�����W

	DXGVERTEX() : x(0.0f), y(0.0f), z(0.0f), nx(0.0f), ny(0.0f), nz(0.0f), tu(0.0f), tv(0.0f) {}
	static inline DWORD FVF() { return DXGFVF_VERTEX; }
};

//------------------------------------------------------------------------------
//	�񋓑̒�`
//------------------------------------------------------------------------------
// �@���v�Z
enum DXGCOMPUTENORMAL {
	DXGCOMPUTE_DONOT_INPLACE = -1,	// ���̓��f�������̂܂܎g�p
	DXGCOMPUTE_NONE          =  0,	// �Ȃ�
	DXGCOMPUTE_NORMAL        =  1,	// �@��
	DXGCOMPUTE_TANGENT       =  2,	// �ڐ�
	DXGCOMPUTE_BINORMAL      =  4,	// �]�@��
	DXGCOMPUTE_NORMALTANGENT         = DXGCOMPUTE_NORMAL  | DXGCOMPUTE_TANGENT,
	DXGCOMPUTE_NORMALBINORMAL        = DXGCOMPUTE_NORMAL  | DXGCOMPUTE_BINORMAL,
	DXGCOMPUTE_TANGENTBINORMAL       = DXGCOMPUTE_TANGENT | DXGCOMPUTE_BINORMAL,
	DXGCOMPUTE_NORMALTANGENTBINORMAL = DXGCOMPUTE_NORMAL  | DXGCOMPUTE_TANGENT | DXGCOMPUTE_BINORMAL,
	DXGCOMPUTE_ALL                   = DXGCOMPUTE_NORMALTANGENTBINORMAL
};

// �V�F�C�v�^�C�v
enum DXGSHAPETYPE {
	DXGSHAPE_BOX = 1,		// ������
	DXGSHAPE_CYLINDER,		// �~���A�~��
	DXGSHAPE_POLYGON,		// ���p�`
	DXGSHAPE_SPHERE,		// ��
	DXGSHAPE_TORUS,			// �g�[���X
	DXGSHAPE_TEAPOT			// �e�B�[�|�b�g
};

// �A���t�@�u�����h�`��t���O
enum DXGDRAWALPHA {
	// �`��t���O
	// �\��
	DXGDRAWA_FACE                = 0x0008,	// �\�ʕ`��
	DXGDRAWA_FACE_ZDISABLE       = 0x0004,	// ���Ȃ��@�@(����r�Ȃ�)
	DXGDRAWA_FACE_ZWRDISABLE     = 0x0002,	// �������Ȃ�(����r����)
	DXGDRAWA_FACE_ZWRDEFER       = 0x0001,	// ���x������
	// �w��
	DXGDRAWA_BACKFACE            = 0x0080,
	DXGDRAWA_BACKFACE_ZDISABLE   = 0x0040,
	DXGDRAWA_BACKFACE_ZWRDISABLE = 0x0020,
	DXGDRAWA_BACKFACE_ZWRDEFER   = 0x0010,
	// �J�����O�Ȃ�
	DXGDRAWA_CULLNONE            = 0x0800,
	DXGDRAWA_CULLNONE_ZDISABLE   = 0x0400,
	DXGDRAWA_CULLNONE_ZWRDISABLE = 0x0200,
	DXGDRAWA_CULLNONE_ZWRDEFER   = 0x0100,
	// ���o��
	DXGDRAWA_ZWRITE_FACE         = 0x1000,	// �Ō�ɕ\�ʂ̂��o��
	DXGDRAWA_ZWRITE_BACKFACE     = 0x2000,	// �Ō�ɗ��ʂ̂��o��
	DXGDRAWA_ZWRITE_CULLNONE     = 0x3000,	// �Ō�ɗ��ʂ̂��o��
	// �����t���O
	DXGDRAWA_FACE2               = DXGDRAWA_FACE      | DXGDRAWA_FACE_ZWRDEFER,
	DXGDRAWA_FACE3               = DXGDRAWA_FACE2     | DXGDRAWA_FACE_ZDISABLE,
	DXGDRAWA_BACKFACE2           = DXGDRAWA_BACKFACE  | DXGDRAWA_BACKFACE_ZWRDEFER,
	DXGDRAWA_BACKFACE3           = DXGDRAWA_BACKFACE2 | DXGDRAWA_BACKFACE_ZDISABLE,
	DXGDRAWA_CULLNONE2           = DXGDRAWA_CULLNONE  | DXGDRAWA_CULLNONE_ZWRDEFER,
	DXGDRAWA_CULLNONE3           = DXGDRAWA_CULLNONE2 | DXGDRAWA_CULLNONE_ZDISABLE,
	DXGDRAWA_BOTHFACES           = DXGDRAWA_FACE      | DXGDRAWA_BACKFACE,
	DXGDRAWA_BOTHFACES2          = DXGDRAWA_FACE      | DXGDRAWA_FACE_ZWRDISABLE     |
								   DXGDRAWA_BACKFACE  | DXGDRAWA_BACKFACE_ZWRDISABLE | DXGDRAWA_ZWRITE_CULLNONE,
	DXGDRAWA_BOTHFACES3          = DXGDRAWA_FACE      | DXGDRAWA_BACKFACE3,
	DXGDRAWA_ALLFACES            = DXGDRAWA_CULLNONE2 | DXGDRAWA_CULLNONE_ZDISABLE,
};

//------------------------------------------------------------------------------
//	�\���̒�`
//------------------------------------------------------------------------------
// �V���v���V�F�C�v�\����
struct DXGSIMPLESHAPE {
	DXGSHAPETYPE   Type;			// �`��
	float		   Width;			// �����̂̕�
	float		   Height;			// �����̂̍���
	union{
		float	   Depth;			// �����̂̉��s��
		float	   Length;			// �~���A���p�`�̕ӂ̒���
	};
	union{
		float	   Radius;			// ���̔��a
		float	   Radius1;			// �~���̐擪�̔��a
		float	   InnerRadius;		// �g�[���X�̓����̔��a
	};
	union{
		float	   Radius2;			// �~���̌���̔��a
		float	   OuterRadius;		// �g�[���X�̊O���̔��a
	};
	union{
		UINT	   Sides;			// ���p�`�A�g�[���X�̉��f�ʂ̕ӂ̐�
		UINT	   Slices;			// �~���A���̉��f�ʂ̕ӂ̐�
	};
	union{
		UINT	   Stacks;			// ���f�ʂ̐�
		UINT	   Rings;			// �g�[���X���\������̐�
	};
};

// ���b�N���\����
struct DXGLOCKED_DESC {
	D3DFORMAT   Format;
    UINT        Width;
    UINT        Height;
	int         Pitch;
	void*       pBits;
};

// ���C
struct DXGRAY {
	D3DXVECTOR3   Position;
	D3DXVECTOR3   Direction;

	DXGRAY() : Position(0.0f, 0.0f, 0.0f), Direction(0.0f, 0.0f, 0.0f) {}
	DXGRAY(const D3DXVECTOR3& pos, const D3DXVECTOR3& dir) : Position(pos), Direction(dir) {}
};

// ���E��
struct DXGBOUNDINGSPHERE {
	D3DXVECTOR3   Center;
	float         Radius;

	DXGBOUNDINGSPHERE() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f) {}
	DXGBOUNDINGSPHERE(const D3DXVECTOR3& center, const float radius)
		: Center(center), Radius(radius) {}
};

// OBB(Oriented Bounding Box)
struct DXGOBB {
	D3DXVECTOR3   Center;
	D3DXVECTOR3   Axis[3];
	D3DXVECTOR3   Radius;

	DXGOBB() : Center(0.0f, 0.0f, 0.0f), Radius(0.0f, 0.0f, 0.0f)
	{
		Axis[0] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		Axis[1] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		Axis[2] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	}

	DXGOBB(const DXGOBB& obb) : Center(obb.Center), Radius(obb.Radius)
	{
		Axis[0] = obb.Axis[0];
		Axis[1] = obb.Axis[1];
		Axis[2] = obb.Axis[2];
	}

	inline DXGOBB& operator =(const DXGOBB& obb)
	{
		Center  = obb.Center;

		Axis[0] = obb.Axis[0];
		Axis[1] = obb.Axis[1];
		Axis[2] = obb.Axis[2];

		Radius  = obb.Radius;

		return *this;
	}
};

// ������
struct DXGVIEWFRUSTUM {
	D3DXPLANE   Left;
	D3DXPLANE   Right;
	D3DXPLANE   Top;
	D3DXPLANE   Bottom;
	D3DXPLANE   Near;
	D3DXPLANE   Far;
};
