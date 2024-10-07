#ifndef _PLATFORM_H_
#define _PLATFORM_H_

#include "camera.h"
#include "util.h"

extern bool is_debug;

class Platform {
public:
	Platform() = default;
	~Platform() = default;

public:
	struct CollisionShape {
		float y;
		float left, right;
	};

public:
	// 数据部分
	CollisionShape shape;
	// 渲染部分
	IMAGE* img = nullptr;
	POINT render_position = { 0 };

	void on_draw(const Camera& camera) const{
		putimage_alpha(camera, render_position.x, render_position.y, img);
		// std::cout << "I am here!\n";
		
		if (is_debug) {
			setlinecolor(RGB(255, 0, 0));
			line(camera, (int)shape.left, (int)shape.y, (int)shape.right, (int)shape.y);
		}
	
	}

};

#endif // !_PLATFORM_H_
