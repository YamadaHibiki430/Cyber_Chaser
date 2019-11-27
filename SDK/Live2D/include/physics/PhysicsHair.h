/**
 *  PhysicsHair.h
 *
 *  Created on: 2010/11/29
 *  Copyright(c) Live2D Inc. All rights reserved.
 *  [[ CONFIDENTIAL ]]
 */
#ifndef __LIVE2D_PHYSICS_HAIR_H__
#define __LIVE2D_PHYSICS_HAIR_H__


#include "../Live2D.h"
#include "../memory/LDObject.h"
#include "../ALive2DModel.h"
#define _USE_MATH_DEFINES

#include	"../util/UtMath.h"

//--------- LIVE2D NAMESPACE ------------
namespace live2d 
{
	class IPhysicsSrc ;
	class IPhysicsTarget ;
	class MemoryParam ;

#ifndef __SKIP_DOC__

	
	class PhysicsPoint  : public live2d::LDObject 
	{
	public:
		PhysicsPoint()
			: mass(1) , x(0) ,y(0) ,vx(0),vy(0) ,ax(0),ay(0) , fx(0) , fy(0)
			, last_x(0) , last_y(0) , last_vx(0) , last_vy(0)
		{

		}
		
		void setupLast()
		{
			this->last_x = x ;
			this->last_y = y ;
			this->last_vx = vx ;
			this->last_vy = vy ;
		}

	public:
		float 	mass ;					// 質�?kg)
		
		float 	x  , y ;				// 位置(m)
		float 	vx ,vy ;				// 速度(m/s)
		float 	ax ,ay ;				// �??度(m/s2)
			
		float 	fx ,fy ;				// 外力

		float 	last_x , last_y ;		// 
		float 	last_vx , last_vy ;		// 
		
	} ;

	
#endif // __SKIP_DOC__


	
	class PhysicsHair : public live2d::LDObject 
	{
	public:
		typedef enum 
		{
			SRC_TO_X = 0 ,
			SRC_TO_Y ,
			SRC_TO_G_ANGLE 
		} Src ;

		typedef enum 
		{
			TARGET_FROM_ANGLE = 0 ,// 髪の毛モ�?��の�?��の角度
			TARGET_FROM_ANGLE_V  // 髪の毛モ�?��の�?��の角�?度?�髪揺れ�?変形方向�?速度方向に依存する�?
		} Target ;

	public:
		// Constructor
		PhysicsHair() ;
		PhysicsHair(float baseLengthM , float airRegistance, float mass ) ;

		// Destructor
		virtual ~PhysicsHair();

	public:
		//  セ�?��ア�??
		//  baseLengthM 		髪の毛�?長さ（メートル??
		//  airRegistance 	空気抵抗（デフォルト�? 0.5??
		//  mass				髪の毛�?質量�?g??
		void setup(float baseLengthM , float airRegistance, float mass ) ;

		// 外部から点の位置初期化などを行った�?合に呼ぶ
		void setup() ;


		// 髪の毛�?基準点の X に紐づける値
		void addSrcParam( PhysicsHair::Src srcType , const char * paramID , float scale , float weight ) ;

		// 髪の毛�?基準点の X に紐づける値
		void addTargetParam( PhysicsHair::Target targetType , const char * paramID , float scale , float weight ) ;
		
		//  更新
		void update(live2d::ALive2DModel * model , long long time);

		//  髪の根�??質点取�?
		PhysicsPoint & getPhysicsPoint1(){ return p1 ; }

		//  髪の先端の質点取�?
		PhysicsPoint & getPhysicsPoint2(){ return p2 ; }

		//  重力角度取�?
		float getGravityAngleDeg(){ return this->gravityAngleDeg ; }

		//  重力角度セ�?��
		void setGravityAngleDeg(float angleDeg){ this->gravityAngleDeg = angleDeg ; }
		
		//  角�?度の取�?
		float getAngleP1toP2Deg(){ return angleP1toP2Deg ; }

		//  �??度の計算結果取�?
		float getAngleP1toP2Deg_velocity(){ return angleP1toP2Deg_v ; }
		

	private:
		//  更新
		void update_exe(live2d::ALive2DModel *& model, float dt);

		inline float calc_angleP1toP2()
		{
			return (float)(-180*(L2D_ATAN2(p1.x - p2.x, -(p1.y - p2.y)))/M_PI_F) ;
		}

	private:
		PhysicsPoint 						p1 ;							// 質点?�（髪の根�?�?
		PhysicsPoint 						p2 ;							// 質点?�（髪の先端??

		float		 						baseLengthM;					// 
		float 								gravityAngleDeg;				// 
		float 								airResistance;					// 

		float 								angleP1toP2Deg ;				// 計算結果
		float 								last_angleP1toP2Deg ;

		float 								angleP1toP2Deg_v ;				// �??度の計算結果

		// 時刻�??
		long long 							startTime;						// 
		long long 							lastTime;						// 

	    
		live2d::LDVector<IPhysicsSrc*>* 	srcListPtr ;					// 物�?��算に関係づける�??パラメータ値

	   
		live2d::LDVector<IPhysicsTarget*>*	targetListPtr ;				 	// 物�?��算結果を対応づけるパラメータ値

		live2d::MemoryParam*  				memoryManagement ;
		live2d::AMemoryHolder*				memoryHolderFixedMain ;

	};

}
//--------- LIVE2D NAMESPACE ------------



#endif	// __LIVE2D_PHYSICS_HAIR_H__
