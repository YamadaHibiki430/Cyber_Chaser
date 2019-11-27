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
	
		// IDを取得す�?
		// ・同�??値を持つ全てのIDが�?同じポインタを指すことを保証する ?�同�??確認が�?= 比�?で良�?�?\n
		// ・Live2D::dispose()時に解放され�?
		static DrawDataID * getID( const LDString & str ) ;

		static DrawDataID * getID(  const RefString& refStr ) ;
	
		static void releaseStored_notForClientCall() ;
	
	
	public:
		/****************************************************************************
		@~english

		@~japanese
		@brief		IDをC�?���?�?���?として取�?
		@return		ID�?���?
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


