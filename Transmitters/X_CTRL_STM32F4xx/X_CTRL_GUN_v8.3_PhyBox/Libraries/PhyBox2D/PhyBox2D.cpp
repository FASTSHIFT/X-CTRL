
#include "phybox2d.h"
#include "math.h"

static float linear(float val, float min1, float max1, float min2, float max2)
{
    if(abs(min1 - max1) > 1e-6f)
    {
        float k = (val - min1) / (max1 - min1);
        return min2 - min2 * k + max2 * k;
    }
    return 0;
}
static float limit(float val, float min, float max)
{
    if(val < min)
    {
        return min;
    }
    else if(val > max)
    {
        return max;
    }
    else
    {
        return val;
    }
}
void Point2D_StructInit(Point2D_TypeDef *p2d, float x, float y)
{
    p2d->X = x;
    p2d->Y = y;
}
float Point2D_Normalize(Point2D_TypeDef *p2d)
{
    float x = p2d->X;
    float y = p2d->Y;
    float norm = sqrt(x * x + y * y);
    p2d->X = x / norm;
    p2d->Y = y / norm;
    return norm;
}
void Point2D_Negetive(Point2D_TypeDef *p2d)
{
    p2d->X = -p2d->X;
    p2d->Y = -p2d->Y;
}
Point2D_TypeDef Point2D_Add(Point2D_TypeDef *n1, Point2D_TypeDef *n2)
{
    Point2D_TypeDef retval;
    retval.X = n1->X + n2->X;
    retval.Y = n1->Y + n2->Y;
    return retval;
}
Point2D_TypeDef Point2D_Sub(Point2D_TypeDef *n1, Point2D_TypeDef *n2)
{
    Point2D_TypeDef retval;
    retval.X = n1->X - n2->X;
    retval.Y = n1->Y - n2->Y;
    return retval;
}
float Point2D_GetDistance(Point2D_TypeDef *n1, Point2D_TypeDef *n2)
{
    Point2D_TypeDef retval = Point2D_Sub(n1, n2);
    return Point2D_Normalize(&retval);
}
int Point2D_NotEmpty(Point2D_TypeDef *p2d)
{
    if(abs(p2d->X) > 1e-6f)
    {
        return 1;
    }
    if(abs(p2d->Y) > 1e-6f)
    {
        return 1;
    }
    return 0;
}
void Field_Add(Field_TypeDef *pField, Particle_TypeDef *p)
{
    pField->Children[pField->Count ++] = p;
}
void Field_StructInit(Field_TypeDef *pField, float dt)
{
    pField->UpdateInterval = dt;
    pField->Count = 0;
    Point2D_StructInit(&pField->Gravity, 0, 0);
    for(int i = 0; i < Field_ParticleCount_Max; i++)
    {
        pField->Children[i] = 0;
    }
}
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
)
{
    Particle_TypeDef retval;
    retval.Mass = mass;
    retval.RepulsionRadius = repRadius;
    retval.AttractiveRadius = attRadius;
    retval.Force.X = 0;
    retval.Force.Y = 0;
    retval.IsFreeze = isFreeze;
    retval.MaxAttractive = attractive;
    retval.MaxRepulsion = repulsion;
    retval.Position.X = pos_x;
    retval.Position.Y = pos_y;
    retval.Velocity.X = vec_x;
    retval.Velocity.Y = vec_y;
    retval.Resistance = resistance;
    return retval;
}
Point2D_TypeDef Point2D(float x, float y)
{
    Point2D_TypeDef p2d;
    p2d.X = x;
    p2d.Y = y;
    return p2d;
}
void Field_Update(Field_TypeDef *pField)
{
    for(int i = 0; i < pField->Count; i++)
    {
        /* gravity */
        Particle_TypeDef *pMass = pField->Children[i];
        pMass->Force = pField->Gravity;
    }

    for(int i = 0; i < pField->Count; i++)
    {
        Particle_TypeDef *pMass = pField->Children[i];
        Point2D_TypeDef vel = pMass->Velocity;
        float norm = Point2D_Normalize(&vel);
        /* friction */
        if(norm > FrictionThreshold)
        {
            Point2D_Negetive(&vel);
            pMass->Force.X += vel.X * pMass->Resistance;
            pMass->Force.Y += vel.Y * pMass->Resistance;
        }
        /* attractive */
        for(int a = 0; a < pField->Count; a++)
        {
            if(a != i)
            {
                Particle_TypeDef *pMass2 = pField->Children[a];
                Point2D_TypeDef sub = Point2D_Sub(&pMass->Position, &pMass2->Position);
                float distance = Point2D_Normalize(&sub);
                if(distance < pMass->AttractiveRadius)
                {
                    pMass2->Force.X += sub.X * pMass->MaxAttractive;
                    pMass2->Force.Y += sub.Y * pMass->MaxAttractive;
                    pMass->Force.X -= sub.X * pMass->MaxAttractive;
                    pMass->Force.Y -= sub.Y * pMass->MaxAttractive;
                }
            }
        }
        /* repulsion */
        for(int a = 0; a < pField->Count; a++)
        {
            if(a != i)
            {
                Particle_TypeDef *pMass2 = pField->Children[a];
                Point2D_TypeDef sub = Point2D_Sub(&pMass2->Position, &pMass->Position);
                float distance = Point2D_Normalize(&sub);
                float force = linear(limit(distance, 0, pMass->RepulsionRadius), 0, pMass->RepulsionRadius, pMass->MaxRepulsion, 0);
                pMass2->Force.X += sub.X * force;
                pMass2->Force.Y += sub.Y * force;
                pMass->Force.X -= sub.X * force;
                pMass->Force.Y -= sub.Y * force;
            }
        }
    }
    for(int i = 0; i < pField->Count; i++)
    {
        /* gravity */
        Particle_TypeDef *pMass = pField->Children[i];
        if(pMass->IsFreeze)
        {
            pMass->Velocity.X = 0;
            pMass->Velocity.Y = 0;
        }
        else
        {
            pMass->Velocity.X += pMass->Force.X / pMass->Mass * pField->UpdateInterval;
            pMass->Velocity.Y += pMass->Force.Y / pMass->Mass * pField->UpdateInterval;
            pMass->Position.X += pMass->Velocity.X * pField->UpdateInterval;
            pMass->Position.Y += pMass->Velocity.Y * pField->UpdateInterval;
        }
    }
}


