#ifndef _SCENE_H_
#define _SCENE_H_

#include "camera.h"

#include<graphics.h>

class Scene {
public:
	Scene() = default;
	~Scene() = default;

	virtual void on_enter(){}  // 进入
	virtual void on_update(int delta){} // 处理数据
	virtual void on_draw(const Camera& camera){}   // 渲染图片
	virtual void on_input(const ExMessage& msg){} // 输入数据
	virtual void on_exit(){} // 退出

private:

};



#endif // !_SCENE_H_
