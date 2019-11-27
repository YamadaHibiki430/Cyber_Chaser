/*
================================================================================
					 オブジェクト指向＆＆ゲームプログラミング
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 【対象ＯＳ】
     Microsoft Windows 7以降

 【コンパイラ】
     Microsoft VisualC++ 2013

 【プログラム】
	 Stream.cpp
				ストリームクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Stream.hpp"

//------------------------------------------------------------------------------
//	ストリームリーダークラスコンストラクタ
//------------------------------------------------------------------------------
CStreamReader::CStreamReader() : m_fpStream(NULL)
{
}

//------------------------------------------------------------------------------
//	ストリームリーダークラスデストラクタ
//------------------------------------------------------------------------------
CStreamReader::~CStreamReader()
{
	Close();
}

//------------------------------------------------------------------------------
//	ファイルオープン
//------------------------------------------------------------------------------
bool CStreamReader::Open(LPCTSTR inFileName, const Encoding inEncode)
{
	Close();

	// 指定エンコーディングモードで開く
	errno_t   error;
	switch(inEncode) {
	case Encoding_ASCII  :  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("r"             ));	break;
	case Encoding_UTF8   :  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("r,ccs=UTF-8"   ));	break;
	case Encoding_Unicode:  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("r,ccs=UTF-16LE"));	break;
	}

	if(error != 0) {
		::OutputDebugString(TEXT("*** Error - ファイルオープン失敗(CStreamReader_Open)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	ファイルクローズ
//------------------------------------------------------------------------------
void CStreamReader::Close()
{
	if(m_fpStream != NULL) {
		::fclose(m_fpStream);
		m_fpStream = NULL;
	}
}

//------------------------------------------------------------------------------
//	ファイル読み込み(１文字)
//------------------------------------------------------------------------------
_TINT CStreamReader::Read()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_Read)\n"));
		return (_TINT)EOF;
	}
#endif

	return ::_fgettc(m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル読み込み(文字列)
//------------------------------------------------------------------------------
size_t CStreamReader::Read(TCHAR* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_Read)\n"));
		return 0;
	}
	if(pBuf == NULL || inCount == 0) {
		::OutputDebugString(TEXT("*** Error - パラメータ異常(CStreamReader_Read)\n"));
		return 0;
	}
#endif

	::_fgetts(pBuf, inCount, m_fpStream);
	if(::ferror(m_fpStream) != 0)
		return 0;

	return ::lstrlen(pBuf);
}

//------------------------------------------------------------------------------
//	ファイル読み込み(１行)
//------------------------------------------------------------------------------
size_t CStreamReader::ReadLine(TCHAR* pBuf)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadLine)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CStreamReader_ReadLine)\n"));
		return 0;
	}
#endif

	size_t   len = 0;
	while(true) {
		const _TINT c = ::_fgettc(m_fpStream);
		if(c == (_TINT)EOF || c == TEXT('\n'))
			break;
		*(pBuf + len) = (TCHAR)c;
		len++;
	}
	*(pBuf + len) = TEXT('\0');

	return len;
}

//------------------------------------------------------------------------------
//	ファイル読み込み(指定文字数)
//------------------------------------------------------------------------------
size_t CStreamReader::ReadBlock(TCHAR* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadBlock)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CStreamReader_ReadBlock)\n"));
		return 0;
	}
#endif

	DWORD   idx = 0;
	while(idx < inCount && ::feof(m_fpStream) == 0) {
		// １行読み込み
		::_fgetts(&pBuf[idx], inCount - idx, m_fpStream);
		if(::ferror(m_fpStream) != 0)
			break;

		// バッファインデックス設定
		idx += ::lstrlen(pBuf);		// '\0'は取り除く
	}

	return idx + 1;		// 終端の'\0'の分
}

//------------------------------------------------------------------------------
//	ファイル読み込み(終端まで)
//------------------------------------------------------------------------------
size_t CStreamReader::ReadToEnd(TCHAR* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadToEnd)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CStreamReader_ReadToEnd)\n"));
		return 0;
	}
#endif

	DWORD   idx = 0;
	while(idx < inCount - 1 && ::feof(m_fpStream) == 0) {
		// １行読み込み
		::_fgetts(&pBuf[idx], inCount - idx, m_fpStream);
		if(::ferror(m_fpStream) != 0)
			break;

		// バッファインデックス設定
		idx += ::lstrlen(pBuf);		// '\0'は取り除く
	}

	return idx + 1;		// 終端の'\0'の分
}

//------------------------------------------------------------------------------
//	ファイル読み込み(文字)
//------------------------------------------------------------------------------
TCHAR CStreamReader::ReadChar()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadChar)\n"));
		return (TCHAR)EOF;
	}
#endif

	return ::_fgettc(m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル読み込み(論理値)
//------------------------------------------------------------------------------
bool CStreamReader::ReadBool()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadBool)\n"));
		return false;
	}
#endif

	int   n = 0;
	::_ftscanf_s(m_fpStream, TEXT("%d"), &n);

	return n != 0;
}

//------------------------------------------------------------------------------
//	ファイル読み込み(符号なし８ビット整数)
//------------------------------------------------------------------------------
BYTE CStreamReader::ReadByte()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadByte)\n"));
		return 0;
	}
#endif

	int   n = 0;
	::_ftscanf_s(m_fpStream, TEXT("%d"), &n);

	return (BYTE)n;
}

//------------------------------------------------------------------------------
//	ファイル読み込み(16ビット整数)
//------------------------------------------------------------------------------
INT16 CStreamReader::ReadInt16()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadInt16)\n"));
		return 0;
	}
#endif

	INT16   n = 0;
	::_ftscanf_s(m_fpStream, TEXT("%hd"), &n);

	return n;
}

//------------------------------------------------------------------------------
//	ファイル読み込み(32ビット整数)
//------------------------------------------------------------------------------
INT32 CStreamReader::ReadInt32()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadInt32)\n"));
		return 0;
	}
#endif

	INT32   n = 0;
	::_ftscanf_s(m_fpStream, TEXT("%d"), &n);

	return n;
}

//------------------------------------------------------------------------------
//	ファイル読み込み(64ビット整数)
//------------------------------------------------------------------------------
INT64 CStreamReader::ReadInt64()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadInt64)\n"));
		return 0;
	}
#endif

	INT64   n = 0;
	::_ftscanf_s(m_fpStream, TEXT("%I64d"), &n);

	return n;
}

//------------------------------------------------------------------------------
//	ファイル読み込み(符号なし16ビット整数)
//------------------------------------------------------------------------------
UINT16 CStreamReader::ReadUInt16()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadUInt16)\n"));
		return 0;
	}
#endif

	UINT16   u = 0;
	::_ftscanf_s(m_fpStream, TEXT("%hu"), &u);

	return u;
}

//------------------------------------------------------------------------------
//	ファイル読み込み(符号なし32ビット整数)
//------------------------------------------------------------------------------
UINT32 CStreamReader::ReadUInt32()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadUInt32)\n"));
		return 0;
	}
#endif

	UINT32   u = 0;
	::_ftscanf_s(m_fpStream, TEXT("%u"), &u);

	return u;
}

//------------------------------------------------------------------------------
//	ファイル読み込み(符号なし64ビット整数)
//------------------------------------------------------------------------------
UINT64 CStreamReader::ReadUInt64()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadUInt64)\n"));
		return 0;
	}
#endif

	UINT64   u = 0;
	::_ftscanf_s(m_fpStream, TEXT("%I64u"), &u);

	return u;
}

//------------------------------------------------------------------------------
//	ファイル読み込み(単精度浮動小数)
//------------------------------------------------------------------------------
float CStreamReader::ReadFloat()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadFloat)\n"));
		return 0.0f;
	}
#endif

	float   f = 0.0f;
	::_ftscanf_s(m_fpStream, TEXT("%f"), &f);

	return f;
}

//------------------------------------------------------------------------------
//	ファイル読み込み(倍精度浮動小数)
//------------------------------------------------------------------------------
double CStreamReader::ReadDouble()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_ReadDouble)\n"));
		return 0.0;
	}
#endif

	double   d = 0.0;
	::_ftscanf_s(m_fpStream, TEXT("%lf"), &d);

	return d;
}

//------------------------------------------------------------------------------
//	ファイル終端か調べる
//------------------------------------------------------------------------------
bool CStreamReader::IsEOF()
{
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamReader_IsEOF)\n"));
		return true;
	}

	if(::feof(m_fpStream) == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	ストリームライタークラスコンストラクタ
//------------------------------------------------------------------------------
CStreamWriter::CStreamWriter() : m_fpStream(NULL)
{
}

//------------------------------------------------------------------------------
//	ストリームライタークラスデストラクタ
//------------------------------------------------------------------------------
CStreamWriter::~CStreamWriter()
{
	Close();
}

//------------------------------------------------------------------------------
//	ファイルオープン
//------------------------------------------------------------------------------
bool CStreamWriter::Open(LPCTSTR inFileName, const Encoding inEncode)
{
	Close();

	// 指定エンコーディングモードで開く
	errno_t   error;
	switch(inEncode) {
	case Encoding_ASCII  :  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("wc"             ));	break;
	case Encoding_UTF8   :  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("wc,ccs=UTF-8"   ));	break;
	case Encoding_Unicode:  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("wc,ccs=UTF-16LE"));	break;
	}

	if(error != 0) {
		::OutputDebugString(TEXT("*** Error - ファイルオープン失敗(CStreamWriter_Open)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	ファイルクローズ
//------------------------------------------------------------------------------
void CStreamWriter::Close()
{
	if(m_fpStream != NULL) {
		::fclose(m_fpStream);
		m_fpStream = NULL;
	}
}

//------------------------------------------------------------------------------
//	ファイル書き込み(文字列)
//------------------------------------------------------------------------------
void CStreamWriter::WriteString(LPCTSTR inString)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamWriter_WriteString)\n"));
		return;
	}
#endif

	::_fputts(inString, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル書き込み(書式付き文字列)
//------------------------------------------------------------------------------
void CStreamWriter::WriteFormatString(LPCTSTR inString, ...)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamWriter_WriteFormatString)\n"));
		return;
	}
#endif

	va_list   args;
	va_start(args, inString);

	::_vftprintf_s(m_fpStream, inString, args);

	va_end(args);
}

//------------------------------------------------------------------------------
//	ファイル書き込み(１文字)
//------------------------------------------------------------------------------
void CStreamWriter::WriteChar(TCHAR inVal)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamWriter_WriteChar)\n"));
		return;
	}
#endif

	::_fputtc(inVal, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル書き込み(論理値)
//------------------------------------------------------------------------------
void CStreamWriter::WriteBool(bool inVal)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamWriter_WriteBool)\n"));
		return;
	}
#endif

	const TCHAR   c = inVal ? TEXT('1') : TEXT('0');
	::_fputtc(c, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイルキャッシュ書き出し
//------------------------------------------------------------------------------
void CStreamWriter::Flush()
{
#ifdef _DEBUG
	if(m_fpStream == NULL)
		return;
#endif

	::fflush(m_fpStream);
}

//------------------------------------------------------------------------------
//	エラー状態か調べる
//------------------------------------------------------------------------------
bool CStreamWriter::IsError()
{
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CStreamWriter_IsError)\n"));
		return true;
	}

	if(::ferror(m_fpStream) == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	バイナリーリーダークラスコンストラクタ
//------------------------------------------------------------------------------
CBinaryReader::CBinaryReader() : m_fpStream(NULL)
{
}

//------------------------------------------------------------------------------
//	バイナリーリーダークラスデストラクタ
//------------------------------------------------------------------------------
CBinaryReader::~CBinaryReader()
{
	Close();
}

//------------------------------------------------------------------------------
//	ファイルオープン
//------------------------------------------------------------------------------
bool CBinaryReader::Open(LPCTSTR inFileName)
{
	Close();

	if(::_tfopen_s(&m_fpStream, inFileName, TEXT("rb")) != 0) {
		::OutputDebugString(TEXT("*** Error - ファイルオープン失敗(CBinaryReader_Open)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	ファイルクローズ
//------------------------------------------------------------------------------
void CBinaryReader::Close()
{
	if(m_fpStream != NULL) {
		::fclose(m_fpStream);
		m_fpStream = NULL;
	}
}

//------------------------------------------------------------------------------
//	ファイル読み込み
//------------------------------------------------------------------------------
size_t CBinaryReader::Read(void* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryReader_Read)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CBinaryReader_Read)\n"));
		return 0;
	}
#endif

	return ::fread(pBuf, sizeof(BYTE), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル読み込み(BYTE配列)
//------------------------------------------------------------------------------
size_t CBinaryReader::ReadBytes(BYTE* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryReader_ReadBytes)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CBinaryReader_ReadBytes)\n"));
		return 0;
	}
#endif

	return ::fread(pBuf, sizeof(BYTE), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル読み込み(文字配列)
//------------------------------------------------------------------------------
size_t CBinaryReader::ReadChars(char* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryReader_ReadChars)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CBinaryReader_ReadChars)\n"));
		return 0;
	}
#endif

	return ::fread(pBuf, sizeof(char), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル読み込み(Unicode文字配列)
//------------------------------------------------------------------------------
size_t CBinaryReader::ReadWChars(wchar_t* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryReader_ReadWChars)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CBinaryReader_ReadWChars)\n"));
		return 0;
	}
#endif

	return ::fread(pBuf, sizeof(wchar_t), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル読み込み(テンプレート)
//------------------------------------------------------------------------------
template<typename T> T CBinaryReader::Read()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryReader_Read<template>)\n"));
		return false;
	}
#endif

	T   t = 0;
	::fread(&t, sizeof(t), 1, m_fpStream);

	return t;
}

//------------------------------------------------------------------------------
//	ファイル読み込み位置移動
//------------------------------------------------------------------------------
long CBinaryReader::Seek(const long inOffset, const SeekOrigin inOrigin)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryReader_Seek)\n"));
		return 0;
	}
#endif

	::fseek(m_fpStream, inOffset, inOrigin);

	return ::ftell(m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル終端か調べる
//------------------------------------------------------------------------------
bool CBinaryReader::IsEOF()
{
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryReader_IsEOF)\n"));
		return true;
	}

	if(::feof(m_fpStream) == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	バイナリーライタークラスコンストラクタ
//------------------------------------------------------------------------------
CBinaryWriter::CBinaryWriter() : m_fpStream(NULL)
{
}

//------------------------------------------------------------------------------
//	バイナリーライタークラスデストラクタ
//------------------------------------------------------------------------------
CBinaryWriter::~CBinaryWriter()
{
	Close();
}

//------------------------------------------------------------------------------
//	ファイルオープン
//------------------------------------------------------------------------------
bool CBinaryWriter::Open(LPCTSTR inFileName)
{
	Close();

	if(::_tfopen_s(&m_fpStream, inFileName, TEXT("wcb")) != 0) {
		::OutputDebugString(TEXT("*** Error - ファイルオープン失敗(CBinaryWriter_Open)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	ファイルクローズ
//------------------------------------------------------------------------------
void CBinaryWriter::Close()
{
	if(m_fpStream != NULL) {
		::fclose(m_fpStream);
		m_fpStream = NULL;
	}
}

//------------------------------------------------------------------------------
//	ファイル書き込み
//------------------------------------------------------------------------------
size_t CBinaryWriter::Write(void* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryWriter_Write)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CBinaryWriter_Write)\n"));
		return 0;
	}
#endif

	return ::fwrite(pBuf, sizeof(BYTE), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル書き込み(バイト配列)
//------------------------------------------------------------------------------
size_t CBinaryWriter::WriteBytes(BYTE* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryWriter_WriteBytes)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CBinaryWriter_WriteBytes)\n"));
		return 0;
	}
#endif

	return ::fwrite(pBuf, sizeof(BYTE), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル書き込み(文字配列)
//------------------------------------------------------------------------------
size_t CBinaryWriter::WriteChars(char* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryWriter_WriteChars)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CBinaryWriter_WriteChars)\n"));
		return 0;
	}
#endif

	return ::fwrite(pBuf, sizeof(char), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル書き込み(Unicode文字配列)
//------------------------------------------------------------------------------
size_t CBinaryWriter::WriteWChars(wchar_t* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryWriter_WriteWChars)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CBinaryWriter_WriteWChars)\n"));
		return 0;
	}
#endif

	return ::fwrite(pBuf, sizeof(wchar_t), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイル書き込み(指定値)
//------------------------------------------------------------------------------
size_t CBinaryWriter::WriteFill(const BYTE inFill, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryWriter_WriteFill)\n"));
		return 0;
	}
#endif

	size_t   out = 0;
	while(out < inCount) {
		if(::fwrite(&inFill, sizeof(inFill), 1, m_fpStream) != 1)
			break;
		out++;
	}

	return out;
}

//------------------------------------------------------------------------------
//	ファイル書き込み(テンプレート)
//------------------------------------------------------------------------------
template<typename T> bool CBinaryWriter::Write(const T& inT)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryWriter::Write<template>)\n"));
		return false;
	}
#endif

	return ::fwrite(&inT, sizeof(inT), 1, m_fpStream) == 1;
}

//------------------------------------------------------------------------------
//	ファイル書き込み位置移動
//------------------------------------------------------------------------------
long CBinaryWriter::Seek(const long inOffset, const SeekOrigin inOrigin)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryWriter_Seek)\n"));
		return 0;
	}
#endif

	::fseek(m_fpStream, inOffset, inOrigin);

	return ::ftell(m_fpStream);
}

//------------------------------------------------------------------------------
//	ファイルキャッシュ書き出し
//------------------------------------------------------------------------------
void CBinaryWriter::Flush()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryWriter_Flush)\n"));
		return;
	}
#endif

	::fflush(m_fpStream);
}

//------------------------------------------------------------------------------
//	エラー状態か調べる
//------------------------------------------------------------------------------
bool CBinaryWriter::IsError()
{
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイル未オープン(CBinaryWriter_IsError)\n"));
		return true;
	}

	if(::ferror(m_fpStream) == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	メモリストリームクラスコンストラクタ
//------------------------------------------------------------------------------
CMemoryStream::CMemoryStream()
	: m_pBufferStart(NULL), m_pBufferEnd(NULL), m_pBuffer(NULL), m_pAlloc(NULL),
	  m_hFile(INVALID_HANDLE_VALUE), m_hMapFile(NULL), m_pMemFile(NULL)
{
}

//------------------------------------------------------------------------------
//	メモリストリームクラスデストラクタ
//------------------------------------------------------------------------------
CMemoryStream::~CMemoryStream()
{
	Dispose();
}

//------------------------------------------------------------------------------
//	メモリストリーム生成
//------------------------------------------------------------------------------
bool CMemoryStream::Create(const DWORD inMemSize)
{
	Dispose();

	m_pAlloc = new BYTE[inMemSize];
	if(m_pAlloc == NULL) {
		::OutputDebugString(TEXT("*** Error - メモリ確保失敗(CMemoryStream_Create)\n"));
		return false;
	}

	// ポインタ設定
	m_pBufferStart = m_pAlloc;
	m_pBufferEnd   = m_pBufferStart + inMemSize;
	m_pBuffer      = m_pBufferStart;

	return true;
}

//------------------------------------------------------------------------------
//	メモリストリーム生成
//------------------------------------------------------------------------------
bool CMemoryStream::Create(const void* pBuffer, const DWORD inMemSize)
{
	Dispose();

	// ポインタ設定
	m_pBufferStart = (BYTE*)pBuffer;
	m_pBufferEnd   = m_pBufferStart + inMemSize;
	m_pBuffer      = m_pBufferStart;

	return true;
}

//------------------------------------------------------------------------------
//	メモリストリーム生成
//------------------------------------------------------------------------------
bool CMemoryStream::Create(LPCTSTR inFileName, const DWORD inFileSize)
{
	Dispose();

	// ファイルオープン
	m_hFile = ::CreateFile(inFileName, GENERIC_READ | GENERIC_WRITE,
						   FILE_SHARE_READ, NULL, OPEN_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE) {
		::OutputDebugString(TEXT("*** Error - ファイルオープン失敗(CMemoryStream_Create)\n"));
		return false;
	}

	// ファイルサイズ設定
	if(inFileSize != 0) {
		LARGE_INTEGER   l;
		l.LowPart  = inFileSize;
		l.HighPart = 0;

		::SetFilePointerEx(m_hFile, l, NULL, FILE_BEGIN);
		::SetEndOfFile(m_hFile);
	}

	// ファイルマッピングオブジェクト生成
	m_hMapFile = ::CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if(m_hMapFile == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイルマッピングオブジェクト生成失敗(CMemoryStream_Create)\n"));
		::CloseHandle(m_hFile);		m_hFile    = INVALID_HANDLE_VALUE;
		return false;
	}

	// ファイルマッピング
	m_pMemFile = ::MapViewOfFile(m_hMapFile, FILE_MAP_WRITE, 0, 0, 0);
	if(m_pMemFile == NULL) {
		::OutputDebugString(TEXT("*** Error - ファイルマッピング失敗(CMemoryStream_Create)\n"));
		::CloseHandle(m_hMapFile);	m_hMapFile = NULL;
		::CloseHandle(m_hFile   );	m_hFile    = INVALID_HANDLE_VALUE;
		return false;
	}

	// ポインタ設定
	m_pBufferStart = (BYTE*)m_pMemFile;
	m_pBufferEnd   = m_pBufferStart + ::GetFileSize(m_hFile, NULL);
	m_pBuffer      = m_pBufferStart;

	return true;
}

//------------------------------------------------------------------------------
//	メモリストリーム解放
//------------------------------------------------------------------------------
void CMemoryStream::Dispose()
{
	// メモリマップドファイル解放
	if(m_pMemFile != NULL) {
		::UnmapViewOfFile(m_pMemFile);
		m_pMemFile = NULL;
	}
	if(m_hMapFile != NULL) {
		::CloseHandle(m_hMapFile);
		m_hMapFile = NULL;
	}
	if(m_hFile    != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hFile);
		m_hFile    = INVALID_HANDLE_VALUE;
	}

	// 確保メモリ解放
	delete[] m_pAlloc;
	m_pAlloc = NULL;

	// 変数初期化
	m_pBufferStart = NULL;
	m_pBufferEnd   = NULL;
	m_pBuffer      = NULL;
}

//------------------------------------------------------------------------------
//	メモリ読み込み(バッファ)
//------------------------------------------------------------------------------
DWORD CMemoryStream::Read(void* pBuf, const DWORD inSize)
{
	if(m_pBuffer >= m_pBufferEnd)
		return 0;

#ifdef _DEBUG
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CMemoryStream_Read)\n"));
		return 0;
	}
#endif

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;				// 残りサイズ
	const DWORD   COUNT = inSize <= LEFT ? inSize : LEFT;		// 書き込み数

	::CopyMemory(pBuf, m_pBuffer, COUNT);
	m_pBuffer += COUNT;

	return COUNT;
}

//------------------------------------------------------------------------------
//	メモリ読み込み(１バイト)
//------------------------------------------------------------------------------
BYTE CMemoryStream::ReadByte()
{
	if(m_pBuffer >= m_pBufferEnd)
		return 0;

	const BYTE   b = *m_pBuffer++;
	
	return b;
}

//------------------------------------------------------------------------------
//	メモリコピー
//------------------------------------------------------------------------------
DWORD CMemoryStream::Copy(void* pBuf, const DWORD inSize)
{
#ifdef _DEBUG
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CMemoryStream_Write)\n"));
		return 0;
	}
#endif

	if(m_pBuffer >= m_pBufferEnd)
		return 0;

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;			// 残りサイズ
	const DWORD   COUNT = inSize <= LEFT ? inSize : LEFT;	// 書き込み数

	::MoveMemory(m_pBuffer, pBuf, COUNT);
	m_pBuffer += COUNT;

	return COUNT;
}

//------------------------------------------------------------------------------
//	メモリ書き込み(バッファ)
//------------------------------------------------------------------------------
DWORD CMemoryStream::Write(void* pBuf, const DWORD inSize)
{
#ifdef _DEBUG
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - ポインタ異常(CMemoryStream_Write)\n"));
		return 0;
	}
#endif

	if(m_pBuffer >= m_pBufferEnd)
		return 0;

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;			// 残りサイズ
	const DWORD   COUNT = inSize <= LEFT ? inSize : LEFT;	// 書き込み数

	::CopyMemory(m_pBuffer, pBuf, COUNT);
	m_pBuffer += COUNT;

	return COUNT;
}

//------------------------------------------------------------------------------
//	メモリ書き込み(１バイト)
//------------------------------------------------------------------------------
void CMemoryStream::WriteByte(const BYTE inVal)
{
	if(m_pBuffer >= m_pBufferEnd)
		return;

	*m_pBuffer++ = inVal;
}

//------------------------------------------------------------------------------
//	ファイル出力
//------------------------------------------------------------------------------
bool CMemoryStream::WriteTo(LPCTSTR inFileName)
{
	CBinaryWriter   writer;
	if(writer.Open(inFileName) == false) {
		::OutputDebugString(TEXT("*** Error - ファイル生成失敗(CMemoryStream_WriteTo)\n"));
		return false;
	}

	writer.Write(m_pBufferStart, m_pBufferEnd - m_pBufferStart);
	writer.Close();

	return true;
}

//------------------------------------------------------------------------------
//	指定値ブロック書き込み
//------------------------------------------------------------------------------
DWORD CMemoryStream::Fill(const BYTE inFill, const DWORD inCount)
{
#ifdef _DEBUG
	if(m_pBuffer == NULL) {
		::OutputDebugString(TEXT("*** Error - メモリストリーム未生成(CMemoryStream_Fill)\n"));
		return 0;
	}
#endif

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;				// 残りサイズ
	const DWORD   COUNT = inCount <= LEFT ? inCount : LEFT;		// 書き込み数

	::FillMemory(m_pBuffer, COUNT, inFill);
	m_pBuffer += COUNT;

	return COUNT;
}

//------------------------------------------------------------------------------
//	位置移動
//------------------------------------------------------------------------------
DWORD CMemoryStream::Seek(const long inOffset, const SeekOrigin inOrigin)
{
	// ポインタ設定
	switch(inOrigin) {
	case SeekOrigin_Begin  :  m_pBuffer = m_pBufferStart;	break;
	case SeekOrigin_Current:  m_pBuffer = m_pBuffer;		break;
	case SeekOrigin_End    :  m_pBuffer = m_pBufferEnd;		break;
	}

	// 範囲設定
	if(m_pBuffer < m_pBufferStart)
		m_pBuffer = m_pBufferStart;
	else if(m_pBuffer > m_pBufferEnd)
		m_pBuffer = m_pBufferEnd;

	return m_pBuffer - m_pBufferStart;
}

//------------------------------------------------------------------------------
//	キャッシュ書き出し
//------------------------------------------------------------------------------
void CMemoryStream::Flush()
{
	if(m_pMemFile == NULL)
		return;

	::FlushViewOfFile(m_pMemFile, 0);
}

//------------------------------------------------------------------------------
//	メモリ読み込み(テンプレート)
//------------------------------------------------------------------------------
template<typename T> T CMemoryStream::Read()
{
	if(m_pBuffer >= m_pBufferEnd)
		return 0;

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;					// 残りサイズ
	const DWORD   COUNT = sizeof(T) <= LEFT ? sizeof(T) : LEFT;		// 読み込み数

	T   t = 0;
	::CopyMemory(&t, m_pBuffer, COUNT);
	m_pBuffer += COUNT;

	return t;
}

//------------------------------------------------------------------------------
//	メモリ書き込み(テンプレート)
//------------------------------------------------------------------------------
template<typename T> void CMemoryStream::Write(const T& inT)
{
	if(m_pBuffer >= m_pBufferEnd)
		return;

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;					// 残りサイズ
	const DWORD   COUNT = sizeof(T) <= LEFT ? sizeof(T) : LEFT;		// 書き込み数

	::CopyMemory(m_pBuffer, &inT, COUNT);
	m_pBuffer += COUNT;
}

//------------------------------------------------------------------------------
//	フォルダ内ファイル数取得
//------------------------------------------------------------------------------
DWORD DirectoryInfo::GetFileEntries(LPCTSTR inFileName)
{
	WIN32_FIND_DATA   fd;
	HANDLE            hFind = ::FindFirstFile(inFileName, &fd);
	if(hFind == INVALID_HANDLE_VALUE)
		return 0;

	DWORD   entreis = 0;
	while(true) {
		// ディレクトリでなければインクリメント
		if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			entreis++;

		// 次のファイルへ
		if(::FindNextFile(hFind, &fd) == 0)
			break;
	}
	::FindClose(hFind);

	return entreis;
}

//------------------------------------------------------------------------------
//	フォルダ内ファイルリスト取得
//------------------------------------------------------------------------------
DWORD DirectoryInfo::GetFileList(FileInfoList& outFileList, LPCTSTR inFileName)
{
	outFileList.clear();

	WIN32_FIND_DATA   fd;
	HANDLE            hFind = ::FindFirstFile(inFileName, &fd);
	if(hFind == INVALID_HANDLE_VALUE)
		return 0;

	while(true) {
		// ディレクトリでなければ情報取得
		if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			FileInfo   fi;
			::_tcscpy_s(fi.FileName, MAX_PATH, fd.cFileName);
			fi.FileSize = fd.nFileSizeLow;
			outFileList.push_back(fi);
		}

		// 次のファイルへ
		if(::FindNextFile(hFind, &fd) == 0)
			break;
	}
	::FindClose(hFind);

	return outFileList.size();
}
