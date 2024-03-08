#include "tgaimage.h"
#include<iostream>
#include"model.h"


const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
constexpr int width = 1500;
constexpr int height = 1500;
void swap(int& a, int& b) {
	int temp = a;
	a = b;
	b = temp;
}
void line(int x0, int y0, int x1, int y1, TGAImage&image, TGAColor color) {
	int dx = abs(x0 - x1);
	int dy = abs(y0 - y1);
	bool steep = false;
	if (dx < dy) {
		swap(y0, x0);
		swap(y1, x1);
		steep = true;
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}
	dx = abs(x0 - x1);
	dy = abs(y0 - y1);
    int offset=((y1-y0)>0?1:-1);
	int d2x = 2 * dx;
	int d2y = 2 * dy;
	int p0 =d2y-dx;
	int x = x0;
	int y = y0;
	
	for (; x<=x1; ++x) {
		if (steep) {
			image.set(y,x, color);
		}
		else {
			image.set(x, y, color);
		}
		if (dx != 0&&dy != 0) {
			p0 += d2y;
			if (p0 > 0) {
				y += offset;
				p0 -= d2x;
			}
		}
	}
	
}
void draw_soild_tranigle(Model model, TGAImage& image, TGAColor color,std::vector<Vec3f>&vertex) {
	constexpr int wid = width / 2;
	constexpr int hei = height / 2;
	Vec3f vec1 = vertex[0];
	Vec3f vec2 = vertex[1];
	Vec3f vec3 = vertex[2];
	int x0 = vec1.x * wid / 100 + wid;
	int y0 = vec1.y * hei / 100 + hei;
	int x1 = vec2.x * wid / 100 + wid;
	int y1 = vec2.y * hei / 100 + hei;
	int x2 = vec3.x * wid / 100 + wid;
	int y2 = vec3.y * hei / 100 + hei;
	int dx = abs(x0 - x1);
	int dy = abs(y0 - y1);
	bool steep = false;
	if (dx < dy) {
		swap(y0, x0);
		swap(y1, x1);
		steep = true;
	}
	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}
	dx = abs(x0 - x1);
	dy = abs(y0 - y1);
	int offset = ((y1 - y0) > 0 ? 1 : -1);
	int d2x = 2 * dx;
	int d2y = 2 * dy;
	int p0 = d2y - dx;
	int x = x0;
	int y = y0;

	for (; x <= x1; ++x) {
		if (steep) {
			image.set(y, x, color);
			line(y, x, x2, y2, image, white);
		}
		else {
			image.set(x, y, color);
			line(x, y, x2, y2, image, white);
		}
		if (dx != 0 && dy != 0) {
			p0 += d2y;
			if (p0 > 0) {
				y += offset;
				p0 -= d2x;
			}
		}
	}
}
void draw_soild_model(Model model, TGAImage& image, TGAColor color) {
	constexpr int wid = width / 2;
	constexpr int hei = height / 2;

	std::vector<int> oneface;
	
	for (int i = 0; i < model.nfaces(); i++) {
		std::vector<Vec3f> vertex;
		oneface = model.face(i);
		for (int j = 0; j < 3; j++) {
			Vec3f p1 = model.vert(oneface[j]);
			vertex.push_back(p1);
		}
		draw_soild_tranigle(model, image, color,vertex);
	}
}
void draw_empty_model(Model model,TGAImage&image,TGAColor color) {
	constexpr int wid = width / 2;
	constexpr int hei = height/ 2;

	std::vector<int> oneface;
	for (int i = 0; i < model.nfaces(); i++) {
		oneface = model.face(i);
		for (int j = 0; j < 3; j++) {
			Vec3f p1 = model.vert(oneface[j]);
			Vec3f p2 = model.vert(oneface[(j + 1) % 3]);
			int x0 = p1.x * wid/100 + wid;
			int y0 = p1.y * hei/100 + hei;
			int x1 = p2.x * wid/100 + wid;
			int y1 = p2.y * hei/100 + hei;
			line(x0, y0, x1, y1,image,color);
		}
	}
}

int main() {
	
	

        TGAImage image( width,height, TGAImage::RGB);
		Model model1("D:\\vs_project\\each_project\\tinyRender\\tinyRender\\dependence\\proaniya.obj");
		draw_empty_model(model1, image, white);
		
		
        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image

	    image.write_tga_file("output.tga");
	    
	
  
       return 0;
	
}

