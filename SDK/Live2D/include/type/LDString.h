/**
 *  LDString.h
 * 
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */

#ifndef __LIVE2D_LD_STRING_H__
#define __LIVE2D_LD_STRING_H__


#include "../memory/LDObject.h"
#include "../io/RefString.h"
# include <string.h>

// 旧版との互換性維持�?ための宣�??今後使用中止予�?
#define l2d_string			live2d::LDString


//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{ 
	
	class LDString  : public live2d::LDObject 
	{
	public:
		// Constructor
		LDString();
		LDString( const char * s , MemoryParam* memParam = NULL ) ;
		LDString( const char * s , int length , MemoryParam* memParam = NULL) ;
		LDString( const RefString& refStr , MemoryParam* memParam = NULL) ;
		LDString( const LDString & s , MemoryParam* memParam = NULL) ;
		LDString( const char * s , int length , bool useptr , MemoryParam* memParam = NULL) ;// useptr==trueなら引数のポインタを保持する?��?インタはDestructorでL2D_DELETEされる�?
	
		// Destructor
		~LDString();
	
	
	public:
	
		//  =演算子�?オーバ�?ロー�?
		LDString& operator=( const LDString & s ) ; 
		LDString& operator=( const char* c ) ;
	
		//  ==演算子�?オーバ�?ロー�?
		bool operator==( const RefString & s ) const ;
		bool operator==( const LDString & s ) const ;
		bool operator==( const char* c ) const ;
	
		//  <演算子�?オーバ�?ロー�?
		bool operator<( const LDString & s ) const ;
		bool operator<( const char* c ) const ;
	
		//  >演算子�?オーバ�?ロー�?
		bool operator>( const LDString & s ) const ;
		bool operator>( const char* c ) const ;
	
		bool equals( const RefString& refStr ) const 
		{
			if( refStr.length != this->_length ) return false ;// サイズ違い
			const char *c1 = this->_ptr ;
			const char *c2 = refStr.ptr_not_zero_end ;
			// �?��違�?���?�?��のはPARAMの比�??特性??
			for( int i = this->_length -1 ; i >= 0 ; --i )
			{
				if( c1[i] != c2[i] ) return false ;
			}
			return true ;
		}
	
		// メモリを浪費するので、基本�?���?���?��等�?特定用途で用�?��
		LDString operator+( const LDString & s ) const ;
		LDString operator+( const char* s ) const ;
	
		//  +=演算子�?オーバ�?ロー�?
		LDString& operator+=( const LDString & s ) ;
		LDString& operator+=( const char* s ) ;
	
		// 
		LDString& append( const char* p , int length ) ;
		LDString& append( int count , const char p ) ;
	
		//  長さ取�?
		unsigned int length() const {	return _length ; }
	
		//  サイズ取�?
		int size() const {	return _length ; }
	
		//  C�?��文字�?としてのポインタ取�?
		const char* c_str() const 
		{ 
			return _ptr ; 
		}
	
		//  現在のポインタをクリアする
		void clear() ;
	
		//  ハッシュコード取�?
		int getHashcode()
		{
			if( _hashcode == -1 ) _hashcode = calcHashcode( this->_ptr , this->_length ) ;
			return _hashcode ;
		}
	
	protected:
		void copy( const char* ptr , int length ) ;// Constructor等で用�?��

		void init( const char * s , int length , bool useptr ) ;// useptr==trueなら引数のポインタを保持する?��?インタはDestructorでL2D_DELETEされる�?
		int calcHashcode( const char* p1 , int length ) ;
	
	private:
		static const int DEFAULT_SIZE = 10 ;
	
	private:
		char * 				_ptr ;					// C�?���?のポインタ
		int 				_length ;				// 半角文字数?�メモリ確保�?�?��に0が�?るため_length+1??
		int 				_hashcode ;				//
	
		int 				instanceNo ;			// インスタンス番号
		static int 			totalInstanceNo ;		// 通算�?インスタンス番号
	
		MemoryParam* 		memParam ;				//
	};

} 
//------------------------- LIVE2D NAMESPACE -----------

#endif	// __LIVE2D_LD_STRING_H__
