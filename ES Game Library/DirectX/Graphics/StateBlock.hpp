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
				ステートブロッククラスヘッダーファイル

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include <d3d9.h>
#include <map>

//------------------------------------------------------------------------------
//	ステートブロックインタフェース定義
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
//	ステートブロッククラス定義
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
	// ステート値保存構造体
	struct STATE {
		DWORD   Value;		// 設定値
		DWORD	Capture;	// 復元値
	};

	IDirect3DDevice9*                     m_pD3DDevice;

	std::map<D3DRENDERSTATETYPE, STATE>   m_RenderState;

	std::map<std::pair<DWORD, D3DTEXTURESTAGESTATETYPE>, STATE>   m_StageState;
	std::map<std::pair<DWORD, D3DSAMPLERSTATETYPE>,      STATE>   m_SamplerState;
};

//------------------------------------------------------------------------------
//	NULLステートブロッククラス定義
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
