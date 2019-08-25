#ifndef __ENGINE3D_H
#define __ENGINE3D_H

#include "stdint.h"
#include "math.h"

namespace Engine3D {
typedef struct Vertex {
    int16_t location[3];
} Vertex_t;

typedef struct Edge {
    int16_t connection[2];
    uint16_t color;
} Edge_t;

typedef struct Object {
    Vertex_t *vertex;
    uint32_t numVertex;
    
    Edge_t *edge;
    uint32_t numEdge;
    
    float quat[4];
    float offset[3];
} Object_t;

class Model3D {
    typedef void(*DrawLineCallbackFunc_t)(int16_t,int16_t,int16_t,int16_t,uint16_t);
public:
    Model3D(Object_t *obj, float *qView, float* projection);
    ~Model3D();
    DrawLineCallbackFunc_t DrawLine_Callback;
    void move(float x, float y, float z);
    void render();
    void rotate(float* q);
    void rotate(float x, float y, float z, float degree);
    void rotateTo(float* q);
    
private:
    Object_t *mObject;
    Vertex_t *mVertex;
    float *Projection;
    float *Qview;

    float fastinvsqrt(float x);
    void qproduct(const float* p, const float* q, float* qr);
    void qnormalized(float* q);
    void iqRot(float q[], int16_t v[], float result[]);
    void matMulVect(float M[], float V[], int16_t result[]);
};

}


#endif
