/**
 *  BReader.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_B_READER_H__
#define __LIVE2D_B_READER_H__


#ifndef __SKIP_DOC__


#include "../memory/LDObject.h"
#include "ByteBuffer.h"

#include "../type/LDVector.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d
{
	class AMemoryHolder ;
	
	
	class BReader : public live2d::LDObject 
	{
	public:
		static const int FLAG_READ_AS_USHORT_ARRAY	= 0x1 ;// int[]を�?ロード時にushort[]として扱�?
		static const int FLAG_MALLOC_ON_GPU			= 0x2 ;// GPU上にメモリを確保す�?
	
	
		// ロード時にオブジェクトを保管する�??ブルのサイズ
		// 小さ�?��vectorのresizeが発生する�?大きいとメモリを圧迫する
		// 使用メモリは sizeof(void*) * LOAD_OBJECT_INITIAL_CAPACITYとな�?
		// 今後�?、モ�?��の中にオブジェクト数を埋め込�?��式に変更する
		static const int LOAD_OBJECT_INITIAL_CAPACITY = 10000 ; 
	
	
	    // 開放の方法が確定せず安�?ではな�?��め�?��外部で開放するように変更
		//BReader( const char * buf , int length , bool deleteBufOnDestructor );
		BReader( const char * buf , int length );
		virtual ~BReader();
		
		//  ファイルフォーマット�?バ�?ジョン 
		int getFormatVersion(){ return formatVersion ; }
	
		void setFormatVersion(int version){ formatVersion = version ; }
		
		void rollback( int byteLen ) ;// �?���?バイト数戻�?
	
		// Flagsには以下を渡�?
		//	FLAG_READ_AS_USHORT_ARRAY	= 0x1 ;// int[]を�?ロード時にushort[]として扱�?
		//	FLAG_MALLOC_ON_GPU			= 0x2 ;// GPU上にメモリを確保す�?
		void * readObject( MemoryParam * memParam , int cno = -1 , int flags = 0 ) ;
		bool readBit() ;
		int readNum(){ return bin.readNum() ; } ;// 可変長の長さを取得す�?1..4バイトまでを長さに応じて使�??ける
		float readFloat(){	checkBits() ; return bin.readFloat() ;	}
		double readDouble(){ checkBits() ; return bin.readDouble() ;	}
		long long readLong(){ checkBits() ; return bin.readLong() ;	}
		int readInt() { checkBits() ; return bin.readInt() ;}
		bool readBoolean(){ checkBits() ; return bin.readBoolean() ;}
		char readByte(){ checkBits() ; return bin.readByte() ;}// 1byte�?��す??avaのバイトと対応�?charはJavaと型が異なる�?で名称から回避??
		short readShort(){ checkBits() ; return bin.readShort() ;	}
	
		//  ?��?インタ?�を返す�?外部でL2D_FREE( )が�?�?
		double* readArrayDouble(MemoryParam* memParam,int *ret_length){ checkBits() ; return bin.readArrayDouble(memParam, ret_length) ;	}// size, float* を戻す�?��がある
		float* readArrayFloat(MemoryParam* memParam,int *ret_length){ checkBits() ; return bin.readArrayFloat(memParam, ret_length) ;	}// size, float* を戻す�?��がある
	
		int * readArrayInt(MemoryParam* memParam,int *ret_length){ checkBits() ; return bin.readArrayInt(memParam, ret_length ) ;}
		unsigned short * readArrayIntAsUShort(MemoryParam* memParam,int *ret_length){ checkBits() ; return bin.readArrayIntAsUShort(memParam, ret_length ) ;}
	
		LDVector<double>* readArrayDoubleAsVector(MemoryParam* memParam){ checkBits() ; return bin.readArrayDoubleAsVector(memParam) ;	}// size, float* を戻す�?��がある
		LDVector<float>* readArrayFloatAsVector(MemoryParam* memParam){ checkBits() ; return bin.readArrayFloatAsVector(memParam) ;	}// size, float* を戻す�?��がある
		LDVector<int> * readArrayIntAsVector(MemoryParam* memParam){ checkBits() ; return bin.readArrayIntAsVector(memParam) ;}
		
		//  マルチバイト文字�?未検証??ytesで長さを�?��して�?��のでここでのエラーはでな�?�?
		live2d::LDString* readString(MemoryParam* memParam ){ checkBits() ; return bin.readString(memParam) ;	}
		RefString& readStringAsRef(){ checkBits() ; return bin.readStringAsRef() ;	}
	
		// cのポインタを返し、終端記号まで進める
		const char* readCStr(){ checkBits() ; return bin.readCStr() ; }
		char* getCurPtr(){ return bin.getCurPtr() ; }
		void setEndian( bool isBigEndian ) { bin.setEndian(isBigEndian) ; }
		void skip( int bytes ){ bin.skip( bytes ) ; }

		// 読み込みながらEndianを�?れ替えて�?��場合にtrue
		// �?��読み込んだバッファを�?度ロードする�?合に二回目を同じEndianで扱えるようにする
		void setChangeEndian( bool change ) { bin.setChangeEndian( change ) ; }
	
	#if L2D_SAMPLE
		static void TEST_1() ;
		static void TEST_MODEL_LOAD() ;
	#endif
	
		
	private:
		void* read_array_ent( MemoryParam* memParam ,int targetClassNo, int restDimension) ;// 戻り�?は�??列型 ( プリミティブもあるので、�?��しもObject[]ではな�?)
	
		void checkBits()
		{
			if( bitCount != 0 ) bitCount = 0 ;
		}
		
		int formatVersion ;
		
		ByteBuffer bin ;
		int bitCount ; //= 0 ;
		int bitBuff ;//= 0 ;// 現在のビットフラグを管�?
	
		// TODO オブジェクトリスト�?管�?��強�?
		LDVector<void*> loadObjects ;// 生�?されたオブジェクト�?全て登録される�?ロード結果なので中身は解放しな�?
		
	};
}
//------------------------- LIVE2D NAMESPACE -------------

#endif // __SKIP_DOC__

#endif		// __LIVE2D_B_READER_H__
