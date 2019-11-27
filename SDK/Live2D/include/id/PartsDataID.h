/**
 *  PartsDataID.h
 * 
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_PARTS_DATA_ID_H__
#define __LIVE2D_PARTS_DATA_ID_H__


#include "../Live2D.h"
#include "ID.h"

//------------ LIVE2D NAMESPACE ------------
namespace live2d
{ 
	
	class PartsDataID  : public ID
	{
	public:
		const static int L2D_PARTS_ID_INITIAL_CAPACITY = 64 ;
		
	public:
		// IDを取得す�?
		// ・同�??値を持つ全てのIDが�?同じポインタを指すことを保証する ?�同�??確認が�?= 比�?で良�?�?\n
		// ・Live2D::dispose()時に解放され�?
		static PartsDataID * getID(const live2d::LDString &str ) ;

		static PartsDataID * getID(  const RefString& refStr ) ;
	
		// リリース用メソ�?��??ive2D::diposeから呼ばれる)
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
		// Constructor
		PartsDataID() ;									
		PartsDataID( const char* str ) ;					
		PartsDataID(const live2d::LDString &str );		

		//Prevention of copy Constructor	
		PartsDataID( const RefString& refStr ) ;		
		PartsDataID( const PartsDataID & ) ;			
		PartsDataID& operator=( const PartsDataID & ) ; 

		// Destructor
		virtual ~PartsDataID();
		
	private:	
		//------------ static ------------
		static LDVector<PartsDataID*>*		idlist ;
		
	private:	
		//------------ this ------------
		live2d::LDString 					id ;// 生�?時に�?��されて�?��ため、Destructorで開放する
	};
}
//------------ LIVE2D NAMESPACE ------------

#endif		// __LIVE2D_PARTS_DATA_ID_H__
