/**
 *  LDVector.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_LDVECTOR_H__
#define __LIVE2D_LDVECTOR_H__


#include <new>

#include "../Live2D.h"
#include "../memory/UtMemory.h"

#include "../util/UtDebug.h"

#include "../memory/LDObject.h"
#include "../memory/MemoryParam.h"
#include "../type/LDString.h"


#ifndef NULL
#  define NULL    0
#endif


// l2d_vector : 旧版との互換性維持�?ための宣�??今後使用中止予�?
#define l2d_vector			live2d::LDVector


//--------- LIVE2D NAMESPACE ------------
namespace live2d
{ 

	
	template<class T> class LDVector  : public LDObject 
	{
	
	public:
		//
		LDVector() ;
	
		LDVector(MemoryParam* memParam);
		
		LDVector(MemoryParam* memParam , int initialCapacity, int initialSize, bool zeroClear);
		//
		virtual ~LDVector();
	
	public:
		//
		T* getPtr()
		{	
			return _ptr ;
		}
	
		//-- [] --
		T& operator[](int index)
		{	
# if _LDVECTOR_DEBUG_
			if( index < 0 || _size <= index ){	printf("out of bounds error1@LDVector\n") ;	}
# endif
			return _ptr[index] ;  
		}

		//
		const T& operator[] ( int index ) const 
		{	
#if _LDVECTOR_DEBUG_
			if( index < 0 || _size <= index ){	printf("out of bounds error2@LDVector\n") ;	}
#endif
			return _ptr[index] ;	
		}
	
		//
		T& at(int index)
		{	
#if _LDVECTOR_DEBUG_
			if( index < 0 || _size <= index ){	printf("out of bounds error3@LDVector\n") ;	}
#endif	
			return _ptr[index] ;  
		}// 仮実�?
		
		//-- else --
		// push_back。�?インタ、数値型などのConstructor・Destructorを呼び出す�?��がな�??合�? fakseに設�?
		void push_back( const T& value , bool callPlacementNew = true ) ;
		void clear() ;
	
		unsigned int size(){	return _size ; }
	
		//-----------------------------------------
		// vector#resize()に相当する�?�?
		// プリミティブ�?ポインタ型などの時には、updateSize(size,value, false)
		// で呼び出すと多少パフォーマンスが改�?��る�?合がある�?
		void resize( int size , T value = T() )
		{
			updateSize( size , value , true ) ;	
		}
	
		//-----------------------------------------
		// updateSize()   
		// 	vector#resize()に相当するサイズ変更
		// 					
		// 	param size					新しいサイズ
		// 	param value					リサイズ時に埋める�?
		// 	param callPlacementNew		�?��化�?ヒント（�?常 trueで良�?��、�?インタまた�?プリミティブ型の�?
		// 								false にすると配置newを回避して処�?��軽減する可能性がある�?
		// 								resize時に値を単純に代入する場合�? false (プリミティブ�?ポインタ??
		// 								resize時に配置newを呼び出す�?合�?true?�クラスインスタンス??
		//-----------------------------------------
		void updateSize( int size , T value = T() , bool callPlacementNew = true ) ;
	
		void prepare_capacity( int newsize ) ;
	
		class iterator ;
		void insert( iterator pos, iterator start, iterator end , bool callPlacementNew = true ) ;
	
		void assign( int size , T value = T() , bool callPlacementNew = true ) ;
	
		bool remove( int index )
		{
			if( index < 0 || _size <= index ) return false ;// 削除�?���?
			_ptr[index].~T() ;
	
			// 削除(メモリをシフトする)、最後�?�?��を削除する場合�?move不�?
			if( index < _size-1 ) memmove( &(_ptr[index]) , &(_ptr[index+1]) , sizeof(T)*(_size-index-1) ) ;
			--_size ;
			return true ;
		}
	
		
		class iterator 
		{
			//  +++++++++++++++++++++++フレン�?++++++++++++++++++++++++++++++++++++
			friend class LDVector ;
		public:
			iterator() : _index(0) , _vector(NULL){}
			iterator(LDVector<T> *v) : _index(0) , _vector(v){}
			iterator(LDVector<T> *v, int idx) :  _index(idx) , _vector(v){}
	
		public:
			//  =演算子�?オーバ�?ロー�?
			iterator& operator=( const iterator & ite )
			{
	
	
				this->_index = ite._index ;
				this->_vector = ite._vector ;
				return *this ;
			}

			//  前置++演算子�?オーバ�?ロー�?
			iterator& operator++()
			{
	
	
	
				this->_index ++ ;
				return *this ;
			}
			//  前置--演算子�?オーバ�?ロー�?
			iterator& operator--()
			{
				this->_index -- ;
				return *this ;
			}

			//  後置++演算子�?オーバ�?ロー�?intは後置用の�?��ー引数)
			iterator operator++(int)
			{
			    iterator iteold( this->_vector , this->_index++ ) ;// 古�??を保�?
				return iteold ;// 古�??を返す
			}

			//  後置--演算子�?オーバ�?ロー�?intは後置用の�?��ー引数)
			iterator operator--(int)
			{
			    iterator iteold( this->_vector , this->_index-- ) ;// 古�??を保�?
				return iteold ;
			}

			//  *演算子�?オーバ�?ロー�?
			T& operator*()const
			{
				return this->_vector->_ptr[this->_index] ;
			}

			//  !=演算子�?オーバ�?ロー�?
			bool operator!=( const iterator & ite )const
			{	
				return (this->_index != ite._index) || (this->_vector != ite._vector) ;
			}
	
		private:
			int 				_index ;
			LDVector<T> *		_vector ;
		} ;
	
		
		class const_iterator 
		{
			// ++++++++++++++++++++++++++++フレン�?+++++++++++++++++++++++++++++++
			friend class LDVector ;
	
		public:
			// Constructor
			const_iterator() :  _index(0) , _vector(NULL){}
			const_iterator(const LDVector<T> *v) : _index(0) ,  _vector(v){}
			const_iterator(const LDVector<T> *v, int idx) : _index(idx) , _vector(v) {}
	
	
		public:
			//  =演算子�?オーバ�?ロー�?
			const_iterator& operator=( const const_iterator & ite )
			{
				this->_index = ite._index ;
				this->_vector = ite._vector ;
				return *this ;
			}
			//  前置演算�?++)のオーバ�?ロー�?
			const_iterator& operator++()
			{
				this->_index ++ ;
				return *this ;
			}
			//  前置演算�?--)のオーバ�?ロー�?
			const_iterator& operator--()
			{
				this->_index -- ;
				return *this ;
			}

			//  後置演算�?++)のオーバ�?ロー�?intは後置のための�?��ー引数)
			const_iterator operator++(int)
			{
			    const_iterator iteold( this->_vector , this->_index++ ) ;// 古�??を保�?
				return iteold ;// 古�??を返す
			}

			//  後置演算�?--)のオーバ�?ロー�?intは後置のための�?��ー引数)
			const_iterator operator--(int)
			{
			    const_iterator iteold( this->_vector , this->_index-- ) ;// 古�??を保�?
				return iteold ;
			}

			//  *演算子�?オーバ�?ロー�?
			T& operator*()const
			{
				return this->_vector->_ptr[this->_index] ;
			}

			//  !=演算子�?オーバ�?ロー�?
			bool operator!=( const const_iterator & ite )const
			{
				return (this->_index != ite._index) || (this->_vector != ite._vector) ;
			}
	
		private:
			int 					_index ;
			const LDVector<T> *		_vector ;
		} ;
	
		//--------- iterator method ---------

		//  開�?
		const iterator begin()
		{
			iterator ite(this , 0) ;
			return ite ;
		}
	
		//  終�?
		const iterator end()
		{
			iterator ite(this , _size) ;// 終�?
	
	
			return ite ;
		}
	
		//  開�?
		const const_iterator begin() const 
		{
			const_iterator ite(this , 0) ;
			return ite ;
		}
	
		//  終�?
		const const_iterator end() const 
		{
			const_iterator ite(this , _size) ;// 終�?
			return ite ;
		}
	
		//  削除
		const iterator erase(const iterator& ite)
		{
			int index = ite._index ;
			if( index < 0 || _size <= index ) return ite ;// 削除�?���?
	
	
			// 削除(メモリをシフトする)、最後�?�?��を削除する場合�?move不�?
			if( index < _size-1 ) memmove( &(_ptr[index]) , &(_ptr[index+1]) , sizeof(T)*(_size-index-1) ) ;
			--_size ;
	
			iterator ite2(this , index) ;// 終�?
			return ite2 ;
		}
	
		//  削除
		const const_iterator erase(const const_iterator& ite)
		{
			int index = ite._index ;
			if( index < 0 || _size <= index ) return ite ;// 削除�?���?
	
			// 削除(メモリをシフトする)、最後�?�?��を削除する場合�?move不�?
			if( index < _size-1 ) memmove( &(_ptr[index]) , &(_ptr[index+1]) , sizeof(T)*(_size-index-1) ) ;
	
			--_size ;
	
			const_iterator ite2(this , index) ;// 終�?
			return ite2 ;
		}
	
		//
		void dumpAsInt()
		{
			for(int i = 0 ; i < _size ; i++ ) UtDebug::print("%d ," , _ptr[i] ) ;
	
			UtDebug::print("\n") ;
		}
	
	private:
		static const int DEFAULT_SIZE = 10 ;
	private:
		LDVector( const LDVector & ) ;			
		LDVector& operator=( const LDVector & ) ;
	
	private:	
		T * 			_ptr ;
		int 			_size ;			// アイ�?��数
		int 			_capacity ;		// キャパシ�?��。データサイズは_capacity * sizeof(T)  
	
		MemoryParam* 	memoryGroup ;
	};
	
	//======================================================
	//======================================================
	template<class T> 
	LDVector<T>::LDVector()
	: _ptr(NULL) , _size(0), _capacity(0), memoryGroup(L2D_MEM_TMP)
	{
	
	}
	
	//======================================================
	template<class T> 
	LDVector<T>::LDVector(MemoryParam* memParam)
	: _ptr(NULL) , _size(0), _capacity(0), memoryGroup(memParam)
	{
	
	}
	
	//======================================================
	//======================================================
	template<class T> 
	LDVector<T>::LDVector(MemoryParam* memParam , int initialCapacity , int initialSize, bool zeroClear)
		:  memoryGroup(memParam)
	{
		if( memParam == NULL )
		{
			UtDebug::println( "BREAK POINT@LDVector") ;
		}
	
		if( initialCapacity < 1 )
		{
			_ptr = NULL ;
			_capacity = 0 ;
			_size = 0 ;
		}
		else{
			_ptr = (T *)L2D_MALLOC( memParam ,  sizeof(T) * initialCapacity ) ;// ここ�?�� calloc により、確保したバイトを0で埋め�?
	
			if( zeroClear )
			{
				memset( _ptr , 0, sizeof(T) * initialCapacity ) ;
			}
	
			if( _ptr == NULL )
			{
				//TODO ERROR
				_capacity = 0 ;
				_size = 0 ;
				return ;
			}
			_capacity = initialCapacity ;
			_size = initialSize ;
		}
	}
	
	//======================================================
	//======================================================
	template<class T> 
	LDVector<T>::~LDVector()
	{
	
		clear() ;
	}
	
	//======================================================
	//======================================================
	//push_back
	// push_back時にConstructorの呼び出しが�?��な場合に、callPlacementNew = trueとする
	// 実体�?型�?場合に�?���?ポインタ、�?リミティブ型の場合�?不�?
	template<class T>
	void LDVector<T>::push_back( const T& value , bool callPlacementNew )
	{
	
		if( _size >= _capacity )
		{
			prepare_capacity( _capacity == 0 ? DEFAULT_SIZE : _capacity * 2 ) ;
		}
	
		// placement new �?���?アドレスに、実体を生�?する
		if( callPlacementNew )
		{
			L2D_PLACEMENT_NEW( &_ptr[ _size ++ ] ) T( value ) ;
		}
		else{
			_ptr[ _size ++ ] = value ;
		}
	}
	
	//======================================================
	//push_back
	template<class T> 
	void LDVector<T>::prepare_capacity( int newsize )
	{
		if( newsize > _capacity )
		{
			if( _capacity == 0 )
			{
				_ptr = (T *)L2D_MALLOC( memoryGroup ,  sizeof(T) * newsize ) ;
				if( _ptr == NULL )
				{
					//TODO ERROR
					return ;
				}
				_capacity = newsize ;
			}
			else{
				int tmp_capacity = newsize ;
				T* tmp = (T *)L2D_MALLOC( memoryGroup ,  sizeof(T) * tmp_capacity ) ;
	
				if( tmp == NULL )
				{
	
					//TODO ERROR
					return ;
				}
				else{
					memcpy( (void*)tmp , (void*)_ptr , sizeof(T) *_capacity ) ;// 通常のMALLOCになったためコピ�?する
					L2D_FREE( _ptr ) ;
	
					_ptr = tmp ;
					_capacity = newsize ;
				}
			}
		}
	}
	
	
	
	
	
	//======================================================
	//======================================================
	template<class T> 
	void LDVector<T>::clear()
	{
	
		for( int i = 0 ; i < _size ; i++ )
		{
			_ptr[i].~T() ;
		}
	
		L2D_FREE( _ptr ) ;
	
		_ptr = NULL ;
		_size = 0 ;
		_capacity = 0 ;
	}
	
	
	template<class T> 
	void LDVector<T>::updateSize( int new_size , T value , bool callPlacementNew )
	{
		int cur_size = this->_size ;
		if( cur_size < new_size )
		{
			prepare_capacity( new_size ) ;// capacity更新
	
			if( callPlacementNew )
			{
				for( int i = _size ; i < new_size ; i++ )
				{
					// placement new �?���?アドレスに、実体を生�?する
					L2D_PLACEMENT_NEW ( &_ptr[ i ] ) T( value ) ;
				}
			}
			else{
				for( int i = _size ; i < new_size ; i++ )
				{
					_ptr[ i ] = value ;
				}
			}
		}
		else{//new_size <= _size
			//---
			for( int i = new_size ; i < _size ; i++ )
			{
				_ptr[i].~T() ;// 不要なので破�?���?
			}
	
		}
		this->_size = new_size ;
	}
	
	//-------------
	template<class T> 
	void LDVector<T>::assign( int new_size , T value , bool callPlacementNew )
	{
		int cur_size = this->_size ;
	
		// 全て�?��トラク�?
		for( int i = 0 ; i < _size ; i++ )
		{
			_ptr[i].~T() ;
	
		}
	
		//
		if( cur_size < new_size )
		{
			prepare_capacity( new_size ) ;// capacity更新
		}
	
		if( callPlacementNew )
		{
			for( int i = 0 ; i < new_size ; i++ )
			{
				L2D_PLACEMENT_NEW ( &_ptr[ i ] ) T( value ) ;// placement new �?���?アドレスに、実体を生�?する
			}
		}
		else{
			for( int i = 0 ; i < new_size ; i++ )
			{
				_ptr[ i ] = value ;
			}
		}
	
		this->_size = new_size ;
	}
	
	
	//--------------------------------------------
	template<class T> 
	void LDVector<T>::insert( iterator pos, iterator start, iterator end , bool callPlacementNew )
	{
	
		int dst_si = pos._index ;
		int src_si = start._index ;
		int src_ei = end._index ;
	
		int addcount = src_ei - src_si ;
	
		prepare_capacity( _size + addcount ) ;
	
	
		// 挿入用に既存データをシフトして隙間を作る
		if( _size-dst_si > 0 )
		{
			memmove( &(_ptr[dst_si+addcount]) , &(_ptr[dst_si]) , sizeof(T)*(_size-dst_si) ) ;
	
		}
	
		// placement new �?���?アドレスに、実体を生�?する
		if( callPlacementNew )
		{
			for( int i = src_si ; i < src_ei ; i++ , dst_si++ )
			{
				L2D_PLACEMENT_NEW ( &_ptr[ dst_si ] ) T( start._vector->_ptr[i] ) ;
			}
	
		}
		else{
			for( int i = src_si ; i < src_ei ; i++ , dst_si++ )
			{
				_ptr[ dst_si ] = start._vector->_ptr[i] ;
			}
		}
	
		this->_size = _size + addcount ;
	}

}
//------------------------- LIVE2D NAMESPACE ------------


#endif		// __LIVE2D_LDVECTOR_H__
