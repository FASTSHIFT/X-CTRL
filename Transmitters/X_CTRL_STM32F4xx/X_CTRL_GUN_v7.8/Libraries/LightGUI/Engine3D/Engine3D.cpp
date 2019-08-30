#include "Engine3D.h"

#define PI 3.1415926535897932384626433832795

using namespace Engine3D;

Model3D::Model3D(Object_t *obj, float *qView, float* projection)
{
    mObject = obj;
    Projection = projection;
    Qview = qView;
    mVertex = new Vertex_t[mObject->numVertex];
}

Model3D::~Model3D()
{
    delete []mVertex;
}

void Model3D::move(float x, float y, float z)
{
    mObject->offset[0] = x;
    mObject->offset[1] = y;
    mObject->offset[2] = z;
}

float Model3D::fastinvsqrt(float x)
{
    float halfx = 0.5f * x;
    float y = x;
    long i = *(long*)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*)&i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

void Model3D::qproduct(const float* p, const float* q, float* qr)
{
    qr[0] = p[0] * q[0] - p[1] * q[1] - p[2] * q[2] - p[3] * q[3];
    qr[1] = p[0] * q[1] + p[1] * q[0] + p[2] * q[3] - p[3] * q[2];
    qr[2] = p[0] * q[2] - p[1] * q[3] + p[2] * q[0] + p[3] * q[1];
    qr[3] = p[0] * q[3] + p[1] * q[2] - p[2] * q[1] + p[3] * q[0];
}

void Model3D::iqRot(float q[], int16_t vertex[], float result[])
{
    float prod[4];
    prod[0] =  - q[1] * vertex[0] - q[2] * vertex[1] - q[3] * vertex[2];
    prod[1] = q[0] * vertex[0] + q[2] * vertex[2] - q[3] * vertex[1];
    prod[2] = q[0] * vertex[1] - q[1] * vertex[2] + q[3] * vertex[0];
    prod[3] = q[0] * vertex[2] + q[1] * vertex[1] - q[2] * vertex[0];

    result[0] = -prod[0] * q[1] + prod[1] * q[0] - prod[2] * q[3] + prod[3] * q[2];
    result[1] = -prod[0] * q[2] + prod[1] * q[3] + prod[2] * q[0] - prod[3] * q[1];
    result[2] = -prod[0] * q[3] - prod[1] * q[2] + prod[2] * q[1] + prod[3] * q[0];
}

void Model3D::matMulVect(float M[], float V[], int16_t result[])
{
    for(uint8_t i = 0; i < 2; i++)
    {
        result[i] = (M[i * 4] * V[0] + M[i * 4 + 1] * V[1] + M[i * 4 + 2] * V[2] + M[i * 4 + 3]) / -V[2] * 128;
    }
}

void Model3D::qnormalized(float* q)
{
    float invnorm;
    invnorm = fastinvsqrt(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    if (invnorm < 100000000)
    {
        q[0] *= invnorm;
        q[1] *= invnorm;
        q[2] *= invnorm;
        q[3] *= invnorm;
    }
    else
    {
        q[0] = 1;
        q[1] = 0;
        q[2] = 0;
        q[3] = 0;
    }
}

void Model3D::rotate(float* q)
{
    float qtemp[4];
    qproduct(q, mObject->quat, qtemp);
    qnormalized(qtemp);
    mObject->quat[0] = qtemp[0];
    mObject->quat[1] = qtemp[1];
    mObject->quat[2] = qtemp[2];
    mObject->quat[3] = qtemp[3];
}

void Model3D::rotateTo(float* q)
{
    float qtemp[4];
    qproduct(mObject->quat, q, qtemp);
    qnormalized(qtemp);
    mObject->quat[0] = q[0];
    mObject->quat[1] = q[1];
    mObject->quat[2] = q[2];
    mObject->quat[3] = q[3];
}

void Model3D::rotate(float x, float y, float z, float degree)
{
    float qdelta[4];
    float invnorm = 1 / sqrt(x * x + y * y + z * z);
    x *= invnorm;
    y *= invnorm;
    z *= invnorm;
    float rad = degree / 180.0f * PI;
    float svalue = sin(rad / 2);
    qdelta[0] = cos(rad / 2);
    qdelta[1] = svalue * x;
    qdelta[2] = svalue * y;
    qdelta[3] = svalue * z;
    invnorm = 1 / sqrt(qdelta[0] * qdelta[0] + qdelta[1] * qdelta[1] + qdelta[2] * qdelta[2] + qdelta[3] * qdelta[3]);
    qdelta[0] *= invnorm;
    qdelta[1] *= invnorm;
    qdelta[2] *= invnorm;
    qdelta[3] *= invnorm;
    rotate(qdelta);
}

void Model3D::render()
{
    float qtemp[4];
    qproduct(Qview, mObject->quat, qtemp);

    for (uint32_t i = 0; i < mObject->numVertex; i++)
    {
        float vtemp[3];
        iqRot(qtemp, mObject->vertex[i].location, vtemp);
        vtemp[0] += mObject->offset[0];
        vtemp[1] += mObject->offset[1];
        vtemp[2] += mObject->offset[2];

        matMulVect(Projection, vtemp, mVertex[i].location);
    }

    for (uint32_t i = 0; i < mObject->numEdge; i++)
    {
        uint32_t p1 = mObject->edge[i].connection[0];
        uint32_t p2 = mObject->edge[i].connection[1];
        if(DrawLine_Callback)
            DrawLine_Callback(
                mVertex[p1].location[0] + 64, 
                mVertex[p1].location[1] + 32, 
                mVertex[p2].location[0] + 64, 
                mVertex[p2].location[1] + 32, 
                mObject->edge[i].color
        );
    }
}
