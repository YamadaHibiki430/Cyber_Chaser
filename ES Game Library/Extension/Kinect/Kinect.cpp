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
	 Kinect.cpp
				Kinectクラス

 【バージョン】
	 * Version    2015.10.00      2015/08/28  03:10:03

================================================================================
*/

//------------------------------------------------------------------------------
//	インクルードファイル
//------------------------------------------------------------------------------
#include "Kinect.hpp"
#include <cassert>

//------------------------------------------------------------------------------
//	スタティックリンクライブラリ
//------------------------------------------------------------------------------
#pragma comment(lib, "KinectBackgroundRemoval180_32.lib")

//------------------------------------------------------------------------------
//	コンストラクタ
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

	m_SkeletonColor[0] = D3DCOLOR_XRGB(255,   0,   0);	// ID0:赤
	m_SkeletonColor[1] = D3DCOLOR_XRGB(  0, 255,   0);	// ID1:緑
	m_SkeletonColor[2] = D3DCOLOR_XRGB(  0,   0, 255);	// ID2:青
	m_SkeletonColor[3] = D3DCOLOR_XRGB(255, 255,   0);	// ID3:黄
	m_SkeletonColor[4] = D3DCOLOR_XRGB(255,   0, 255);	// ID4:紫
	m_SkeletonColor[5] = D3DCOLOR_XRGB(  0, 255, 255);	// ID5:水色
}

//------------------------------------------------------------------------------
//	デストラクタ
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
//	カラーストリームオープン
//------------------------------------------------------------------------------
bool CKinect::ColorStreamOpen(const NUI_IMAGE_RESOLUTION inResolution,
							  const NUI_IMAGE inImageType)
{
	if(m_ColorResolution != NUI_IMAGE_RESOLUTION_INVALID) {
		::OutputDebugString(TEXT("*** Error - カラーストリーム既オープン(CKinect_ColorStreamOpen)\n"));
		return false;
	}

	// ストリームオープン
	m_hColorEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_pKinect->NuiImageStreamOpen((NUI_IMAGE_TYPE)inImageType, inResolution,
									 0, 2, m_hColorEvent, &m_hColorStream)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - カラーストリームオープン失敗(CKinect_ColorStreamOpen)\n"));
		return false;
	}
	m_ColorResolution = inResolution;

	// コーディネートマッパー取得
	if(m_pCoordMapper == NULL)
		m_pKinect->NuiGetCoordinateMapper(&m_pCoordMapper);

	// カラーカメラセッティング取得
	m_pKinect->NuiGetColorCameraSettings(&m_pCameraSettings);

	// サイズ取得
	DWORD   width, height;
	::NuiImageResolutionToSize(inResolution, width, height);

	// テクスチャ生成
	if(::D3DXCreateTexture(m_pD3DDevice, width, height, 1, 0, D3DFMT_X8R8G8B8,
						   D3DPOOL_MANAGED, &m_pColorTexture)
	   != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CKinect_ColorStreamOpen)\n"));
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	デプスストリームオープン
//------------------------------------------------------------------------------
bool CKinect::DepthStreamOpen(const NUI_IMAGE_RESOLUTION inResolution)
{
	if(m_DepthResolution != NUI_IMAGE_RESOLUTION_INVALID) {
		::OutputDebugString(TEXT("*** Error - デプスストリーム既オープン(CKinect_DepthStreamOpen)\n"));
		return false;
	}

	// ストリームオープン
	m_hDepthEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_pKinect->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, inResolution,
									 0, 2, m_hDepthEvent, &m_hDepthStream)
	   != S_OK) {
		::OutputDebugString(TEXT("*** Error - デプスストリームオープン失敗(CKinect_DepthStreamOpen)\n"));
		return false;
	}
	m_DepthResolution = inResolution;

	return true;
}

//------------------------------------------------------------------------------
//	スケルトントラッキング有効化
//------------------------------------------------------------------------------
bool CKinect::SkeletonTrackingEnable()
{
	m_hSkeletonEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_pKinect->NuiSkeletonTrackingEnable(m_hSkeletonEvent, 0) != S_OK) {
		::OutputDebugString(TEXT("*** Error - スケルトントラッキング有効化失敗(CKinect_SkeletonTrackingEnable)\n"));
		return false;
	}

	// コーディネートマッパー取得
	if(m_pCoordMapper == NULL)
		m_pKinect->NuiGetCoordinateMapper(&m_pCoordMapper);

	return true;
}

//------------------------------------------------------------------------------
//	カラーフレーム取得
//------------------------------------------------------------------------------
IDirect3DTexture9* CKinect::GetColorFrame()
{
	// イベント確認
	if(::WaitForSingleObject(m_hColorEvent, 0) != WAIT_OBJECT_0)
		return m_pColorTexture;

	// カラーフレーム取得
    NUI_IMAGE_FRAME   image_frame;
	if(m_pKinect->NuiImageStreamGetNextFrame(m_hColorStream, 0, &image_frame) != S_OK)
		return m_pColorTexture;

	// フレームテクスチャ情報取得
	NUI_SURFACE_DESC   surface_desc;
	image_frame.pFrameTexture->GetLevelDesc(0, &surface_desc);

	// フレームテクスチャロック
	NUI_LOCKED_RECT    frame_lock;
	if(image_frame.pFrameTexture->LockRect(0, &frame_lock, NULL, 0) != S_OK) {
		::OutputDebugString(TEXT("*** Error - フレームテクスチャロック失敗(CKinect_GetColorFrame)\n"));
		return m_pColorTexture;
	}

	// 転送先テクスチャロック
	D3DLOCKED_RECT   texture_lock;
	if(m_pColorTexture->LockRect(0, &texture_lock, NULL, 0) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 転送先テクスチャロック失敗(CKinect_GetColorFrame\n"));
		image_frame.pFrameTexture->UnlockRect(0);
		return m_pColorTexture;
	}

	// ピクセル転送
	if(image_frame.eImageType == NUI_IMAGE_TYPE_COLOR) {
		// カラーイメージ
		DWORD*   pFrame;
		DWORD*   pTexture;
		for(UINT y = 0; y < surface_desc.Height; y++) {
			pFrame   = (DWORD*)((BYTE*)  frame_lock.pBits + y *   frame_lock.Pitch);
			pTexture = (DWORD*)((BYTE*)texture_lock.pBits + y * texture_lock.Pitch);

			::CopyMemory(pTexture, pFrame, frame_lock.Pitch);
		}
	} else if(image_frame.eImageType == NUI_IMAGE_TYPE_COLOR_INFRARED) {
		// 赤外線イメージ
		 WORD*   pFrame;
		DWORD*   pTexture;
		for(UINT y = 0; y < surface_desc.Height; y++) {
			pFrame   = ( WORD*)((BYTE*)  frame_lock.pBits + y *   frame_lock.Pitch);
			pTexture = (DWORD*)((BYTE*)texture_lock.pBits + y * texture_lock.Pitch);

			for(UINT x = 0; x < surface_desc.Width; x++) {
				const DWORD   COLOR = *pFrame >> 8;		// 16ビットから8ビットへ減色
				*pTexture = COLOR | COLOR << 8 | COLOR << 16 | 0xff000000;
				pFrame  ++;
				pTexture++;
			}
		}
	} else if(image_frame.eImageType == NUI_IMAGE_TYPE_COLOR_RAW_BAYER) {
		// RawBayerイメージ
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

	// 背景除去処理
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
//	デプスフレーム取得
//------------------------------------------------------------------------------
DEPTH_DESC CKinect::GetDepthFrame()
{
	// デプスフレーム取得
	HRESULT           result = S_FALSE;
    NUI_IMAGE_FRAME   image_frame;
	if(::WaitForSingleObject(m_hDepthEvent, 0) == WAIT_OBJECT_0)
		result = m_pKinect->NuiImageStreamGetNextFrame(m_hDepthStream, 0, &image_frame);

	if(result == S_OK) {
		// 旧デプスフレーム解放
		SafeRelease(m_pDepthDataTexture);
		m_pKinect->NuiImageStreamReleaseFrame(m_hDepthStream, &m_DepthFrame);

		// デプスフレーム保存
		m_DepthFrame = image_frame;
	} else {
		if(m_DepthFrame.pFrameTexture == NULL) {
			// デプスフレーム未取得
			DEPTH_DESC   desc;
			::ZeroMemory(&desc, sizeof(desc));
			return desc;
		}
	}

	// テクスチャ情報取得
	NUI_SURFACE_DESC   surface_desc;
	m_DepthFrame.pFrameTexture->GetLevelDesc(0, &surface_desc);

	// デプス情報設定
	DEPTH_DESC   depth_desc;
	depth_desc.pFrameTexture = m_DepthFrame.pFrameTexture;
	depth_desc.Pitch         = m_DepthFrame.pFrameTexture->Pitch();
	depth_desc.Width         = surface_desc.Width;
	depth_desc.Height        = surface_desc.Height;
	depth_desc.BytesPerPixel = sizeof(USHORT);

	return depth_desc;
}

//------------------------------------------------------------------------------
//	デプスイメージ取得
//------------------------------------------------------------------------------
DEPTH_DESC CKinect::GetDepthImage()
{
	// デプスフレーム取得
	HRESULT           result = S_FALSE;
	NUI_IMAGE_FRAME   image_frame;
	if(::WaitForSingleObject(m_hDepthEvent, 0) == WAIT_OBJECT_0)
		result = m_pKinect->NuiImageStreamGetNextFrame(m_hDepthStream, 0, &image_frame);

	if(result == S_OK) {
		// 旧デプスフレーム解放
		SafeRelease(m_pDepthDataTexture);
		m_pKinect->NuiImageStreamReleaseFrame(m_hDepthStream, &m_DepthFrame);

		// デプスフレーム保存
		m_DepthFrame = image_frame;
	} else {
		if(m_pDepthDataTexture != NULL) {
			// デプスデータテクスチャ取得済み
			// テクスチャ情報取得
			NUI_SURFACE_DESC   surface_desc;
			m_pDepthDataTexture->GetLevelDesc(0, &surface_desc);

			// デプス情報設定
			DEPTH_DESC   depth_desc;
			depth_desc.pFrameTexture = m_pDepthDataTexture;
			depth_desc.Pitch         = m_pDepthDataTexture->Pitch();
			depth_desc.Width         = surface_desc.Width;
			depth_desc.Height        = surface_desc.Height;
			depth_desc.BytesPerPixel = sizeof(NUI_DEPTH_IMAGE_PIXEL);

			return depth_desc;
		}
	}

	// デプステクスチャ取得
	BOOL   near_mode = FALSE;
	if(m_pKinect->NuiImageFrameGetDepthImagePixelFrameTexture(m_hDepthStream, &image_frame, 
															  &near_mode, &m_pDepthDataTexture) != S_OK) {
		::OutputDebugString(TEXT("*** Error - デプステクスチャ取得失敗(CKinect_GetDepthImagePixel\n"));

		DEPTH_DESC   desc;
		::ZeroMemory(&desc, sizeof(desc));
		return desc;
	}

	// テクスチャ情報取得
	NUI_SURFACE_DESC   surface_desc;
	m_pDepthDataTexture->GetLevelDesc(0, &surface_desc);

	// デプス情報設定
	DEPTH_DESC   depth_desc;
	depth_desc.pFrameTexture = m_pDepthDataTexture;
	depth_desc.Pitch         = m_pDepthDataTexture->Pitch();
	depth_desc.Width         = surface_desc.Width;
	depth_desc.Height        = surface_desc.Height;
	depth_desc.BytesPerPixel = sizeof(NUI_DEPTH_IMAGE_PIXEL);

	// 背景除去処理
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
//	スケルトンフレーム取得
//------------------------------------------------------------------------------
NUI_SKELETON_FRAME CKinect::GetSkeletonFrame()
{
	// イベント確認
	if(::WaitForSingleObject(m_hSkeletonEvent, 0) != WAIT_OBJECT_0)
		return m_SkeletonFrame;

	// スケルトンフレーム取得
	NUI_SKELETON_FRAME   skeleton;
	if(m_pKinect->NuiSkeletonGetNextFrame(0, &skeleton) != S_OK)
		return m_SkeletonFrame;

	// ノイズ除去
	m_SkeletonFrame = skeleton;
	::NuiTransformSmooth(&m_SkeletonFrame, NULL);

	// 背景除去処理
	if(m_pBGRemovalStream != NULL) {
		SetTrackedSkeleton();
		m_pBGRemovalStream->ProcessSkeleton(NUI_SKELETON_COUNT,
											m_SkeletonFrame.SkeletonData,
											m_SkeletonFrame.liTimeStamp);
	}

	return m_SkeletonFrame;
}

//------------------------------------------------------------------------------
//	背景除去スケルトン設定
//------------------------------------------------------------------------------
void CKinect::SetTrackedSkeleton()
{
	float   closest_skeleton_distance = FLT_MAX;
	DWORD   closest_skeleton          = NUI_SKELETON_INVALID_TRACKING_ID;

	// スケルトンID検索(最近接のスケルトン)
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

	// スケルトンID設定
	if(closest_skeleton != NUI_SKELETON_INVALID_TRACKING_ID) {
		m_pBGRemovalStream->SetTrackedPlayer(closest_skeleton);
		m_TrackedSkeleton = closest_skeleton;
	}

	return;
}

//------------------------------------------------------------------------------
//	トラック中インデックス取得
//------------------------------------------------------------------------------
TRACKED_INDEX CKinect::GetTrackedIndex(NUI_SKELETON_FRAME& inSkeletonFrame)
{
	int    index[2] = {-1, -1};
	UINT   count    = 0;

	// トラック中インデックス検索
	for(UINT idx = 0; idx < NUI_SKELETON_COUNT; idx++) {
		if(inSkeletonFrame.SkeletonData[idx].eTrackingState == NUI_SKELETON_TRACKED) {
			index[count] = idx;
			count++;
		}
	}

	// トラック中インデックス設定
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
//	赤外線エミッタ設定
//------------------------------------------------------------------------------
void CKinect::SetForceInfraredEmitter(const BOOL inEnabled)
{
	m_pKinect->NuiSetForceInfraredEmitterOff(!inEnabled);
}

//------------------------------------------------------------------------------
//	カラーカメラ－設定リセット
//------------------------------------------------------------------------------
void CKinect::CameraResetSettings()
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->ResetCameraSettingsToDefault();
}

//------------------------------------------------------------------------------
//	カラーカメラ－自動露出設定
//------------------------------------------------------------------------------
void CKinect::CameraSetAutoExposure(const BOOL inEnabled)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetAutoExposure(inEnabled);
}

//------------------------------------------------------------------------------
//	カラーカメラ－自動ホワイトバランス設定
//------------------------------------------------------------------------------
void CKinect::CameraSetAutoWhiteBalance(const BOOL inEnabled)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetAutoWhiteBalance(inEnabled);
}

//------------------------------------------------------------------------------
//	カラーカメラ－明るさ設定([0.0 - 1.0] default:0.2156)
//------------------------------------------------------------------------------
void CKinect::CameraSetBrightness(const double inBrightness)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetBrightness(inBrightness);
}

//------------------------------------------------------------------------------
//	カラーカメラ－コントラスト設定([0.5 - 2.0] default:1.0)
//------------------------------------------------------------------------------
void CKinect::CameraSetContrast(const double inContrast)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetContrast(inContrast);
}

//------------------------------------------------------------------------------
//	カラーカメラ－ゲイン設定([1.0 - 16.0] default:1.0)
//------------------------------------------------------------------------------
void CKinect::CameraSetGain(const double inGain)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetGain(inGain);
}

//------------------------------------------------------------------------------
//	カラーカメラ－ガンマ設定([1.0 - 2.8] default:2.2)
//------------------------------------------------------------------------------
void CKinect::CameraSetGamma(const double inGamma)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetGamma(inGamma);
}

//------------------------------------------------------------------------------
//	カラーカメラ－色相設定([+/-22.0] default:0.0)
//------------------------------------------------------------------------------
void CKinect::CameraSetHue(const double inHue)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetHue(inHue);
}

//------------------------------------------------------------------------------
//	カラーカメラ－彩度設定([0.0 - 2.0] default:1.0)
//------------------------------------------------------------------------------
void CKinect::CameraSetSaturation(const double inSaturation)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetSaturation(inSaturation);
}

//------------------------------------------------------------------------------
//	カラーカメラ－シャープネス設定([0.0 - 1.0] default:0.5)
//------------------------------------------------------------------------------
void CKinect::CameraSetSharpness(const double inSharpness)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetSharpness(inSharpness);
}

//------------------------------------------------------------------------------
//	カラーカメラ－ホワイトバランス設定([2700 - 6500] default:2700)
//------------------------------------------------------------------------------
void CKinect::CameraSetWhiteBalance(const LONG inWhiteBalance)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetWhiteBalance(inWhiteBalance);
}

//------------------------------------------------------------------------------
//	カラーカメラ－バックライトモード設定(default:AVERAGE_BRIGHTNESS)
//------------------------------------------------------------------------------
void CKinect::CameraSetBacklightMode(const NUI_BACKLIGHT_COMPENSATION_MODE inMode)
{
	if(m_pCameraSettings == NULL)
		return;
	m_pCameraSettings->SetBacklightCompensationMode(inMode);
}
	
//------------------------------------------------------------------------------
//	背景除去有効
//------------------------------------------------------------------------------
bool CKinect::BackgroundRemovedEnable()
{
	BackgroundRemovedDisable();

	// BackgroundRemovedインターフェース生成
	if(::NuiCreateBackgroundRemovedColorStream(m_pKinect, &m_pBGRemovalStream) != S_OK) {
		::OutputDebugString(TEXT("*** Error - BackgroundRemovedインターフェース生成失敗(CKinect_BackgroundRemovedEnable\n"));
		return false;
	}

	if(m_ColorResolution == NUI_IMAGE_RESOLUTION_INVALID)
		ColorStreamOpen(NUI_IMAGE_RESOLUTION_640x480, NUI_IMAGE_COLOR);
	if(m_DepthResolution == NUI_IMAGE_RESOLUTION_INVALID)
		DepthStreamOpen(NUI_IMAGE_RESOLUTION_320x240);
	SkeletonTrackingEnable();

	// 背景除去有効化
	m_hBGRemovalEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(m_pBGRemovalStream->Enable(m_ColorResolution, m_DepthResolution, m_hBGRemovalEvent) != S_OK) {
		::OutputDebugString(TEXT("*** Error - 背景除去有効化失敗(CKinect_BackgroundRemovedEnable\n"));
		BackgroundRemovedDisable();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
//	背景除去無効
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
//	背景除去フレーム取得
//------------------------------------------------------------------------------
IDirect3DTexture9* CKinect::GetBackgroundRemovedFrame()
{
	// 背景除去設定
	GetColorFrame();
	GetDepthImage();
	GetSkeletonFrame();

	// イベント確認
	if(::WaitForSingleObject(m_hBGRemovalEvent, 0) != WAIT_OBJECT_0)
		return m_pBGRemovalTexture;

	// フレーム取得
    NUI_BACKGROUND_REMOVED_COLOR_FRAME   bgremoved_frame;
	if(m_pBGRemovalStream->GetNextFrame(0, &bgremoved_frame) != S_OK)
		return m_pBGRemovalTexture;

	// サイズ取得
	DWORD   frame_width, frame_height;
	::NuiImageResolutionToSize(bgremoved_frame.backgroundRemovedColorFrameResolution,
							   frame_width, frame_height);

	// テクスチャ生成
	if(m_pBGRemovalTexture == NULL) {
		if(::D3DXCreateTexture(m_pD3DDevice, frame_width, frame_height, 1, 0, D3DFMT_A8R8G8B8,
							   D3DPOOL_MANAGED, &m_pBGRemovalTexture)
		   != D3D_OK) {
			::OutputDebugString(TEXT("*** Error - テクスチャ生成失敗(CKinect_GetBackgroundRemovedFrame)\n"));
			return false;
		}
	}

	// 転送先テクスチャロック
	D3DLOCKED_RECT   texture_lock;
	if(m_pColorTexture->LockRect(0, &texture_lock, NULL, 0) != D3D_OK) {
		::OutputDebugString(TEXT("*** Error - 転送先テクスチャロック失敗(CKinect_GetBackgroundRemovedFrame\n"));
		return m_pBGRemovalTexture;
	}

	// ピクセル転送
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
//	スケルトン座標->カメラ座標変換
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
//	スケルトン座標->カメラ座標変換
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
//	スケルトン座標->カメラ座標変換
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
//	スケルトンフレーム->トランスフォーム済み座標変換
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
					// カメラ座標へ変換
					NUI_COLOR_IMAGE_POINT   color_pos;
					m_pCoordMapper->MapSkeletonPointToColorPoint(&inSkeletonFrame.SkeletonData[id].SkeletonPositions[pos],
																 NUI_IMAGE_TYPE_COLOR, m_ColorResolution, &color_pos);

					// トランスフォーム済み座標へ変換
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
//	スケルトンフレーム->トランスフォーム済みボーン形式変換
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

		// スケルトンフレーム->カメラ座標変換
		struct __vector3 {
			float   x, y, z;
		} skeleton_pos[NUI_SKELETON_POSITION_COUNT];
		for(UINT pos = 0; pos < NUI_SKELETON_POSITION_COUNT; pos++) {
			// カメラ座標へ変換
			NUI_COLOR_IMAGE_POINT   color_pos;
			m_pCoordMapper->MapSkeletonPointToColorPoint(&inSkeletonFrame.SkeletonData[id].SkeletonPositions[pos],
														 NUI_IMAGE_TYPE_COLOR, m_ColorResolution, &color_pos);
			skeleton_pos[pos].x = (float)color_pos.x * inScale + inXOffset;
			skeleton_pos[pos].y = (float)color_pos.y * inScale + inYOffset;
			skeleton_pos[pos].z = inSkeletonFrame.SkeletonData[id].SkeletonPositions[pos].z / 4.0f;
		}

		// ボーン表示形式へ変換(ラインリスト)
		UINT   shoulder_center, hip_center;

		// 頭-肩(中央)
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

		// 肩(中央)-背
		outVertesies[num_vertex]       = outVertesies[shoulder_center];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_SPINE].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_SPINE].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_SPINE].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 背-尻(中央)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HIP_CENTER].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HIP_CENTER].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HIP_CENTER].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		hip_center                     = num_vertex;
		num_vertex++;

		// 肩(中央)-肩(左)
		outVertesies[num_vertex]       = outVertesies[shoulder_center];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 肩(左)-肘(左)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 肘(左)-手首(左)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 手首(左)-手(左)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HAND_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HAND_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HAND_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 肩(中央)-肩(右)
		outVertesies[num_vertex]       = outVertesies[shoulder_center];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_SHOULDER_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 肩(右)-肘(右)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_ELBOW_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 肘(右)-手首(右)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_WRIST_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 手首(右)-手(右)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HAND_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HAND_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HAND_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 尻(中央)-尻(左)
		outVertesies[num_vertex]       = outVertesies[hip_center];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HIP_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HIP_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HIP_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 尻(左)-膝(左)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 膝(左)-足首(左)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 足首(左)-足(左)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_LEFT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_LEFT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_LEFT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 尻(中央)-尻(右)
		outVertesies[num_vertex]       = outVertesies[hip_center];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_HIP_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_HIP_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_HIP_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 尻(右)-膝(右)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_KNEE_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 膝(右)-足首(右)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_ANKLE_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;

		// 足首(右)-足(右)
		outVertesies[num_vertex]       = outVertesies[num_vertex - 1];
		num_vertex++;

		outVertesies[num_vertex].x     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_RIGHT].x;
		outVertesies[num_vertex].y     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_RIGHT].y;
		outVertesies[num_vertex].z     = skeleton_pos[NUI_SKELETON_POSITION_FOOT_RIGHT].z;
		outVertesies[num_vertex].rhw   = 1.0f;
		outVertesies[num_vertex].color = m_SkeletonColor[id];
		num_vertex++;
	}	// for(id)

	return num_vertex >> 1;	// ÷２
}

//------------------------------------------------------------------------------
//	デプスフレーム取得
//------------------------------------------------------------------------------
DEPTH_DESC CNullKinect::GetDepthFrame()
{
	DEPTH_DESC   depth_desc;
	::ZeroMemory(&depth_desc, sizeof(depth_desc));

	return depth_desc;
}

//------------------------------------------------------------------------------
//	デプスイメージ取得
//------------------------------------------------------------------------------
DEPTH_DESC CNullKinect::GetDepthImage()
{
	DEPTH_DESC   depth_desc;
	::ZeroMemory(&depth_desc, sizeof(depth_desc));

	return depth_desc;
}

//------------------------------------------------------------------------------
//	スケルトンフレーム取得
//------------------------------------------------------------------------------
NUI_SKELETON_FRAME CNullKinect::GetSkeletonFrame()
{
	NUI_SKELETON_FRAME   skeleton;
	::ZeroMemory(&skeleton, sizeof(skeleton));

	return skeleton;
}

//------------------------------------------------------------------------------
//	トラック中インデックス取得
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
