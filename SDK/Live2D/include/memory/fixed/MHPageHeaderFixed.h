/*
 *  MHPageHeaderFixed.h
 *
 *	�?��破�??み対応したメモリペ�?ジ
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */

#pragma once


#ifndef __SKIP_DOC__

#include "../../Live2D.h"
#include "../LDUnmanagedObject.h"
#include "../APageHeader.h"
#include "../AMemoryHolder.h"


//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{

	class MHPageHeaderFixed : public APageHeader // MemoryManagement対象�?
	{
	public:
		MHPageHeaderFixed();

		int getPageNo(){ return pageNo ; }
		int getPageAmount(){ return pageAmount ; }

		void* getStartPtr(){ return (char*)this + ENTRY_OFFSET - sizeof(AllocHeader) ; }

		virtual void  free_exe( void* ptr ) ;

	protected:
		~MHPageHeaderFixed(){}// placement newを使�?deleteはしな�?

	public:// ---�?��下�?パブリ�?��参�?を許可 ---
		MHPageHeaderFixed*	nextPage ;		// [4-8]隣のペ�?ジ

	//	char*				startPtr ;		// [4-8]ここに記�?する?�確保後�?固定�?
		char*				endPtr ;		// [4-8]ポインタの終端?�確保後�?固定�?こ�?アドレス自体�?�?��外�?
		l2d_uint32			size ;			// [4-8]こ�?ペ�?ジのメモリサイズ?�確保後�?固定�?

		char*				curPtr ;		// [4-8]次に確保できるポインタ(AllocHeaderの先�?にな�??�可変�?
		l2d_uint32			rest ;			// [4-8]こ�?ペ�?ジの残りサイズ?�可変�?
		l2d_uint32			pageNo ;		// [4]こ�?ペ�?ジの通し番号??D??


		//--- static ---
		static int			pageAmount ;	// 全体�?ペ�?ジ数?�開放しても減らさな�?�?
	};



}
//--------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__
