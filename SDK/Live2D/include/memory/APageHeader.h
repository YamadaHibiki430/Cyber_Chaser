/*
 *  APageHeader.h
 *
 *  メモリ保持のための抽象クラス
 * 
 *  通常のメモリ管�?��、寿命管�?��のメモリ管�?��抽象�?
 * 
 * 
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */

#pragma once


#ifndef __SKIP_DOC__

#include "LDUnmanagedObject.h"
//#include "../memory/MemoryPage.h"
#include "LDAllocator.h"


//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{


	//==========================================================
	//==========================================================
	class APageHeader // placement newを使�??deleteしな�?
	//class APageHeader  : public LDUnmanagedObject // メモリ管�?��象�?
	{
	public:
		const static int ENTRY_OFFSET = 64 ;// ペ�?ジの�??の�??タへのオフセ�?��??hunkは-4(32bit) -8(64bit))

		APageHeader(){}

		
		virtual void  free_exe( void* ptr ) = 0 ;


	protected:
		~APageHeader(){}// placement newを使�?deleteはしな�?

	};


}
//--------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__
