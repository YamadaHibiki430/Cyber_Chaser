/**
 *  Live2DMotion.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_MOTION_H__
#define __LIVE2D_MOTION_H__


#include "../Live2D.h"
#include "../memory/LDObject.h"
#include "../ALive2DModel.h"
#include "../type/LDVector.h"
#include "AMotion.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d
{

	class Motion ;
	class MotionQueueEnt ;
	class MemoryParam ;

	
	class Live2DMotion : public live2d::AMotion
	{
	public:
		// Constructor
		Live2DMotion();

		// Destructor
		virtual ~Live2DMotion();
		
	public:
		//  パラメータ更新
		virtual void updateParamExe( live2d::ALive2DModel * model , long long timeMSec , float weight , MotionQueueEnt *motionQueueEnt) ;

		//  ループ設�?
		void setLoop( bool _loop ){ this->loop = _loop ; }

		//  ループするか?
		bool isLoop( ){ return this->loop ; }
		
		//  ループ時にフェードインが有効かど�?��を設定する�?�?��ォルトでは有効�?
		void setLoopFadeIn( bool _loopFadeIn ){ this->loopFadeIn = _loopFadeIn ; }
		
		//  ループ時のフェードインが有効かど�?��を取�?
		bool isLoopFadeIn( ){ return this->loopFadeIn ; }
		
		//  モーションの長さを返す。ループ�?時�?-1
		virtual int getDurationMSec() ;
		
		
		//  mtnファイルで定義されて�?���??のモーションの長さを返す
		virtual int getLoopDurationMSec()
		{
			return loopDurationMSec ; 
		}

		//  モーション�??タをロー�?2byte�?��非対�?
		static Live2DMotion * loadMotion( const live2d::LDString & filepath ) ;
		//  モーション�??タをロー�?2byte�?��非対�?
		static Live2DMotion * loadMotion( const void * buf , int bufSize ) ;
		
		void dump() ;
		

		//SDK2.0
		// パラメータごとのフェード�?設�?
		//void setParamFadeIn(const char* paramID,int value);
		//void setParamFadeOut(const char* paramID,int value);
		void setParamFadeIn(LDString paramID,int value);
		void setParamFadeOut(LDString paramID,int value);
		
		int getParamFadeIn(const char* paramID);
		int getParamFadeOut(const char* paramID);


	private:
		live2d::LDVector<Motion*>* 	motionsPtr ;			//  モーションリス�?

		float 						srcFps ;				//  ロードしたファイルのFPS。記述が無ければ�?��ォルト�?15fpsとな�?
		int 						maxLength ;				//  

		int 						loopDurationMSec ;		//  mtnファイルで定義される�??のモーションの長�?
		bool 						loop ;					//  ループするか?
		bool 						loopFadeIn;				//  ループ時にフェードインが有効かど�?��のフラグ。�?期�?では有効�?
		int 						objectNoForDebug ;		//  �?���?��用
		
		float 						lastWeight ;
		live2d::MemoryParam*		memoryManagement ;
		live2d::AMemoryHolder*		memoryHolderFixedMain ;
	};




#ifndef __SKIP_DOC__

	/***************************************************************************
	�?��のパラメータにつ�?��のアクション定義
	***************************************************************************/
	class Motion : public live2d::LDObject
	{
	public:
		static const int MOTION_TYPE_PARAM = 0 ;
		static const int MOTION_TYPE_PARTS_VISIBLE = 1 ;
		static const int MOTION_TYPE_PARAM_FADEIN = 2 ;//SDK2.0
		static const int MOTION_TYPE_PARAM_FADEOUT = 3 ;//SDK2.0
		
		// �?��変更不可??< で比�?��て�?��ため ??
		static const int MOTION_TYPE_LAYOUT_X = 100 ;
		static const int MOTION_TYPE_LAYOUT_Y = 101 ;
		static const int MOTION_TYPE_LAYOUT_ANCHOR_X = 102 ;
		static const int MOTION_TYPE_LAYOUT_ANCHOR_Y = 103 ;
		static const int MOTION_TYPE_LAYOUT_SCALE_X = 104 ;
		static const int MOTION_TYPE_LAYOUT_SCALE_Y = 105 ;
		
	public:
		Motion(live2d::MemoryParam* memParam) ;
		virtual ~Motion() ;
		
	public:
		int getParamIndex( live2d::ALive2DModel* model ) ;

	public:
		live2d::LDString * 			paramIDStr ;				//  パラメータID�?���?
		
		live2d::ParamID* 			cached_paramID ;			//  �?��のモ�?��でモーションを使�?��わすとIndexのキャ�?��ュができな�?
		int  						cached_paramIndex ;			//
		live2d::ALive2DModel* 		cached_model_ofParamIndex ;

		live2d::LDVector<float> 	values ;
		
		int 						motionType ;				//  モーションタイ�?

		//SDK2.0
		int 	fadeInMsec ;		// こ�?パラメータのフェードインする時間。無効値のとき�?モーション全体�?設定を優先す�?
		int 	fadeOutMsec ;		
	};

#endif // __SKIP_DOC__

}
//--------- LIVE2D NAMESPACE ------------

#endif		// __LIVE2D_MOTION_H__
