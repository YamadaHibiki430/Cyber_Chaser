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
				�X�e�[�g�u���b�N�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "StateBlock.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CStateBlock::CStateBlock(IDirect3DDevice9* pD3DDevice) : m_pD3DDevice(pD3DDevice)
{
	assert(m_pD3DDevice != NULL);
	m_pD3DDevice->AddRef();
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CStateBlock::~CStateBlock()
{
	m_pD3DDevice->Release();
}

//------------------------------------------------------------------------------
//	�����_�����O�X�e�[�g�ݒ�
//------------------------------------------------------------------------------
void CStateBlock::SetRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue)
{
	// �}�b�v�ɓo�^����Ă��邩���ׂ�
	std::map<D3DRENDERSTATETYPE, STATE>::iterator it;
	it = m_RenderState.find(inState);
	if(it == m_RenderState.end()) {
		// ���o�^
		STATE   state;
		state.Value   = inValue;
		state.Capture = inValue;
		m_RenderState.insert(std::pair<D3DRENDERSTATETYPE, STATE>(inState, state));
	} else{
		it->second.Value = inValue;
	}
}

//------------------------------------------------------------------------------
//	�e�N�X�`���X�e�[�W�X�e�[�g�ݒ�
//------------------------------------------------------------------------------
void CStateBlock::SetStageState(const DWORD inStage, const D3DTEXTURESTAGESTATETYPE inType,
								const DWORD inValue)
{
	// �}�b�v�ɓo�^����Ă��邩���ׂ�
	std::map<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>::iterator it;
	it = m_StageState.find(std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>(inStage, inType));
	if(it == m_StageState.end()) {
		// ���o�^
		STATE   state;
		state.Value   = inValue;
		state.Capture = inValue;
		m_StageState.insert( std::pair<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>
							(std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>(inStage, inType), state) );
	} else{
		it->second.Value = inValue;
	}
}

//------------------------------------------------------------------------------
//	�T���v���[�X�e�[�g�ݒ�
//------------------------------------------------------------------------------
void CStateBlock::SetSamplerState(const DWORD inStage, const D3DSAMPLERSTATETYPE inType,
								  const DWORD inValue)
{
	// �}�b�v�ɓo�^����Ă��邩���ׂ�
	std::map<std::pair<DWORD, D3DSAMPLERSTATETYPE>, STATE>::iterator it;
	it = m_SamplerState.find(std::pair<DWORD, D3DSAMPLERSTATETYPE>(inStage, inType));
	if(it == m_SamplerState.end()) {
		// ���o�^
		STATE   state;
		state.Value   = inValue;
		state.Capture = inValue;
		m_SamplerState.insert( std::pair<std::pair<DWORD, D3DSAMPLERSTATETYPE>, STATE>
							  (std::pair<DWORD, D3DSAMPLERSTATETYPE>(inStage, inType), state) );
	} else{
		it->second.Value = inValue;
	}
}

//------------------------------------------------------------------------------
//	�X�e�[�g�K�p
//------------------------------------------------------------------------------
void CStateBlock::Apply()
{
	// �����_�����O�X�e�[�g
	std::map<D3DRENDERSTATETYPE, STATE>::iterator   rit;
	for(rit = m_RenderState.begin(); rit != m_RenderState.end(); rit++)
		m_pD3DDevice->SetRenderState(rit->first, rit->second.Value);

	// �e�N�X�`���X�e�[�W�X�e�[�g
	std::map<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>::iterator   tit;
	for(tit = m_StageState.begin(); tit != m_StageState.end(); tit++)
		m_pD3DDevice->SetTextureStageState(tit->first.first, tit->first.second, tit->second.Value);

	// �T���v���[�X�e�[�g
	std::map<std::pair<DWORD, D3DSAMPLERSTATETYPE>, STATE>::iterator   sit;
	for(sit = m_SamplerState.begin(); sit != m_SamplerState.end(); sit++)
		m_pD3DDevice->SetSamplerState(sit->first.first, sit->first.second, sit->second.Value);
}

//------------------------------------------------------------------------------
//	�X�e�[�g���ݒl�ۑ�
//------------------------------------------------------------------------------
void CStateBlock::Capture()
{
	// �����_�����O�X�e�[�g
	std::map<D3DRENDERSTATETYPE, STATE>::iterator   rit;
	for(rit = m_RenderState.begin(); rit != m_RenderState.end(); rit++)
		m_pD3DDevice->GetRenderState(rit->first, &rit->second.Capture);

	// �e�N�X�`���X�e�[�W�X�e�[�g
	std::map<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>::iterator   tit;
	for(tit = m_StageState.begin(); tit != m_StageState.end(); tit++)
		m_pD3DDevice->GetTextureStageState(tit->first.first, tit->first.second, &tit->second.Capture);

	// �T���v���[�X�e�[�g
	std::map<std::pair<DWORD, D3DSAMPLERSTATETYPE>, STATE>::iterator   sit;
	for(sit = m_SamplerState.begin(); sit != m_SamplerState.end(); sit++)
		m_pD3DDevice->GetSamplerState(sit->first.first, sit->first.second, &sit->second.Capture);
}

//------------------------------------------------------------------------------
//	�X�e�[�g����
//------------------------------------------------------------------------------
void CStateBlock::Restore()
{
	// �T���v���[�X�e�[�g
	std::map<std::pair<DWORD, D3DSAMPLERSTATETYPE>, STATE>::iterator   sit;
	for(sit = m_SamplerState.begin(); sit != m_SamplerState.end(); sit++)
		m_pD3DDevice->SetSamplerState(sit->first.first, sit->first.second, sit->second.Capture);

	// �e�N�X�`���X�e�[�W�X�e�[�g
	std::map<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>::iterator   tit;
	for(tit = m_StageState.begin(); tit != m_StageState.end(); tit++)
		m_pD3DDevice->SetTextureStageState(tit->first.first, tit->first.second, tit->second.Capture);

	// �����_�����O�X�e�[�g
	std::map<D3DRENDERSTATETYPE, STATE>::iterator   rit;
	for(rit = m_RenderState.begin(); rit != m_RenderState.end(); rit++)
		m_pD3DDevice->SetRenderState(rit->first, rit->second.Capture);
}
