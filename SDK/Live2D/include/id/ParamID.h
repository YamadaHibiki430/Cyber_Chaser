/**
 *  ParamID.h
 * 
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_PARAM_ID_H__
#define __LIVE2D_PARAM_ID_H__


#include "../Live2D.h"
#include "ID.h"

//------------ LIVE2D NAMESPACE ------------
namespace live2d
{ 
	
	class ParamID  : public ID
	{
	public:
		const static int L2D_PARAM_ID_INITIAL_CAPACITY = 64 ;
	
	
	public:
		
		const char * toChar() const { return id.c_str() ; }
	
	
		// IDを生成す�?
		//  ・同�??値を持つ全てのIDが�?同じポインタを指すことを保証する ?�同�??確認が�?= 比�?で良�?�?\n
		//  ・Live2D::dispose()時に解放され�?
		static ParamID * getID( const live2d::LDString & tmp_idstr ) ;
		static ParamID * getID( const char*  tmp_idstr ) ;
		static ParamID * getID(  const RefString& refStr ) ;
	
		/*
		 * リリース用メソ�?��??ive2D::diposeから呼ばれる)
		 */
		static void releaseStored_notForClientCall() ;
	
	
	
	
	#if L2D_SAMPLE
		static ParamID* SAMPLE_ID_X(){ return getID("ANGLE_X") ; }
		static ParamID* SAMPLE_ID_Y(){ return getID("ANGLE_Y") ; }
		static ParamID* SAMPLE_ID_ANGLE_Z(){ return getID("ANGLE_Z") ; }
		static ParamID* SAMPLE_ID_SMILE(){ return getID("SMILE") ; }
	#endif
		

		
	private:
		ParamID() ;
		ParamID( const char* str ) ;					
		ParamID( const live2d::LDString& _tmp_str);
		ParamID( const RefString& refStr ) ;
		//Prevention of copy Constructor	
		ParamID( const ParamID & ) ;
		ParamID& operator=( const ParamID & ) ;		

		virtual ~ParamID();
		
	private:
		static LDVector<ParamID*>* 		idlist ;		
	
	private:
		live2d::LDString 				id ;			// ID�?���?
	};
}
//------------ LIVE2D NAMESPACE ------------

#endif		// __LIVE2D_PARAM_ID_H__
