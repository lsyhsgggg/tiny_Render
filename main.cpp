#include "tgaimage.h"
#include<iostream>
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
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
int main() {

        TGAImage image(100, 100, TGAImage::RGB);


		line(10, 0, 13, 5, image, red);


        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image

	    image.write_tga_file("output.tga");
	    
	
  
       return 0;
	
}

