#pragma once
#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;//set of vertexs
	std::vector<std::vector<int> > faces_;// set of face
public:
	Model(const char* filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<int> face(int idx);
};