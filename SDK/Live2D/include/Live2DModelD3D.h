/*
 *  Live2DModelD3D.h
 *
 * 
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_L2DMODEL_D3D_H__
#define __LIVE2D_L2DMODEL_D3D_H__

#include "Live2D.h"

#ifdef L2D_TARGET_D3D

#include "ALive2DModel.h"
#include "model/ModelImpl.h"
#include "graphics/DrawParam_D3D.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{
	class ModelContext ;

	
	class Live2DModelD3D : public live2d::ALive2DModel
	{
	public:
		Live2DModelD3D(void) ;
		virtual ~Live2DModelD3D(void) ;

	public:
		// 登録されたテクスチャを�?�?���?
		// �?��スチャを外部で共有して�?��場合があるため
		// 登録されたテクスチャは自動的には破�?��れな�??
		// 破�?��るにはDestructorの前に明示�?��呼ぶ�?��がある�?
		void deleteTextures() ;
		
		virtual void draw() ;

		void setTexture( int textureNo , LPDIRECT3DTEXTURE9 openGLTextureNo ) ;
		
		static Live2DModelD3D * loadModel( const live2d::LDString & filepath ) ;
		static Live2DModelD3D * loadModel( const void * buf , int bufSize ) ;

		// 新しく利用できるModelの�?��スチャ番号を確�?Avatar用)
		virtual int generateModelTextureNo() ;
		
		// Modelの�?��スチャ番号を生�?Avatar用)
		virtual void releaseModelTextureNo(int no) ;
		
		
		void setDevice( LPDIRECT3DDEVICE9 device )
		{
			drawParamD3D->setDevice(device) ;
		}

		int getErrorD3D_tmp()
		{
			return drawParamD3D->getErrorD3D_tmp() ;
		}


		// Direct3Dの�?��イスロスト時の処�?
		void deviceLostD3D( ) ;

		virtual live2d::DrawParam* getDrawParam(){ return drawParamD3D ; }

		void setTextureColor(int textureNo,float r,float g,float b);
		void setTextureBlendMode(int textureNo,int mode);
		void setTextureInterpolate(int textureNo, float interpolate);

	private:
		// Prevention of copy Constructor
		Live2DModelD3D( const Live2DModelD3D & ) ;				
		Live2DModelD3D& operator=( const Live2DModelD3D & ) ; 	
		 
	private:
		live2d::DrawParam_D3D * 		drawParamD3D ;		// Direct3Dの描画パラメータ格納用

	};
}
//--------- LIVE2D NAMESPACE ------------


#endif		//L2D_TARGET_D3D
#endif		//__LIVE2D_L2DMODEL_D3D_H__
