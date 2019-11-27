
#ifndef	__EFFEKSEERRENDERER_DX11_BASE_PRE_H__
#define	__EFFEKSEERRENDERER_DX11_BASE_PRE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

#include <windows.h>
#include <d3d11.h>

#if _WIN32
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "d3d11.lib" )
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class Renderer;

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_DX11_PRE_BASE_H__

#ifndef	__EFFEKSEERRENDERER_RENDERER_H__
#define	__EFFEKSEERRENDERER_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <Effekseer.h>

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRenderer
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------

/**
	@brief	�w�i��c�܂���G�t�F�N�g��`�悷��O�Ɏ��s�����R�[���o�b�N
*/
class DistortingCallback
{
public:
	DistortingCallback() {}
	virtual ~DistortingCallback() {}

	virtual void OnDistorting() {}
};
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
class Renderer
{
protected:
	Renderer() {}
	virtual ~Renderer() {}

public:
	/**
		@brief	�f�o�C�X���X�g�������������Ɏ��s����B
	*/
	virtual void OnLostDevice() = 0;

	/**
		@brief	�f�o�C�X�����Z�b�g���ꂽ���Ɏ��s����B
	*/
	virtual void OnResetDevice() = 0;

	/**
		@brief	���̃C���X�^���X��j������B
	*/
	virtual void Destory() = 0;

	/**
		@brief	�X�e�[�g�𕜋A���邩�ǂ����̃t���O��ݒ肷��B
	*/
	virtual void SetRestorationOfStatesFlag(bool flag) = 0;

	/**
		@brief	�`����J�n���鎞�Ɏ��s����B
	*/
	virtual bool BeginRendering() = 0;

	/**
		@brief	�`����I�����鎞�Ɏ��s����B
	*/
	virtual bool EndRendering() = 0;

	/**
		@brief	���C�g�̕������擾����B
	*/
	virtual const ::Effekseer::Vector3D& GetLightDirection() const = 0;

	/**
		@brief	���C�g�̕�����ݒ肷��B
	*/
	virtual void SetLightDirection( ::Effekseer::Vector3D& direction ) = 0;

	/**
		@brief	���C�g�̐F���擾����B
	*/
	virtual const ::Effekseer::Color& GetLightColor() const = 0;

	/**
		@brief	���C�g�̐F��ݒ肷��B
	*/
	virtual void SetLightColor( ::Effekseer::Color& color ) = 0;

	/**
		@brief	���C�g�̊����̐F���擾����B
	*/
	virtual const ::Effekseer::Color& GetLightAmbientColor() const = 0;

	/**
		@brief	���C�g�̊����̐F��ݒ肷��B
	*/
	virtual void SetLightAmbientColor( ::Effekseer::Color& color ) = 0;

		/**
		@brief	�ő�`��X�v���C�g�����擾����B
	*/
	virtual int32_t GetSquareMaxCount() const = 0;

	/**
		@brief	���e�s����擾����B
	*/
	virtual const ::Effekseer::Matrix44& GetProjectionMatrix() const = 0;

	/**
		@brief	���e�s���ݒ肷��B
	*/
	virtual void SetProjectionMatrix( const ::Effekseer::Matrix44& mat ) = 0;

	/**
		@brief	�J�����s����擾����B
	*/
	virtual const ::Effekseer::Matrix44& GetCameraMatrix() const = 0;

	/**
		@brief	�J�����s���ݒ肷��B
	*/
	virtual void SetCameraMatrix( const ::Effekseer::Matrix44& mat ) = 0;

	/**
		@brief	�J�����v���W�F�N�V�����s����擾����B
	*/
	virtual ::Effekseer::Matrix44& GetCameraProjectionMatrix() = 0;

	/**
		@brief	�X�v���C�g�����_���[�𐶐�����B
	*/
	virtual ::Effekseer::SpriteRenderer* CreateSpriteRenderer() = 0;

	/**
		@brief	���{�������_���[�𐶐�����B
	*/
	virtual ::Effekseer::RibbonRenderer* CreateRibbonRenderer() = 0;

	/**
		@brief	�����O�����_���[�𐶐�����B
	*/
	virtual ::Effekseer::RingRenderer* CreateRingRenderer() = 0;

	/**
		@brief	���f�������_���[�𐶐�����B
	*/
	virtual ::Effekseer::ModelRenderer* CreateModelRenderer() = 0;

	/**
		@brief	�O�Ճ����_���[�𐶐�����B
	*/
	virtual ::Effekseer::TrackRenderer* CreateTrackRenderer() = 0;

	/**
		@brief	�W���̃e�N�X�`���Ǎ��N���X�𐶐�����B
	*/
	virtual ::Effekseer::TextureLoader* CreateTextureLoader( ::Effekseer::FileInterface* fileInterface = NULL ) = 0;

	/**
		@brief	�W���̃��f���Ǎ��N���X�𐶐�����B
	*/
	virtual ::Effekseer::ModelLoader* CreateModelLoader( ::Effekseer::FileInterface* fileInterface = NULL ) = 0;

	/**
		@brief	�����_�[�X�e�[�g�������I�Ƀ��Z�b�g����B
	*/
	virtual void ResetRenderState() = 0;

	/**
	@brief	�w�i��c�܂���G�t�F�N�g���`�悳���O�ɌĂ΂��R�[���o�b�N���擾����B
	*/
	virtual DistortingCallback* GetDistortingCallback() = 0;

	/**
	@brief	�w�i��c�܂���G�t�F�N�g���`�悳���O�ɌĂ΂��R�[���o�b�N��ݒ肷��B
	*/
	virtual void SetDistortingCallback(DistortingCallback* callback) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_RENDERER_H__
#ifndef	__EFFEKSEERRENDERER_DX11_RENDERER_H__
#define	__EFFEKSEERRENDERER_DX11_RENDERER_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererDX11
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	�`��N���X
*/
class Renderer
	: public ::EffekseerRenderer::Renderer
{
protected:
	Renderer() {}
	virtual ~Renderer() {}

public:
	/**
		@brief	�C���X�^���X�𐶐�����B
		@param	device	[in]	DirectX�̃f�o�C�X
		@param	squareMaxCount	[in]	�ő�`��X�v���C�g��
		@return	�C���X�^���X
	*/
	static Renderer* Create( ID3D11Device* device, ID3D11DeviceContext* context, int32_t squareMaxCount );

	/**
		@brief	�f�o�C�X���擾����B
	*/
	virtual ID3D11Device* GetDevice() = 0;

	/**
	@brief	�w�i���擾����B
	*/
	virtual ID3D11ShaderResourceView* GetBackground() = 0;

	/**
	@brief	�w�i��ݒ肷��B
	*/
	virtual void SetBackground(ID3D11ShaderResourceView* background) = 0;
};

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
/**
	@brief	���f��
*/
class Model : public Effekseer::Model
{
private:

public:
	ID3D11Buffer*		VertexBuffer;
	ID3D11Buffer*		IndexBuffer;
	int32_t				VertexCount;
	int32_t				IndexCount;
	int32_t				FaceCount;
	int32_t				ModelCount;

	Model( uint8_t* data, int32_t size )
		: Effekseer::Model	( data, size )
		, VertexBuffer	( NULL )
		, IndexBuffer	( NULL )
		, VertexCount		( 0 )
		, IndexCount		( 0 )
		, FaceCount			( 0 )
		, ModelCount		( 0 )
	{
	}

	virtual ~Model()
	{
		ES_SAFE_RELEASE( VertexBuffer );
		ES_SAFE_RELEASE( IndexBuffer );
	}
};


//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif	// __EFFEKSEERRENDERER_DX11_RENDERER_H__