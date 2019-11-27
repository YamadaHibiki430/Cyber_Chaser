/**
 *  UtMotion.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */

#ifndef __LIVE2D_UTMOTION_H__
#define __LIVE2D_UTMOTION_H__


#ifndef __SKIP_DOC__

// --------- LIVE2D NAMESPACE ------------
namespace live2d 
{ 
	
	class UtMotion 
	{
	public:
		// シンプルなイージングを掛けた値を返す(スロースター�?スローストッ�?
		static float getEasingSine(float value) ;
		
	private:
		UtMotion();
		~UtMotion();

		// Prevention of copy Constructor
		UtMotion( const UtMotion & ) ;				
		UtMotion& operator=( const UtMotion & ) ;
	};

} 
//------------------------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif		// __LIVE2D_UTMOTION_H__
