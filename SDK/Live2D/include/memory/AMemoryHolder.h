/*
 *  AMemoryHolder.h
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

#include "LDObject.h"
#include "LDAllocator.h"
#include "APageHeader.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{

	//==========================================================
	//==========================================================
	class AllocHeader 
	{
		// 継承しな�?�?tableを持たせな�?�?
	public:
		void  free_exe( void* ptr ){ ptrToPageHeader->free_exe( ptr ) ; }


	private:
		AllocHeader() ;
		~AllocHeader() ;

	public:
		APageHeader*	ptrToPageHeader ;
	} ;



	//==========================================================
	//==========================================================
	class AMemoryHolder  : public LDObject //
	{
	public:
		static const int		CHECK_VALUE = 0x600DC0DE ;// Oはゼロであることに注�?
		static const int		MIN_CHUNK_REST = 32 ;// これ以下�?残り容量�?場合�?、filledPageへ
		static const int		MIN_ALIGN = 4 ;// アライメントがコレ以下�?場合�?こ�?値にする


		AMemoryHolder();
		virtual ~AMemoryHolder() ;

		void setMemoryParam( MemoryParam* group ){ this->memParam = group ; }

		virtual void* malloc_exe( l2d_size_t size  , int align) = 0 ;
		
		virtual void free_exe( APageHeader* header ,void* ptr )  = 0 ;
		virtual void clear() = 0 ;

		virtual void healthCheck() { };

		int getCheckValue(){ return CHECK_VALUE ; }


	protected:
		MemoryParam*	memParam ;

	};


}
//---------- namespace live2d -------------

#endif // __SKIP_DOC__

