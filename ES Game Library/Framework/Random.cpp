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
	 Random.cpp
				�����N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Random.hpp"

//------------------------------------------------------------------------------
//	��ݒ�
//------------------------------------------------------------------------------
void CRandom::SetSeed(const unsigned long inSeed)
{
	m_State[0] = inSeed;
	for (int i = 1; i < N; i++) {
		m_State[i]  = (1812433253 * (m_State[i - 1] ^ (m_State[i - 1] >> 30)) + i);
	}
	m_Left = 1;
}

//------------------------------------------------------------------------------
//	��������
//------------------------------------------------------------------------------
unsigned long CRandom::Sample()
{
	if(--m_Left == 0)
		NextState();

	unsigned long   random = *m_pNext++;
	random ^= (random >> 11);
	random ^= (random <<  7) & 0x9d2c5680;
	random ^= (random << 15) & 0xefc60000;
	random ^= (random >> 18);

	return random;
}

//------------------------------------------------------------------------------
//	���v�f�ݒ�
//------------------------------------------------------------------------------
void CRandom::NextState()
{
	unsigned long*   pState = m_State;

	m_Left   = N;
	m_pNext = m_State;

	int   i;
	for(i = N - M + 1; --i > 0; pState++)
		*pState = *(pState + M) ^ Twist(*pState, *(pState + 1));
	for(i = M; --i > 0; pState++)
		*pState = *(pState + (M - N)) ^ Twist(*pState, m_State[0]);

	*pState = *(pState + (M - N)) ^ Twist(*pState, m_State[0]);
}

//------------------------------------------------------------------------------
//	�w���p�[�֐�
//------------------------------------------------------------------------------
float Math_Random()
{
	static CRandom   random;
	return random.NextFloat();
}

int Math_RandomInt()
{
	static CRandom   random;
	return random.Next();
}
