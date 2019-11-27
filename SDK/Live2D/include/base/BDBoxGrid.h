/*
 *  BDBoxGrid.h
 * 
 *  格子状の点による座標変換
 * 
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_BD_BOX_GRID_H__
#define __LIVE2D_BD_BOX_GRID_H__


#ifndef __SKIP_DOC__

#include "../Live2D.h"
#include "../param/PivotManager.h"
#include "IBaseData.h"
#include "IBaseContext.h"


//------------ LIVE2D NAMESPACE ------------
namespace live2d
{ 
	
	class BDBoxGrid : public IBaseData 
	{
	public:
		BDBoxGrid();
		virtual ~BDBoxGrid();

	public:
		void initDirect(MemoryParam*	memParam) ;
	
		virtual void readV2(BReader & br , MemoryParam* memParam ) ;
	
		virtual IBaseContext* init(ModelContext &mdc) ;
	
		virtual void setupInterpolate(ModelContext &mdc , IBaseContext* cdata ) ;
		
		virtual void setupTransform(ModelContext &mdc, IBaseContext* cdata ) ;
		
		virtual void transformPoints( ModelContext &mdc, IBaseContext* cdata 
				, l2d_pointf * srcPoints , l2d_pointf * dstPoints 
				, int numPoint , int pt_offset , int pt_step ) ;
	
	
		
		inline int getNumPts(){ return (col+1)*(row+1) ; }
	
		
		virtual int getType()
		{
			return IBaseData::TYPE_BD_BOX_GRID ;
		}
	
		//-------------------------------------
	#ifdef L2D_DEBUG
		virtual void dump() ;
	#endif
	
	private:
		// Prevention of copy Constructor
		BDBoxGrid( const BDBoxGrid & ) ;
		BDBoxGrid& operator=( const BDBoxGrid & ) ;
	
		void transformPoints_sdk2( l2d_pointf* src , l2d_pointf* dst 
				, int numPoint , int src_offset , int src_step , l2d_pointf* pt , int _col , int _row ) ;

		void transformPoints_sdk1( ModelContext &mdc, IBaseContext* cdata 
				, l2d_pointf * srcPoints , l2d_pointf * dstPoints 
				, int numPoint , int pt_offset , int pt_step ) ;

	private:
		int							col ;			// 列数?�面の数なので 点数は+1 ??
		int							row ;			// 行数
		 
		PivotManager*				pivotManager ;	// パラメータと関連して補間する要�?��管�?
		LDVector<l2d_pointf*>*		pivotPoints ;	// 格子を形成する点列[numPivots][ 2*(col+1) *(row+1) ]
	};
	
	
	class BDBoxGridContext : public IBaseContext 
	{
	public:
		BDBoxGridContext(BDBoxGrid *src) ;
		virtual ~BDBoxGridContext() ;
		
	public:
		int 				tmpBaseDataIndex ;		// IBaseDataのイン�?��クス。不変なので�?��取得すれ�?よい
		l2d_pointf * 		interpolatedPoints ; 	// 補間済み配�?(localToTarget) row(y) が外�?のループ�?, col(x) �??のルー�?�?���?常の画像と同じ並び for(y){ for(x){ x,y }  }
		l2d_pointf * 		transformedPoints ;		// 変換済み配�?(localToDst)
	} ;
} 
//------------ LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif	// __LIVE2D_BD_BOX_GRID_H__


