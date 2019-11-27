/**
 *  FileFormat2.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#include "../io/BReader.h"

#ifndef __LIVE2D_FILE_FORMAT2_H__
#define __LIVE2D_FILE_FORMAT2_H__


#ifndef __SKIP_DOC__

#include "../memory/LDObject.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d
{ 
	class ClassDef ;
	
	
	class FileFormat2 : public live2d::LDObject 
	{
	public:
		//  新しいバ�?ジョンを加えた場合�?、�?���?��版にも設定す�?
		static const int LIVE2D_FORMAT_VERSION_V2_6_INTIAL 		= 6;	// �??のバ�?ジョン
		static const int LIVE2D_FORMAT_VERSION_V2_7_OPACITY 	= 7;	// 半�?明に対�?
		static const int LIVE2D_FORMAT_VERSION_V2_8_TEX_OPTION 	= 8;	// �?��スチャにオプションを付け�?
		static const int LIVE2D_FORMAT_VERSION_V2_9_AVATAR_PARTS= 9;	// アバターのパ�?�?��処�?���?
		static const int LIVE2D_FORMAT_VERSION_V2_10_SDK2       = 10 ;//SDK2.0

		// ---- 対応可能な�?��版�?ファイル ---- Live2D#BUILD_NO 1000
		static const int LIVE2D_FORMAT_VERSION_AVAILABLE	 	= LIVE2D_FORMAT_VERSION_V2_10_SDK2 ;
	
		// --- EOF のフラグ
		static const int LIVE2D_FORMAT_EOF_VALUE = 0x88888888;			// �??のバ�?ジョン
	
		// こ�?バ�?ジョンで、クラスのロード方法を使�??ける(Live2DFormat.LIVE2D_FORMAT_VERSIONもあ�?
	//	static const int FILE_FORMAT_VERSION = 0 ;
		static const int NULL_NO	= 0;		// 
		static const int ARRAY_NO	= 23;		// 
		static const int OBJECT_REF = 33;		// 読み込み済みオブジェクト�?参�?
	
	public:
		FileFormat2();
		virtual ~FileFormat2();
		
	public:
		//  �?��したクラス取�?
		static ClassDef * getClass( int classNo ) ;
		
		static void * newInstance(MemoryParam* memParam , int classNo) ;

		static bool isSerializable(int classNo) ;

		//  �?���?クラス番号が�?リミティブ型ならtrue
		static bool isPrimitive(int classNo){ return ( 2 <= classNo && classNo <= 9 ) ; }
	
		
		static bool isPrimitiveDouble(int classNo){ return classNo == 30 ; } // 決め打ち(変更不可)

		
		static bool isPrimitiveFloat(int classNo){ return classNo == 31 ; }// 決め打ち(変更不可)

		
		static bool isPrimitiveInt(int classNo){ return classNo == 29 ; }// 決め打ち(変更不可)
	
	};
} 
//------------------------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif		// __LIVE2D_FILE_FORMAT2_H__
