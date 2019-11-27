/*
 *  IContextData.h
 *  
 *  ImmutableのDrawData、BaseDataの描画用の�??タを保持する�?
 *  
 *  Created by joe on 11/04/29.
 *  Copyright 2011 Live2D Inc. All rights reserved.
 */

#ifndef __LIVE2D_I_DRAW_DATA_H__
#define __LIVE2D_I_DRAW_DATA_H__


#ifndef __SKIP_DOC__

#include "../memory/LDObject.h"

//------------ LIVE2D NAMESPACE ------------
namespace live2d
{ 
	class IDrawData ;
	
	class IDrawContext : public live2d::LDObject 
	{
	public:
		IDrawContext(IDrawData* src) ;
		virtual ~IDrawContext(){}
	
		void* getSrcPtr(){ return this->srcPtr ; } 
	
		//  パ�?�?��号セ�?��
		void setPartsIndex( int p ){ this->partsIndex = p ; }
	
		//  パ�?�?��号取�?
		int getPartsIndex() { return this->partsIndex ; }
	
		//  利用できるか取得�?TODO 更新
		bool isAvailable(){ return available && ! paramOutside ; }
	
	public:
		int 			interpolatedDrawOrder ;		// 描画�??補間結果
		float 			interpolatedOpacity ;		// 不�?明度の補間結果
		
		bool			paramOutside ;				// 補間結果として、パラメータが�?��外�?ときに true 
		
		float			partsOpacity ;				// パ�?�??不�?明度
		bool			available ;					// TODO 更新
	
	    float			baseOpacity;		        // SDK2.0 �?��するデフォーマ�?累積�?明度

	private:
		IDrawData* 		srcPtr ;					
		int 			partsIndex ;				
	};
}
//------------ LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif	// __LIVE2D_I_DRAW_DATA_H__

