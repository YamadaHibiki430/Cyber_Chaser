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
	 Kinect.cpp
				Kinect�N���X

 �y�o�[�W�����z
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	�C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Kinect.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	�X�^�e�B�b�N�����N���C�u����
//------------------------------------------------------------------------------
#pragma comment(lib, "KinectBackgroundRemoval180_32.lib")

//------------------------------------------------------------------------------
//	�R���X�g���N�^
//------------------------------------------------------------------------------
CKinect::CKinect(INuiSensor* pSensor, IDirect3DDevice9* pD3DDevice)
	: m_pKinect(pSensor), m_pD3DDevice(pD3DDevice),
	  m_pCoordMapper(NULL), m_pCameraSettings(NULL), 
	  m_hColorStream(INVALID_HANDLE_VALUE), m_hColorEvent(INVALID_HANDLE_VALUE),
	  m_pColorTexture(NULL), m_ColorResolution(NUI_IMAGE_RESOLUTION_INVALID),
	  m_hDepthStream(INVALID_HANDLE_VALUE), m_hDepthEvent(INVALID_HANDLE_VALUE),
	  m_pDepthDataTexture(NULL), m_DepthResolution(NUI_IMAGE_RESOLUTION_INVALID),
	  m_hSkeletonEvent(INVALID_HANDLE_VALUE),
	  m_pBGRemovalStream(NULL), m_hBGRemovalEvent(INVALID_HANDLE_VALUE),
	  m_pBGRemovalTexture(NULL), m_TrackedSkeleton(NUI_SKELETON_INVALID_TRACKING_ID)
{
	assert(m_pKinect != NULL);
	m_pKinect->AddRef();

	assert(m_pD3DDevice != NULL);
	m_pD3DDevice->AddRef();

	::ZeroMemory(&m_DepthFrame   , sizeof(m_DepthFrame   ));
	::ZeroMemory(&m_SkeletonFrame, sizeof(m_SkeletonFrame));

	m_SkeletonColor[0] = D3DCOLOR_XRGB(255,   0,   0);	// ID0:��
	m_SkeletonColor[1] = D3DCOLOR_XRGB(  0, 255,   0);	// ID1:��
	m_SkeletonColor[2] = D3DCOLOR_XRGB(  0,   0, 255);	// ID2:��
	m_SkeletonColor[3] = D3DCOLOR_XRGB(255, 255,   0);	// ID3:��
	m_SkeletonColor[4] = D3DCOLOR_XRGB(255,   0, 255);	// ID4:��
	m_SkeletonColor[5] = D3DCOLOR_XRGB(  0, 255, 255);	// ID5:���F
}

//------------------------------------------------------------------------------
//	�f�X�g���N�^
//------------------------------------------------------------------------------
CKinect::~CKinect()
{
	BackgroundRemovedDisable();

	SafeRelease(m_pDepthDataTexture);
	if(m_DepthFrame.pFrameTexture != NULL)
		m_pKinect->NuiImageStreamReleaseFrame(m_hDepthStream, &m_DepthFrame);

	SafeRelease(m_pColorTexture   );

	SafeRelease(m_pCameraSettings);
	SafeRelease(m_pCoordMapper   );

	m_pD3DDevice->Release();

	m_pKinect->NuiShutdown();
	m_pKinect->Release();

	SafeCloseHandle(m_hSkeletonEvent);
	SafeCloseHandle(m_hDepthEvent);
	SafeCloseHandle(m_hColorEvent);
}

//------------------------------------------------------------------------------
//	�J���[�X�g���[���I�[�v��
//------------------------------------------------------------------------------
bool CKinect::ColorStreamOpen(const NUI_IMAGE_RESOLUTION inResolution,
							  const NUI_IMAGE inImageType)
{
	if(m_ColorResolution != NUI_IMAGE_RESOLUTION_INVALID) {
		::OutputDebugString(TEXT("*** Error - �J���[�X�g���[�����I�[�v��(CKinect_ColorStreamOpen)\n"));
		return false;
	}

	// �X�g���[���I�[�v��
	m_hColorEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_pKinect->NuiImageStreamOpen((NUI_IMAGE_TYPE)inImageType, inResolution,
									 0, 2, m_hColorEvent, &m_hColorStream)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - �J���[�X�g���[���I�[�v�����s(CKinect_ColorStreamOpen)\n"));
		return false;
	}
	m_ColorResolution = inResolution;

	// �R�[�f�B�l�[�g�}�b�p�[�擾
	if(m_pCoordMapper == NULL)
		m_pKinect->NuiGetCoordinateMapper(&m_pCoordMapper);

	// �J���[�J�����Z�b�e�B���O�擾
	m_pKinect->NuiGetColorCameraSettings(&m_pCameraSettings);

	// �T�C�Y�擾
	DWORD   width, height;
	::NuiImageResolutionToSize(inResolution, width, height);

	// �e�N�X�`������
	if(::D3DXCreateTexture(m_pD3DDevice, width, height, 1, 0, D3DFMT_X8R8G8B8,
						   D3DPOOL_MANAGED, &m_pColorTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CKinect_ColorStreamOpen)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�f�v�X�X�g���[���I�[�v��
//------------------------------------------------------------------------------
bool CKinect::DepthStreamOpen(const NUI_IMAGE_RESOLUTION inResolution)
{
	if(m_DepthResolution != NUI_IMAGE_RESOLUTION_INVALID) {
		::OutputDebugString(TEXT("*** Error - �f�v�X�X�g���[�����I�[�v��(CKinect_DepthStreamOpen)\n"));
		return false;
	}

	// �X�g���[���I�[�v��
	m_hDepthEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_pKinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, inResolution,
									 0, 2, m_hDepthEvent, &m_hDepthStream)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - �f�v�X�X�g���[���I�[�v�����s(CKinect_DepthStreamOpen)\n"));
		return false;
	}
	m_DepthResolution = inResolution;

	return true;
}

//------------------------------------------------------------------------------
//	�X�P���g���g���b�L���O�L����
//------------------------------------------------------------------------------
bool CKinect::SkeletonTrackingEnable()
{
	m_hSkeletonEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_pKinect->NuiSkeletonTrackingEnable(m_hSkeletonEvent, 0) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �X�P���g���g���b�L���O�L�������s(CKinect_SkeletonTrackingEnable)\n"));
		return false;
	}

	// �R�[�f�B�l�[�g�}�b�p�[�擾
	if(m_pCoordMapper == NULL)
		m_pKinect->NuiGetCoordinateMapper(&m_pCoordMapper);

	return true;
}

//------------------------------------------------------------------------------
//	�J���[�t���[���擾
//------------------------------------------------------------------------------
IDirect3DTexture9* CKinect::GetColorFrame()
{
	// �C�x���g�m�F
	if(::WaitForSingleObject(m_hColorEvent, 0) != WAIT_OBJECT_0)
		return m_pColorTexture;

	// �J���[�t���[���擾
    NUI_IMAGE_FRAME   image_frame;
	if(m_pKinect->NuiImageStreamGetNextFrame(m_hColorStream, 0, &image_frame) != S_OK)
		return m_pColorTexture;

	// �t���[���e�N�X�`�����擾
	NUI_SURFACE_DESC   surface_desc;
	image_frame.pFrameTexture->GetLevelDesc(0, &surface_desc);

	// �t���[���e�N�X�`�����b�N
	NUI_LOCKED_RECT    frame_lock;
	if(image_frame.pFrameTexture->LockRect(0, &frame_lock, NULL, 0) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �t���[���e�N�X�`�����b�N���s(CKinect_GetColorFrame)\n"));
		return m_pColorTexture;
	}

	// �]����e�N�X�`�����b�N
	D3DLOCKED_RECT   texture_lock;
	if(m_pColorTexture->LockRect(0, &texture_lock, NULL, 0) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �]����e�N�X�`�����b�N���s(CKinect_GetColorFrame\n"));
		image_frame.pFrameTexture->UnlockRect(0);
		return m_pColorTexture;
	}

	// �s�N�Z���]��
	if(image_frame.eImageType == NUI_IMAGE_TYPE_COLOR) {
		// �J���[�C���[�W
		DWORD*   pFrame;
		DWORD*   pTexture;
		for(UINT y = 0; y < surface_desc.Height; y++) {
			pFrame   = (DWORD*)((BYTE*)  frame_lock.pBits + y *   frame_lock.Pitch);
			pTexture = (DWORD*)((BYTE*)texture_lock.pBits + y * texture_lock.Pitch);

			::CopyMemory(pTexture, pFrame, frame_lock.Pitch);
		}
	} else if(image_frame.eImageType == NUI_IMAGE_TYPE_COLOR_INFRARED) {
		// �ԊO���C���[�W
		 WORD*   pFrame;
		DWORD*   pTexture;
		for(UINT y = 0; y < surface_desc.Height; y++) {
			pFrame   = ( WORD*)((BYTE*)  frame_lock.pBits + y *   frame_lock.Pitch);
			pTexture = (DWORD*)((BYTE*)texture_lock.pBits + y * texture_lock.Pitch);

			for(UINT x = 0; x < surface_desc.Width; x++) {
				const DWORD   COLOR = *pFrame >> 8;		// 16�r�b�g����8�r�b�g�֌��F
				*pTexture = COLOR | COLOR << 8 | COLOR << 16 | 0xff000000;
				pFrame  ++;
				pTexture++;
			}
		}
	} else if(image_frame.eImageType == NUI_IMAGE_TYPE_COLOR_RAW_BAYER) {
		// RawBayer�C���[�W
		 BYTE*   pFrame;
		DWORD*   pTexture;
		for(UINT y = 0; y < surface_desc.Height; y++) {
			pFrame   = ( BYTE*)          frame_lock.pBits + y *   frame_lock.Pitch;
			pTexture = (DWORD*)((BYTE*)texture_lock.pBits + y * texture_lock.Pitch);

			for(UINT x = 0; x < surface_desc.Width; x++) {
				const DWORD   COLOR = *pFrame;
				*pTexture = COLOR | COLOR << 8 | COLOR << 16 | 0xff000000;
				pFrame  ++;
				pTexture++;
			}
		}
	}

	// �w�i��������
	if(m_pBGRemovalStream != NULL) {
		m_pBGRemovalStream->ProcessColor(frame_lock.Pitch * surface_desc.Height,
										 frame_lock.pBits, image_frame.liTimeStamp);
	}

	m_pColorTexture->UnlockRect(0);
	image_frame.pFrameTexture->UnlockRect(0);

	m_pKinect->NuiImageStreamReleaseFrame(m_hColorStream, &image_frame);

	return m_pColorTexture;
}

//------------------------------------------------------------------------------
//	�f�v�X�t���[���擾
//------------------------------------------------------------------------------
DEPTH_DESC CKinect::GetDepthFrame()
{
	// �f�v�X�t���[���擾
	HRESULT           result = S_FALSE;
    NUI_IMAGE_FRAME   image_frame;
	if(::WaitForSingleObject(m_hDepthEvent, 0) == WAIT_OBJECT_0)
		result = m_pKinect->NuiImageStreamGetNextFrame(m_hDepthStream, 0, &image_frame);

	if(result == S_OK) {
		// ���f�v�X�t���[�����
		SafeRelease(m_pDepthDataTexture);
		m_pKinect->NuiImageStreamReleaseFrame(m_hDepthStream, &m_DepthFrame);

		// �f�v�X�t���[���ۑ�
		m_DepthFrame = image_frame;
	} else {
		if(m_DepthFrame.pFrameTexture == NULL) {
			// �f�v�X�t���[�����擾
			DEPTH_DESC   desc;
			::ZeroMemory(&desc, sizeof(desc));
			return desc;
		}
	}

	// �e�N�X�`�����擾
	NUI_SURFACE_DESC   surface_desc;
	m_DepthFrame.pFrameTexture->GetLevelDesc(0, &surface_desc);

	// �f�v�X���ݒ�
	DEPTH_DESC   depth_desc;
	depth_desc.pFrameTexture = m_DepthFrame.pFrameTexture;
	depth_desc.Pitch         = m_DepthFrame.pFrameTexture->Pitch();
	depth_desc.Width         = surface_desc.Width;
	depth_desc.Height        = surface_desc.Height;
	depth_desc.BytesPerPixel = sizeof(USHORT);

	return depth_desc;
}

//------------------------------------------------------------------------------
//	�f�v�X�C���[�W�擾
//------------------------------------------------------------------------------
DEPTH_DESC CKinect::GetDepthImage()
{
	// �f�v�X�t���[���擾
	HRESULT           result = S_FALSE;
	NUI_IMAGE_FRAME   image_frame;
	if(::WaitForSingleObject(m_hDepthEvent, 0) == WAIT_OBJECT_0)
		result = m_pKinect->NuiImageStreamGetNextFrame(m_hDepthStream, 0, &image_frame);

	if(result == S_OK) {
		// ���f�v�X�t���[�����
		SafeRelease(m_pDepthDataTexture);
		m_pKinect->NuiImageStreamReleaseFrame(m_hDepthStream, &m_DepthFrame);

		// �f�v�X�t���[���ۑ�
		m_DepthFrame = image_frame;
	} else {
		if(m_pDepthDataTexture != NULL) {
			// �f�v�X�f�[�^�e�N�X�`���擾�ς�
			// �e�N�X�`�����擾
			NUI_SURFACE_DESC   surface_desc;
			m_pDepthDataTexture->GetLevelDesc(0, &surface_desc);

			// �f�v�X���ݒ�
			DEPTH_DESC   depth_desc;
			depth_desc.pFrameTexture = m_pDepthDataTexture;
			depth_desc.Pitch         = m_pDepthDataTexture->Pitch();
			depth_desc.Width         = surface_desc.Width;
			depth_desc.Height        = surface_desc.Height;
			depth_desc.BytesPerPixel = sizeof(NUI_DEPTH_IMAGE_PIXEL);

			return depth_desc;
		}
	}

	// �f�v�X�e�N�X�`���擾
	BOOL   near_mode = FALSE;
	if(m_pKinect->NuiImageFrameGetDepthImagePixelFrameTexture(m_hDepthStream, &image_frame, 
															  &near_mode, &m_pDepthDataTexture) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �f�v�X�e�N�X�`���擾���s(CKinect_GetDepthImagePixel\n"));

		DEPTH_DESC   desc;
		::ZeroMemory(&desc, sizeof(desc));
		return desc;
	}

	// �e�N�X�`�����擾
	NUI_SURFACE_DESC   surface_desc;
	m_pDepthDataTexture->GetLevelDesc(0, &surface_desc);

	// �f�v�X���ݒ�
	DEPTH_DESC   depth_desc;
	depth_desc.pFrameTexture = m_pDepthDataTexture;
	depth_desc.Pitch         = m_pDepthDataTexture->Pitch();
	depth_desc.Width         = surface_desc.Width;
	depth_desc.Height        = surface_desc.Height;
	depth_desc.BytesPerPixel = sizeof(NUI_DEPTH_IMAGE_PIXEL);

	// �w�i��������
	if(m_pBGRemovalStream != NULL) {
		NUI_LOCKED_RECT   lock_rect;
		if(depth_desc.pFrameTexture->LockRect(0, &lock_rect, NULL, 0) == S_OK) {
			m_pBGRemovalStream->ProcessDepth(depth_desc.Pitch * depth_desc.Height,
											 lock_rect.pBits, m_DepthFrame.liTimeStamp);
			depth_desc.pFrameTexture->UnlockRect(0);
		}
	}

	return depth_desc;
}

//------------------------------------------------------------------------------
//	�X�P���g���t���[���擾
//------------------------------------------------------------------------------
NUI_SKELETON_FRAME CKinect::GetSkeletonFrame()
{
	// �C�x���g�m�F
	if(::WaitForSingleObject(m_hSkeletonEvent, 0) != WAIT_OBJECT_0)
		return m_SkeletonFrame;

	// �X�P���g���t���[���擾
	NUI_SKELETON_FRAME   skeleton;
	if(m_pKinect->NuiSkeletonGetNextFrame(0, &skeleton) != S_OK)
		return m_SkeletonFrame;

	// �m�C�Y����
	m_SkeletonFrame = skeleton;
	::NuiTransformSmooth(&m_SkeletonFrame, NULL);

	// �w�i��������
	if(m_pBGRemovalStream != NULL) {
		SetTrackedSkeleton();
		m_pBGRemovalStream->ProcessSkeleton(NUI_SKELETON_COUNT,
											m_SkeletonFrame.SkeletonData,
											m_SkeletonFrame.liTimeStamp);
	}

	return m_SkeletonFrame;
}

//------------------------------------------------------------------------------
//	�w�i�����X�P���g���ݒ�
//------------------------------------------------------------------------------
void CKinect::SetTrackedSkeleton()
{
	float   closest_skeleton_distance = FLT_MAX;
	DWORD   closest_skeleton          = NUI_SKELETON_INVALID_TRACKING_ID;

	// �X�P���g��ID����(�ŋߐڂ̃X�P���g��)
	for(UINT i = 0; i < NUI_SKELETON_COUNT; i++) {
		if(m_SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED) {
			if(m_TrackedSkeleton == m_SkeletonFrame.SkeletonData[i].dwTrackingID)
				return;

			if(m_SkeletonFrame.SkeletonData[i].Position.z < closest_skeleton_distance) {
				closest_skeleton          = m_SkeletonFrame.SkeletonData[i].dwTrackingID;
				closest_skeleton_distance = m_SkeletonFrame.SkeletonData[i].Position.z;
			}
		}
	}

	// �X�P���g��ID�ݒ�
	if(closest_skeleton != NUI_SKELETON_INVALID_TRACKING_ID) {
		m_pBGRemovalStream->SetTrackedPlayer(closest_skeleton);
		m_TrackedSkeleton = closest_skeleton;
	}

	return;
}

//------------------------------------------------------------------------------
//	�g���b�N���C���f�b�N�X�擾
//------------------------------------------------------------------------------
TRACKED_INDEX CKinect::GetTrackedIndex(NUI_SKELETON_FRAME& inSkeletonFrame)
{
	int    index[2] = {-1, -1};
	UINT   count    = 0;

	// �g���b�N���C���f�b�N�X����
	for(UINT idx = 0; idx < NUI_SKELETON_COUNT; idx++) {
		if(inSkeletonFrame.SkeletonData[idx].eTrackingState == NUI_SKELETON_TRACKED) {
			index[count] = idx;
			count++;
		}
	}

	// �g���b�N���C���f�b�N�X�ݒ�
	TRACKED_INDEX   tracked_index;
	tracked_index.First  = index[0];
	tracked_index.Second = index[1];
	tracked_index.ZFront = tracked_index.First;
	tracked_index.ZBack  = tracked_index.Second;

	if(count >= 2) {
		if(inSkeletonFrame.SkeletonData[tracked_index.First ].Position.z >
		   inSkeletonFrame.SkeletonData[tracked_index.Second].Position.z) {
			tracked_index.ZFront = tracked_index.Second;
			tracked_index.ZBack  = tracked_index.First;
		}
	}

	return tracked_index;
}

//------------------------------------------------------------------------------
//	�ԊO���G�~�b�^�ݒ�
//------------------------------------------------------------------------------
void CKinect::SetForceInfraredEmitter(const BOOL inEnabled)
{
	m_pKinect->NuiSetForceInfraredEmitterOff(!inEnabled);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�ݒ胊�Z�b�g
//------------------------------------------------------------------------------
void CKinect::CameraResetSettings()
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->ResetCameraSettingsToDefault();
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�����I�o�ݒ�
//------------------------------------------------------------------------------
void CKinect::CameraSetAutoExposure(const BOOL inEnabled)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetAutoExposure(inEnabled);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�����z���C�g�o�����X�ݒ�
//------------------------------------------------------------------------------
void CKinect::CameraSetAutoWhiteBalance(const BOOL inEnabled)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetAutoWhiteBalance(inEnabled);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|���邳�ݒ�([0.0 - 1.0] default:0.2156)
//------------------------------------------------------------------------------
void CKinect::CameraSetBrightness(const double inBrightness)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetBrightness(inBrightness);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�R���g���X�g�ݒ�([0.5 - 2.0] default:1.0)
//------------------------------------------------------------------------------
void CKinect::CameraSetContrast(const double inContrast)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetContrast(inContrast);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�Q�C���ݒ�([1.0 - 16.0] default:1.0)
//------------------------------------------------------------------------------
void CKinect::CameraSetGain(const double inGain)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetGain(inGain);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�K���}�ݒ�([1.0 - 2.8] default:2.2)
//------------------------------------------------------------------------------
void CKinect::CameraSetGamma(const double inGamma)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetGamma(inGamma);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�F���ݒ�([+/-22.0] default:0.0)
//------------------------------------------------------------------------------
void CKinect::CameraSetHue(const double inHue)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetHue(inHue);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�ʓx�ݒ�([0.0 - 2.0] default:1.0)
//------------------------------------------------------------------------------
void CKinect::CameraSetSaturation(const double inSaturation)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetSaturation(inSaturation);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�V���[�v�l�X�ݒ�([0.0 - 1.0] default:0.5)
//------------------------------------------------------------------------------
void CKinect::CameraSetSharpness(const double inSharpness)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetSharpness(inSharpness);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�z���C�g�o�����X�ݒ�([2700 - 6500] default:2700)
//------------------------------------------------------------------------------
void CKinect::CameraSetWhiteBalance(const LONG inWhiteBalance)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetWhiteBalance(inWhiteBalance);
}

//------------------------------------------------------------------------------
//	�J���[�J�����|�o�b�N���C�g���[�h�ݒ�(default:AVERAGE_BRIGHTNESS)
//------------------------------------------------------------------------------
void CKinect::CameraSetBacklightMode(const NUI_BACKLIGHT_COMPENSATION_MODE inMode)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetBacklightCompensationMode(inMode);
}
	
//------------------------------------------------------------------------------
//	�w�i�����L��
//------------------------------------------------------------------------------
bool CKinect::BackgroundRemovedEnable()
{
	BackgroundRemovedDisable();

	// BackgroundRemoved�C���^�[�t�F�[�X����
	if(::NuiCreateBackgroundRemovedColorStream(m_pKinect, &m_pBGRemovalStream) != S_OK) {
		::OutputDebugString(TEXT("*** Error - BackgroundRemoved�C���^�[�t�F�[�X�������s(CKinect_BackgroundRemovedEnable\n"));
		return false;
	}

	if(m_ColorResolution == NUI_IMAGE_RESOLUTION_INVALID)
		ColorStreamOpen(NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_COLOR);
	if(m_DepthResolution == NUI_IMAGE_RESOLUTION_INVALID)
		DepthStreamOpen(NUI_IMAGE_RESOLUTION_320x240);
	SkeletonTrackingEnable();

	// �w�i�����L����
	m_hBGRemovalEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_pBGRemovalStream->Enable(m_ColorResolution, m_DepthResolution, m_hBGRemovalEvent) != S_OK) {
		::OutputDebugString(TEXT("*** Error - �w�i�����L�������s(CKinect_BackgroundRemovedEnable\n"));
		BackgroundRemovedDisable();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	�w�i��������
//------------------------------------------------------------------------------
void CKinect::BackgroundRemovedDisable()
{
	if(m_pBGRemovalStream == NULL)
		return;

	m_TrackedSkeleton = NUI_SKELETON_INVALID_TRACKING_ID;

	SafeRelease(m_pBGRemovalTexture);

	m_pBGRemovalStream->Disable();
	m_pBGRemovalStream->Release();
	m_pBGRemovalStream = NULL;

	SafeCloseHandle(m_hBGRemovalEvent);
}

//------------------------------------------------------------------------------
//	�w�i�����t���[���擾
//------------------------------------------------------------------------------
IDirect3DTexture9* CKinect::GetBackgroundRemovedFrame()
{
	// �w�i�����ݒ�
	GetColorFrame();
	GetDepthImage();
	GetSkeletonFrame();

	// �C�x���g�m�F
	if(::WaitForSingleObject(m_hBGRemovalEvent, 0) != WAIT_OBJECT_0)
		return m_pBGRemovalTexture;

	// �t���[���擾
    NUI_BACKGROUND_REMOVED_COLOR_FRAME   bgremoved_frame;
	if(m_pBGRemovalStream->GetNextFrame(0, &bgremoved_frame) != S_OK)
		return m_pBGRemovalTexture;

	// �T�C�Y�擾
	DWORD   frame_width, frame_height;
	::NuiImageResolutionToSize(bgremoved_frame.backgroundRemovedColorFrameResolution,
							   frame_width, frame_height);

	// �e�N�X�`������
	if(m_pBGRemovalTexture == NULL) {
		if(::D3DXCreateTexture(m_pD3DDevice, frame_width, frame_height, 1, 0, D3DFMT_A8R8G8B8,
							   D3DPOOL_MANAGED, &m_pBGRemovalTexture)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - �e�N�X�`���������s(CKinect_GetBackgroundRemovedFrame)\n"));
			return false;
		}
	}

	// �]����e�N�X�`�����b�N
	D3DLOCKED_RECT   texture_lock;
	if(m_pColorTexture->LockRect(0, &texture_lock, NULL, 0) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - �]����e�N�X�`�����b�N���s(CKinect_GetBackgroundRemovedFrame\n"));
		return m_pBGRemovalTexture;
	}

	// �s�N�Z���]��
	DWORD*   pFrame;
	BYTE*    pTexture;
	for(UINT y = 0; y < frame_height; y++) {
		pFrame   = (DWORD*)bgremoved_frame.pBackgroundRemovedColorData + y * frame_width;
		pTexture = ( BYTE*)texture_lock.pBits + y * texture_lock.Pitch;
		::CopyMemory(pTexture, pFrame, frame_width * sizeof(DWORD));
	}

	m_pBGRemovalTexture->UnlockRect(0);
	m_pBGRemovalStream ->ReleaseFrame(&bgremoved_frame);

	return m_pBGRemovalTexture;
}

//------------------------------------------------------------------------------
//	�X�P���g�����W->�J�������W�ϊ�
//------------------------------------------------------------------------------
void CKinect::SkeletonPointToColorPoint(NUI_SKELETON_FRAME& ioSkeletonFrame)
{
	for(UINT id = 0; id < NUI_SKELETON_COUNT; id++) {
		for(UINT pos = 0; pos < NUI_SKELETON_POSITION_COUNT; pos++) {
			NUI_COLOR_IMAGE_POINT   color_pos;
			m_pCoordMapper->MapSkeletonPointToColorPoint(&ioSkeletonFrame.SkeletonData[id].SkeletonPositions[pos],
														 NUI_IMAGE_TYPE_COLOR, m_ColorResolution, &color_pos);
			ioSkeletonFrame.SkeletonData[id].SkeletonPositions[pos].x  = (float)color_pos.x;
			ioSkeletonFrame.SkeletonData[id].SkeletonPositions[pos].y  = (float)color_pos.y;
			ioSkeletonFrame.SkeletonData[id].SkeletonPositions[pos].z /= 4.0f;
		}
	}
}

//------------------------------------------------------------------------------
//	�X�P���g�����W->�J�������W�ϊ�
//------------------------------------------------------------------------------
void CKinect::SkeletonPointToColorPoint(NUI_SKELETON_DATA& ioSkeletonData)
{
	NUI_COLOR_IMAGE_POINT   color_pos;
	for(UINT pos = 0; pos < NUI_SKELETON_POSITION_COUNT; pos++) {
		m_pCoordMapper->MapSkeletonPointToColorPoint(&ioSkeletonData.SkeletonPositions[pos],
													 NUI_IMAGE_TYPE_COLOR, m_ColorResolution, &color_pos);
		ioSkeletonData.SkeletonPositions[pos].x  = (float)color_pos.x;
		ioSkeletonData.SkeletonPositions[pos].y  = (float)color_pos.y;
		ioSkeletonData.SkeletonPositions[pos].z /= 4.0f;
	}
}

//------------------------------------------------------------------------------
//	�X�P���g�����W->�J�������W�ϊ�
//------------------------------------------------------------------------------
D3DXVECTOR3 CKinect::SkeletonPointToColorPoint(_Vector4& inSkeletonPosition)
{
	NUI_COLOR_IMAGE_POINT   color_pos;
	m_pCoordMapper->MapSkeletonPointToColorPoint(&inSkeletonPosition,
												 NUI_IMAGE_TYPE_COLOR, m_ColorResolution, &color_pos);
	D3DXVECTOR3   point;
	point.x = (float)color_pos.x;
	point.y = (float)color_pos.y;
	point.z = inSkeletonPosition.z / 4.0f;

	return point;
}

//------------------------------------------------------------------------------
//	�X�P���g���t���[��->�g�����X�t�H�[���ςݍ��W�ϊ�
//------------------------------------------------------------------------------
UINT CKinect::SkeletonFrameToTLVertex(NUITLVERTEX outVertesies[],
									  NUI_SKELETON_FRAME& inSkeletonFrame,
									  const float inXOffset, const float inYOffset,
									  const float inScale)
{
	UINT    num_vertex = 0;
	for(UINT id = 0; id < NUI_SKELETON_COUNT; id++) {
		if(inSkeletonFrame.SkeletonData[id].eTrackingState == NUI_SKELETON_TRACKED) {
			for(UINT pos = 0; pos < NUI_SKELETON_POSITION_COUNT; pos++) {
				if(inSkeletonFrame.SkeletonData[id].eSkeletonPositionTrackingState[pos]
				   != NUI_SKELETON_POSITION_NOT_TRACKED) {
					// �J�������W�֕ϊ�
					NUI_COLOR_IMAGE_POINT   color_pos;
					m_pCoordMapper->MapSkeletonPointToColorPoint(&inSkeletonFrame.SkeletonData[id].SkeletonPositions[pos],
																 NUI_IMAGE_TYPE_COLOR, m_ColorResolution, &color_pos);

					// �g�����X�t�H�[���ςݍ��W�֕ϊ�
					outVertesies[num_vertex].x     = (float)color_pos.x * inScale + inXOffset;
					outVertesies[num_vertex].y     = (float)color_pos.y * inScale + inYOffset;
					outVertesies[num_vertex].z     = inSkeletonFrame.SkeletonData[id].SkeletonPositions[pos].z / 4.0f;
					outVertesies[num_vertex].rhw   = 1.0f;
					outVertesies[num_vertex].color = m_SkeletonColor[id];

					num_vertex++;
				}	// if
			}	// for(pos)
		}	// if
	}	// for(id)

	return num_vertex;
}

//------------------------------------------------------------------------------
//	�X�P���g���t���[��->�g�����X�t�H�[���ς݃{�[���`���ϊ�
//------------------------------------------------------------------------------
UINT CKinect::SkeletonFrameToTLVBone(NUITLVERTEX outVertesies[],
									 NUI_SKELETON_FRAME& inSkeletonFrame,
									 const float inXOffset, const float inYOffset,
									 const float inScale)
{
	UINT   num_vertex = 0;
	for(UINT id = 0; id < NUI_SKELETON_COUNT; id++) {
		if(inSkeletonFrame.SkeletonData[id].eTrackingState != NUI_SKELETON_TRACKED)
			continue;

		// �X�P���g���t���[��->�J�������W�ϊ�
		struct __vector3 {
			float   x, y, z;
		} skeleton_pos[NUI_SKELETON_POSITION_COUNT];
		for(UINT pos = 0; pos < NUI_SKELETON_POSITION_COUNT; pos++) {
			// �J�������W�֕ϊ�
			NUI_COLOR_IMAGE_POINT   color_pos;
			m_pCoordMapper->MapSkeletonPointToColorPoint(&inSkeletonFrame.SkeletonData[id].SkeletonPositions[pos],
														 NUI_IMAGE_TYPE_COLOR, m_ColorResolution, &color_pos);
			skeleton_pos[pos].x = (float)color_pos.x * inScale + inXOffset;
			skeleton_pos[pos].y = (float)color_pos.y * inScale + inYOffset;
			skeleton_pos[pos].z = inSkeletonFrame.SkeletonData[id].SkeletonPositions[pos].z / 4.0f;
		}

		// �{�[���\���`���֕ϊ�(���C�����X�g)
		UINT   shoulder_center, hip_center;

		// ��-��(����)
		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HEAD].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HEAD].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HEAD].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_CENTER].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_CENTER].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_CENTER].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		shoulder_center                = num_vertex;
		num_vertex++;

		// ��(����)-�w
		outVertesies[num_vertex]       = outVertesies[shoulder_center];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_SPINE].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_SPINE].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_SPINE].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// �w-�K(����)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HIP_CENTER].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HIP_CENTER].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HIP_CENTER].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		hip_center                     = num_vertex;
		num_vertex++;

		// ��(����)-��(��)
		outVertesies[num_vertex]       = outVertesies[shoulder_center];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// ��(��)-�I(��)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// �I(��)-���(��)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// ���(��)-��(��)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HAND_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HAND_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HAND_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// ��(����)-��(�E)
		outVertesies[num_vertex]       = outVertesies[shoulder_center];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// ��(�E)-�I(�E)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// �I(�E)-���(�E)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// ���(�E)-��(�E)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HAND_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HAND_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HAND_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// �K(����)-�K(��)
		outVertesies[num_vertex]       = outVertesies[hip_center];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HIP_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HIP_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HIP_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// �K(��)-�G(��)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// �G(��)-����(��)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// ����(��)-��(��)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// �K(����)-�K(�E)
		outVertesies[num_vertex]       = outVertesies[hip_center];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HIP_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HIP_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HIP_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// �K(�E)-�G(�E)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// �G(�E)-����(�E)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// ����(�E)-��(�E)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;
	}	// for(id)

	return num_vertex >> 1;	// ���Q
}

//------------------------------------------------------------------------------
//	�f�v�X�t���[���擾
//------------------------------------------------------------------------------
DEPTH_DESC CNullKinect::GetDepthFrame()
{
	DEPTH_DESC   depth_desc;
	::ZeroMemory(&depth_desc, sizeof(depth_desc));

	return depth_desc;
}

//------------------------------------------------------------------------------
//	�f�v�X�C���[�W�擾
//------------------------------------------------------------------------------
DEPTH_DESC CNullKinect::GetDepthImage()
{
	DEPTH_DESC   depth_desc;
	::ZeroMemory(&depth_desc, sizeof(depth_desc));

	return depth_desc;
}

//------------------------------------------------------------------------------
//	�X�P���g���t���[���擾
//------------------------------------------------------------------------------
NUI_SKELETON_FRAME CNullKinect::GetSkeletonFrame()
{
	NUI_SKELETON_FRAME   skeleton;
	::ZeroMemory(&skeleton, sizeof(skeleton));

	return skeleton;
}

//------------------------------------------------------------------------------
//	�g���b�N���C���f�b�N�X�擾
//------------------------------------------------------------------------------
TRACKED_INDEX CNullKinect::GetTrackedIndex(NUI_SKELETON_FRAME& inSkeletonFrame)
{
	TRACKED_INDEX   ti;
	ti.First  = -1;
	ti.Second = -1;
	ti.ZFront = -1;
	ti.ZBack  = -1;

	return ti;
}
