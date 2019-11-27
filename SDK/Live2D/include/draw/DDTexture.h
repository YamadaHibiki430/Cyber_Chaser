/**
 *  DDTexture.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_DDTEXTURE_H__
#define __LIVE2D_DDTEXTURE_H__


#ifndef __SKIP_DOC__

#include "../Live2D.h"
#include "IDrawData.h"
#include "../param/PivotManager.h"
#include "IDrawContext.h"

#ifdef L2D_TARGET_D3D
# include <d3dx9.h>
# include "../graphics/DrawParam_D3D.h"
#endif

#ifdef L2D_TARGET_PS4
#include <gnmx.h>
#endif

//------------ LIVE2D NAMESPACE ------------
namespace live2d
{ 
	class DDTextureContext ;
	
	
	
	class DDTexture : public IDrawData
	{
	public:
		static const int OPTION_FLAG_BARCODE_KANOJO_COLOR_CONVERT = 0x1 ;// バ�?コードカノジョで色変を行う要�??場�?
		
		static const int MASK_COLOR_COMPOSITION = 0x1E ;//011110 
	
		static const int COLOR_COMPOSITION_NORMAL  = 0 ;// 通常
		static const int COLOR_COMPOSITION_SCREEN  = 1 ;// スクリーン
		static const int COLOR_COMPOSITION_MULTIPLY= 2 ;// 乗�?
	
		static int INSTANCE_COUNT ;
	
	public:
		DDTexture() ;
		virtual ~DDTexture(void) ;
		
	public:
		virtual void readV2(BReader & br , MemoryParam* memParam ) ;

		void initDirect(MemoryParam* memParam) ;
	
		//  �?��したテクスチャ番号にセ�?��
		void setTextureNo(int no){ this->textureNo = no ; }
	
		//  �?��スチャ番号取�?
		int getTextureNo(){ return this->textureNo ; }
		
		//  uv配�? [ numPts*2 ]
		l2d_uvmapf * getUvMap(){ return this->uvmap ; }
		
		//  点数を返す
		int getNumPoints(){ return this->numPts ; }
	
		//  ポリゴン数を返す
		int getNumPolygons(){ return this->numPolygons ; }
		
		virtual IDrawContext* init(ModelContext &mdc) ;
		
		virtual void setupInterpolate(ModelContext &mdc , IDrawContext* cdata ) ;
		
		virtual void setupTransform(ModelContext &mdc , IDrawContext* cdata ) ;
	
		virtual void draw( DrawParam & dp , ModelContext &mdc , IDrawContext* cdata ) ;
	
	#ifdef L2D_TARGET_D3D
	
		void setupBufD3D( DrawParam_D3D& dpD3D , ModelContext &mdc , DDTextureContext* cdata ) ;
	
		virtual void deviceLost( IDrawContext* drawContext ) ;
	#endif
		
		//  �?��スチャタイプ取�?
		virtual int getType(){ return TYPE_DD_TEXTURE ; }// 
	
		int getOptionFlag(){ return this->optionFlag ; }
	
		virtual void setZ_TestImpl( ModelContext &mdc , IDrawContext* _cdata , float z ) ;
	
		l2d_index*  getIndexArray(int* polygonCount);

	#if AVATAR_OPTION_A
		//  色取�?
		int getOption_KanojoColor(){ return this->optionKanojoColor ; }
	
	#endif
	
	#ifdef L2D_DEBUG
		virtual void dump() ;
	#endif
	
	private:
		// Prevention of copy Constructor
		DDTexture( const DDTexture & ) ;				
		DDTexture& operator=( const DDTexture & ) ;		
	
	private:
		int						textureNo ;				// 使用する�?��スチャ番号?�モ�?��の管�?��るテクスチャ番号??
		int						numPts ;				// 頂点の数??SPのsceGumDrawArray では 65535までになる�?
		int						numPolygons ;			// ポリゴンの数
		int						optionFlag ;			// オプション設�?
		
		// --- �?��に応じて設定されるオプション値
	#if AVATAR_OPTION_A
		int						optionKanojoColor ;		// Avatarのオプション??ptionFlag & DDMorph.OPTION_FLAG_BARCODE_KANOJO_COLOR_CONVERTが立って�?��場合�?
	#endif
		
		l2d_index *				indexArray ;			// 頂点イン�?��クス配�? [numPolygons*3]
	
		//  points ( Pivot配�? )のルール ( �? xnum=3 , ynum=4 , znum=2 のパラメータの場�?)
		//  [x0,y0,z0],[x1,y0,z0],[x2,y0,z0]  , [x0,y1,z0],[x1,y1,z0],[x2,y1,z0].. とな�?
		
		LDVector<l2d_paramf*>*	pivotPoints ;			// 補間用のpivotを�?て持つ頂点配�? [numPivots][numPts*2]
		l2d_uvmapf *			uvmap ;					// uv配�? [ numPts*2 ]
		int						colorCompositionType ;	// 色合�?のタイ�?
		bool					culling ;				// 1.0beta3
	};
	
	
	class DDTextureContext : public IDrawContext 
	{
	public:
		DDTextureContext(IDrawData* src) ;
		virtual ~DDTextureContext() ;
		
	public:		
		// 変換済みの点列を返す 
		// pointCount は点の数が返る?�点??,y?�がpointCount個並ぶので、pointCount*2の配�?となる�?
		l2d_pointf* getTransformedPoints( int* pointCount ) ;
	
	public:		
		int 				tmpBaseDataIndex ;		// IBaseDataのイン�?��クス。不変なので�?��取得すれ�?よい
		l2d_pointf * 		interpolatedPoints ;	// 補間された点�?	PSPの場合�?、[numPts*5]�? OpenGLの場合�? [numPts*2]( texture x,y , 補間・変形された後�?点 x,y , z=0 ) 
		l2d_pointf * 		transformedPoints ;		// 変形された点�?
		l2d_pointf * 		drawPoints ;			// 描画用の点列（ダブルバッファ用の場合�?スワ�??して使われる�?.9.00b14
		
		unsigned char 		not_updated_count ;		// 更新されなかった回数をカウントする\n?�回目と?�回目で意味を持つ�?ndouble bufferのとき二回更新されなければそ�?まま前�?値を使える
		
	#ifdef L2D_TARGET_D3D
		LPDIRECT3DVERTEXBUFFER9 		pUvBuf ; // Uv配�?
		LPDIRECT3DINDEXBUFFER9 			pIndexBuf ; // イン�?��クス配�?
	#endif

	#ifdef L2D_TARGET_PS4
		bool initializedBuffer;
		float* color;//透�?度含めた掛け合わせる色。float[4]。シェー�?��
		sce::Gnm::Buffer buffers[2];//position , textureUV 
	#endif
	} ;
} 
//------------ LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif	// __LIVE2D_DDTEXTURE_H__

