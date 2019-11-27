/**
 *  UtString.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */

#ifndef __LIVE2D_UTSTRING_H__
#define __LIVE2D_UTSTRING_H__



#ifndef __SKIP_DOC__

#include "../type/LDVector.h"



//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{ 
	class UtString 
	{
	public:
		
		// 戻り�?が�?��される�?でオーバ�?ヘッド�?大きい。デバッグなどで使�?
		static live2d::LDString toString( const char * msg , ... ) ;
	
		// textがstartWordで始まって�?��場合にtrueを返す
		static bool startsWith( const char * text , const char * startWord ) ;
	
		// mtnで定義されるfloat�?��を解釈する�?�?��表示には未対�?
		static float strToFloat( const char* str , int len , int pos , int* ret_endpos ) ;
	
	private:
		UtString();									
	
		//Prevention of copy Constructor
		UtString( const UtString & ) ;				
		UtString& operator=( const UtString & ) ; 	
		
		~UtString();
	
	};
	
}
//------------------------- LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif		// __LIVE2D_UTSTRING_H__
