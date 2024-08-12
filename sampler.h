#pragma once
#include "tgaimage.h"
#include "geometry.h"
class Sampler
{
public:

    Sampler(const char*filename):filename(filename){
		texture.read_tga_file(filename);
	}
	TGAColor color(float x,float y) {
	
		return texture.get(x * texture.get_width(), y * texture.get_height());
	}
	const char* filename;
	TGAImage texture;
};

