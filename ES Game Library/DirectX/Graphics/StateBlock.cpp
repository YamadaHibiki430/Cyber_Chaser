/*
================================================================================
					 オブジェクト指向＆＆ゲームプログラミング
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 【対象ＯＳ】
     Microsoft Windows 7以降

 【コンパイラ】
     Microsoft VisualC++ 2013

 【プログラム】
	 StateBlock.hpp
				ステートブロッククラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "StateBlock.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	コンストラクタ
//------------------------------------------------------------------------------
CStateBlock::CStateBlock(IDirect3DDevice9* pD3DDevice) : m_pD3DDevice(pD3DDevice)
{
	assert(m_pD3DDevice != NULL);
	m_pD3DDevice->AddRef();
}

//------------------------------------------------------------------------------
//	デストラクタ
//------------------------------------------------------------------------------
CStateBlock::~CStateBlock()
{
	m_pD3DDevice->Release();
}

//------------------------------------------------------------------------------
//	レンダリングステート設定
//------------------------------------------------------------------------------
void CStateBlock::SetRenderState(const D3DRENDERSTATETYPE inState, const DWORD inValue)
{
	// マップに登録されているか調べる
	std::map<D3DRENDERSTATETYPE, STATE>::iterator it;
	it = m_RenderState.find(inState);
	if(it == m_RenderState.end()) {
		// 未登録
		STATE   state;
		state.Value   = inValue;
		state.Capture = inValue;
		m_RenderState.insert(std::pair<D3DRENDERSTATETYPE, STATE>(inState, state));
	} else{
		it->second.Value = inValue;
	}
}

//------------------------------------------------------------------------------
//	テクスチャステージステート設定
//------------------------------------------------------------------------------
void CStateBlock::SetStageState(const DWORD inStage, const D3DTEXTURESTAGESTATETYPE inType,
								const DWORD inValue)
{
	// マップに登録されているか調べる
	std::map<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>::iterator it;
	it = m_StageState.find(std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>(inStage, inType));
	if(it == m_StageState.end()) {
		// 未登録
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
//	サンプラーステート設定
//------------------------------------------------------------------------------
void CStateBlock::SetSamplerState(const DWORD inStage, const D3DSAMPLERSTATETYPE inType,
								  const DWORD inValue)
{
	// マップに登録されているか調べる
	std::map<std::pair<DWORD, D3DSAMPLERSTATETYPE>, STATE>::iterator it;
	it = m_SamplerState.find(std::pair<DWORD, D3DSAMPLERSTATETYPE>(inStage, inType));
	if(it == m_SamplerState.end()) {
		// 未登録
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
//	ステート適用
//------------------------------------------------------------------------------
void CStateBlock::Apply()
{
	// レンダリングステート
	std::map<D3DRENDERSTATETYPE, STATE>::iterator   rit;
	for(rit = m_RenderState.begin(); rit != m_RenderState.end(); rit++)
		m_pD3DDevice->SetRenderState(rit->first, rit->second.Value);

	// テクスチャステージステート
	std::map<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>::iterator   tit;
	for(tit = m_StageState.begin(); tit != m_StageState.end(); tit++)
		m_pD3DDevice->SetTextureStageState(tit->first.first, tit->first.second, tit->second.Value);

	// サンプラーステート
	std::map<std::pair<DWORD, D3DSAMPLERSTATETYPE>, STATE>::iterator   sit;
	for(sit = m_SamplerState.begin(); sit != m_SamplerState.end(); sit++)
		m_pD3DDevice->SetSamplerState(sit->first.first, sit->first.second, sit->second.Value);
}

//------------------------------------------------------------------------------
//	ステート現在値保存
//------------------------------------------------------------------------------
void CStateBlock::Capture()
{
	// レンダリングステート
	std::map<D3DRENDERSTATETYPE, STATE>::iterator   rit;
	for(rit = m_RenderState.begin(); rit != m_RenderState.end(); rit++)
		m_pD3DDevice->GetRenderState(rit->first, &rit->second.Capture);

	// テクスチャステージステート
	std::map<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>::iterator   tit;
	for(tit = m_StageState.begin(); tit != m_StageState.end(); tit++)
		m_pD3DDevice->GetTextureStageState(tit->first.first, tit->first.second, &tit->second.Capture);

	// サンプラーステート
	std::map<std::pair<DWORD, D3DSAMPLERSTATETYPE>, STATE>::iterator   sit;
	for(sit = m_SamplerState.begin(); sit != m_SamplerState.end(); sit++)
		m_pD3DDevice->GetSamplerState(sit->first.first, sit->first.second, &sit->second.Capture);
}

//------------------------------------------------------------------------------
//	ステート復元
//------------------------------------------------------------------------------
void CStateBlock::Restore()
{
	// サンプラーステート
	std::map<std::pair<DWORD, D3DSAMPLERSTATETYPE>, STATE>::iterator   sit;
	for(sit = m_SamplerState.begin(); sit != m_SamplerState.end(); sit++)
		m_pD3DDevice->SetSamplerState(sit->first.first, sit->first.second, sit->second.Capture);

	// テクスチャステージステート
	std::map<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>::iterator   tit;
	for(tit = m_StageState.begin(); tit != m_StageState.end(); tit++)
		m_pD3DDevice->SetTextureStageState(tit->first.first, tit->first.second, tit->second.Capture);

	// レンダリングステート
	std::map<D3DRENDERSTATETYPE, STATE>::iterator   rit;
	for(rit = m_RenderState.begin(); rit != m_RenderState.end(); rit++)
		m_pD3DDevice->SetRenderState(rit->first, rit->second.Capture);
}
