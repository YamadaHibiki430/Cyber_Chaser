/**
 *  UtMath.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */

#ifndef __LIVE2D_UTMATH_H__
#define __LIVE2D_UTMATH_H__


#ifndef __SKIP_DOC__


#define _USE_MATH_DEFINES
#include <math.h>
#include "../Live2D.h"

// ------------  SIN/COSの高�?�?------------
#ifdef L2D_TARGET_PSP
#include "../graphics/DrawParam_PSP.h"
#elif 0
// --- 高�?処�?

#  define USE_MY_SIN 1 // 個�?値は直接設定しな�?
<<<<<<< HEAD
#  include "util/UtMath.h"
// UtMath.h , <math.h> の�?��れかが�?�?
=======

//  UtMath.h , <math.h> の�?��れかが�?�?
>>>>>>> remotes/gitlab/1.0.03
#  define L2D_SIN(x) live2d::UtMath::fsin(x)
#  define L2D_COS(x) live2d::UtMath::fcos(x)
#  define L2D_ATAN2(y,x) atan2(y,x)
#  define L2D_SQRT(x)	sqrt(x)

#else 
	// ---- 通常のsin()
#  include <math.h>
#  define L2D_SIN(x) sin(x)
#  define L2D_COS(x) cos(x)
#  define L2D_ATAN2(y,x) atan2(y,x)
#  define L2D_SQRT(x)	sqrt(x)
#endif

#define M_PI_F	3.1415f



//--------- LIVE2D NAMESPACE ------------
namespace live2d
{
	class UtMath 
	{
	public:
		static const double		DEG_TO_RAD_D;		// 		
		static const float		DEG_TO_RAD_F;		// 
		static const double		RAD_TO_DEG_D;		// 
		static const float		RAD_TO_DEG_F;		// 
		
		// 
		inline static float range( float v , float min , float max )
		{
			if( v < min ) v = min ;
			else if( v > max ) v = max ;
			return v ;
		}
	
		// 原点からの方向�?クトル?�つのなす角を求め�?
		static double getAngleNotAbs( l2d_pointf* v1 , l2d_pointf* v2 ) ;
	
		// Q1からQ2への角�?変化量を取得する�?
		static double getAngleDiff( double Q1 , double Q2 ) ;
	
		// サインを求め�?
		static double fsin(double x) ;
	
		// コサインを求め�?
		static double fcos(double x)
		{
	#if USE_MY_SIN
			return fsin(x+M_PI_F/2);
	#else
			return cos(x) ;
	#endif
		}
	
	private:
		UtMath();
		
		// Prevention of copy Constructor
		UtMath( const UtMath &);				
		UtMath& operator=( const UtMath & );	
		~UtMath();
	
		static const double sintable[128] ;
	};

}
//------------------------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif	// __LIVE2D_UTMATH_H__
