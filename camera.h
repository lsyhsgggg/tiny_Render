#pragma once
#include"geometry.h"
#include"vector"
class Camera {
	public:
	Vec3f pos;
	Matrix LookAt;
	Camera():Camera(Vec3f(0,0,3.0),Vec3f(0,0,0),Vec3f(0,1,0)){}
	Camera(Vec3f eye, Vec3f center, Vec3f up):pos(eye){
        Vec3f z = normalize(eye - center);
        Vec3f x = normalize(cross(up, z));
        Vec3f y = normalize(cross(z, x));
        Matrix Minv = Matrix::identity(4);
        Matrix Tr = Matrix::identity(4);
        for (int i = 0; i < 3; i++) {
            Minv[0][i] = x[i];
            Minv[1][i] = y[i];
            Minv[2][i] = z[i];
            Tr[i][3] = -eye[i];
        }
        LookAt = Minv * Tr;
	}

};
class gjjg{};