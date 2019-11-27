/**
 *  LDRectF.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_LDRECTF_H__
#define __LIVE2D_LDRECTF_H__


#ifndef __SKIP_DOC__


#include "../Live2D.h"
#include "../memory/LDObject.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{
	
	class LDRectF   : public live2d::LDObject 
	{
	
	public:
		// Constructor
		LDRectF();
		LDRectF(l2d_pointf x , l2d_pointf y , l2d_pointf w , l2d_pointf h );
	
		// Destructor
		virtual ~LDRectF();
	
	public:
		//  中央X取�?
		l2d_pointf getCenterX(){ return x + 0.5f*width ; } 
	
		//  中央Y取�?
		l2d_pointf getCenterY(){ return y + 0.5f*height; }
	
		//  右端取�?
		l2d_pointf getRight(){ return x + width ; }
	
		//  下端取�?
		l2d_pointf geBottom(){ return y + height ; }
		
	public:
		l2d_pointf x;				// 左端X
		l2d_pointf y;				// 上端Y
		l2d_pointf width;			// �?
		l2d_pointf height;			// 高さ
	};
}
//------------------------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif	// __LIVE2D_LDRECTF_H__
