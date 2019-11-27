/**
 *  AMotion.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_A_MOTION_H__
#define __LIVE2D_A_MOTION_H__


#include "../memory/LDObject.h"

#include "../ALive2DModel.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{

	class MotionQueueEnt ;

	
	class AMotion : public live2d::LDObject 
	{
	public:
		AMotion();
		virtual ~AMotion() ;

	public:
		//  パラメータ更新
		void updateParam( live2d::ALive2DModel * model , MotionQueueEnt *motionQueueEnt ) ;

	public:

		
		void setFadeIn( int fadeInMsec ){ this->fadeInMsec = fadeInMsec ; }

		
		void setFadeOut( int fadeOutMsec){ this->fadeOutMsec = fadeOutMsec ; }
		
		
		int getFadeOut(){ return this->fadeOutMsec ; }

		
		int getFadeIn(){ return this->fadeInMsec ; }

		
		void setWeight( float weight){ this->weight = weight ; }
		
		
		float getWeight(){ return this->weight ; }

		/*****************************************************************
		 * モーションの長さを返す?�ループ�?とき�? -1??
		 * ループではな�??合�?、オーバ�?ライドす�?
		 * 
		 * 正の値の時�?取得される時間で終�?���?
		 * -1のとき�?外部から停止命令が無�?��り終わらな�??�?��な�?
		******************************************************************/
		virtual int getDurationMSec(){ return -1 ; }
		
		/*****************************************************************
		 * ループ１回�??長さを返す (0.9.00b8)
		 * 
		 * 主にモーションの処�??部で利用する
		 * 
		 * サブクラスLive2DMotionの場合�?、mtnファイルで定義される�??のモーションの長さを返す
		 * ループしな�??合�? getDurationMSec()と同じ値を返す
		 * ループ�?���??長さが定義できな�??合（�?ログラ�?��に動き続けるサブクラスなど?��?場合�?-1を返す
		******************************************************************/
		virtual int getLoopDurationMSec(){ return -1 ; }


		/*****************************************************************
		 * モーション再生の開始時刻を設定す�?
		 * 途中から再生する場合に使用する
		 *
		 * version 0.9.00より搭�?
		******************************************************************/
		void setOffsetMSec( int offsetMsec ){ this->offsetMsec = offsetMsec ; }

		
		void reinit(){}

	protected:
		//  パラメータア�??�??�?
		virtual void updateParamExe( live2d::ALive2DModel * model , long long timeMSec , float weight , MotionQueueEnt *motionQueueEnt ) = 0 ;

		//  イージングを取�?
		float getEasing( float time , float totalTime , float accelerateTime ) ;

	protected:
		int 	fadeInMsec ;		// フェードインする場合�?これが無�?��、突然向きが変わったりする
		int 	fadeOutMsec ;		// フェードアウト時�?
		float 	weight ;			// Modelに設定されて�?��値と掛け合わせる度合い。１なら上書きす�?

		int 	offsetMsec ;		// 再生の開始時刻

	};

}
//--------- LIVE2D NAMESPACE ------------


#endif		// __LIVE2D_A_MOTION_H__
