/**
 *  MotionQueueEnt.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_MOTION_QUEUE_ENT_H__
#define __LIVE2D_MOTION_QUEUE_ENT_H__


#ifndef __SKIP_DOC__

#include "../memory/LDObject.h"
#include "../ALive2DModel.h"

#include "../type/LDVector.h"


//--------- LIVE2D NAMESPACE ------------
namespace live2d
{
	class AMotion ;
	class Live2DMotion ;

	/********************************************************************************
	@brief	MotionQueueManagerで再生して�?���?��ーションの管�?��ラス
	
	MotionQueueManager* mqm = ... \n
	int motionQueueEntNo = mqm->startMotion(...)\n
	MotionQueueEnt ent = mqm->getMotionQueueEnt(motionQueueEntNo)
	********************************************************************************/
	class MotionQueueEnt : public live2d::LDObject 
	{
		// フレン�?
		friend class MotionQueueManager ;
		friend class AMotion ;
		friend class Live2DMotion ;
		friend class Live2DMotionBin ;
	public:
		MotionQueueEnt();
		virtual ~MotionQueueEnt(void);

	public:
		//  フェードアウト開�?
		void startFadeout(long long fadeOutMsec)  ;

		//  終�?���?
		bool isFinished() { return finished ; }

		//  開始時間取�?
		long long getStartTimeMSec(){ return startTimeMSec ; }

		//  フェードイン開始時間取�?
		long long getFadeInStartTimeMSec(){ return fadeInStartTimeMSec ; }

		//  フェードイン終�?��間取�?
		long long getEndTimeMSec(){ return endTimeMSec ; }

		//  開始時間設�?
		void setStartTimeMSec(long long t){ this->startTimeMSec = t ; }

		//  フェードイン開始時間設�?
		void setFadeInStartTimeMSec(long long t){ this->fadeInStartTimeMSec = t ; }

		//  フェードイン終�?��間設�?
		void setEndTimeMSec(long long t){ this->endTimeMSec = t ; }

		//  終�?��定設�?
		void setFinished( bool f ){ this->finished = f ; }

		//  モーションが有効か返す
		bool isAvailable(){ return available ; }

		//  有効化を設�?
		void setAvailable( bool v ){ this->available = v ; }

		//---- state for callback(Unity etc) ----
		//  状態設�?
		void setState( long long time , float weight ){ this->state_time = time ; this->state_weight = weight ; }

		//  時間状態取�?
		long long getState_time(){ return this->state_time ; }

		//  "重み"の状態取�?
		float getState_weight(){ return this->state_weight ; }
	    
	private:
		// パブリ�?��な設定�?
		bool			autoDelete;					//  自動削除
		AMotion* 		motion;						//  モーション

		bool 			available;					//  有効化フラグ
		bool 			finished ;					//  終�?��ラグ
		bool 			started ;					//  開始フラグ??.9.00以降�?
		long long 		startTimeMSec ;				//  開始時刻
		long long 		fadeInStartTimeMSec ;		//  フェードイン開始時刻?�ループ�?時�?初回のみ??

		// 終�?��定時刻
		// �?1の場合�?終�?��定時刻なし（未初期化�?また�?�?��ープを意味する??
		// 	非loopの場合�? endTimeMSec = ( startTimeMSec + 1回�?の時間 )
		// 	別モーションの割込�?MotionQueueManagerから startFadeout()が呼ばれる
		//		newEndTimeMSec = (割込時刻+fadeOutMSec) ;
		//	if( endTimeMSec<0 /*ループ時*/ || newEndTimeMSec < endTimeMSec ){
		//			endTimeMSec = newEndTimeMSec 
		// 終�?��定時刻を過ぎた場合�?モーションが�?�?��れる
		long long endTimeMSec ;

		// ---- インスタンスの識別番号
		static int static_motionQueueEntNo ;// インスタンスごとに�?��の値を持つ識別番号の次の値
		int motionQueueEntNo ;// インスタンスごとに�?��の値を持つ識別番号??ew時に設定�?0.8.09追�?�?

	private:
		//---- state for callback(Unity etc) ----
		long long   	state_time ;				//  時刻の状�?
		float       	state_weight ;				//  重みの状�?
	};

}
//--------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif	// __LIVE2D_MOTION_QUEUE_ENT_H__
