/**
 * ParamPivots.h
 *
 *
 * 補間可能な IDrawData / IBaseDataで利用され�?
 * パラメータごとに、ピボットをとる�?を設定する�?
 *
 * 例�?ANGLE_X ( 0 ) => pivots { -30 , 0 , 30 }
 *
 * エ�?��タ以外では静的な値となる�?
 *
 * �?��、描画用の�?���??タを保持することもある�?
 *
 * 
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_PARAMPIVOTS_H__
#define __LIVE2D_PARAMPIVOTS_H__


#ifndef __SKIP_DOC__


#include "../Live2D.h"
#include "../type/LDVector.h"
#include "../ModelContext.h"
#include "../io/ISerializableV2.h"


//------------ LIVE2D NAMESPACE ------------
namespace live2d
{
	class ParamID ;
	
	/**********************************************************************************************
	@brief	パラメータのキーの管�?��ラス
	**********************************************************************************************/
	class ParamPivots : public ISerializableV2 
	{
	public:
		static const int PARAM_INDEX_NOT_INIT = -2 ;	// 値が存在しな�??合が -1を取る�?で -2
	
	public:
		ParamPivots();
		virtual ~ParamPivots();

	public:
		virtual void readV2(BReader & br , MemoryParam* memParam ) ;
	
	public:
		
		inline int getParamIndex( int initVersion )
		{ 
			if( this->indexInitVersion != initVersion ) 
			{
				_paramIndex = PARAM_INDEX_NOT_INIT ;
			}
	
			return _paramIndex ;
		}

		
		inline void setParamIndex_(int index , int initVersion )
		{ 
			this->_paramIndex = index ;
			this->indexInitVersion = initVersion ;
		}
	
		
		inline ParamID * getParamID() const { return paramID ; }
	
		
		inline void setParamID(ParamID * v){ paramID = v ; }
	
		
		inline int getPivotCount(){ return pivotCount ; }
	
	
		
		inline l2d_paramf * getPivotValue(){ return pivotValue ; } 
	
		
		inline void setPivotValue(int _pivotCount , l2d_paramf * _values)
		{
			this->pivotCount = _pivotCount ;
			this->pivotValue = _values ; 
		} 
	
		
		inline int getTmpPivotIndex(){ return tmpPivotIndex ; } 
	
		
		inline void setTmpPivotIndex(int v){ tmpPivotIndex = v ; } 
	
		
		inline float getTmpT(){ return tmpT ; } 
	
		
		inline void setTmpT(float t){ tmpT = t ; } 
	
		
#if L2D_SAMPLE
		void DUMP() ;
#endif
	
	
	private:
		//Prevention of copy Constructor
		ParamPivots( const ParamPivots & ) ;			
		ParamPivots& operator=( const ParamPivots & ) ;
	
		
	private:
		//---- Field ----
		int				pivotCount ;
		ParamID*		paramID ;			// パラメータIDは解放しなくてよい
		l2d_paramf*		pivotValue ;		// ld_paramfの配�?[] �?��の値をピボットに持つ?�外部で管�?��るためdelete不�?
		
		// ---- 計算時の�?���??
		int				_paramIndex ;		// 初期化前はPARAM_INDEX_NOT_INIT
		int				indexInitVersion ;	// indexをキャ�?��ュした時�? modelDrawContextのバ�?ジョン
		
		int				tmpPivotIndex ;		// �?��ピ�?�?��
		float			tmpT ;				// tmpPivotIndex , tmpPivotIndex+1 の�??比�???の場合�?�??しな�?�?
	};
} 
//------------ LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif		// __LIVE2D_PARAMPIVOTS_H__
