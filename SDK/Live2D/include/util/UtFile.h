/**
 *  UtFile.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */

#ifndef __LIVE2D_UTFILE_H__
#define __LIVE2D_UTFILE_H__


#include "../type/LDVector.h"


//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{ 
	
	class UtFile 
	{
	public:
		// ファイルからバイナリ�??タをロードして返す。失敗した�?合�? NULL が返る�?
		// �?�� UtFile::releaseLoadBuffer() で破�?��る�?
		static char* loadFile( const live2d::LDString &filepath , int * ret_bufsize ) ;

		// ファイルからバイナリ�??タをロードして返す。失敗した�?合�? NULL が返る�?
		// �?�� UtFile::releaseLoadBuffer() で破�?��る�?
		static char* loadFile( const char * filepath , int * ret_bufsize ) ;
	
		// loadFile()でロードしたバ�?��ァを�?�?���?
		static void releaseLoadBuffer( char * buf ) ;
		
	private:
		UtFile();	
									
		//Prevention of copy Constructor
		UtFile( const UtFile & ) ;				
		UtFile& operator=( const UtFile & ) ; 	
	
		~UtFile();
	
	};
} 
//------------------------- LIVE2D NAMESPACE ------------

#endif		// __LIVE2D_UTFILE_H__
