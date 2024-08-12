#pragma once
#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;//set of vertexs
	std::vector<std::vector<int> > faces_;// set of face
	std::vector<Vec3f> vert_texs_;//set of vertex_textures
	std::vector<Vec3f> normals;
public:
	Model(const char* filename);
	Model(const char* filename,int i);
	~Model();
	int nverts();
	int nfaces();
	int nvtextures();
	int nNormals();
	Vec3f vert(int i);
	Vec3f vert_tex(int i);
	std::vector<int> face(int idx);
	Vec3f vert_norm(int i);
};
