/**
 *  MHPageHeaderTmp.h
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
#include "../AMemoryHolder.h"
#include "../APageHeader.h"


//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{

	class MemoryHolderTmp ;
	class MHBin ;


	//==========================================================
	// 	ペ�?ジヘッ�?クラス
	// 
	// 	Pageの先�?にPageHeaderが�?り�?以降に�??タが並ぶ
	// 	PageHeaderのサイズは64-sizeof(AllocHeader)以下とする
	// 	chunkの先�?は、this + 64 - sizeof( AllocHeader )となり�?ポインタは this + 64とな�?
	//==========================================================
	class MHPageHeaderTmp : public APageHeader 
	{
	public:
		// placement newを行う
		MHPageHeaderTmp( MemoryHolderTmp* holder , MHBin* bin , l2d_size_t pageSize , l2d_size_t chunkSize , l2d_size_t pageOffset ) ;

		static l2d_size_t  calcChunkCount( l2d_size_t  pageSize , l2d_size_t  chunkSize ) ;

		// 使用可能な�??の要�??イン�?��クスを取り�?し�?使用中フラグを立て�?
		int getFirstEmptyIndexAndSetFlag(MHBin* bin) ;

		void*  getPtr( int index ) ;

		AllocHeader*  getAllocHeaderPtr( int index ) ;

		int  getIndexOfPtr( void* ptr ) ;

		// 使用可能な�??の要�??イン�?��クスを取り�?し�?使用中フラグを立て�?
		void setFlag( int index , bool flag ) ;

		bool getFlag( int index ) ;

		// 開放する
		virtual void  free_exe( void* ptr ){ holder->free_exe( this , ptr ) ; }
		

	protected:
		~MHPageHeaderTmp(){}// placement newを使�?deleteはしな�?

	public:// 仮
		// []�??�?2のバイト数と�?4bitのバイト数
		//void*					vtable			// [4-8]
		MemoryHolderTmp*		holder ;		// [4-8]
		l2d_uint32				bitmask[3] ;	// [12]	32*3=�?��96個�?フラグを持つ�?��、かならずしもPageに96個保持するとは限らな�?
												// 		使用中のも�?はフラグが立つ�?
		l2d_uint16				emptyCount ;	// [2]
		l2d_uint16				chunkCount ;	// [2]	チャンクの合計数
		l2d_uint32				chunkSize ;		// [4]	�?��の�??タのサイズ??llocHeaderを含�?�?
		l2d_uint32				pageSize ;		// [4]
		// ここまで 32-36
	
		MHPageHeaderTmp*		nextPage ;		// [4-8]
		l2d_uint8					pageOffset ;	// [1]	ペ�?ジがアライメント調整される�?合�?調整バイト数?�最大32??
		l2d_uint8					binNo ;			// [1]	MHBinの番号

		// 8-16

		// 64bit版を�?��ると�?��で 64-8 = 56byte
	} ;

}
//--------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__
