/**
 *  ModelContext.h
 * 
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_MODEL_CONTEXT_H__
#define __LIVE2D_MODEL_CONTEXT_H__


#include "Live2D.h"
#include "memory/LDObject.h"
#include "type/LDVector.h"
#include "DEF.h"

#if L2D_VERBOSE
#include "util/UtDebug.h"
#endif

//------------ LIVE2D NAMESPACE ------------
namespace live2d
{ 
	
	class ParamID ;
	class BaseDataID ;
	class PartsDataID ;
	class DrawDataID ;
	class IBaseData ;
	class IDrawData ;
	class PartsData ;
	class ALive2DModel ;
	class DrawParam ;
	class IBaseContext ;
	class IDrawContext ;
	class PartsDataContext ;
		
	#define _PARAM_FLOAT_MIN_ (-1000000) 
	#define _PARAM_FLOAT_MAX_ ( 1000000) 
	
	
	class ModelContext : public live2d::LDObject 
	{
	public:
		static const unsigned short NOT_USED_ORDER ; 
		static const unsigned short NO_NEXT ; 
	
	
	public:
		ModelContext(ALive2DModel *model) ;
		virtual ~ModelContext(void) ;
	
	public:
		// 解放
		void release() ;
		
		// 初期化�?setup()の前に�?���?��呼ぶ。構�?が変わった�?合も呼ぶ�?
		void init() ;
	
		// メモリを�?��管�?��るため�?MemoryParamを返す
		MemoryParam*  getMemoryParam(){ return memoryManagement ; }
	
		// 初期化�?バ�?ジョン�??取得（�?初期化されるとキャ�?��ュが無効になるた�?�?
		inline int getInitVersion() const { return initVersion ; }
	
		inline bool requireSetup() const { return needSetup ; }
		
		// 現在のパラメータ値で描画用の�??タを生成す�?
		bool update() ;
	
		// 描画
		void draw(DrawParam &dp) ;
	
	
				
		inline bool isParamUpdated( int paramIndex ) const { 
# if L2D_FORCE_UPDATE
			return true ;
# else
			return (*updatedFloatParamFlagsPtr)[ paramIndex ] == PARAM_UPDATED ; 
# endif
		}

		// paramIDからアクセス用のparamIndexを取�?
		int getParamIndex( ParamID * paramID )   ;

		// BaseDataIDからアクセス用の baseIndex を取得す�?
		int getBaseDataIndex( BaseDataID * baseID ) ;

		// PartsDataIDからアクセス用の partsIndex を取得す�?
		int getPartsDataIndex( PartsDataID * partsID ) ;

		// drawDataIDからアクセス用の drawDataIndex を取得す�?
		int getDrawDataIndex( DrawDataID * drawDataID ) ;
	
		inline unsigned short * getTmpPivotTableIndicesRef(){ return tmpPivotTableIndices_short ; }
		inline float * getTmpT_ArrayRef(){ return tmpT_array ; }
		
	
		// floatパラメータ追�?
		int addFloatParam( ParamID * id , l2d_paramf value , l2d_paramf min , l2d_paramf max ) ;
	
	
		void setBaseData( unsigned int baseDataIndex , IBaseData * baseData ) ;
	
	
		void setParamFloat( unsigned int paramIndex , l2d_paramf value ) ;
	
		float getParamMax( unsigned int paramIndex ){ return (*floatParamMaxListPtr)[paramIndex] ; }
		float getParamMin( unsigned int paramIndex ){ return (*floatParamMinListPtr)[paramIndex] ; }
		
		// 前回saveParamしたパラメータ値を復�?�?aveParamされて�?��ければ何もしな�?�?
		void loadParam() ;

		// 現在のパラメータ値を保�?
		void saveParam() ;
	
		// パ�?�??不�?明度をセ�?��
		void setPartsOpacity( int partIndex , float opacity ) ;
	
		// パ�?�??不�?明度取�?
		float getPartsOpacity( int partIndex ) ;
		
	
		
		inline IBaseData* getBaseData( unsigned int baseDataIndex )
		{

	# if L2D_RANGE_CHECK		
			if( baseDataIndex >= baseDataListPtr->size() )
			{
	
				L2D_THROW( "out of range ModelDrawContext@getBaseData()" ) ;
			}
	# endif
			return (*baseDataListPtr)[ baseDataIndex ] ;// �?��外�?未定義?�外部でチェ�?��する??
		}

		
		inline IDrawData* getDrawData( unsigned int drawDataIndex )
		{
	# if L2D_RANGE_CHECK		
			if( drawDataIndex >= (*drawDataListPtr).size() )
			{
	
				L2D_THROW( "out of range ModelDrawContext@getBaseData()" ) ;
			}
	# endif
			return (*drawDataListPtr)[ drawDataIndex ] ;// �?��外�?未定義?�外部でチェ�?��する??
		}
	
		
		inline IBaseContext* getBaseContext( unsigned int baseDataIndex )
		{
			return (*baseContextListPtr)[ baseDataIndex ] ;// �?��外�?未定義?�外部でチェ�?��する??
		}

		
		inline IDrawContext* getDrawContext( unsigned int drawDataIndex )
		{
			return (*drawContextListPtr)[ drawDataIndex ] ;// �?��外�?未定義?�外部でチェ�?��する??
		}

		
		inline PartsDataContext* getPartsContext( unsigned int partsDataIndex )
		{
			return (*partsContextListPtr)[ partsDataIndex ] ;// �?��外�?未定義?�外部でチェ�?��する??
		}

		
		inline int getBaseDataCount(){	return (int)baseDataListPtr->size() ; }

		
		inline int getDrawDataCount(){	return (int)drawDataListPtr->size() ; }

		
		inline int getPartsDataCount(){	return (int)partsDataListPtr->size() ; }
		
		// parts�??タを取得しま�?
		inline l2d_paramf getParamFloat( unsigned int paramIndex )
		{

	# if L2D_RANGE_CHECK
			if( paramIndex >= floatParamListPtr->size() )
			{
	
				L2D_THROW( "out of range ModelDrawContext@getParamFloat()" ) ;
			}
	# endif
			return (*floatParamListPtr)[ paramIndex ] ;
		}
		
		// DirectX�?��イスロスト時に呼び出�?
		void deviceLost( ) ;
		
		/*
		 * ZBufferを設定す�?
		 *
		 * 通常描画には�?��な�??でZ値は設定されな�?���?
		 * フェードイン・アウトなどの透�?度を描画する場合に使�?��す�?
		 * update()とdraw()の間で呼び出して下さ�??
		 * 
		 * startZ	�?��背面にあるポリゴンのZ値
		 * stepZ	Z値を更新するス�?��プ�?
		 * 
		 */
		void updateZBuffer_TestImpl( float startZ , float stepZ ) ;
	
	#if L2D_SAMPLE
		void DUMP_PARAMS() ;
	#endif
	
	
	private:
		enum { PARAM_NOT_UPDATED = 0,	PARAM_UPDATED	};
		
		//Prevention of copy Constructor
		ModelContext( const ModelContext & ) ;				
		ModelContext& operator=( const ModelContext & ) ; 	
		
	private:	
		bool 							needSetup ;						// �??のsetupがすんで�?���?��け�?�?true
		ALive2DModel * 					model ;							// 
	
	
		int 							initVersion ;					// initを行ったバージョン。このバ�?ジョンが異なる�?合�?、キャ�?��ュされたindex値などが無効とみな�?
		// ------------ パラメータ値??loat??------------
		LDVector<ParamID *>*			floatParamIDListPtr ;			// 解放しな�?
		LDVector<l2d_paramf>*			floatParamListPtr ;				// float 型�?パラメータ値�??ブル??aramIndexでアクセスする??
		LDVector<l2d_paramf>*			lastFloatParamListPtr ;			// 前回upate時�?パラメータ値�??ブル??aramIndexでアクセスする??.9.00b19 2011/10/27
		
		//-- 
		LDVector<l2d_paramf>*			floatParamMinListPtr ;			// float 型�?パラメータ、最小�?�??ブル??aramIndexでアクセスする??
		LDVector<l2d_paramf>*			floatParamMaxListPtr ;			// float 型�?パラメータ、最大値�??ブル??aramIndexでアクセスする??
		
		LDVector<l2d_paramf>*			savedFloatParamListPtr ;		// saveParam() により保存された状態�?float 型�?パラメータ値�??ブル??aramIndexでアクセスする??
	
		// 更新されたパラメータのフラグを立て�? updatedFloatParamFlags[paramIndex] = 0 or 1(updated)
		LDVector<char>*					updatedFloatParamFlagsPtr ;		// boolフラグは良くな�??でunsigned char
		
		// ------------ setup効�?��のための ------------
		// 座標変換 
		// 	予め依存関係�?無�??序で並べておくことで、setup時�?OnDemandBuilderを不要にする
		// 	indexでアクセスするためinit()以外で�?��変更はしな�?
		LDVector<IBaseData*>*			baseDataListPtr ;				// 解放しな�?Model以下に�?���?
		
		// 	visibleの drawDataのみをリスト化する。setup時�?オー�??などの制御ではこ�? index を用�?��
		LDVector<IDrawData*>*			drawDataListPtr ;				// 解放しな�?Model以下に�?���?
	
		LDVector<PartsData*>*			partsDataListPtr ;				// 解放しな�?Model以下に�?���?
		
		// --- context data (描画用の演算結果のリス�?---
		LDVector<IBaseContext*>*		baseContextListPtr ;			// 解放する
		LDVector<IDrawContext*>*		drawContextListPtr ;			// 解放する
		LDVector<PartsDataContext*>*	partsContextListPtr ;			// 解放する
	
		// オー�??管�?��?�末尾にメモ??
		// 添え字�?同じ , 0は番兵
		LDVector<unsigned short>*		orderList_firstDrawIndexPtr ;	// order 1000 同じオー�??の�??の sub listのindex
		LDVector<unsigned short>*		orderList_lastDrawIndexPtr  ;	// order 1000 同じオー�??の�?���? sub listのindex
		
		// 添え字�? drawDataList と同じ
		LDVector<unsigned short>*		nextList_drawIndexPtr ;			// drawDataのindexと�??する。�?期�?NO_NEXT
		
		// ----------- 補間の補助用�?���??タ?��?ルチスレ�?��未対応�?-----------
		unsigned short 					tmpPivotTableIndices_short [DEF::PIVOT_TABLE_SIZE] ;	// 現在補間しよ�?��して�?��要�??�?��ボット番号を記録する(�?���?-1?�番兵)
		float 							tmpT_array[DEF::MAX_INTERPOLATION*2] ;					// 少し大き目に確�?
	
		MemoryParam*					memoryManagement ;
		AMemoryHolder*					memoryHolderFixedMain ;
		AMemoryHolder*					memoryHolderFixedGPU ;
		
	};
}
//------------ LIVE2D NAMESPACE ------------

#endif		// __LIVE2D_MODEL_CONTEXT_H__
