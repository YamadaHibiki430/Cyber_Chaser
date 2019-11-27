/*
 *  MemoryParam.h
 *
 * メモリ確保用パラメータ
 * 
 * �?��メソ�?��にこ�?パラメータを伝播させてメモリ管�?��制御する�?
 * 
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */


#pragma once


#ifndef __SKIP_DOC__

#include "LDUnmanagedObject.h"
#include "fixed/MemoryHolderFixed.h"
#include "tmp/MemoryHolderTmp.h"
#include "LDAllocator.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{


	class MemoryPage ;

	//----------------------------------------
	class MemoryStackEnt : public LDUnmanagedObject// ここ�?��はLDObjectを継承しな�?
	{
		friend class MemoryParam ;
	public:
		MemoryStackEnt() : nextEnt(NULL), allocType(0) , align(0) , holder(NULL)
		{
		}

		virtual ~MemoryStackEnt(){}

	private:
		MemoryStackEnt*		nextEnt ;

		int					allocType ;
		int					align ;
		AMemoryHolder*		holder ;// 開放しな�?
	} ;


	//----------------------------------------
	class MemoryParam : public LDObject// TmpをPlacement newにすることで、Tmpメモリ利用を可能に変更
	{
	public:

	//	MemoryParam();
		MemoryParam(AMemoryHolder* main , AMemoryHolder* gpu);
		
		virtual ~MemoryParam() ;

		// Allocの設定を�?��す�?戻り�?は直前�?設定�?
		LDAllocator::Type  setAllocType( LDAllocator::Type  allocType )
		{
			LDAllocator::Type  ret = this->curAllocType	 ;
			this->curAllocType	= allocType ;
			this->curMemoryHolder	= getMemoryHolder( allocType ) ;	
			return ret ;
		}

		// Allocの設定を取り出�?戻り�?は直前�?設定�?
		int  setAllocAlign( int align )
		{
			int ret = this->curAlign ;
			this->curAlign = align ;
			return ret ;
		}

		// Allocの設定を取り出�?
		LDAllocator::Type getAllocType( )
		{
			return this->curAllocType ;
		}

		// Allocの設定を取り出�?
		int  getAllocAlign( )
		{
			return curAlign ;
		}


		void* malloc_exe( l2d_size_t size  )
		{
			return curMemoryHolder->malloc_exe( size , curAlign ) ;
		}

		AMemoryHolder*  getCurMemoryHolder()
		{
			return curMemoryHolder ;
		}

		AMemoryHolder*  getMemoryHolder( LDAllocator::Type  allocType ) ;

		void clear() ;

		// 開放しな�??で外部で管�?���?
		void setMemoryHolderMain(AMemoryHolder*	h)
		{
			this->memoryHolderMain = h ;
		}

		// 開放しな�??で外部で管�?���?
		void setMemoryHolderGPU(AMemoryHolder*	h)
		{
			this->memoryHolderGPU = h ;
		}

		void checkMemory()
		{
			memoryHolderMain->healthCheck();
			memoryHolderGPU->healthCheck();
		}
	private:

		AMemoryHolder*			curMemoryHolder ;
		LDAllocator::Type		curAllocType ;
		int						curAlign ;

		AMemoryHolder*			memoryHolderMain ;	
		AMemoryHolder*			memoryHolderGPU ;	

	};


}
//--------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__
