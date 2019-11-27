/*
 *  IContextData.h
 *  
 *  ImmutableのDrawData、BaseDataの描画用の�??タを保持する�?
 *  
 *  Created by Live2D on 11/04/29.
 *  Copyright 2011 Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __IBASE_CONTEXT__
#define __IBASE_CONTEXT__


#ifndef __SKIP_DOC__

#include "../memory/LDObject.h"


//------------ LIVE2D NAMESPACE ------------
namespace live2d
{ 
	class IBaseData ;
		
	class IBaseContext : public live2d::LDObject 
	{
	public:
		IBaseContext(IBaseData* src) ;
		virtual ~IBaseContext(){}

		void* getSrcPtr(){ return this->srcPtr ; } 
		
		void setPartsIndex( int p ){ this->partsIndex = p ; }
		int getPartsIndex() { return this->partsIndex ; }

		bool isOutsideParam() {	return this->outsideParam;	}
		void setOutsideParam( bool outsideParam ) {	this->outsideParam = outsideParam ;	}

		bool isAvailable()
		{
			return available && ! outsideParam ;
		}

		void setAvailable(bool available)
		{
			this->available = available;
		}

		/*
		 * 基準面から自身までの倍率の総乗を返す�?
		 * BDAffineの場合�?、�?身のスケールと親までのスケールの積にな�?
		 */
		float getTotalScale()
		{
			return totalScale ;
		}
	
		void setTotalScale_notForClient(float totalScale)
		{
			this->totalScale = totalScale ;
		}

		float getInterpolatedOpacity(){	return interpolatedOpacity ;}

		void setInterpolatedOpacity(float interpolatedOpacity){this->interpolatedOpacity = interpolatedOpacity ;}

		float getTotalOpacity(){return totalOpacity ;}
	
		void setTotalOpacity(float totalOpacity){this->totalOpacity = totalOpacity ;}


	private:
		IBaseData* srcPtr ;
		int partsIndex ;
		
		bool outsideParam ;
		bool available ;

		
	protected:
		// 基準面から自身までの倍率の総�?
		// BDAffineの scaleX,YはXYの意味を適�?��使えな�??で scaleX のみ用�?���?
		// scaleXと親のtotalScaleを掛けたも�?が�?身のtotalScaleとな�?
		// BDBoxGridのように全て点で変換を定義する場合�?、�?�??意味を持たな�??
		// BDAffine親�?BDAffine子�?場合に倍率を反�?��るために用�?��
		float		totalScale;

		// �?��ォーマ�?透�?度(SDK 2.0以�?
		float		interpolatedOpacity;		// 不�?明度の補間結果
		float		totalOpacity;				// 基準面から自身までの不�?明度の累積（�?身を含�?�?

	};
}
//------------ LIVE2D NAMESPACE ------------

#endif // __SKIP_DOC__

#endif // __IBASE_CONTEXT__
