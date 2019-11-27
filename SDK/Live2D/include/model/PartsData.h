/**
 *  PartsData.h
 *
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_PARTS_DATA_H__
#define __LIVE2D_PARTS_DATA_H__


#include "../Live2D.h"

#include "../io/ISerializableV2.h"


//------------ LIVE2D NAMESPACE ------------
namespace live2d
{ 
	class BReader ;
	class IBaseData ;
	class IDrawData ;
	class PartsDataID ;
	class ModelContext ;
	class PartsDataContext ;
		
	
	class PartsData : public ISerializableV2
	{
	public:
		static int INSTANCE_COUNT ;
	
	public:
		PartsData();
		virtual ~PartsData(void);
		
	public:
		// Initialize
		void initDirect(MemoryParam* memParam) ;// シリアライズせずにメモリを確保する�?合�?こちらを呼び出�?
		PartsDataContext* init(ModelContext &mdc) ;
		
	
		
		bool isVisible(){ return visible ; }
	
	
		
		bool isLocked(){ return locked ;}
	
	
		
		void setVisible( bool v ){ visible = v ; }
	
	
		
		void setLocked( bool v ){ locked = v ; }
		
		
		LDVector<IBaseData*> * getBaseData(){ return baseDataList ; }

		
		LDVector<IDrawData*> * getDrawData(){ return drawDataList ; }
	
		
		void setBaseData( LDVector<IBaseData*>* baseDataList ) ;
	
		
		void setDrawData( LDVector<IDrawData*>* drawDataList ) ;
	
		
		
		PartsDataID * getPartsDataID(){ return partsID ; }

		
		void setPartsDataID(PartsDataID *id){ partsID = id ; }
	
		
		PartsDataID * getPartsID(){ return partsID ; }
	
		
		void setPartsID(PartsDataID *id){ partsID = id ; }
		
		
		void addBaseData( IBaseData * baseData ) ;// baseDataのポインタは、PartsDataが�?�?���?

		void addDrawData( IDrawData * drawData ) ;// drawDataのポインタは、PartsDataが�?�?���?
		
		
		virtual void readV2(BReader & br , MemoryParam* memParam ) ;
	
		
#if L2D_SAMPLE
		static PartsData * CREATE_SAMPLE( PartsDataID * id ) ;
#endif
	
		
	private:
		// Prevention of copy Constructor
		PartsData( const PartsData & ) ;			
		PartsData& operator=(const PartsData &) ;	
	
		
	private:
		bool					visible ;			// = true ;
		bool					locked  ;			// = false ;
		PartsDataID *			partsID ;			// shared(L2D_DELETE不�?
		
		LDVector<IBaseData*>*	baseDataList ;		// ベ�?ス�??タリス�?
		LDVector<IDrawData*>*	drawDataList ;		// 描画�??タリス�?
	//	MemoryParam*			memParam ;
	};
	
	
	
	class PartsDataContext : public LDObject 
	{
	public:
		PartsDataContext(PartsData* src) ;
		virtual ~PartsDataContext() ;
		
	
	public:
		
		float getPartsOpacity(){ return partsOpacity ; }
	
	
		
		void setPartsOpacity(float v){ partsOpacity = v ; }
	
	private:
		float 			partsOpacity ;		// パ�?�??不�?明度を�?納用
		PartsData * 	srcPtr ;			//
	} ;
} 
//------------ LIVE2D NAMESPACE ------------

#endif		// __LIVE2D_PARTS_DATA_H__
