/**
 *  EyeBlinkMotion.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_EYE_BLINK_MOTION_H__
#define __LIVE2D_EYE_BLINK_MOTION_H__


#include "../memory/LDObject.h"

#include "../type/LDVector.h"

#include "../ALive2DModel.h"


//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{

	
	class EyeBlinkMotion : public live2d::LDObject 
	{

	public:
		// 眼の状態定数
		enum EYE_STATE{
			STATE_FIRST = 0 , 
			STATE_INTERVAL ,
			STATE_CLOSING ,// 閉じて�?��途中
			STATE_CLOSED , // 閉じて�?��状�?
			STATE_OPENING ,// 開いて�?��途中
		};

	public:
		// Constructor
		EyeBlinkMotion();

		// Destructor
		virtual ~EyeBlinkMotion();

	public:
		//  次回�?まばたきモーションの時刻を設�?
		long long calcNextBlink() ;

		//  インターバル時間の設�?
		void setInterval( int blinkIntervalMsec) ;

		//  まばたきモーションの設�?
		void setEyeMotion( int closingMotionMsec , int closedMotionMsec , int openingMotionMsec ) ;

		//  �?��したモ�?��のパラメータ設�?
		void setParam( live2d::ALive2DModel *model ) ;

	private:
		long long 			nextBlinkTime ;				// 次回眼パチする時刻??sec??
		
		int 				eyeState ;					// 現在の状�?
		long long 			stateStartTime ;			// 現在のstateが開始した時刻

		bool 				closeIfZero;				// IDで�?��された眼のパラメータが�?0のときに閉じるな�?true �?の時に閉じるな�?false
		
		live2d::LDString 	eyeID_L ;					// 左目のID
		live2d::LDString 	eyeID_R ;					// 右目のID

		int					blinkIntervalMsec ;			// 
		int 				closingMotionMsec ;			// 眼が閉じるまでの時間
		int 				closedMotionMsec  ;			// 閉じたままで�?��時間
		int 				openingMotionMsec ;			// 眼が開くまでの時間

	};

}
//--------- LIVE2D NAMESPACE ------------


#endif		// __LIVE2D_EYE_BLINK_MOTION_H__
