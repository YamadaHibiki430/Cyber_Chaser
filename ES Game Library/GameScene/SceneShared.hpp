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
	 SceneShared.hpp
				�V�[�����L�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <map>
#include <string>
#include <tchar.h>

//------------------------------------------------------------------------------
//	�V�[�����L�N���X��`
//------------------------------------------------------------------------------
class CSceneShared {
public:
	virtual ~CSceneShared() {}

	// �V���O���g���C���X�^���X�擾
	static CSceneShared& GetInstance()
	{
		static CSceneShared   theSceneShared;
		return theSceneShared;
	}

	// ����
	inline void SetIntegerForKey   (const char* pKey, const int    inValue) { m_MapInt[pKey] = inValue;    }
	inline void RemoveIntegerForKey(const char* pKey)						{ m_MapInt.erase(pKey);        }
	int GetIntegerForKey(const char* pKey);

	// �P���x��������
	inline void SetFloatForKey     (const char* pKey, const float  inValue) { m_MapFloat[pKey] = inValue;  }
	inline void RemoveFloatForKey  (const char* pKey)						{ m_MapFloat.erase(pKey);      }
	float GetFloatForKey(const char* pKey);

	// �{���x��������
	inline void SetDoubleForKey    (const char* pKey, const double inValue) { m_MapDouble[pKey] = inValue; }
	inline void RemoveDoubleForKey (const char* pKey)						{ m_MapDouble.erase(pKey);     }
	double GetDoubleForKey(const char* pKey);

	// �u�[��
	inline void SetBoolForKey      (const char* pKey, const bool   inValue)	{ m_MapBool[pKey] = inValue;   }
	inline void RemoveBoolForKey   (const char* pKey)						{ m_MapBool.erase(pKey);       }
	bool GetBoolForKey(const char* pKey);

	// �f�[�^
	inline void SetDataForKey      (const char* pKey,       void*  pPtr   )	{ m_MapVoidPtr[pKey] = pPtr;   }
	inline void RemoveDataForKey   (const char* pKey)						{ m_MapVoidPtr.erase(pKey);    }
	void* GetDataForKey(const char* pKey);

	// ������
	inline void SetStringForKey(const char* pKey, const std::basic_string<TCHAR> inString)
	{ m_MapString[pKey] = inString; }
	inline void RemoveStringForKey(const char* pKey)
	{ m_MapString.erase(pKey);      }
	std::basic_string<TCHAR> GetStringForKey(const char* pKey);

private:
	CSceneShared() {}

	std::map<std::basic_string<char>, int   >   m_MapInt;
	std::map<std::basic_string<char>, float >   m_MapFloat;
	std::map<std::basic_string<char>, double>   m_MapDouble;
	std::map<std::basic_string<char>, bool  >   m_MapBool;
	std::map<std::basic_string<char>, void* >   m_MapVoidPtr;
	std::map<std::basic_string<char>,
				   std::basic_string<TCHAR> >   m_MapString;
};

//------------------------------------------------------------------------------
//	�}�N����`
//------------------------------------------------------------------------------
inline CSceneShared& SceneShared()
{ return CSceneShared::GetInstance(); }
