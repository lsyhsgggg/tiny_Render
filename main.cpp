
#include<iostream>
#include"model.h"
#include<algorithm>
#include<memory>
#include <limits>
#include<cmath>
#include"sampler.h"
#include"camera.h"
#include"geometry.h"
#include <random>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
//全局变量
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor black = TGAColor(0, 0, 0, 255);
constexpr int width = 1000;
constexpr int height = 1000;
constexpr int num = width * height;
bool early_z = true;
Matrix modelmat(4,4);
Matrix view(4,4);
Matrix projection(4,4);
Matrix comb2 ;
Matrix shadow2play;
Matrix comb;
Matrix combinverse;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> dis_neg1_to_1(-1.0f, 1.0f);
std::uniform_real_distribution<float> dis_0_to_1(0.0f, 1.0f);
Camera mainCamera;
Sampler t1("D:\\vs_project\\each_project\\tinyRender\\tinyRender\\dependence\\african_head_diffuse.tga");
Sampler normalmap("D:\\vs_project\\each_project\\tinyRender\\tinyRender\\dependence\\african_head_nm_tangent.tga");
Sampler tangentmap("D:\\vs_project\\each_project\\tinyRender\\tinyRender\\dependence\\african_head_nm.tga");
Camera light(Vec3f(2,2,2), Vec3f(0, 0, 0), Vec3f(0, 1, 0));
Matrix lightcameraview;
Vec2<int> viewport(float x, float y);

Vec3f Unpackcolor(const TGAColor& color) {
	float xval = color.r / 255.0f * 2.0f - 1.0f;
	float yval = color.b / 255.0f * 2.0f - 1.0f;
	float zval = color.g / 255.0f * 2.0f - 1.0f;
	return Vec3f(xval, yval, zval);
}
void swap(int& a, int& b) {
	int temp = a;
	a = b;
	b = temp;
}
Matrix BuildProjMatrix(const float& fov, const float& aspect, const float& n, const float& f) {
	float c = 1.0f / tan(fov * M_PI / 360.0f);
	Matrix proj=proj.identity(4);

	proj[0][0] = c / aspect;
	proj[1][1] = c;
	proj[2][2] = -(f + n) / (f - n);
	proj[2][3] = -2.0f * f * n / (f - n);
	proj[3][2] = -1.0f;
	proj[3][3] = 0.0f;

	return proj;
}
Matrix transformationMatrix(const Vec3f& translation, const Vec3f& scale, const Vec3f& rotationAxis, float angle) {
	// Translation matrix
	Matrix T = Matrix::identity(4);
	T[0][3] = translation.x;
	T[1][3] = translation.y;
	T[2][3] = translation.z;

	// Scale matrix
	Matrix S = Matrix::identity(4);
	S[0][0] = scale.x;
	S[1][1] = scale.y;
	S[2][2] = scale.z;

	// Rotation matrix (Rodrigues' rotation formula)
	Matrix R = Matrix::identity(4);
	float radians = angle * M_PI / 180;
	float c = cos(radians);
	float s = sin(radians);
	float omc = 1.0f - c;
	float x = rotationAxis.x;
	float y = rotationAxis.y;
	float z = rotationAxis.z;

	R[0][0] = c + x * x * omc;
	R[0][1] = x * y * omc - z * s;
	R[0][2] = x * z * omc + y * s;

	R[1][0] = y * x * omc + z * s;
	R[1][1] = c + y * y * omc;
	R[1][2] = y * z * omc - x * s;

	R[2][0] = z * x * omc - y * s;
	R[2][1] = z * y * omc + x * s;
	R[2][2] = c + z * z * omc;

	// Combine the transformations: R * S * T
	return T*(R*S) ;
}
struct Light {
	Vec3f Dir;
	Vec3f color;
	Vec3f pos;
	Light(Vec3f Dir, Vec3f color,Vec3f pos):Dir(Dir),color(color),pos(pos){}
	Light() :Light(Vec3f(0.0, 0.0, -1.0), Vec3f(1.0, 1.0, 1.0),Vec3f(0.0,0.0,-1.0)) {}
};

struct v2f {
	Vec3f pos;
	Vec3f normal;
	Vec3f texcoord;
	Vec3f tangent;
	Vec3f bitangent;
	Vec3f worldpos;
	
};

void drawline( Vec2i t0, Vec2i t1, TGAImage&image,const TGAColor&color) {
	bool steep = false;
	int k = 1;
	if (abs(t0.y - t1.y) > abs(t0.x - t1.x)) {
		swap(t0.x, t0.y);
		swap(t1.x, t1.y);
		steep = true;
	}
	if (t1.x < t0.x) {
		swap(t0.x, t1.x);
		swap(t0.y, t1.y);
	}
	if (t1.y < t0.y) k = -1;
	int dx = abs(t1.x - t0.x);
	int error2 = 0;
	int derror2 = abs(t1.y - t0.y)*2;
	int y = t0.y;
	for (int left = t0.x; left <= t1.x; left++) {
		if (steep) {
			image.set(y, left, color);
		}
		else {
			image.set(left, y, color);
		}
		error2 -= derror2;
		if (error2 + dx < 0) { 
			y += k; 
			error2 += 2 * dx;
		}
	}
}
void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			image.set(y, x, color);
		}
		else {
			image.set(x, y, color);
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}
void triangle_empty(Vec2i&p1,Vec2i&p2,Vec2i&p3, TGAImage& image, const TGAColor& color) {
	drawline(p1, p2, image, color);
	drawline(p3, p2, image, color);
	drawline(p1, p3, image, color);
}

Vec3f barycentric(Vec2i* pts, Vec2i P) {
	Vec3f u = Vec3f(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]) ^ Vec3f(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]);
	/* `pts` and `P` has integer value as coordinates
	   so `abs(u[2])` < 1 means `u[2]` is 0, that means
	   triangle is degenerate, in this case return something with negative coordinates */
	if (std::abs(u.z) < 1) return Vec3f(-1, 1, 1);
	return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}
float clamp(float val, float left, float right) {
	if (val < left) return left;
	if (val > right) return right;
	return val;
}
TGAColor Vec3fToTGAColor(const Vec3f& vec) {
	// Clamp the values to be between 0 and 1
	float r = clamp(vec.x, 0.0f, 1.0f);
	float g = clamp(vec.y, 0.0f, 1.0f);
	float b = clamp(vec.z, 0.0f, 1.0f);

	// Convert to range 0-255
	unsigned char red = static_cast<unsigned char>(r * 255);
	unsigned char green = static_cast<unsigned char>(g * 255);
	unsigned char blue = static_cast<unsigned char>(b * 255);

	// Return the TGAColor, with full alpha (255)
	return TGAColor(red, green, blue, 255);
}
Vec3f TGAColorToVec3f(const TGAColor& color) {
	float r = clamp(color.r / 255.0f, 0.0f, 1.0f);
	float g = clamp(color.g / 255.0f, 0.0f, 1.0f);
	float b = clamp(color.b / 255.0f, 0.0f, 1.0f);
	return Vec3f(r, g, b);
}
inline TGAColor FragmentShader_1(Vec2i& P, v2f& frag, TGAImage& image, std::shared_ptr<float[]>z_buffer, std::shared_ptr<float[]>shadow_buffer) {
	Light light1(Vec3f(-1, -1, -1), Vec3f(1.0, 1.0, 1.0), Vec3f(2, 2, 2));
	
	Vec3f ambient(0.8, 0.8, 0.8);
	
	


	Matrix tbn = tbn.identity(3);
	tbn[0] = { frag.tangent.x,frag.normal.x,frag.bitangent.x };
	tbn[1] = { frag.tangent.y,frag.normal.y,frag.bitangent.y };
	tbn[2] = { frag.tangent.z,frag.normal.z,frag.bitangent.z };
	Vec3f Normal = Unpackcolor(normalmap.color(frag.texcoord.x, frag.texcoord.y));
	auto hgj = modelmat * frag.normal;
	
	Vec3f Normal = normalize(Vec3f(hgj[0][0], hgj[1][0], hgj[2][0]));

	/*Matrix ans = tbn * Normal;
	Normal = Vec3f(ans[0][0], ans[1][0], ans[2][0]);*/

	float k = 0.0f;
	Matrix hjg = projection * view;
	for (int i = 0; i < 64; ++i) {
		float randomx = dis_neg1_to_1(gen);
		float randomz = dis_neg1_to_1(gen);
		float len = dis_0_to_1(gen);
		float randomy = std::sqrt(clamp(1 - randomx * randomx - randomz * randomz,0.0f,1.0f));
		Vec3f testlight(randomx, randomy, randomz);
		Matrix ggg = tbn * testlight;
		testlight = Vec3f(ggg[0][0], ggg[1][0], ggg[2][0]);
		Vec3f testpoint = frag.worldpos + testlight * len;
		auto gdsk = hjg * testpoint;
		Vec3f hh= Vec3f(gdsk[0][0] / gdsk[3][0], gdsk[1][0] / gdsk[3][0], gdsk[2][0] / gdsk[3][0]);
		auto shadow_coord = viewport(hh.x, hh.y);
		int pos1 = shadow_coord.x + shadow_coord.y * width;
		if (pos1 > -1 && pos1 < num - 1) {
			if (hh.z - z_buffer[pos1] < 0.001f) { k++; }
		}
	}
	k /= 64;
	

	

	/*float k1 = std::max(dot(-normalize(light1.Dir), Normal), 0.0f);
	auto gg = comb2 *combinverse*frag.pos;
	Vec3f h= Vec3f(gg[0][0] / gg[3][0], gg[1][0] / gg[3][0], gg[2][0] / gg[3][0]);

	auto shadow_coord = viewport(h.x, h.y);
	int pos1 = shadow_coord.x + shadow_coord.y * width;
	
	if (pos1 > -1 && pos1 < num-1) {
		if (h.z- shadow_buffer[pos1] > 0.01f) { k1 *= 0.3; }
	}
	Vec3f albedo = TGAColorToVec3f(t1.color(frag.texcoord.x, frag.texcoord.y));
	Vec3f diffuse = k1 * light1.color;*/
	return Vec3fToTGAColor(ambient);
}
void PostProcessing(std::shared_ptr<float[]>z_buffer, TGAImage& image) {
	for (int x = 0;x <width; x++) {
		for (int y = 0; y <height; y++) {
		
			int pos = x + y * width;
			float currentz = z_buffer[pos];
			if (currentz == std::numeric_limits<float>::infinity()) continue;
			float currentx = (y * 2 - width) / width;
			float currenty = (x * 2 - height) / height;
			
			float currentw = (currentz + projection[2][2]) * projection[2][3];
			Vec3f currcoord((currentx * currentw) / projection[0][0], (currenty * currentw) / projection[1][1], ((currentz * currentw) -projection[2][3]) / projection[2][2]);
			float ans=0;
			for (int row = y - 1; row < y + 1; row++) {
				for (int col = x - 1; col < x + 1;col++) {
					int pos1 = col + row * width;
					if (pos1 <0 ||pos1==pos|| pos1 > num - 1) { 
						ans++;
						continue;
					}
					float newz = z_buffer[pos1];
					if (newz == std::numeric_limits<float>::infinity()) {
						ans++;
						continue;
					}
					float newx = (col * 2 - width) / width;
					float newy = (row * 2 - height) / height;
					
					float w = (newz + projection[2][2]) * projection[2][3];
					Vec3f coord((newx * w)/projection[0][0], (newy * w)/projection[1][1], ((newz * w)-projection[2][3])/projection[2][2]);

					Vec3f minus = coord - currcoord;
					float len = sqrt(minus.x * minus.x + minus.y * minus.y+minus.z*minus.z);
					float lenx = sqrt(minus.x * minus.x + minus.y * minus.y);
					float cosine = lenx / len;
				/*	if(cosine==0)std::cout << lenx << " " <<len<< std::endl;*/
				
					ans += cosine;
				/*	if ((minus.z / lenx) > 1) continue;
					ans++;*/

				}
			}
			ans /= 8;
			auto gg = image.get(x, y);
			int r = gg.r * ans;
			int g= gg.g * ans;
			int b= gg.b * ans;
			TGAColor color(r, g, b,1);
			image.set(x, y, color);
			
		}
	}
	
}

inline TGAColor FragmentShader_2(Vec2i& P, float depth, TGAImage& image,float maxz,float minz){
	//std::cout << maxz << " " << minz << " " << depth << std::endl;
	if (depth == std::numeric_limits<float>::infinity()) return black;

	
	float dis = (depth-minz)/(maxz-minz);
	//std::cout << maxz << " " << minz << " " << dis << std::endl;

	Vec3f vec(dis, dis, dis);
	// Clamp the values to be between 0 and 1
	float r = clamp(vec.x, 0.0f, 1.0f);
	float g = clamp(vec.y, 0.0f, 1.0f);
	float b = clamp(vec.z, 0.0f, 1.0f);

	// Convert to range 0-255
	unsigned char red = static_cast<unsigned char>((1-r) * 255);
	unsigned char green = static_cast<unsigned char>((1-g) * 255);
	unsigned char blue = static_cast<unsigned char>((1-b) * 255);

	// Return the TGAColor, with full alpha (255)
	return TGAColor(red, green, blue, 255);
     

}

bool Depth_test(const v2f& frag, std::shared_ptr<float[]> z_buffer,Vec2i&P){
	int pos = P.x + P.y * width;
	if (frag.pos.z <= z_buffer[pos]) {
		z_buffer[pos] = frag.pos.z;
		return true;
	}
	return false;
}

void Little_Rasterize(Vec2i* pts, v2f* info, TGAImage& image, const TGAColor& color, std::shared_ptr<float[]>shadow_buffer,float*num) {
	
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i bboxmax(0, 0);
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	 
	for (int i = 0; i < 3; i++) {
		bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
		bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
	}
	Vec2i P;

	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts, P);//光栅化的重要步骤
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			//插值属性
			v2f frag;
			frag.pos = bc_screen.x * info[0].pos + bc_screen.y * info[1].pos + bc_screen.z * info[2].pos;
			if (frag.pos.z < 0)std::cout << frag.pos.z << std::endl;
			if (early_z) {
				if (!Depth_test(frag, shadow_buffer, P)) continue;
			}
			int pos = P.x + P.y * width;
			num[0] = (num[0] < shadow_buffer[pos] ? shadow_buffer[pos] : num[0]);
			num[1] = (num[1] > shadow_buffer[pos] ? shadow_buffer[pos] : num[1]);
			//
		}
	}

	
}

void Rasterization(Vec2i* pts,v2f*info,TGAImage& image,const TGAColor&color, std::shared_ptr<float[]>z_buffer, std::shared_ptr<float[]>shadow_buffer) {
	Vec2i bboxmin(image.get_width() - 1, image.get_height() - 1);
	Vec2i bboxmax(0, 0);
	Vec2i clamp(image.get_width() - 1, image.get_height() - 1);
	for (int i = 0; i < 3; i++) {
		bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
		bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

		bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
		bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
	}
	Vec2i P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts, P);//光栅化的重要步骤
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			//插值属性
			v2f frag;
			frag.pos = bc_screen.x * info[0].pos + bc_screen.y * info[1].pos + bc_screen.z * info[2].pos;
			if(frag.pos.z<0)std::cout << frag.pos.z << std::endl;///////////////////////////////////////////////////////////////////////////////////////
			if (early_z) {
				if (!Depth_test(frag,z_buffer,P)) continue;
			}
			frag.normal= normalize(bc_screen.x * info[0].normal + bc_screen.y * info[1].normal + bc_screen.z * info[2].normal);
			frag.texcoord = bc_screen.x * info[0].texcoord + bc_screen.y * info[1].texcoord + bc_screen.z * info[2].texcoord;
			frag.bitangent=normalize(bc_screen.x * info[0].bitangent + bc_screen.y * info[1].bitangent + bc_screen.z * info[2].bitangent);
			frag.tangent= normalize(bc_screen.x * info[0].tangent + bc_screen.y * info[1].tangent + bc_screen.z * info[2].tangent);
			//
			image.set(P.x, P.y, FragmentShader_1(P,frag,image, z_buffer,shadow_buffer));//运行片段着色器
		}
	}
}
Vec2<int> viewport(float x,float y){
	int sx = (x * width + width) / 2;
	int sy = (y * height + height) / 2;
	return Vec2<int>(sx, sy);
}
inline void VertexShader_2(Model& model, TGAImage& image, const TGAColor& color, std::shared_ptr<float[]>shadow_buffer) {
	
	float num[2];
	num[0] = -std::numeric_limits<float>::infinity();
	num[1] = std::numeric_limits<float>::infinity();

	for (int i = 0; i < model.nfaces(); i++) {
		v2f info[3];

		info[0].pos = model.vert(model.face(i)[0]);
		info[1].pos = model.vert(model.face(i)[3]);
		info[2].pos = model.vert(model.face(i)[6]);

		Matrix v1 = comb2 * info[0].pos;
		//std::cout << info[0].pos.x << " " << info[0].pos.y << " " << info[0].pos.z << " " << std::endl;
		info[0].pos = Vec3f(v1[0][0] / v1[3][0], v1[1][0] / v1[3][0], v1[2][0] / v1[3][0]);
		//std::cout << info[0].pos.x << " "<< info[0].pos.y <<" "<< info[0].pos.z <<" " << std::endl;
		Matrix v2 = comb2 * info[1].pos;
		info[1].pos = Vec3f(v2[0][0] / v2[3][0], v2[1][0] / v2[3][0], v2[2][0] / v2[3][0]);
		//std::cout << info[1].pos.x << " " << info[1].pos.y << " " << info[1].pos.z << " " << std::endl;
		Matrix v3 = comb2 * info[2].pos;
		info[2].pos = Vec3f(v3[0][0] / v3[3][0], v3[1][0] / v3[3][0], v3[2][0] / v3[3][0]);
		//std::cout << info[2].pos.x << " " << info[2].pos.y << " " << info[2].pos.z << " " << std::endl;


		Vec2i sets[3];
		sets[0] = viewport(info[0].pos.x, info[0].pos.y);
		sets[1] = viewport(info[1].pos.x, info[1].pos.y);
		sets[2] = viewport(info[2].pos.x, info[2].pos.y);


	
		Little_Rasterize(sets, info, image, color, shadow_buffer,num);
	}
	//for (int x = 0;x <width; x++) {
	//	for (int y = 0; y <height; y++) {
	//		int pos = x + y * width;
	//		Vec2i P(x, y);
	//
	//		image.set(x, y, FragmentShader_2(P, shadow_buffer[pos], image, num[0], num[1]));//运行片段着色器
	//	}
	//}


 }
inline void VertexShader_1(Model&model, TGAImage& image, const TGAColor& color, std::shared_ptr<float[]>z_buffer, std::shared_ptr<float[]>shadow_buffer){

	for (int i = 0; i < model.nfaces(); i++) {
		    v2f info[3];

			info[0].pos = model.vert(model.face(i)[0]);
			info[1].pos = model.vert(model.face(i)[3]);
			info[2].pos = model.vert(model.face(i)[6]);
			
			auto k1 = modelmat * info[0].pos;
			info[0].worldpos = Vec3f(k1[0][0], k1[1][0], k1[2][0]);
			auto k2 = modelmat * info[1].pos;
			info[1].worldpos = Vec3f(k2[0][0], k2[1][0], k2[2][0]);
			auto k3 = modelmat * info[2].pos;
			info[2].worldpos = Vec3f(k3[0][0], k3[1][0], k3[2][0]);


			Matrix v1 = comb * info[0].pos;
			info[0].pos = Vec3f(v1[0][0] / v1[3][0], v1[1][0] / v1[3][0], v1[2][0] / v1[3][0]);
			Matrix v2 = comb * info[1].pos;
			info[1].pos = Vec3f(v2[0][0] / v2[3][0], v2[1][0] / v2[3][0], v2[2][0] / v2[3][0]);
			Matrix v3 = comb * info[2].pos;
			info[2].pos = Vec3f(v3[0][0] / v3[3][0], v3[1][0] / v3[3][0], v3[2][0] / v3[3][0]);






			info[0].normal = model.vert_norm(model.face(i)[1]);
			info[1].normal = model.vert_norm(model.face(i)[4]);
			info[2].normal = model.vert_norm(model.face(i)[7]);
			Matrix mat3 = mat3x3(modelmat);
			mat3 = mat3.inverse();
			mat3 = mat3.transpose();

			Matrix n1 = mat3 * info[0].normal;
			info[0].normal = Vec3f(n1[0][0], n1[1][0],n1[2][0]);
			Matrix n2 = mat3 * info[1].normal;
		    info[1].normal = Vec3f(n2[0][0], n2[1][0], n2[2][0]);
			Matrix n3 = mat3 * info[2].normal;
			info[2].normal = Vec3f(n3[0][0], n3[1][0], n3[2][0]);

			info[0].texcoord = model.vert_tex(model.face(i)[2]);
			info[1].texcoord = model.vert_tex(model.face(i)[5]);
			info[2].texcoord = model.vert_tex(model.face(i)[8]);

		
			
			Vec2i sets[3];
			sets[0] = viewport(info[0].pos.x, info[0].pos.y);
			sets[1] = viewport(info[1].pos.x, info[1].pos.y);
			sets[2] = viewport(info[2].pos.x, info[2].pos.y);
			

			for (int i = 0; i < 3; i++) {
				Vec3f tangent = Unpackcolor(tangentmap.color(sets[i].u, sets[i].v));
				Matrix temp = modelmat * tangent;
				tangent = Vec3f(temp[0][0], temp[1][0], temp[2][0]);
				Vec3f bitagnet = -normalize(cross(tangent, info[i].normal));

				info[i].tangent = tangent;
				info[i].bitangent = bitagnet;
			}

	
			Rasterization(sets,info,image, color,z_buffer, shadow_buffer);
	}
}

int main() {
	TGAImage image(width, height, TGAImage::RGB);
	t1.texture.flip_vertically();
	tangentmap.texture.flip_vertically();
	normalmap.texture.flip_vertically();
	std::shared_ptr<float[]> z_buffer(new float[num],std::default_delete<float[]>());
	std::shared_ptr<float[]> shadow_buffer(new float[num], std::default_delete<float[]>());
	for (int i = 0; i < num; i++) { 
		z_buffer[i] = std::numeric_limits<float>::infinity();
		shadow_buffer[i] = std::numeric_limits<float>::infinity();
	}

	Model model_1("D:\\vs_project\\each_project\\tinyRender\\tinyRender\\dependence\\african_head.obj");

	modelmat = transformationMatrix(Vec3f(0, 0, 0), Vec3f(1, 1, 1), Vec3f(0, 1, 0),0);
	view = mainCamera.LookAt;
	projection = BuildProjMatrix(45,float(16/9),0.1f, 100.0f);
	lightcameraview = light.LookAt;
	comb = projection * view * modelmat;
	combinverse = comb.inverse();
	 comb2 = projection * lightcameraview * modelmat;
	shadow2play = projection * lightcameraview;


	VertexShader_2(model_1, image, white, shadow_buffer);
	v2f info[3];
	VertexShader_1(model_1, image, white, z_buffer,shadow_buffer);
	PostProcessing(z_buffer,image);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;

}
