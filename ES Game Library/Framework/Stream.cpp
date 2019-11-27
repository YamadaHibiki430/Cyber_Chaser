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
	 Stream.cpp
				�X�g���[���N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Stream.hpp"

//------------------------------------------------------------------------------
//	�X�g���[�����[�_�[�N���X�R���X�g���N�^
//------------------------------------------------------------------------------
CStreamReader::CStreamReader() : m_fpStream(NULL)
{
}

//------------------------------------------------------------------------------
//	�X�g���[�����[�_�[�N���X�f�X�g���N�^
//------------------------------------------------------------------------------
CStreamReader::~CStreamReader()
{
	Close();
}

//------------------------------------------------------------------------------
//	�t�@�C���I�[�v��
//------------------------------------------------------------------------------
bool CStreamReader::Open(LPCTSTR inFileName, const Encoding inEncode)
{
	Close();

	// �w��G���R�[�f�B���O���[�h�ŊJ��
	errno_t   error;
	switch(inEncode) {
	case Encoding_ASCII  :  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("r"             ));	break;
	case Encoding_UTF8   :  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("r,ccs=UTF-8"   ));	break;
	case Encoding_Unicode:  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("r,ccs=UTF-16LE"));	break;
	}

	if(error != 0) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���I�[�v�����s(CStreamReader_Open)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�t�@�C���N���[�Y
//------------------------------------------------------------------------------
void CStreamReader::Close()
{
	if(m_fpStream != NULL) {
		::fclose(m_fpStream);
		m_fpStream = NULL;
	}
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�P����)
//------------------------------------------------------------------------------
_TINT CStreamReader::Read()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_Read)\n"));
		return (_TINT)EOF;
	}
#endif

	return ::_fgettc(m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(������)
//------------------------------------------------------------------------------
size_t CStreamReader::Read(TCHAR* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_Read)\n"));
		return 0;
	}
	if(pBuf == NULL || inCount == 0) {
		::OutputDebugString(TEXT("*** Error - �p�����[�^�ُ�(CStreamReader_Read)\n"));
		return 0;
	}
#endif

	::_fgetts(pBuf, inCount, m_fpStream);
	if(::ferror(m_fpStream) != 0)
		return 0;

	return ::lstrlen(pBuf);
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�P�s)
//------------------------------------------------------------------------------
size_t CStreamReader::ReadLine(TCHAR* pBuf)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadLine)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CStreamReader_ReadLine)\n"));
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
//	�t�@�C���ǂݍ���(�w�蕶����)
//------------------------------------------------------------------------------
size_t CStreamReader::ReadBlock(TCHAR* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadBlock)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CStreamReader_ReadBlock)\n"));
		return 0;
	}
#endif

	DWORD   idx = 0;
	while(idx < inCount && ::feof(m_fpStream) == 0) {
		// �P�s�ǂݍ���
		::_fgetts(&pBuf[idx], inCount - idx, m_fpStream);
		if(::ferror(m_fpStream) != 0)
			break;

		// �o�b�t�@�C���f�b�N�X�ݒ�
		idx += ::lstrlen(pBuf);		// '\0'�͎�菜��
	}

	return idx + 1;		// �I�[��'\0'�̕�
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�I�[�܂�)
//------------------------------------------------------------------------------
size_t CStreamReader::ReadToEnd(TCHAR* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadToEnd)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CStreamReader_ReadToEnd)\n"));
		return 0;
	}
#endif

	DWORD   idx = 0;
	while(idx < inCount - 1 && ::feof(m_fpStream) == 0) {
		// �P�s�ǂݍ���
		::_fgetts(&pBuf[idx], inCount - idx, m_fpStream);
		if(::ferror(m_fpStream) != 0)
			break;

		// �o�b�t�@�C���f�b�N�X�ݒ�
		idx += ::lstrlen(pBuf);		// '\0'�͎�菜��
	}

	return idx + 1;		// �I�[��'\0'�̕�
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(����)
//------------------------------------------------------------------------------
TCHAR CStreamReader::ReadChar()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadChar)\n"));
		return (TCHAR)EOF;
	}
#endif

	return ::_fgettc(m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�_���l)
//------------------------------------------------------------------------------
bool CStreamReader::ReadBool()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadBool)\n"));
		return false;
	}
#endif

	int   n = 0;
	::_ftscanf_s(m_fpStream, TEXT("%d"), &n);

	return n != 0;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�����Ȃ��W�r�b�g����)
//------------------------------------------------------------------------------
BYTE CStreamReader::ReadByte()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadByte)\n"));
		return 0;
	}
#endif

	int   n = 0;
	::_ftscanf_s(m_fpStream, TEXT("%d"), &n);

	return (BYTE)n;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(16�r�b�g����)
//------------------------------------------------------------------------------
INT16 CStreamReader::ReadInt16()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadInt16)\n"));
		return 0;
	}
#endif

	INT16   n = 0;
	::_ftscanf_s(m_fpStream, TEXT("%hd"), &n);

	return n;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(32�r�b�g����)
//------------------------------------------------------------------------------
INT32 CStreamReader::ReadInt32()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadInt32)\n"));
		return 0;
	}
#endif

	INT32   n = 0;
	::_ftscanf_s(m_fpStream, TEXT("%d"), &n);

	return n;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(64�r�b�g����)
//------------------------------------------------------------------------------
INT64 CStreamReader::ReadInt64()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadInt64)\n"));
		return 0;
	}
#endif

	INT64   n = 0;
	::_ftscanf_s(m_fpStream, TEXT("%I64d"), &n);

	return n;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�����Ȃ�16�r�b�g����)
//------------------------------------------------------------------------------
UINT16 CStreamReader::ReadUInt16()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadUInt16)\n"));
		return 0;
	}
#endif

	UINT16   u = 0;
	::_ftscanf_s(m_fpStream, TEXT("%hu"), &u);

	return u;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�����Ȃ�32�r�b�g����)
//------------------------------------------------------------------------------
UINT32 CStreamReader::ReadUInt32()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadUInt32)\n"));
		return 0;
	}
#endif

	UINT32   u = 0;
	::_ftscanf_s(m_fpStream, TEXT("%u"), &u);

	return u;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�����Ȃ�64�r�b�g����)
//------------------------------------------------------------------------------
UINT64 CStreamReader::ReadUInt64()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadUInt64)\n"));
		return 0;
	}
#endif

	UINT64   u = 0;
	::_ftscanf_s(m_fpStream, TEXT("%I64u"), &u);

	return u;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�P���x��������)
//------------------------------------------------------------------------------
float CStreamReader::ReadFloat()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadFloat)\n"));
		return 0.0f;
	}
#endif

	float   f = 0.0f;
	::_ftscanf_s(m_fpStream, TEXT("%f"), &f);

	return f;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�{���x��������)
//------------------------------------------------------------------------------
double CStreamReader::ReadDouble()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_ReadDouble)\n"));
		return 0.0;
	}
#endif

	double   d = 0.0;
	::_ftscanf_s(m_fpStream, TEXT("%lf"), &d);

	return d;
}

//------------------------------------------------------------------------------
//	�t�@�C���I�[�����ׂ�
//------------------------------------------------------------------------------
bool CStreamReader::IsEOF()
{
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamReader_IsEOF)\n"));
		return true;
	}

	if(::feof(m_fpStream) == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	�X�g���[�����C�^�[�N���X�R���X�g���N�^
//------------------------------------------------------------------------------
CStreamWriter::CStreamWriter() : m_fpStream(NULL)
{
}

//------------------------------------------------------------------------------
//	�X�g���[�����C�^�[�N���X�f�X�g���N�^
//------------------------------------------------------------------------------
CStreamWriter::~CStreamWriter()
{
	Close();
}

//------------------------------------------------------------------------------
//	�t�@�C���I�[�v��
//------------------------------------------------------------------------------
bool CStreamWriter::Open(LPCTSTR inFileName, const Encoding inEncode)
{
	Close();

	// �w��G���R�[�f�B���O���[�h�ŊJ��
	errno_t   error;
	switch(inEncode) {
	case Encoding_ASCII  :  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("wc"             ));	break;
	case Encoding_UTF8   :  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("wc,ccs=UTF-8"   ));	break;
	case Encoding_Unicode:  error = ::_tfopen_s(&m_fpStream, inFileName, TEXT("wc,ccs=UTF-16LE"));	break;
	}

	if(error != 0) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���I�[�v�����s(CStreamWriter_Open)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�t�@�C���N���[�Y
//------------------------------------------------------------------------------
void CStreamWriter::Close()
{
	if(m_fpStream != NULL) {
		::fclose(m_fpStream);
		m_fpStream = NULL;
	}
}

//------------------------------------------------------------------------------
//	�t�@�C����������(������)
//------------------------------------------------------------------------------
void CStreamWriter::WriteString(LPCTSTR inString)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamWriter_WriteString)\n"));
		return;
	}
#endif

	::_fputts(inString, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C����������(�����t��������)
//------------------------------------------------------------------------------
void CStreamWriter::WriteFormatString(LPCTSTR inString, ...)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamWriter_WriteFormatString)\n"));
		return;
	}
#endif

	va_list   args;
	va_start(args, inString);

	::_vftprintf_s(m_fpStream, inString, args);

	va_end(args);
}

//------------------------------------------------------------------------------
//	�t�@�C����������(�P����)
//------------------------------------------------------------------------------
void CStreamWriter::WriteChar(TCHAR inVal)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamWriter_WriteChar)\n"));
		return;
	}
#endif

	::_fputtc(inVal, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C����������(�_���l)
//------------------------------------------------------------------------------
void CStreamWriter::WriteBool(bool inVal)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamWriter_WriteBool)\n"));
		return;
	}
#endif

	const TCHAR   c = inVal ? TEXT('1') : TEXT('0');
	::_fputtc(c, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C���L���b�V�������o��
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
//	�G���[��Ԃ����ׂ�
//------------------------------------------------------------------------------
bool CStreamWriter::IsError()
{
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CStreamWriter_IsError)\n"));
		return true;
	}

	if(::ferror(m_fpStream) == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	�o�C�i���[���[�_�[�N���X�R���X�g���N�^
//------------------------------------------------------------------------------
CBinaryReader::CBinaryReader() : m_fpStream(NULL)
{
}

//------------------------------------------------------------------------------
//	�o�C�i���[���[�_�[�N���X�f�X�g���N�^
//------------------------------------------------------------------------------
CBinaryReader::~CBinaryReader()
{
	Close();
}

//------------------------------------------------------------------------------
//	�t�@�C���I�[�v��
//------------------------------------------------------------------------------
bool CBinaryReader::Open(LPCTSTR inFileName)
{
	Close();

	if(::_tfopen_s(&m_fpStream, inFileName, TEXT("rb")) != 0) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���I�[�v�����s(CBinaryReader_Open)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�t�@�C���N���[�Y
//------------------------------------------------------------------------------
void CBinaryReader::Close()
{
	if(m_fpStream != NULL) {
		::fclose(m_fpStream);
		m_fpStream = NULL;
	}
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���
//------------------------------------------------------------------------------
size_t CBinaryReader::Read(void* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryReader_Read)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CBinaryReader_Read)\n"));
		return 0;
	}
#endif

	return ::fread(pBuf, sizeof(BYTE), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(BYTE�z��)
//------------------------------------------------------------------------------
size_t CBinaryReader::ReadBytes(BYTE* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryReader_ReadBytes)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CBinaryReader_ReadBytes)\n"));
		return 0;
	}
#endif

	return ::fread(pBuf, sizeof(BYTE), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�����z��)
//------------------------------------------------------------------------------
size_t CBinaryReader::ReadChars(char* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryReader_ReadChars)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CBinaryReader_ReadChars)\n"));
		return 0;
	}
#endif

	return ::fread(pBuf, sizeof(char), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(Unicode�����z��)
//------------------------------------------------------------------------------
size_t CBinaryReader::ReadWChars(wchar_t* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryReader_ReadWChars)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CBinaryReader_ReadWChars)\n"));
		return 0;
	}
#endif

	return ::fread(pBuf, sizeof(wchar_t), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ���(�e���v���[�g)
//------------------------------------------------------------------------------
template<typename T> T CBinaryReader::Read()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryReader_Read<template>)\n"));
		return false;
	}
#endif

	T   t = 0;
	::fread(&t, sizeof(t), 1, m_fpStream);

	return t;
}

//------------------------------------------------------------------------------
//	�t�@�C���ǂݍ��݈ʒu�ړ�
//------------------------------------------------------------------------------
long CBinaryReader::Seek(const long inOffset, const SeekOrigin inOrigin)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryReader_Seek)\n"));
		return 0;
	}
#endif

	::fseek(m_fpStream, inOffset, inOrigin);

	return ::ftell(m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C���I�[�����ׂ�
//------------------------------------------------------------------------------
bool CBinaryReader::IsEOF()
{
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryReader_IsEOF)\n"));
		return true;
	}

	if(::feof(m_fpStream) == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	�o�C�i���[���C�^�[�N���X�R���X�g���N�^
//------------------------------------------------------------------------------
CBinaryWriter::CBinaryWriter() : m_fpStream(NULL)
{
}

//------------------------------------------------------------------------------
//	�o�C�i���[���C�^�[�N���X�f�X�g���N�^
//------------------------------------------------------------------------------
CBinaryWriter::~CBinaryWriter()
{
	Close();
}

//------------------------------------------------------------------------------
//	�t�@�C���I�[�v��
//------------------------------------------------------------------------------
bool CBinaryWriter::Open(LPCTSTR inFileName)
{
	Close();

	if(::_tfopen_s(&m_fpStream, inFileName, TEXT("wcb")) != 0) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���I�[�v�����s(CBinaryWriter_Open)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�t�@�C���N���[�Y
//------------------------------------------------------------------------------
void CBinaryWriter::Close()
{
	if(m_fpStream != NULL) {
		::fclose(m_fpStream);
		m_fpStream = NULL;
	}
}

//------------------------------------------------------------------------------
//	�t�@�C����������
//------------------------------------------------------------------------------
size_t CBinaryWriter::Write(void* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryWriter_Write)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CBinaryWriter_Write)\n"));
		return 0;
	}
#endif

	return ::fwrite(pBuf, sizeof(BYTE), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C����������(�o�C�g�z��)
//------------------------------------------------------------------------------
size_t CBinaryWriter::WriteBytes(BYTE* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryWriter_WriteBytes)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CBinaryWriter_WriteBytes)\n"));
		return 0;
	}
#endif

	return ::fwrite(pBuf, sizeof(BYTE), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C����������(�����z��)
//------------------------------------------------------------------------------
size_t CBinaryWriter::WriteChars(char* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryWriter_WriteChars)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CBinaryWriter_WriteChars)\n"));
		return 0;
	}
#endif

	return ::fwrite(pBuf, sizeof(char), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C����������(Unicode�����z��)
//------------------------------------------------------------------------------
size_t CBinaryWriter::WriteWChars(wchar_t* pBuf, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryWriter_WriteWChars)\n"));
		return 0;
	}
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CBinaryWriter_WriteWChars)\n"));
		return 0;
	}
#endif

	return ::fwrite(pBuf, sizeof(wchar_t), inCount, m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C����������(�w��l)
//------------------------------------------------------------------------------
size_t CBinaryWriter::WriteFill(const BYTE inFill, const size_t inCount)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryWriter_WriteFill)\n"));
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
//	�t�@�C����������(�e���v���[�g)
//------------------------------------------------------------------------------
template<typename T> bool CBinaryWriter::Write(const T& inT)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryWriter::Write<template>)\n"));
		return false;
	}
#endif

	return ::fwrite(&inT, sizeof(inT), 1, m_fpStream) == 1;
}

//------------------------------------------------------------------------------
//	�t�@�C���������݈ʒu�ړ�
//------------------------------------------------------------------------------
long CBinaryWriter::Seek(const long inOffset, const SeekOrigin inOrigin)
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryWriter_Seek)\n"));
		return 0;
	}
#endif

	::fseek(m_fpStream, inOffset, inOrigin);

	return ::ftell(m_fpStream);
}

//------------------------------------------------------------------------------
//	�t�@�C���L���b�V�������o��
//------------------------------------------------------------------------------
void CBinaryWriter::Flush()
{
#ifdef _DEBUG
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryWriter_Flush)\n"));
		return;
	}
#endif

	::fflush(m_fpStream);
}

//------------------------------------------------------------------------------
//	�G���[��Ԃ����ׂ�
//------------------------------------------------------------------------------
bool CBinaryWriter::IsError()
{
	if(m_fpStream == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C�����I�[�v��(CBinaryWriter_IsError)\n"));
		return true;
	}

	if(::ferror(m_fpStream) == 0)
		return false;

	return true;
}

//------------------------------------------------------------------------------
//	�������X�g���[���N���X�R���X�g���N�^
//------------------------------------------------------------------------------
CMemoryStream::CMemoryStream()
	: m_pBufferStart(NULL), m_pBufferEnd(NULL), m_pBuffer(NULL), m_pAlloc(NULL),
	  m_hFile(INVALID_HANDLE_VALUE), m_hMapFile(NULL), m_pMemFile(NULL)
{
}

//------------------------------------------------------------------------------
//	�������X�g���[���N���X�f�X�g���N�^
//------------------------------------------------------------------------------
CMemoryStream::~CMemoryStream()
{
	Dispose();
}

//------------------------------------------------------------------------------
//	�������X�g���[������
//------------------------------------------------------------------------------
bool CMemoryStream::Create(const DWORD inMemSize)
{
	Dispose();

	m_pAlloc = new BYTE[inMemSize];
	if(m_pAlloc == NULL) {
		::OutputDebugString(TEXT("*** Error - �������m�ێ��s(CMemoryStream_Create)\n"));
		return false;
	}

	// �|�C���^�ݒ�
	m_pBufferStart = m_pAlloc;
	m_pBufferEnd   = m_pBufferStart + inMemSize;
	m_pBuffer      = m_pBufferStart;

	return true;
}

//------------------------------------------------------------------------------
//	�������X�g���[������
//------------------------------------------------------------------------------
bool CMemoryStream::Create(const void* pBuffer, const DWORD inMemSize)
{
	Dispose();

	// �|�C���^�ݒ�
	m_pBufferStart = (BYTE*)pBuffer;
	m_pBufferEnd   = m_pBufferStart + inMemSize;
	m_pBuffer      = m_pBufferStart;

	return true;
}

//------------------------------------------------------------------------------
//	�������X�g���[������
//------------------------------------------------------------------------------
bool CMemoryStream::Create(LPCTSTR inFileName, const DWORD inFileSize)
{
	Dispose();

	// �t�@�C���I�[�v��
	m_hFile = ::CreateFile(inFileName, GENERIC_READ | GENERIC_WRITE,
						   FILE_SHARE_READ, NULL, OPEN_ALWAYS,
						   FILE_ATTRIBUTE_NORMAL, NULL);
	if(m_hFile == INVALID_HANDLE_VALUE) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���I�[�v�����s(CMemoryStream_Create)\n"));
		return false;
	}

	// �t�@�C���T�C�Y�ݒ�
	if(inFileSize != 0) {
		LARGE_INTEGER   l;
		l.LowPart  = inFileSize;
		l.HighPart = 0;

		::SetFilePointerEx(m_hFile, l, NULL, FILE_BEGIN);
		::SetEndOfFile(m_hFile);
	}

	// �t�@�C���}�b�s���O�I�u�W�F�N�g����
	m_hMapFile = ::CreateFileMapping(m_hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
	if(m_hMapFile == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���}�b�s���O�I�u�W�F�N�g�������s(CMemoryStream_Create)\n"));
		::CloseHandle(m_hFile);		m_hFile    = INVALID_HANDLE_VALUE;
		return false;
	}

	// �t�@�C���}�b�s���O
	m_pMemFile = ::MapViewOfFile(m_hMapFile, FILE_MAP_WRITE, 0, 0, 0);
	if(m_pMemFile == NULL) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���}�b�s���O���s(CMemoryStream_Create)\n"));
		::CloseHandle(m_hMapFile);	m_hMapFile = NULL;
		::CloseHandle(m_hFile   );	m_hFile    = INVALID_HANDLE_VALUE;
		return false;
	}

	// �|�C���^�ݒ�
	m_pBufferStart = (BYTE*)m_pMemFile;
	m_pBufferEnd   = m_pBufferStart + ::GetFileSize(m_hFile, NULL);
	m_pBuffer      = m_pBufferStart;

	return true;
}

//------------------------------------------------------------------------------
//	�������X�g���[�����
//------------------------------------------------------------------------------
void CMemoryStream::Dispose()
{
	// �������}�b�v�h�t�@�C�����
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

	// �m�ۃ��������
	delete[] m_pAlloc;
	m_pAlloc = NULL;

	// �ϐ�������
	m_pBufferStart = NULL;
	m_pBufferEnd   = NULL;
	m_pBuffer      = NULL;
}

//------------------------------------------------------------------------------
//	�������ǂݍ���(�o�b�t�@)
//------------------------------------------------------------------------------
DWORD CMemoryStream::Read(void* pBuf, const DWORD inSize)
{
	if(m_pBuffer >= m_pBufferEnd)
		return 0;

#ifdef _DEBUG
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CMemoryStream_Read)\n"));
		return 0;
	}
#endif

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;				// �c��T�C�Y
	const DWORD   COUNT = inSize <= LEFT ? inSize : LEFT;		// �������ݐ�

	::CopyMemory(pBuf, m_pBuffer, COUNT);
	m_pBuffer += COUNT;

	return COUNT;
}

//------------------------------------------------------------------------------
//	�������ǂݍ���(�P�o�C�g)
//------------------------------------------------------------------------------
BYTE CMemoryStream::ReadByte()
{
	if(m_pBuffer >= m_pBufferEnd)
		return 0;

	const BYTE   b = *m_pBuffer++;
	
	return b;
}

//------------------------------------------------------------------------------
//	�������R�s�[
//------------------------------------------------------------------------------
DWORD CMemoryStream::Copy(void* pBuf, const DWORD inSize)
{
#ifdef _DEBUG
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CMemoryStream_Write)\n"));
		return 0;
	}
#endif

	if(m_pBuffer >= m_pBufferEnd)
		return 0;

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;			// �c��T�C�Y
	const DWORD   COUNT = inSize <= LEFT ? inSize : LEFT;	// �������ݐ�

	::MoveMemory(m_pBuffer, pBuf, COUNT);
	m_pBuffer += COUNT;

	return COUNT;
}

//------------------------------------------------------------------------------
//	��������������(�o�b�t�@)
//------------------------------------------------------------------------------
DWORD CMemoryStream::Write(void* pBuf, const DWORD inSize)
{
#ifdef _DEBUG
	if(pBuf == NULL) {
		::OutputDebugString(TEXT("*** Error - �|�C���^�ُ�(CMemoryStream_Write)\n"));
		return 0;
	}
#endif

	if(m_pBuffer >= m_pBufferEnd)
		return 0;

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;			// �c��T�C�Y
	const DWORD   COUNT = inSize <= LEFT ? inSize : LEFT;	// �������ݐ�

	::CopyMemory(m_pBuffer, pBuf, COUNT);
	m_pBuffer += COUNT;

	return COUNT;
}

//------------------------------------------------------------------------------
//	��������������(�P�o�C�g)
//------------------------------------------------------------------------------
void CMemoryStream::WriteByte(const BYTE inVal)
{
	if(m_pBuffer >= m_pBufferEnd)
		return;

	*m_pBuffer++ = inVal;
}

//------------------------------------------------------------------------------
//	�t�@�C���o��
//------------------------------------------------------------------------------
bool CMemoryStream::WriteTo(LPCTSTR inFileName)
{
	CBinaryWriter   writer;
	if(writer.Open(inFileName) == false) {
		::OutputDebugString(TEXT("*** Error - �t�@�C���������s(CMemoryStream_WriteTo)\n"));
		return false;
	}

	writer.Write(m_pBufferStart, m_pBufferEnd - m_pBufferStart);
	writer.Close();

	return true;
}

//------------------------------------------------------------------------------
//	�w��l�u���b�N��������
//------------------------------------------------------------------------------
DWORD CMemoryStream::Fill(const BYTE inFill, const DWORD inCount)
{
#ifdef _DEBUG
	if(m_pBuffer == NULL) {
		::OutputDebugString(TEXT("*** Error - �������X�g���[��������(CMemoryStream_Fill)\n"));
		return 0;
	}
#endif

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;				// �c��T�C�Y
	const DWORD   COUNT = inCount <= LEFT ? inCount : LEFT;		// �������ݐ�

	::FillMemory(m_pBuffer, COUNT, inFill);
	m_pBuffer += COUNT;

	return COUNT;
}

//------------------------------------------------------------------------------
//	�ʒu�ړ�
//------------------------------------------------------------------------------
DWORD CMemoryStream::Seek(const long inOffset, const SeekOrigin inOrigin)
{
	// �|�C���^�ݒ�
	switch(inOrigin) {
	case SeekOrigin_Begin  :  m_pBuffer = m_pBufferStart;	break;
	case SeekOrigin_Current:  m_pBuffer = m_pBuffer;		break;
	case SeekOrigin_End    :  m_pBuffer = m_pBufferEnd;		break;
	}

	// �͈͐ݒ�
	if(m_pBuffer < m_pBufferStart)
		m_pBuffer = m_pBufferStart;
	else if(m_pBuffer > m_pBufferEnd)
		m_pBuffer = m_pBufferEnd;

	return m_pBuffer - m_pBufferStart;
}

//------------------------------------------------------------------------------
//	�L���b�V�������o��
//------------------------------------------------------------------------------
void CMemoryStream::Flush()
{
	if(m_pMemFile == NULL)
		return;

	::FlushViewOfFile(m_pMemFile, 0);
}

//------------------------------------------------------------------------------
//	�������ǂݍ���(�e���v���[�g)
//------------------------------------------------------------------------------
template<typename T> T CMemoryStream::Read()
{
	if(m_pBuffer >= m_pBufferEnd)
		return 0;

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;					// �c��T�C�Y
	const DWORD   COUNT = sizeof(T) <= LEFT ? sizeof(T) : LEFT;		// �ǂݍ��ݐ�

	T   t = 0;
	::CopyMemory(&t, m_pBuffer, COUNT);
	m_pBuffer += COUNT;

	return t;
}

//------------------------------------------------------------------------------
//	��������������(�e���v���[�g)
//------------------------------------------------------------------------------
template<typename T> void CMemoryStream::Write(const T& inT)
{
	if(m_pBuffer >= m_pBufferEnd)
		return;

	const DWORD   LEFT  = m_pBufferEnd - m_pBuffer;					// �c��T�C�Y
	const DWORD   COUNT = sizeof(T) <= LEFT ? sizeof(T) : LEFT;		// �������ݐ�

	::CopyMemory(m_pBuffer, &inT, COUNT);
	m_pBuffer += COUNT;
}

//------------------------------------------------------------------------------
//	�t�H���_���t�@�C�����擾
//------------------------------------------------------------------------------
DWORD DirectoryInfo::GetFileEntries(LPCTSTR inFileName)
{
	WIN32_FIND_DATA   fd;
	HANDLE            hFind = ::FindFirstFile(inFileName, &fd);
	if(hFind == INVALID_HANDLE_VALUE)
		return 0;

	DWORD   entreis = 0;
	while(true) {
		// �f�B���N�g���łȂ���΃C���N�������g
		if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			entreis++;

		// ���̃t�@�C����
		if(::FindNextFile(hFind, &fd) == 0)
			break;
	}
	::FindClose(hFind);

	return entreis;
}

//------------------------------------------------------------------------------
//	�t�H���_���t�@�C�����X�g�擾
//------------------------------------------------------------------------------
DWORD DirectoryInfo::GetFileList(FileInfoList& outFileList, LPCTSTR inFileName)
{
	outFileList.clear();

	WIN32_FIND_DATA   fd;
	HANDLE            hFind = ::FindFirstFile(inFileName, &fd);
	if(hFind == INVALID_HANDLE_VALUE)
		return 0;

	while(true) {
		// �f�B���N�g���łȂ���Ώ��擾
		if((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0) {
			FileInfo   fi;
			::_tcscpy_s(fi.FileName, MAX_PATH, fd.cFileName);
			fi.FileSize = fd.nFileSizeLow;
			outFileList.push_back(fi);
		}

		// ���̃t�@�C����
		if(::FindNextFile(hFind, &fd) == 0)
			break;
	}
	::FindClose(hFind);

	return outFileList.size();
}
