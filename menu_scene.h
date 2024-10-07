#ifndef _MENU_SCENE_H_
#define _MENU_SCENE_H_

#include "scene.h"
#include "scene_manager.h"
#include "camera.h"
#include "timer.h"
#include "animation.h"

#include <iostream>

extern IMAGE img_menu_background;

extern Atlas atlas_peashooter_run_right;

extern SceneManager scene_manager;

class MenuScene : public Scene {
public:
	MenuScene() = default;
	~MenuScene() = default;

	void on_enter() {
		mciSendString(_T("play bgm_menu repeat from 0"), NULL, 0, NULL);
	}

	void on_update(int delta) {

	}

	void on_draw(const Camera& camera) {
		putimage_alpha(camera, 0, 0, &img_menu_background);
	}

	void on_input(const ExMessage& msg) {
		if (msg.message == WM_KEYDOWN) {
			mciSendString(_T("play ui_confirm from 0"), NULL, 0, NULL);
			scene_manager.switch_to(SceneManager::SceneTpye::Selector);
		}
	}

	void on_exit() {
		std::cout << "���˵��˳�" << std::endl;
	}

private:
	Animation animation_peashooter_run_right;
	Camera camera;
	Timer timer;
};

#endif // !_MENU_SCENE_H_