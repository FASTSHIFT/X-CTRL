
#ifndef __PHYBOX2D_H__
#define __PHYBOX2D_H__

#include "stdint.h"

#define Field_ParticleCount_Max		64
#define FrictionThreshold					1e-3f

typedef struct
{
	float X;
	float Y;
} Point2D_TypeDef;

typedef struct
{
	float Mass;										/* 粒子质量 */
	float RepulsionRadius;				/* 粒子斥力作用范围 */
	float AttractiveRadius;				/* 粒子引力作用范围 */
	Point2D_TypeDef Position;			/* 粒子坐标 */
	Point2D_TypeDef Velocity;			/* 粒子速度 */
	Point2D_TypeDef Force;				/* 粒子合外力 */
	bool IsFreeze;								/* 位置冻结 */
	float Resistance;							/* 动摩擦力 */
	float MaxRepulsion;						/* 斥力系数 */
	float MaxAttractive;					/* 引力系数 */
} Particle_TypeDef;

typedef struct
{
	float UpdateInterval;
	uint32_t Count;
	Particle_TypeDef *Children[Field_ParticleCount_Max];
	Point2D_TypeDef Gravity;
	
} Field_TypeDef;


Particle_TypeDef Particle_StructInit(
	float mass,
	float repulsion,
	float repRadius,
	float attractive,
	float attRadius,
	float resistance,
	bool isFreeze,
	float pos_x,
	float pos_y,
	float vec_x,
	float vec_y
		);
	void Field_Add(Field_TypeDef *pField, Particle_TypeDef *p);
void Point2D_StructInit(Point2D_TypeDef *p2d, float x, float y);
float Point2D_Normalize(Point2D_TypeDef *p2d);
void Point2D_Negetive(Point2D_TypeDef *p2d);
Point2D_TypeDef Point2D_Add(Point2D_TypeDef *n1, Point2D_TypeDef *n2);
Point2D_TypeDef Point2D_Sub(Point2D_TypeDef *n1, Point2D_TypeDef *n2);
float Point2D_GetDistance(Point2D_TypeDef *n1, Point2D_TypeDef *n2);
int Point2D_NotEmpty(Point2D_TypeDef *p2d);
void Field_StructInit(Field_TypeDef *pField, float dt);
Point2D_TypeDef Point2D(float x, float y);
void Field_Update(Field_TypeDef *pField);

#endif

