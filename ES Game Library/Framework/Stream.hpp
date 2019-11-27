/*
================================================================================
					 �I�u�W�F�N�g�w�������Q�[���v���O���~���O
	     Copyright (c) 2011 Human Academy Sendai. All rights reserved.
================================================================================

 �y�Ώۂn�r�z
     Microsoft Windows 7�ȍ~

 �y�R���p�C���z
     Microsoft VisualC++ 2013

 �y�v���O�����z
	 Stream.hpp
				�X�g���[���N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <windows.h>
#include <tchar.h>
#include <cstdio>
#include <list>

//------------------------------------------------------------------------------
//	�X�g���[���V�[�N��_�񋓑�
//------------------------------------------------------------------------------
enum SeekOrigin {
	SeekOrigin_Begin   = SEEK_SET,	// �擪
	SeekOrigin_Current = SEEK_CUR,	// ���݈ʒu
	SeekOrigin_End     = SEEK_END	// �I�[
};

//------------------------------------------------------------------------------
//	�G���R�[�f�B���O�񋓑�
//------------------------------------------------------------------------------
enum Encoding {
	Encoding_ASCII,		// ASCII
	Encoding_UTF8,		// UTF-8
	Encoding_Unicode,	// UTF-16LE
	Encoding_UTF16LE             = Encoding_Unicode,
	Encoding_LittleEndianUnicode = Encoding_Unicode
};

//------------------------------------------------------------------------------
//	�X�g���[�����[�_�[�N���X��`
//------------------------------------------------------------------------------
class CStreamReader {
public:
	CStreamReader();
	virtual ~CStreamReader();

	bool   Open (LPCTSTR inFileName, const Encoding inEncode = Encoding_ASCII);
	void   Close();

	_TINT  Read ();
	size_t Read (TCHAR* pBuf, const size_t inCount);

	size_t ReadLine (TCHAR* pBuf);
	size_t ReadBlock(TCHAR* pBuf, const size_t inCount);
	size_t ReadToEnd(TCHAR* pBuf, const size_t inCount);


	TCHAR  ReadChar  ();
	bool   ReadBool  ();
	BYTE   ReadByte  ();

	INT16  ReadInt16 ();
	INT32  ReadInt32 ();
	INT64  ReadInt64 ();

	UINT16 ReadUInt16();
	UINT32 ReadUInt32();
	UINT64 ReadUInt64();

	float  ReadFloat ();
	double ReadDouble();
	inline float ReadSingle() { return ReadFloat(); }

	bool   IsEOF();

private:
	FILE*   m_fpStream;

	CStreamReader(const CStreamReader&);
	CStreamReader& operator=(const CStreamReader&);
};

//------------------------------------------------------------------------------
//	�X�g���[�����C�^�[�N���X��`
//------------------------------------------------------------------------------
class CStreamWriter {
public:
	CStreamWriter();
	virtual ~CStreamWriter();

	bool Open (LPCTSTR inFileName, const Encoding inEncode = Encoding_ASCII);
	void Close();

	void WriteString(LPCTSTR inString);
	void WriteFormatString(LPCTSTR inString, ...);

	void WriteChar(TCHAR inVal);
	void WriteBool(bool  inVal);

	template<typename T>
	inline void Write(const T inT, LPCTSTR inFormat)
	{
	#ifdef _DEBUG
		if(m_fpStream == NULL) {
			::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamWriter_Write<template>)\n"));
			return;
		}
	#endif

		::_ftprintf_s(m_fpStream, inFormat, inT);
	}

	inline void WriteByte  (BYTE   inVal) { Write<BYTE  >(inVal, TEXT("%d")   ); }

	inline void WriteInt16 (INT16  inVal) { Write<INT16 >(inVal, TEXT("%d")   ); }
	inline void WriteInt32 (INT32  inVal) { Write<INT32 >(inVal, TEXT("%d")   ); }
	inline void WriteInt64 (INT64  inVal) { Write<INT64 >(inVal, TEXT("%I64d")); }

	inline void WriteUInt16(UINT16 inVal) { Write<UINT16>(inVal, TEXT("%u")   ); }
	inline void WriteUInt32(UINT32 inVal) { Write<UINT32>(inVal, TEXT("%u")   ); }
	inline void WriteUInt64(UINT64 inVal) { Write<UINT64>(inVal, TEXT("%I64d")); }

	inline void WriteFloat (float  inVal) { Write<float >(inVal, TEXT("%f")   ); }
	inline void WriteSingle(float  inVal) { Write<float >(inVal, TEXT("%f")   ); }
	inline void WriteDouble(double inVal) { Write<double>(inVal, TEXT("%lf")  ); }

	inline void WriteLineFeed() { WriteChar( TEXT('\n') ); }
	inline void WriteSpace   () { WriteChar( TEXT(' ')  ); }

	void Flush();
	bool IsError();

private:
	FILE*   m_fpStream;

	CStreamWriter(const CStreamWriter&);
	CStreamWriter& operator=(const CStreamWriter&);
};

//------------------------------------------------------------------------------
//	�o�C�i���[���[�_�[�N���X��`
//------------------------------------------------------------------------------
class CBinaryReader {
public:
	CBinaryReader();
	virtual ~CBinaryReader();

	bool    Open (LPCTSTR inFileName);
	void    Close();

	size_t Read      (void*    pBuf, const size_t inCount);

	size_t ReadBytes (BYTE*    pBuf, const size_t inCount);
	size_t ReadChars (char*    pBuf, const size_t inCount);
	size_t ReadWChars(wchar_t* pBuf, const size_t inCount);

	template<typename T> T Read();

	inline bool    ReadBool  () { return Read<bool   >(); }

	inline BYTE    ReadByte  () { return Read<BYTE   >(); }
	inline char    ReadChar  () { return Read<char   >(); }
	inline wchar_t ReadWChar () { return Read<wchar_t>(); }

	inline INT16   ReadInt16 () { return Read<INT16  >(); }
	inline INT32   ReadInt32 () { return Read<INT32  >(); }
	inline INT64   ReadInt64 () { return Read<INT64  >(); }

	inline UINT16  ReadUInt16() { return Read<UINT16 >(); }
	inline UINT32  ReadUInt32() { return Read<UINT32 >(); }
	inline UINT64  ReadUInt64() { return Read<UINT64 >(); }

	inline float   ReadFloat () { return Read<float  >(); }
	inline float   ReadSingle() { return Read<float  >(); }
	inline double  ReadDouble() { return Read<double >(); }

	long   Seek(const long inOffset, const SeekOrigin inOrigin);
	bool   IsEOF();

private:
	FILE*   m_fpStream;

	CBinaryReader(const CBinaryReader&);
	CBinaryReader& operator=(const CBinaryReader&);
};

//------------------------------------------------------------------------------
//	�o�C�i���[���C�^�[�N���X��`
//------------------------------------------------------------------------------
class CBinaryWriter {
public:
	CBinaryWriter();
	virtual ~CBinaryWriter();

	bool   Open (LPCTSTR inFileName);
	void   Close();

	size_t Write      (void*    pBuf, const size_t inCount);

	size_t WriteBytes (BYTE*    pBuf, const size_t inCount);
	size_t WriteChars (char*    pBuf, const size_t inCount);
	size_t WriteWChars(wchar_t* pBuf, const size_t inCount);

	size_t WriteFill  (const BYTE inFill, const size_t inCount);

	template<typename T> bool Write(const T& inT);

	inline bool WriteBool  (bool    inVal) { return Write<bool   >(inVal); }

	inline bool WriteByte  (BYTE    inVal) { return Write<BYTE   >(inVal); }
	inline bool WriteChar  (char    inVal) { return Write<char   >(inVal); }
	inline bool WriteWChar (wchar_t inVal) { return Write<wchar_t>(inVal); }

	inline bool WriteInt16 (INT16   inVal) { return Write<INT16  >(inVal); }
	inline bool WriteInt32 (INT32   inVal) { return Write<INT32  >(inVal); }
	inline bool WriteInt64 (INT64   inVal) { return Write<INT64  >(inVal); }

	inline bool WriteUInt16(UINT16  inVal) { return Write<UINT16 >(inVal); }
	inline bool WriteUInt32(UINT32  inVal) { return Write<UINT32 >(inVal); }
	inline bool WriteUInt64(UINT64  inVal) { return Write<UINT64 >(inVal); }

	inline bool WriteFloat (float   inVal) { return Write<float  >(inVal); }
	inline bool WriteSingle(float   inVal) { return Write<float  >(inVal); }
	inline bool WriteDouble(double  inVal) { return Write<double >(inVal); }

	long   Seek(const long inOffset, const SeekOrigin inOrigin);
	void   Flush();

	bool   IsError();

private:
	FILE*   m_fpStream;

	CBinaryWriter(const CBinaryWriter&);
	CBinaryWriter& operator=(const CBinaryWriter&);
};

//------------------------------------------------------------------------------
//	�������X�g���[���N���X��`
//------------------------------------------------------------------------------
class CMemoryStream {
public:
	CMemoryStream();
	virtual ~CMemoryStream();

	bool  Create(const DWORD inMemSize);
	bool  Create(const void* pBuffer, const DWORD inMemSize);
	bool  Create(LPCTSTR inFileName, const DWORD inFileSize = 0);

	void  Dispose();

	DWORD Read(void* pBuf, const DWORD inSize);
	BYTE  ReadByte();

	DWORD Copy (void* pBuf, const DWORD inSize);

	DWORD Write(void* pBuf, const DWORD inSize);
	void  WriteByte(const BYTE inVal);

	bool  WriteTo(LPCTSTR inFileName);

	DWORD Fill(const BYTE inFill, const DWORD inCount);
	DWORD Seek(const long inOffset, const SeekOrigin inOrigin);
	void  Flush();

	// �A�N�Z�X�֐�
	inline BYTE* GetBuffer  () const { return m_pBuffer; }
	inline DWORD GetLength  () const { return m_pBufferEnd - m_pBufferStart; }
	inline DWORD GetPosition() const { return m_pBuffer    - m_pBufferStart; }

	// �C�����C���֐�
	inline size_t ReadBytes (BYTE*    pBuf, const size_t inCount)
	{ return Read(pBuf, sizeof(BYTE   ) * inCount) / sizeof(BYTE   ); }

	inline size_t ReadChars (char*    pBuf, const size_t inCount)
	{ return Read(pBuf, sizeof(char   ) * inCount) / sizeof(char   ); }

	inline size_t ReadWChars(wchar_t* pBuf, const size_t inCount)
	{ return Read(pBuf, sizeof(wchar_t) * inCount) / sizeof(wchar_t); }

	// �ǂݍ��݃e���v���[�g
	template<typename T> T Read();

	inline bool    ReadBool  () { return Read<bool   > (); }
	inline char    ReadChar  () { return (char)ReadByte(); }
	inline wchar_t ReadWChar () { return Read<wchar_t> (); }

	inline INT16   ReadInt16 () { return Read<INT16  > (); }
	inline INT32   ReadInt32 () { return Read<INT32  > (); }
	inline INT64   ReadInt64 () { return Read<INT64  > (); }

	inline UINT16  ReadUInt16() { return Read<UINT16 > (); }
	inline UINT32  ReadUInt32() { return Read<UINT32 > (); }
	inline UINT64  ReadUInt64() { return Read<UINT64 > (); }

	inline float   ReadFloat () { return Read<float  > (); }
	inline float   ReadSingle() { return Read<float  > (); }
	inline double  ReadDouble() { return Read<double > (); }

	// �������݃e���v���[�g
	template<typename T> void Write(const T& inT);

	inline void WriteBool  (bool    inVal) { Write<bool   >(inVal); }
	inline void WriteChar  (char    inVal) { WriteByte     (inVal); }
	inline void WriteWChar (wchar_t inVal) { Write<wchar_t>(inVal); }

	inline void WriteInt16 (INT16   inVal) { Write<INT16  >(inVal); }
	inline void WriteInt32 (INT32   inVal) { Write<INT32  >(inVal); }
	inline void WriteInt64 (INT64   inVal) { Write<INT64  >(inVal); }

	inline void WriteUInt16(UINT16  inVal) { Write<UINT16 >(inVal); }
	inline void WriteUInt32(UINT32  inVal) { Write<UINT32 >(inVal); }
	inline void WriteUInt64(UINT64  inVal) { Write<UINT64 >(inVal); }

	inline void WriteFloat (float   inVal) { Write<float  >(inVal); }
	inline void WriteSingle(float   inVal) { Write<float  >(inVal); }
	inline void WriteDouble(double  inVal) { Write<double >(inVal); }

private:
	BYTE*    m_pBufferStart;
	BYTE*    m_pBufferEnd;
	BYTE*    m_pBuffer;

	// �������m�ێ��A�h���X
	BYTE*    m_pAlloc;

	// �������}�b�v�h�t�@�C��
	HANDLE   m_hFile;
	HANDLE   m_hMapFile;
	void*    m_pMemFile;

	CMemoryStream(const CMemoryStream&);
	CMemoryStream& operator=(const CMemoryStream&);
};

//------------------------------------------------------------------------------
//	�t�@�C�����^��`
//------------------------------------------------------------------------------
struct FileInfo {
	TCHAR FileName[MAX_PATH];
	DWORD FileSize;
};

typedef std::list<FileInfo> FileInfoList;

//------------------------------------------------------------------------------
//	�t�H���_���
//------------------------------------------------------------------------------
namespace DirectoryInfo
{
	// �t�H���_���t�@�C�����擾
	DWORD GetFileEntries(LPCTSTR inFileName);

	// �t�@�C�����X�g�擾
	DWORD GetFileList(FileInfoList& outFileList, LPCTSTR inFileName);
};
