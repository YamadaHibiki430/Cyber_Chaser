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
	 Kinect.hpp
				Kinect�N���X�w�b�_�[�t�@�C��

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

#pragma once

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <windows.h>
#include <NuiApi.h>
#include <KinectBackgroundRemoval.h>
#include <d3dx9.h>

//------------------------------------------------------------------------------
//	�񋓑̒�`
//------------------------------------------------------------------------------
enum NUI_IMAGE {
	NUI_IMAGE_COLOR    = NUI_IMAGE_TYPE_COLOR,
	NUI_IMAGE_INFRAED  = NUI_IMAGE_TYPE_COLOR_INFRARED,
	NUI_IMAGE_RAWBAYER = NUI_IMAGE_TYPE_COLOR_RAW_BAYER
};

//------------------------------------------------------------------------------
//	�\���̒�`
//------------------------------------------------------------------------------
// �f�v�X���\����
struct DEPTH_DESC {
	INuiFrameTexture*   pFrameTexture;
	DWORD               Pitch;
	DWORD               Width;
	DWORD               Height;
	DWORD               BytesPerPixel;
};

// �g���b�N���C���f�b�N�X
struct TRACKED_INDEX {
	int   First;
	int   Second;
	int   ZFront;
	int   ZBack;
};

// �g�����X�t�H�[���ς݃��C�e�B���O�ςݒ��_
struct NUITLVERTEX {
	float      x, y, z;		// ���_���W
	float      rhw;			// RHW
	D3DCOLOR   color;		// ���_�F

	NUITLVERTEX() : x(0.0f), y(0.0f), z(0.0f), rhw(1.0f), color(0) {}
	static inline DWORD FVF() { return D3DFVF_XYZRHW | D3DFVF_DIFFUSE; }
};

//------------------------------------------------------------------------------
//	Kinect�C���^�t�F�[�X��`
//------------------------------------------------------------------------------
class IKinect {
public:
	virtual ~IKinect() {}

	// �X�g���[���I�[�v��
	virtual bool ColorStreamOpen(const NUI_IMAGE_RESOLUTION inResolution,
								 const NUI_IMAGE inImageType = NUI_IMAGE_COLOR) = 0;
	virtual bool DepthStreamOpen(const NUI_IMAGE_RESOLUTION inResolution) = 0;

	virtual bool SkeletonTrackingEnable() = 0;
	inline  bool SkeletonStreamOpen() { return SkeletonTrackingEnable(); }

	// �C���[�W�擾
	virtual IDirect3DTexture9* GetColorFrame() = 0;

	virtual DEPTH_DESC GetDepthFrame() = 0;
	virtual DEPTH_DESC GetDepthImage() = 0;

	virtual NUI_SKELETON_FRAME GetSkeletonFrame() = 0;
	virtual TRACKED_INDEX GetTrackedIndex(NUI_SKELETON_FRAME& inSkeletonFrame) = 0;

	// �L�l�N�g�ݒ�
	virtual void SetForceInfraredEmitter(const BOOL inEnabled) = 0;	// Windows��Kinect��p

	// �J�����ݒ�
	virtual void CameraResetSettings() = 0;

	virtual void CameraSetAutoExposure    (const BOOL inEnabled ) = 0;
	virtual void CameraSetAutoWhiteBalance(const BOOL inEnabled ) = 0;

	virtual void CameraSetBrightness  (const double inBrightness) = 0;
	virtual void CameraSetContrast    (const double inContrast  ) = 0;
	virtual void CameraSetGain        (const double inGain      ) = 0;
	virtual void CameraSetGamma       (const double inGamma     ) = 0;
	virtual void CameraSetHue         (const double inHue       ) = 0;
	virtual void CameraSetSaturation  (const double inSaturation) = 0;
	virtual void CameraSetSharpness   (const double inSharpness ) = 0;
	virtual void CameraSetWhiteBalance(const LONG inWhiteBalance) = 0;

	virtual void CameraSetBacklightMode(const NUI_BACKLIGHT_COMPENSATION_MODE inMode) = 0;

	// �X�P���g���F�ݒ�
	virtual void SetSkeletonColor(const UINT inID, const D3DCOLOR inColor) = 0;

	// �w�i����
	virtual bool BackgroundRemovedEnable () = 0;
	virtual void BackgroundRemovedDisable() = 0;
	virtual IDirect3DTexture9* GetBackgroundRemovedFrame() = 0;

	// ���W�ϊ�
	virtual void SkeletonPointToColorPoint(NUI_SKELETON_FRAME& ioSkeletonFrame) = 0;
	virtual void SkeletonPointToColorPoint(NUI_SKELETON_DATA&  ioSkeletonData ) = 0;
	virtual D3DXVECTOR3 SkeletonPointToColorPoint(_Vector4& inSkeletonPosition) = 0;

	virtual UINT SkeletonFrameToTLVertex(NUITLVERTEX outVertesies[],
										 NUI_SKELETON_FRAME& inSkeletonFrame,
										 const float inXOffset = 0.0f,
										 const float inYOffset = 0.0f,
										 const float inScale   = 1.0f) = 0;
	virtual UINT SkeletonFrameToTLVBone (NUITLVERTEX outVertesies[],
										 NUI_SKELETON_FRAME& inSkeletonFrame,
										 const float inXOffset = 0.0f,
										 const float inYOffset = 0.0f,
										 const float inScale   = 1.0f) = 0;

	// �I�y���[�^
	virtual operator IDirect3DTexture9*() = 0;
};

//------------------------------------------------------------------------------
//	Kinect�N���X��`
//------------------------------------------------------------------------------
class CKinect : public IKinect {
public:
	CKinect(INuiSensor* pSensor, IDirect3DDevice9* pD3DDevice);
	virtual ~CKinect();

	virtual bool ColorStreamOpen(const NUI_IMAGE_RESOLUTION inResolution,
								 const NUI_IMAGE inImageType);
	virtual bool DepthStreamOpen(const NUI_IMAGE_RESOLUTION inResolution);

	virtual bool SkeletonTrackingEnable();

	virtual IDirect3DTexture9* GetColorFrame();

	virtual DEPTH_DESC GetDepthFrame();
	virtual DEPTH_DESC GetDepthImage();

	virtual NUI_SKELETON_FRAME GetSkeletonFrame();
	virtual TRACKED_INDEX GetTrackedIndex(NUI_SKELETON_FRAME& inSkeletonFrame);

	virtual void SetForceInfraredEmitter(const BOOL inEnabled);

	virtual void CameraResetSettings();

	virtual void CameraSetAutoExposure    (const BOOL inEnabled );
	virtual void CameraSetAutoWhiteBalance(const BOOL inEnabled );

	virtual void CameraSetBrightness  (const double inBrightness);
	virtual void CameraSetContrast    (const double inContrast  );
	virtual void CameraSetGain        (const double inGain      );
	virtual void CameraSetGamma       (const double inGamma     );
	virtual void CameraSetHue         (const double inHue       );
	virtual void CameraSetSaturation  (const double inSaturation);
	virtual void CameraSetSharpness   (const double inSharpness );
	virtual void CameraSetWhiteBalance(const LONG inWhiteBalance);

	virtual void CameraSetBacklightMode(const NUI_BACKLIGHT_COMPENSATION_MODE inMode);

	virtual void SetSkeletonColor(const UINT inID, const D3DCOLOR inColor)
	{ m_SkeletonColor[inID] = inColor; }

	virtual bool BackgroundRemovedEnable ();
	virtual void BackgroundRemovedDisable();
	virtual IDirect3DTexture9* GetBackgroundRemovedFrame();

	virtual void SkeletonPointToColorPoint(NUI_SKELETON_FRAME& ioSkeletonFrame);
	virtual void SkeletonPointToColorPoint(NUI_SKELETON_DATA&  ioSkeletonData );
	virtual D3DXVECTOR3 SkeletonPointToColorPoint(_Vector4& inSkeletonPosition);

	virtual UINT SkeletonFrameToTLVertex(NUITLVERTEX outVertesies[],
										 NUI_SKELETON_FRAME& inSkeletonFrame,
										 const float inXOffset, const float inYOffset,
										 const float inScale);
	virtual UINT SkeletonFrameToTLVBone (NUITLVERTEX outVertesies[],
										 NUI_SKELETON_FRAME& inSkeletonFrame,
										 const float inXOffset, const float inYOffset,
										 const float inScale);

	virtual operator IDirect3DTexture9*()
	{ return !m_pBGRemovalStream ? GetColorFrame() : GetBackgroundRemovedFrame(); }

private:
	void SetTrackedSkeleton();

	template <class T> inline void SafeRelease(T& x)
	{ if(x != NULL) { x->Release(); x = NULL; } }

	inline void SafeCloseHandle(HANDLE& h)
	{ if(h != INVALID_HANDLE_VALUE) { ::CloseHandle(h); h = INVALID_HANDLE_VALUE; } }

private:
	INuiSensor*                         m_pKinect;
	INuiCoordinateMapper*               m_pCoordMapper;
	INuiColorCameraSettings*            m_pCameraSettings;

	IDirect3DDevice9*                   m_pD3DDevice;

	// �J���[
	HANDLE                              m_hColorStream;
	HANDLE                              m_hColorEvent;
	IDirect3DTexture9*                  m_pColorTexture;
	NUI_IMAGE_RESOLUTION                m_ColorResolution;

	// �f�v�X
	HANDLE                              m_hDepthStream;
	HANDLE                              m_hDepthEvent;
	NUI_IMAGE_FRAME                     m_DepthFrame;
	INuiFrameTexture*                   m_pDepthDataTexture;
	NUI_IMAGE_RESOLUTION                m_DepthResolution;

	// �X�P���g��
	HANDLE                              m_hSkeletonEvent;
	NUI_SKELETON_FRAME                  m_SkeletonFrame;
	D3DCOLOR                            m_SkeletonColor[NUI_SKELETON_COUNT];

	// �w�i����
	INuiBackgroundRemovedColorStream*   m_pBGRemovalStream;
	HANDLE                              m_hBGRemovalEvent;
	IDirect3DTexture9*                  m_pBGRemovalTexture;
	DWORD                               m_TrackedSkeleton;
};

//------------------------------------------------------------------------------
//	NULLKinect�N���X��`
//------------------------------------------------------------------------------
class CNullKinect : public IKinect {
public:
	CNullKinect() {}
	virtual ~CNullKinect() {}

	virtual bool ColorStreamOpen(const NUI_IMAGE_RESOLUTION inResolution,
								 const NUI_IMAGE inImageType) { return false; }
	virtual bool DepthStreamOpen(const NUI_IMAGE_RESOLUTION inResolution) { return false; }

	virtual bool SkeletonTrackingEnable() { return false; }

	virtual IDirect3DTexture9* GetColorFrame()    { return NULL; }

	virtual DEPTH_DESC GetDepthFrame();
	virtual DEPTH_DESC GetDepthImage();

	virtual NUI_SKELETON_FRAME GetSkeletonFrame();
	virtual TRACKED_INDEX GetTrackedIndex(NUI_SKELETON_FRAME& inSkeletonFrame);

	virtual void SetForceInfraredEmitter(const BOOL inEnable) {}

	virtual void CameraResetSettings() {}

	virtual void CameraSetAutoExposure    (const BOOL inEnabled ) {}
	virtual void CameraSetAutoWhiteBalance(const BOOL inEnabled ) {}

	virtual void CameraSetBrightness  (const double inBrightness) {}
	virtual void CameraSetContrast    (const double inContrast  ) {}
	virtual void CameraSetGain        (const double inGain      ) {}
	virtual void CameraSetGamma       (const double inGamma     ) {}
	virtual void CameraSetHue         (const double inHue       ) {}
	virtual void CameraSetSaturation  (const double inSaturation) {}
	virtual void CameraSetSharpness   (const double inSharpness ) {}
	virtual void CameraSetWhiteBalance(const LONG inWhiteBalance) {}

	virtual void CameraSetBacklightMode(const NUI_BACKLIGHT_COMPENSATION_MODE inMode) {}

	virtual void SetSkeletonColor(const UINT inID, const D3DCOLOR inColor) {}

	virtual bool BackgroundRemovedEnable () { return false; }
	virtual void BackgroundRemovedDisable() {}
	virtual IDirect3DTexture9* GetBackgroundRemovedFrame() { return NULL; }

	virtual void SkeletonPointToColorPoint(NUI_SKELETON_FRAME& ioSkeletonFrame) {}
	virtual void SkeletonPointToColorPoint(NUI_SKELETON_DATA&  ioSkeletonData ) {}
	virtual D3DXVECTOR3 SkeletonPointToColorPoint(_Vector4& inSkeletonPosition)
	{ return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }

	virtual UINT SkeletonFrameToTLVertex(NUITLVERTEX outVertesies[],
										 NUI_SKELETON_FRAME& inSkeletonFrame,
										 const float inXOffset, const float inYOffset,
										 const float inScale)
	{ return 0; }

	virtual UINT SkeletonFrameToTLVBone (NUITLVERTEX outVertesies[],
										 NUI_SKELETON_FRAME& inSkeletonFrame,
										 const float inXOffset, const float inYOffset,
										 const float inScale)
	{ return 0; }

	virtual operator IDirect3DTexture9*() { return NULL; }
};

//------------------------------------------------------------------------------
//	�w���p�[�֐�
//------------------------------------------------------------------------------
inline D3DXVECTOR3 NUIVector4ToVector3(_Vector4& vec4)
{
	return D3DXVECTOR3(vec4.x, vec4.y, vec4.z);
}
