/**
 *  Live2DMotionBin.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */

#pragma once


#include "../Live2D.h"
#include "../ALive2DModel.h"
#include "../type/LDVector.h"
#include <vector>
#include "AMotion.h"


//--------- LIVE2D NAMESPACE ------------
namespace live2d
{
	class MotionBin ;
	class MotionQueueEnt ;



	
	class Live2DMotionBin : public AMotion{
	public:

		// loadMotionでバイナリを渡した場合に、第?�引数をどのようにするかを�?��す�?
		typedef enum {
			// �?��トラクト時に�?��ータを�?�?��な�?
			DO_NOTHING_ON_DESTRUCT ,		// 外部で破�?
			DUPLICATE_AND_DESTRUCT ,		// 渡した�??タを�?��して使�?��渡した�??タには何もしな�?�?

			// �?��トラクト時に�?���?方式で破�?���?
			FREE_ON_DESTRUCT ,				// free()で破�?
			DELETE_ARRAY_ON_DESTRUCT , 		// delete[]で破�?
			DELETE_NORMAL_ON_DESTRUCT  , 	// 通常のdelete で破�?
			
			L2D_FREE_ON_DESTRUCT ,			// Live2D独自のメモリ管�?��式で破�?
			UTFILE_RELEASE_ON_DESTRUCT		// UtFile::releaseLoadBuffer()で破�?
		} BufType ;



		Live2DMotionBin();
		virtual ~Live2DMotionBin();
		
		virtual void updateParamExe( live2d::ALive2DModel * model , long long timeMSec , float weight , MotionQueueEnt *motionQueueEnt) ;


		void setLoop( bool _loop ){ this->loop = _loop ; }
		bool isLoop( ){ return this->loop ; }
		
		/**
		 * モーションの長さを返す?�ループ�?とき�? -1??
		 * -1のとき�?終わらな�??�?��な�?
		 */
		virtual int getDurationMSec() ;
		
		
		/** 
		 * mtnファイルで定義される�??のモーションの長さ（＝ループ１回�??長さ）を返す (0.9.00b8)
		 */
		virtual int getLoopDurationMSec(){ 
			return loopDurationMSec ; // mtnファイルで定義される�??のモーションの長�?
		}

		/*
		 * モーション�??タをロードす�?
		 * 
		 * ?�バイト文字�?未対�?
		 */
		static Live2DMotionBin * loadMotion( const l2d_string & filepath ) ;

		/*
		 * モーション�??タをバイト�?からロードす�?
		 *
		 * bufType  bufのポインタをどのように破�?��るかポインタ確保�?方式にあわせて以下から指定する�?
		 *
		 *   BufType::DO_NOTHING_ON_DESTRUCT ,		// �?��トラクト時に破�?��な�?��外部で破�?�?
		 *	 BufType::DUPLICATE_AND_DESTRUCT ,		// 渡した�??タを�?��して使�?��渡した�??タには何もしな�?�?
		 *	 BufType::FREE_ON_DESTRUCT ,			// �?��トラクト時にfree()で破�?
		 *	 BufType::DELETE_ARRAY_ON_DESTRUCT , 	// �?��トラクト時にdelete[]で破�?
		 *	 BufType::DELETE_NORMAL_ON_DESTRUCT  , // �?��トラクト時に通常のdelete で破�?
		 *	 BufType::UTFILE_RELEASE_ON_DESTRUCT	// �?��トラクト時にUtFile::releaseLoadBuffer()で破�?
		 *
		 * deleteBufOnDestructor bufをLive2DMotionBinのDestructorで破�?��る�?合にtrue
		 */
		static Live2DMotionBin * loadMotion( 
						const void * buf ,					// モーションファイルをロードした�?インタ
						int bufSize ,						// モーションの�??タサイズ(バイト�?
						Live2DMotionBin::BufType  bufType	// bufの使�?��、�?�??仕方を指定す�?
					) ;
		


		void dump() ;
		


	private:
		const void *				buf ;// ロードされたバイナリ。deleteBufOnDestructorがtrueの場合に破�?

		Live2DMotionBin::BufType	bufType ;// bufをどのように扱�?��??num BufType で�?��する�?
	//	bool						deleteBufOnDestructor ;// Destructorでポインタを�?�?��る�?合にtrue

		//---- 
		MotionBin**					motionPtrArray ;
		int							paramCount ;//count of MotionBin

		// ---- ロードデータ
		float 						srcFps ;// ロードしたファイルのFPS。記述が無ければ�?��ォルト�?15fpsとな�?
		int 						maxLength ;

		int 						loopDurationMSec ;// mtnファイルで定義される�??のモーションの長�?
		bool 						loop ;
		int 						objectNoForDebug ;
		
		float 						lastWeight ;
		live2d::MemoryParam*		memoryManagement ;
		live2d::AMemoryHolder*		memoryHolderFixedMain ;

	};




#ifndef __SKIP_DOC__

	//=================================================
	//  	�?��のパラメータにつ�?��のアクションを定義するクラス
	//=================================================
	class MotionBin : public live2d::LDObject
	{
	public:
		static const int MOTION_TYPE_PARAM = 0 ;
		static const int MOTION_TYPE_PARTS_VISIBLE = 1 ;
		
		// �?��変更不可??< で比�?��て�?��ため ??
		static const int MOTION_TYPE_LAYOUT_X = 100 ;
		static const int MOTION_TYPE_LAYOUT_Y = 101 ;
		static const int MOTION_TYPE_LAYOUT_ANCHOR_X = 102 ;
		static const int MOTION_TYPE_LAYOUT_ANCHOR_Y = 103 ;
		static const int MOTION_TYPE_LAYOUT_SCALE_X = 104 ;
		static const int MOTION_TYPE_LAYOUT_SCALE_Y = 105 ;
		
		MotionBin(void);
		virtual ~MotionBin(void);
		
		int getParamIndex( live2d::ALive2DModel* model ) ;

	public:
		//--- fields ---
		l2d_string * 			paramIDStr ;

		// �?��のモ�?��でモーションを使�?��わすとIndexのキャ�?��ュができな�?
		live2d::ParamID* 		cached_paramID ;
		int  					cached_paramIndex ;//
		live2d::ALive2DModel* 	cached_model_ofParamIndex ;

	//	l2d_vector<float> 		values ;

		// �??タの持ち方を�?float[] short[]型�?二種類使�??ける
		// 通常は�??タサイズを小さくするた�?min,max間をshort(65536段�?6bit)に�?��する
		// 値のレンジが�?��場合�?min,max = -1000,1000のような場合）�?
		// より精度の高いfloat[](仮数部23bit)を用�?��直接値を設定す�?

		// min,maxの間でshort値を設定する�?合�? true
		bool					isShortArray ;

		void*					valuePtr ;// float[]また�?short[]を指す�?ファイルのバイナリを直接参�?するので破�?��な�?
		float					minValue ;
		float					maxValue ;
		
		int						valueCount ;
		
		int 					motionType ;
	};

#endif // __SKIP_DOC__

}
//--------- LIVE2D NAMESPACE ------------
