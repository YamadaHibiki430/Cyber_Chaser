/**
 *  MemoryHolderTmp.h
 *
 *  �?��インスタンス用のメモリ保持用クラス
 * 
 *  通常の確保�?破�?��行うメモリ実�?
 * 
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#pragma once


#ifndef __SKIP_DOC__

#include "../../Live2D.h"
#include "../LDObject.h"
#include "../LDUnmanagedObject.h"
#include "../AMemoryHolder.h"
#include "../APageHeader.h"
#include "MHBin.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{

	class MemoryHolderTmp ;
	class MHPageHeaderTmp ;

	//==========================================================
	// �?��ンポラリ型�?メモリホル�?
	// �??常のメモリ管�?��式（�?由にmalloc/free可能??
	//==========================================================
	class MemoryHolderTmp : public LDUnmanagedObject
	//class MemoryHolderTmp : public AMemoryHolder
	{
	public:
		const static l2d_size_t		LARGE_0					= 0 ;	// 自由なサイズの時に、chunkSize 、pageSizeに設定される
		const static l2d_size_t		PAGE_ALIGN				= 32 ;	//
		const static int		BIN_COUNT				= 6 ;	// BINの数
		const static l2d_size_t		CHUNK_SIZE[ BIN_COUNT ] ;

		//------------
		MemoryHolderTmp(LDAllocator::Type allocType , const char* holderName );
		virtual ~MemoryHolderTmp() ;

		virtual void* malloc_exe( l2d_size_t size  , int align) ;
	
		virtual void free_exe( APageHeader* header , void* ptr ) ;
		virtual void clear() ;

	private:
		MHPageHeaderTmp*	createNewPage( MHBin* bin , l2d_size_t chunkSize ) ;

		// ターゲ�?��pageの前�?pageを取得する�?見つかった�?合�? *ret_found == true 
		// 戻り�?は前�?ペ�?ジ�?ret_found == true で戻り�?がNULLの場合�? first ??in->curPages等）と�??
		MHPageHeaderTmp*  getPrevPage( MHPageHeaderTmp*  first , MHPageHeaderTmp* targetPage , bool* ret_found ) ;

		void dumpPages_exe( MHPageHeaderTmp* p , MHBin* bin ) ;

		void deletePage( MHPageHeaderTmp* page , MHBin* bin ) ;
		MHBin*  getBinForSize( l2d_size_t  size ) ;
		MHBin*	getBin( int binNo ) ;

		void  dumpPages() ;

	private:
		LDAllocator::Type		allocType ;
		const char*				holderName ;	// MemoryHolderの識別�?
		int						holderNo ;		// MemoryHolderの何番目のインスタンス�?

		MHBin					bins[BIN_COUNT] ;
	};


	//==========================================================
	//==========================================================
	class MemoryHolderSocket : public AMemoryHolder
	{
	public:
		MemoryHolderSocket(MemoryHolderTmp* impl) : impl( impl )
		{

		}

		virtual ~MemoryHolderSocket(){}

		virtual void* malloc_exe( l2d_size_t size  , int align){ return impl->malloc_exe( size , align ) ; }
		virtual void free_exe( APageHeader* header ,void* ptr ){ impl->free_exe( header , ptr ) ; }
		virtual void clear(){ impl->clear() ; }

	private:
		MemoryHolderTmp*	impl ;
	} ;

}
//--------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__
