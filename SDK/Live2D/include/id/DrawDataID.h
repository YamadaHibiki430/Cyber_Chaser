/**
 *  DrawDataID.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_DRAW_DATA_ID_H__
#define __LIVE2D_DRAW_DATA_ID_H__


#include "../Live2D.h"
#include "ID.h"

//------------ LIVE2D NAMESPACE ------------
namespace live2d
{ 
	
	class DrawDataID  : public ID
	{
	public:
		const static int L2D_DRAWDATA_ID_INITIAL_CAPACITY = 256 ;
	
	
	public:
	
		// IDãåå¾ãã?
		// ã»åä¸??å¤ãæã¤å¨ã¦ã®IDãã?åããã¤ã³ã¿ãæããã¨ãä¿è¨¼ãã ?åä¸??ç¢ºèªãã?= æ¯è¼?ã§è¯ã?¼?\n
		// ã»Live2D::dispose()æã«è§£æ¾ããã?
		static DrawDataID * getID( const LDString & str ) ;

		static DrawDataID * getID(  const RefString& refStr ) ;
	
		static void releaseStored_notForClientCall() ;
	
	
	public:
		/****************************************************************************
		@~english

		@~japanese
		@brief		IDãCè¨?ªã?æ?­å?ã¨ãã¦åå¾?
		@return		IDæ?­å?
		****************************************************************************/
		const char * toChar(){ return id.c_str() ; }

		
	private:	
		DrawDataID() ;									
		DrawDataID( const char* str ) ;					
		DrawDataID( const RefString& refStr  ) ;		
		DrawDataID(const LDString & tmp_str);			
	
		//Prevention of copy Constructor
		DrawDataID( const DrawDataID & ) ;				
		DrawDataID& operator=( const DrawDataID & ) ;	

		virtual ~DrawDataID();
		
	private:	
		static LDVector<DrawDataID*>* idlist ;
		
	private:	
		LDString id ;
	};
} 
//------------ LIVE2D NAMESPACE ------------

#endif		// __LIVE2D_DRAW_DATA_ID_H__


