#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"


//                  class Model {
//                  private:
//                      std::vector<Vec3f> verts_;//set of vertexs
//                      std::vector<std::vector<int> > faces_;// set of face
//                      std::vector<Vec3f> vert_texs_;//set of vertex_textures
//                  public:
//                      Model(const char* filename);
//                      Model(const char* filename, int i);
//                      ~Model();
//                      int nverts();
//                      int nfaces();
//                      int nvtextures();
//                      Vec3f vert(int i);
//                      Vec3f vert_tex(int i);
//                      std::vector<int> face(int idx);
//                  };
                    
//Model::Model(const char* filename) : verts_(), faces_() {
//    std::ifstream in;
//    in.open(filename, std::ifstream::in);
//    if (!in.is_open()) return;
//    std::string line;
//    while (!in.eof()) {
//        std::getline(in, line);
//        std::istringstream iss(line.c_str());
//        char trash;
//        if (!line.compare(0, 2, "v ")) {
//            iss >> trash;
//            Vec3f v;
//            for (int i = 0; i < 3; i++) iss >> v.raw[i];
//            verts_.push_back(v);
//        }
//        else if (!line.compare(0, 2, "f ")) {
//            std::vector<int> f;
//            int idx,idxn;
//            iss >> trash;
//            while (iss >> idx >> trash >> idxn >>trash >>idxn) {
//                idx--; // in wavefront obj all indices start at 1, not zero
//                idxn--;
//                f.push_back(idx);
//                f.push_back(idxn);
//            }
//            faces_.push_back(f);
//        }
//        else if (!line.compare(0, 3, "vn ")) {
//            iss >> trash >> trash;
//            Vec3f vn;
//            for (int i = 0; i < 3; i++) {
//                iss >> vn.raw[i];
//            }
//            normals.push_back(vn);
//        }
//    }
//    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vn# "<<normals.size()<< std::endl;
//}
Model::Model(const char* filename) : verts_(), faces_() {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (!in.is_open()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int idx,idxn,idxt;
            iss >> trash;
            while (iss >> idx >> trash >> idxt >>trash >>idxn) {
                idx--; // in wavefront obj all indices start at 1, not zero
                idxn--;
                idxt--;
                f.push_back(idx);
                f.push_back(idxn);
                f.push_back(idxt);
            }
            faces_.push_back(f);
        }
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f vn;
            for (int i = 0; i < 3; i++) {
                iss >> vn.raw[i];
            }
            normals.push_back(vn);
        }
        else if (!line.compare(0, 4, "vt  ")) {
            iss >> trash >> trash;
            Vec3f v;

            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            vert_texs_.push_back(v);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vn# "<<normals.size()<< " vt# " <<vert_texs_.size()<< std::endl;
}

Model::Model(const char* filename,int i) : verts_(), faces_() {
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (!in.is_open()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            int idx, idxn;
            iss >> trash;
            while (iss >> idx >> trash >> trash >> idxn) {
                idx--; // in wavefront obj all indices start at 1, not zero
                idxn--;
                f.push_back(idx);
                f.push_back(idxn);
            }
            faces_.push_back(f);
        }
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f vn;
            for (int i = 0; i < 3; i++) {
                iss >> vn.raw[i];
            }
            normals.push_back(vn);
        }
        else if(!line.compare(0, 4, "vt  ")) {
            iss >> trash>>trash;
            Vec3f v;
            
            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            vert_texs_.push_back(v);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " vn# " << normals.size() << std::endl;
}


//Model::Model(const char* filename,int i) : verts_(), faces_() {
//    std::ifstream in;
//    in.open(filename, std::ifstream::in);
//    if (!in.is_open()) return;
//    std::string line;
//    while (!in.eof()) {
//        std::getline(in, line);
//        std::istringstream iss(line.c_str());
//        char trash;
//        if (!line.compare(0, 2, "v ")) {
//            iss >> trash;
//            Vec3f v;
//            for (int i = 0; i < 3; i++) iss >> v.raw[i];
//            verts_.push_back(v);
//        }
//        else if (!line.compare(0, 2, "f ")) {
//            std::vector<int> f;
//            int itrash, idx,tidx;
//            iss >> trash;
//            while (iss >> idx >> trash >> tidx>> trash >> itrash) {
//                idx--; // in wavefront obj all indices start at 1, not zero
//                tidx--;// in wavefront obj all indices start at 1, not zero
//                f.push_back(idx);
//                f.push_back(tidx);
//            }
//            faces_.push_back(f);
//        }
//        else if(!line.compare(0, 4, "vt  ")) {
//            iss >> trash>>trash;
//            Vec3f v;
//            
//            for (int i = 0; i < 3; i++) iss >> v.raw[i];
//            vert_texs_.push_back(v);
//        }
//    }
//    std::cerr << "# v# " << verts_.size() << " #vt# "<<vert_texs_.size() << " f# " << faces_.size() << std::endl;
//}

Model::~Model() {
}

int Model::nverts() {//number of vertexs
    return (int)verts_.size();
}

int Model::nfaces() {//number of faces
    return (int)faces_.size();
}

int Model::nvtextures()
{
    return (int)vert_texs_.size();
}

int Model::nNormals()
{
    return normals.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert_norm(int i)
{
    return normals[i];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

Vec3f Model::vert_tex(int i)
{
    return vert_texs_[i];
}
