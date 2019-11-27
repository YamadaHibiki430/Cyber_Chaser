/**
 *  IDrawData.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */

#ifndef __LIVE2D_IDRAWDATA_H__
#define __LIVE2D_IDRAWDATA_H__


#ifndef __SKIP_DOC__

#include "../Live2D.h"
#include "../io/ISerializableV2.h"
#include "../id/BaseDataID.h"
#include "IDrawContext.h"
#include <stdio.h>


//------------ LIVE2D NAMESPACE ------------
namespace live2d
{
	class DrawDataID ;
	class ModelContext ;
	class PivotManager ;
	class DrawParam ;

	
	class IDrawData : public ISerializableV2
	{
	public:
		static const int BASE_INDEX_NOT_INIT 	= -2 ;
		static const int DEFAULT_ORDER 			= 500 ;
		static const int TYPE_DD_TEXTURE 		= 2 ;
	public:	
		IDrawData();
		virtual ~IDrawData();
	
		virtual void readV2(BReader & br , MemoryParam* memParam ) ;
		
		// オーバ�?ライド不�?
		inline BaseDataID * getTargetBaseDataID(){ return targetBaseDataID ; } 
		inline void setTargetBaseDataID(BaseDataID *id){ targetBaseDataID = id ; } 
	
		bool needTransform()
		{ 
			return ( targetBaseDataID 
					&& (targetBaseDataID != BaseDataID::DST_BASE_ID() ) ) ;
		}
	
		//  描画ID取�?	
		inline DrawDataID * getDrawDataID(){ return drawDataID ; } 

		//  描画IDをセ�?��
		inline void setDrawDataID( DrawDataID *id ){ drawDataID = id ; } 
	
		//  不�?明度取�?
		inline float getOpacity( ModelContext &mdc , IDrawContext* cdata ){ return cdata->interpolatedOpacity ; }
	
		//  描画オー�??取�?
		inline int getDrawOrder(ModelContext &mdc , IDrawContext* cdata ){ return cdata->interpolatedDrawOrder ; }
		
		inline void setDrawOrder(LDVector<int>* orders )
		{
			for( int i = (int)orders->size()-1 ; i >= 0  ; --i )
			{
				int order = (*orders)[i] ;

				if( order < totalMinOrder ) 		totalMinOrder = order ;
				else if( order > totalMaxOrder ) 	totalMaxOrder = order ;// 初期値 を持って�?��場合�? else ifでよい
			}
		} 
	
		
		inline static int getTotalMinOrder(){ return totalMinOrder ; } 
		inline static int getTotalMaxOrder(){ return totalMaxOrder ; } 
		
		// �??�?�?etupInterpolation/draw?�で毎回�?���?���?な�??�?���??に行う
		// �?��パラメータが設定された後に呼び出�?
		virtual IDrawContext* init(ModelContext &mdc) = 0;
		
		// drawの前段階として、補間を行う
		virtual void setupInterpolate(ModelContext &mdc , IDrawContext* cdata ) ;
		
		// drawの前段階として、変形を行う
		virtual void setupTransform(ModelContext &mdc , IDrawContext* cdata ) ;
	
		// 描画を行う
		virtual void draw( DrawParam & dp , ModelContext &mdc , IDrawContext* cdata ) = 0 ;
	
		// IDrawDataの型を返す。TYPE_DD_TEXTUREなど
		virtual int getType() ;
		
		// �?��イスロスト時に�??タを�?�?���?
		virtual void deviceLost( IDrawContext* drawContext ) {}
	
		virtual void setZ_TestImpl( ModelContext &mdc , IDrawContext* _cdata , float z ){}
	
#ifdef L2D_DEBUG
		virtual void dump() = 0 ;
#endif
	
	private:
		//Prevention of copy Constructor
		IDrawData( const IDrawData & ) ;
		IDrawData& operator=( const IDrawData & ) ; 	
	
	private:
		static int			totalMinOrder ;		//  実行時の�??タ確保用(transient)
		static int			totalMaxOrder ;		//  実行時の�??タ確保用(transient)
	
	protected:
		PivotManager *		pivotManager ;
		int 				averageDrawOrder ;	//  描画�?��変更しな�?��式で描く場合�?_L2D_VERSION_STR__ 801以降�?使わな�?�?
	
	private:
		DrawDataID *		drawDataID ;		//  解放しな�?
		BaseDataID *		targetBaseDataID ;	//  解放しな�?
		LDVector<int>*		pivotDrawOrder ;	//  補間用のpivotを�?て持つ頂点配�? [numPivots]
		LDVector<float>*	pivotOpacity ;		//  補間用のpivotを�?て持つ頂点配�? [numPivots]
	
		bool				dirty ;				//  更新されてinit()を呼び出す�?��がある場合（�?期状態も??
	};
}
//------------ LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif	// __LIVE2D_IDRAWDATA_H__
