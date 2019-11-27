/**
 *  MHBin.h
 *
 * �?��インスタンス用のメモリ保持用クラス
 * 
 * 通常の確保�?破�?��行うメモリ実�?
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
	class MHPageHeaderTmp ;


	//==========================================================
	// �?in?�ビン?�クラス
	// �?2 , 64 , 128等バイトサイズごとにペ�?ジ�??タを�?納す�?
	//==========================================================
	class MHBin
	{
	public:
		MHBin() ;

		void init( l2d_uint16 binNo , l2d_size_t _chunkSize , l2d_size_t _pageSize ) ;
		
		l2d_size_t	getChunkSize( l2d_size_t  malloc_size ) ;

	public:// 仮
		l2d_size_t					chunkSize ;			// ?�つのチャンクサイズ�?llocHeaderを含�??0の時�?自由なサイズ
		l2d_size_t					pageSize ;			// ペ�?ジ全体�?サイズ�?llocHeaderを含�??0の時�?自由なサイズ
		l2d_uint16				pageChunkCount ;	// ?�つのペ�?ジに入るチャンク数
		l2d_uint16				binNo ;				// 自身のBin番号??ndex??
		l2d_uint32				bitmask[3] ;		// 使用可能なビットを1とする(使用中と�?��意味ではな�?�?は使用不可

		MHPageHeaderTmp*		filledPages ;		// 全部使用中
		MHPageHeaderTmp*		curPages ;			// 使用可能ペ�?ジ
	} ;

}
//--------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__
