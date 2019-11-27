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
	 StateBlock.hpp
				�X�e�[�g�u���b�N�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <d3d9.h>
#include <map>

//------------------------------------------------------------------------------
//	�X�e�[�g�u���b�N�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class IStateBlock {
public:
	virtual ~IStateBlock() {}

	virtual void SetRenderState (const D3DRENDERSTATETYPE inState, const DWORD inValue) = 0;
	virtual void SetStageState  (const DWORD inStage, const D3DTEXTURESTAGESTATETYPE inType,
								 const DWORD inValue) = 0;
	virtual void SetSamplerState(const DWORD inStage, const D3DSAMPLERSTATETYPE inType,
								 const DWORD inValue) = 0;

	inline void SetFloatRenderState(const D3DRENDERSTATETYPE inState, float inValue)
	{ SetRenderState(inState, *((DWORD*)&inValue)); }

	virtual void Apply()   = 0;

	virtual void Capture() = 0;
	virtual void Restore() = 0;
};

//------------------------------------------------------------------------------
//	�X�e�[�g�u���b�N�N���X��`
//------------------------------------------------------------------------------
class CStateBlock : public IStateBlock {
public:
	CStateBlock(IDirect3DDevice9* pD3DDevice);
	virtual ~CStateBlock();

	virtual void SetRenderState (const D3DRENDERSTATETYPE inState, const DWORD inValue);
	virtual void SetStageState  (const DWORD inStage, const D3DTEXTURESTAGESTATETYPE inType,
								 const DWORD inValue);
	virtual void SetSamplerState(const DWORD inStage, const D3DSAMPLERSTATETYPE      inType,
								 const DWORD inValue);

	virtual void Apply();

	virtual void Capture();
	virtual void Restore();

private:
	// �X�e�[�g�l�ۑ��\����
	struct STATE {
		DWORD   Value;		// �ݒ�l
		DWORD	Capture;	// �����l
	};

	IDirect3DDevice9*                     m_pD3DDevice;

	std::map<D3DRENDERSTATETYPE, STATE>   m_RenderState;

	std::map<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>   m_StageState;
	std::map<std::pair<DWORD, D3DSAMPLERSTATETYPE>,      STATE>   m_SamplerState;
};

//------------------------------------------------------------------------------
//	NULL�X�e�[�g�u���b�N�N���X��`
//------------------------------------------------------------------------------
class CNullStateBlock : public IStateBlock {
public:
	virtual ~CNullStateBlock() {}

	virtual void SetRenderState (const D3DRENDERSTATETYPE inState, const DWORD inValue) {}
	virtual void SetStageState  (const DWORD inStage, const D3DTEXTURESTAGESTATETYPE inType,
								 const DWORD inValue) {}
	virtual void SetSamplerState(const DWORD inStage, const D3DSAMPLERSTATETYPE inType,
								 const DWORD inValue) {}

	virtual void Apply()   {}

	virtual void Capture() {}
	virtual void Restore() {}
};
