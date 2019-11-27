/**
 *  UtSystem.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */

#ifndef __LIVE2D_UTSYSTEM_H__
#define __LIVE2D_UTSYSTEM_H__

#include "../Live2D.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d
{ 
	
	
	
	class UtSystem 
	{
	public:
		// ビッグエン�?��アンかど�?��
		static bool isBigEndian() ;
	
		// シス�?��時刻?�ミリ秒）�?取�?
		static l2d_int64 getTimeMSec() ;
	
		//-------------------------------------------------------------
		// ユーザ定義時刻 ( 0.8.15以�?)
		// モーションなどをシス�?��時刻で行うと、計算�?�??途中でシス�?��時刻が�?み
		// 違う時刻で動きを計算する可能性がある�?
		// ユーザ時刻では、描画の開始前に明示�?��時刻を設定し、描画開始から描画終�?��で
		// 同じ「ユーザ時刻」で処�?��完�?��るよ�?��同期する�?
		// �?��もユーザ時刻を設定して�?���??合�?、getTimeMSec()と同じ値が返る
		//-------------------------------------------------------------
		// ユーザ定義の時刻を取�?
		static l2d_int64 getUserTimeMSec() ;
	
		// ユーザ定義時刻を設�?
		static void setUserTimeMSec(l2d_int64 t) ;
		
		// ユーザ定義時刻に、現在時刻をセ�?��.
		// setUserTimeMSec( getTimeMSec() )と同じ効�?
		static l2d_int64 updateUserTimeMSec() ;
	

		// ユーザ定義時刻をリセ�?��
		static void resetUserTimeMSec();
	

		static void exit(int code) ;
		
	private:
		UtSystem();		
		
		// Prevention of copy Constructor							
		UtSystem( const UtSystem & ) ;				
		UtSystem& operator=( const UtSystem & ) ; 	
		
		~UtSystem();
			
		
	private:
		static const int 	USER_TIME_AUTO = -1 ;	// Userタイ�?��System時刻に追従させる
		static long long 	userTimeMSec ;			//
		
	};
}
//------------------------- LIVE2D NAMESPACE ------------

#endif		// __LIVE2D_UTSYSTEM_H__
