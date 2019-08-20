#pragma once
#include "mathl.h"



struct Matrix4d
{
  double k[16];

  Matrix4d() {
    setIdentity();
  }

  Matrix4d(const Matrix4d &m) {
    for (int i=0; i<16; i++) {
      k[i] = m.k[i];
    }
  }

  Matrix4d &operator=(const Matrix4d &m) {
    memcpy(k, m.k, 16 * sizeof(double));
    return *this;
  }

  void setIdentity() {
    k[0]=k[5]=k[10]=k[15] = 1.0;
    k[1]=k[2]=k[3]=k[4]=k[6]=k[7]=k[8]=k[9]=k[11]=k[12]=k[13]=k[14] = 0.0;
  }

  void translate(double dx, double dy, double dz) {
    k[12] = k[0]*dx + k[4]*dy + k[8]*dz + k[12];
    k[13] = k[1]*dx + k[5]*dy + k[9]*dz + k[13];
    k[14] = k[2]*dx + k[6]*dy + k[10]*dz + k[14];
  }

  void rotateX(double a) {
    double sina = sin(a), cosa = cos(a);
    double k4 = k[4]*cosa + k[8]*sina;
    double k5 = k[5]*cosa + k[9]*sina;
    double k6 = k[6]*cosa + k[10]*sina;
    double k8 = -k[4]*sina + k[8]*cosa;
    double k9 = -k[5]*sina + k[9]*cosa;
    double k10 = -k[6]*sina + k[10]*cosa;
    k[4] = k4;
    k[5] = k5;
    k[6] = k6;
    k[8] = k8;
    k[9] = k9;
    k[10] = k10;
  }

  void rotateY(double a) {
    double sina = sin(a), cosa = cos(a);
    double k0 = k[0]*cosa - k[8]*sina;
    double k1 = k[1]*cosa - k[9]*sina;
    double k2 = k[2]*cosa - k[10]*sina;
    double k8 = k[0]*sina + k[8]*cosa;
    double k9 = k[1]*sina + k[9]*cosa;
    double k10 = k[2]*sina + k[10]*cosa;
    k[0] = k0;
    k[1] = k1;
    k[2] = k2;
    k[8] = k8;
    k[9] = k9;
    k[10] = k10;
  }

  void rotateZ(double a) {
    double sina = sin(a), cosa = cos(a);
    double k0 = k[0]*cosa + k[4]*sina;
    double k1 = k[1]*cosa + k[5]*sina;
    double k2 = k[2]*cosa + k[6]*sina;
    double k4 = -k[0]*sina + k[4]*cosa;
    double k5 = -k[1]*sina + k[5]*cosa;
    double k6 = -k[2]*sina + k[6]*cosa;
    k[0] = k0;
    k[1] = k1;
    k[2] = k2;
    k[4] = k4;
    k[5] = k5;
    k[6] = k6;
  }

  void scale(double s) {
    k[0] *= s;
    k[1] *= s;
    k[2] *= s;
    k[4] *= s;
    k[5] *= s;
    k[6] *= s;
    k[8] *= s;
    k[9] *= s;
    k[10] *= s;
  }

  void scale(double sx, double sy, double sz) {
    k[0] *= sx;
    k[1] *= sx;
    k[2] *= sx;
    k[4] *= sy;
    k[5] *= sy;
    k[6] *= sy;
    k[8] *= sz;
    k[9] *= sz;
    k[10] *= sz;
  }

  void setCamera(double distance, double rotX, double rotY) {
    this->translate(0, 0, -distance);

    this->rotateX(rotX);
    this->rotateY(rotY);
  }

  void mul(Vec3d &v, double w) {
    double x = k[0]*v.x + k[4]*v.y + k[8]*v.z + k[12]*w;
    double y = k[1]*v.x + k[5]*v.y + k[9]*v.z + k[13]*w;
    double z = k[2]*v.x + k[6]*v.y + k[10]*v.z + k[14]*w;
    v.set(x, y, z);
  }

  void mulDirection(Vec3d &v) {
    double x = k[0]*v.x + k[4]*v.y + k[8]*v.z;
    double y = k[1]*v.x + k[5]*v.y + k[9]*v.z;
    double z = k[2]*v.x + k[6]*v.y + k[10]*v.z;
    v.set(x/length(k[0], k[1], k[2]), y/length(k[4], k[5], k[6]), z/length(k[8], k[9], k[10]));
  }


  void mulToLatter(const Vec3d &v, Vec3d &o, double w) {
    double x = k[0]*v.x + k[4]*v.y + k[8]*v.z + k[12]*w;
    double y = k[1]*v.x + k[5]*v.y + k[9]*v.z + k[13]*w;
    double z = k[2]*v.x + k[6]*v.y + k[10]*v.z + k[14]*w;
    o.set(x, y, z);
  }

  void mulToLatterDirection(const Vec3d &v, Vec3d &o) {
    double x = k[0]*v.x + k[4]*v.y + k[8]*v.z;
    double y = k[1]*v.x + k[5]*v.y + k[9]*v.z;
    double z = k[2]*v.x + k[6]*v.y + k[10]*v.z;
    o.set(x/length(k[0], k[1], k[2]), y/length(k[4], k[5], k[6]), z/length(k[8], k[9], k[10]));
  }


  // adapted from http://www.mesa3d.org/
  void invert() {
    double inv[16];
    double det;
    int i;

    inv[0] = k[5]  * k[10] * k[15] -
             k[5]  * k[11] * k[14] -
             k[9]  * k[6]  * k[15] +
             k[9]  * k[7]  * k[14] +
             k[13] * k[6]  * k[11] -
             k[13] * k[7]  * k[10];

    inv[4] = -k[4]  * k[10] * k[15] +
              k[4]  * k[11] * k[14] +
              k[8]  * k[6]  * k[15] -
              k[8]  * k[7]  * k[14] -
              k[12] * k[6]  * k[11] +
              k[12] * k[7]  * k[10];

    inv[8] = k[4]  * k[9] * k[15] -
             k[4]  * k[11] * k[13] -
             k[8]  * k[5] * k[15] +
             k[8]  * k[7] * k[13] +
             k[12] * k[5] * k[11] -
             k[12] * k[7] * k[9];

    inv[12] = -k[4]  * k[9] * k[14] +
               k[4]  * k[10] * k[13] +
               k[8]  * k[5] * k[14] -
               k[8]  * k[6] * k[13] -
               k[12] * k[5] * k[10] +
               k[12] * k[6] * k[9];

    inv[1] = -k[1]  * k[10] * k[15] +
              k[1]  * k[11] * k[14] +
              k[9]  * k[2] * k[15] -
              k[9]  * k[3] * k[14] -
              k[13] * k[2] * k[11] +
              k[13] * k[3] * k[10];

    inv[5] = k[0]  * k[10] * k[15] -
             k[0]  * k[11] * k[14] -
             k[8]  * k[2] * k[15] +
             k[8]  * k[3] * k[14] +
             k[12] * k[2] * k[11] -
             k[12] * k[3] * k[10];

    inv[9] = -k[0]  * k[9] * k[15] +
              k[0]  * k[11] * k[13] +
              k[8]  * k[1] * k[15] -
              k[8]  * k[3] * k[13] -
              k[12] * k[1] * k[11] +
              k[12] * k[3] * k[9];

    inv[13] = k[0]  * k[9] * k[14] -
              k[0]  * k[10] * k[13] -
              k[8]  * k[1] * k[14] +
              k[8]  * k[2] * k[13] +
              k[12] * k[1] * k[10] -
              k[12] * k[2] * k[9];

    inv[2] = k[1]  * k[6] * k[15] -
             k[1]  * k[7] * k[14] -
             k[5]  * k[2] * k[15] +
             k[5]  * k[3] * k[14] +
             k[13] * k[2] * k[7] -
             k[13] * k[3] * k[6];

    inv[6] = -k[0]  * k[6] * k[15] +
              k[0]  * k[7] * k[14] +
              k[4]  * k[2] * k[15] -
              k[4]  * k[3] * k[14] -
              k[12] * k[2] * k[7] +
              k[12] * k[3] * k[6];

    inv[10] = k[0]  * k[5] * k[15] -
              k[0]  * k[7] * k[13] -
              k[4]  * k[1] * k[15] +
              k[4]  * k[3] * k[13] +
              k[12] * k[1] * k[7] -
              k[12] * k[3] * k[5];

    inv[14] = -k[0]  * k[5] * k[14] +
               k[0]  * k[6] * k[13] +
               k[4]  * k[1] * k[14] -
               k[4]  * k[2] * k[13] -
               k[12] * k[1] * k[6] +
               k[12] * k[2] * k[5];

    inv[3] = -k[1] * k[6] * k[11] +
              k[1] * k[7] * k[10] +
              k[5] * k[2] * k[11] -
              k[5] * k[3] * k[10] -
              k[9] * k[2] * k[7] +
              k[9] * k[3] * k[6];

    inv[7] = k[0] * k[6] * k[11] -
             k[0] * k[7] * k[10] -
             k[4] * k[2] * k[11] +
             k[4] * k[3] * k[10] +
             k[8] * k[2] * k[7] -
             k[8] * k[3] * k[6];

    inv[11] = -k[0] * k[5] * k[11] +
               k[0] * k[7] * k[9] +
               k[4] * k[1] * k[11] -
               k[4] * k[3] * k[9] -
               k[8] * k[1] * k[7] +
               k[8] * k[3] * k[5];

    inv[15] = k[0] * k[5] * k[10] -
              k[0] * k[6] * k[9] -
              k[4] * k[1] * k[10] +
              k[4] * k[2] * k[9] +
              k[8] * k[1] * k[6] -
              k[8] * k[2] * k[5];

    det = k[0] * inv[0] + k[1] * inv[4] + k[2] * inv[8] + k[3] * inv[12];

    if(det != 0) {
      for(i=0; i<16; i++) {
        k[i] = inv[i] / det;
      }
    }

  }




  /*void mul(Vec v, Vec out) {
    double x = k[0]*v.x + k[4]*v.y + k[8]*v.z + k[12];
    double y = k[1]*v.x + k[5]*v.y + k[9]*v.z + k[13];
    double z = k[2]*v.x + k[6]*v.y + k[10]*v.z + k[14];
    out.set(x, y, z);
  }*/

  /*void print() {
    println("|"+s(k[0])+"\t"+s(k[4])+"\t"+s(k[8])+"\t"+s(k[12])+"|");
    println("|"+s(k[1])+"\t"+s(k[5])+"\t"+s(k[9])+"\t"+s(k[13])+"|");
    println("|"+s(k[2])+"\t"+s(k[6])+"\t"+s(k[10])+"\t"+s(k[14])+"|");
    println("|"+s(k[3])+"\t"+s(k[7])+"\t"+s(k[11])+"\t"+s(k[15])+"|");
  }*/



};










struct MatrixStack
{
  std::vector<Matrix4d> matrices;
  Matrix4d m;

  MatrixStack() {
    matrices.push_back(m);
  }

  inline

  void push() {
    matrices.push_back(m);
  }

  void pop() {
    if (matrices.size() > 1) {
      matrices.erase(matrices.end()-1);
      m = matrices[matrices.size()-1];

    } else {
      printf("Error at MatrixStack::pop(). Trying to pop from empty matrix stack.\n");
    }
  }
};
