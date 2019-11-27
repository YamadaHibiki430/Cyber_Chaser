/**
 *  MotionQueueManager.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_MOTION_QUEUE_MANAGER_H__
#define __LIVE2D_MOTION_QUEUE_MANAGER_H__


#include "../memory/LDObject.h"
#include "../ALive2DModel.h"

#include "../type/LDVector.h"
#include "AMotion.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d
{
	class Live2DMotion ;
	class MotionQueueEnt ;
	class MemoryParam ;


	
	class MotionQueueManager : public live2d::LDObject 
	{
	public:
		MotionQueueManager();
		virtual ~MotionQueueManager();
		
		// �?���?モーションを開始す�?
		// 同じタイプ�?モーションが既にある場合�?、既存�?モーションに終�?��ラグを立て、フェードアウトを開始させる�?
		int startMotion( AMotion * motion , bool autoDelete ) ;
		
		// モ�?��のパラメータを設定�?更新する?�動きを反映する??
		bool updateParam( live2d::ALive2DModel * model ) ;

		//  全てのモーションが終�?��て�?���?
		bool isFinished() ;
		
		// 引数で�?��したモーションが終�?��て�?��かを返す�?0.8.09追�?�?
		bool isFinished(int motionQueueEntNo ) ;


		//  全てのモーションを停止する
		void stopAllMotions() ;

		//  モーションの開始�?終�?���?��プす�?
		void setMotionDebugMode( bool f ){ this->motionDebugMode = f ; }

		//  MotionQueueEntを取�?
		MotionQueueEnt* getMotionQueueEnt( int entNo ) ;
	    
	#if L2D_VERBOSE
		void DUMP() ;
	#endif

	private:
		live2d::LDVector<MotionQueueEnt *> *motions ;

		bool motionDebugMode ;// モーションの再生・停止をダンプするフラグ

		live2d::MemoryParam*  memoryManagement ;
		live2d::AMemoryHolder*	memoryHolderFixedMain ;
	};

}
//--------- LIVE2D NAMESPACE ------------

#endif		// __LIVE2D_MOTION_QUEUE_MANAGER_H__
