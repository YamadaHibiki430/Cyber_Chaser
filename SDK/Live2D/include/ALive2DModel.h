/**
 *  ALive2DModel.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_ALIVE_2D_MODE_H__
#define __LIVE2D_ALIVE_2D_MODE_H__


#include "type/LDVector.h"
#include "Live2D.h"
#include "memory/LDObject.h"
#include "ModelContext.h"
#include "id/ParamID.h"
#include "id/DrawDataID.h"
#include "id/PartsDataID.h"

//------------ LIVE2D NAMESPACE ------------
namespace live2d
{ 
	class ModelImpl ;
	class DrawParam ;
	
	
	class ALive2DModel : public LDObject
	{
	public:
		static const int FILE_LOAD_EOF_ERROR = 1 ;			// �??タの末尾を読み込めな�??合�?エラー
		static const int FILE_LOAD_VERSION_ERROR = 2 ;		// ロードできな�?��ージョンのファイル
		
		static int INSTANCE_COUNT ;	
	
	
	public:	
		ALive2DModel() ;
		virtual ~ALive2DModel() ;
	
		
	
		// パラメータの値を取得す�?
		float getParamFloat( const char * paramID ) ;
		
		// パラメータの値を設定す�?
		void setParamFloat( const char * paramID , float value , float weight = 1.0f ) ;
		
		// 現在の値に対して�??みをつけて値を加える�?weight=1のとき�?�?��した�?をそのまま�?��る�?
		void addToParamFloat( const char * paramID , float value , float weight = 1.0f ) ;
	
		// 現在の値に対して�??みをつけて値を掛け合わせる�?weight=1のとき�?�?��した�?をそのまま掛け合わせる�?
		void multParamFloat( const char * paramID , float mult , float weight = 1.0f  ) ;
		
		
		inline int getParamIndex( const char * paramID )
		{
			return modelContext->getParamIndex(ParamID::getID(paramID)) ;
		}
	
		
		inline int getParamIndex( ParamID* paramID )
		{
			return modelContext->getParamIndex( paramID ) ;
		}	

		inline float getParamFloat( int paramIndex )
		{
			return modelContext->getParamFloat( paramIndex ) ;
		}
	
		inline void setParamFloat( int paramIndex , float value , float weight = 1.0f)
		{
			if(weight==1)modelContext->setParamFloat( paramIndex , value ) ;
			else modelContext->setParamFloat( paramIndex ,  modelContext->getParamFloat( paramIndex )*(1-weight) + value*weight ) ;
		}
	
		inline void addToParamFloat( int paramIndex, float value , float weight = 1.0f)
		{
			modelContext->setParamFloat( paramIndex , modelContext->getParamFloat( paramIndex ) + value*weight ) ;
		}
	
		inline void multParamFloat( int paramIndex , float mult , float weight = 1.0f)
		{
			modelContext->setParamFloat( paramIndex , modelContext->getParamFloat( paramIndex )*(1.0f + (mult - 1.0f)*weight ) ) ;
		}
	
		// 前回saveParamしたとき�?パラメータ値を復�?��る�?aveParamされて�?��ければ何もしな�?�?
		void loadParam() ;

		// 現在のパラメータ値を�?���?��記�?する。loadParam()で復�?��きる�?
		void saveParam() ;
	
		// �??タ構�?の変更、パー�?��し替えなどを行った�?合に呼ぶ
		virtual void init() ;
	
		// パラメータ変更などを行った�?合に呼ぶ
		virtual void update() ;

		// 描画命令?��?ラ�?��フォー�?��とにオーバ�?ライドされる??
		virtual void draw()  ;
		
		// パ�?�??不�?明度を設�?
		void setPartsOpacity( const char *partsID , float opacity ) ;
		void setPartsOpacity( int partsIndex , float opacity ) ;

		// パ�?�??不�?明度を取�?
		float getPartsOpacity( const char *partsID ) ;
		float getPartsOpacity( int partsIndex ) ;
		
		// パ�?�??表示グループを制御?�非推奨??
		void setupPartsOpacityGroup_alphaImpl( const char* paramGroup[] , float deltaTimeSec ) ;
	
		// モ�?��の�??タ構�?を設�?
		void setModelImpl(ModelImpl* m) ;

		// モ�?��の�??タ構�?を取�?
		ModelImpl* getModelImpl() ;
	
		
		ModelContext * getModelContext(){ return modelContext ; }

		
		int getErrorFlags(){ return Live2D::getError() ; }
		
		// 新しく利用できるLive2D�?��で管�?��れる�?��スチャ番号を確�?Avatar用)
		virtual int generateModelTextureNo() ;
		
		// Live2D�?��で管�?��れる�?��スチャ番号を解放(Avatar用)
		virtual void releaseModelTextureNo(int no) ;
	
		// キャンバスの�?��取�?
		float getCanvasWidth() ;

		// キャンバスの高さを取�?
		float getCanvasHeight() ;
	
		
		virtual DrawParam* getDrawParam(){ return NULL ; }
	
		
		virtual int getDrawDataIndex( const char * drawDataID )
		{
			return modelContext->getDrawDataIndex( DrawDataID::getID(drawDataID)) ;
		}
	
		
		virtual IDrawData* getDrawData( int drawIndex )
		{
			return modelContext->getDrawData( drawIndex ) ;
		}
		
		// 描画用の点列を返す�?
		virtual l2d_pointf* getTransformedPoints( int drawIndex , int*pointCount) ;
		virtual l2d_index* getIndexArray( int drawIndex , int*polygonCount) ;
	
		// 【非推奨】ZBufferを設定す�?
		// 通常、描画には�?��な�??でZ値は設定されな�?��、フェードイン、アウトなどの透�?度を描画する場合に�?���?
		void updateZBuffer_TestImpl( float startZ , float stepZ )
		{
			modelContext->updateZBuffer_TestImpl( startZ , stepZ ) ;
		}
	
		
		inline int getPartsDataIndex( const char * partsID )
		{
			return modelContext->getPartsDataIndex(PartsDataID::getID(partsID)) ;
		}
		
		
		inline int getPartsDataIndex( PartsDataID* partsID )
		{
			return modelContext->getPartsDataIndex( partsID ) ;
		}

		
		void setPremultipliedAlpha(bool b);
		bool isPremultipliedAlpha();
		
		void setAnisotropy(int n);
		int getAnisotropy();


	#if L2D_ALPHA_IMPL_LAYOUT
		// 【仮実�??モ�?��の配置�??を返す
		// ポインタを直接返す.�?��を更新すると直接反映され�?
		//{ x , y , anchorX , anchorY , scaleX , scaleY }
		float* getLayout_alphaImpl(){ return layout_alphaImpl ; }
	#endif
		
	#ifdef L2D_DEBUG
		void dump() ;
	#endif
	
	protected:
		//------------ static method ------------
		// 2013/04/02 beta2 エラーフラグを返すように修正
		static l2d_uint32 loadModel_exe( ALive2DModel *ret , const LDString & filepath ) ;
		static l2d_uint32 loadModel_exe( ALive2DModel *ret , const void * buf , int bufSize ) ;
	
	private:
		//Prevention of copy Constructor
		ALive2DModel( const ALive2DModel & ) ;				
		ALive2DModel& operator=( const ALive2DModel & ) ;
	
		
	protected:
		//------------ field ------------
		ModelImpl*			modelImpl ;						// モ�?��の�??タ構�?
		ModelContext*		modelContext ;					// モ�?��の状�?		
//		int					errorFlags ;					// エラーフラグ(1.00beta2より破�?Live2D::setError()に統�?
	
	#if L2D_ALPHA_IMPL_LAYOUT
		float layout_alphaImpl[6] ;//{ x , y , anchorX , anchorY , scaleX , scaleY }
	#endif
		
	};

} 
//------------ LIVE2D NAMESPACE ------------

#endif		// __LIVE2D_ALIVE_2D_MODE_H__
