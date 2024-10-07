#ifndef _SCCENE_MANAGER_H_
#define _SCCENE_MANAGER_H_

#include "scene.h"

extern Scene* menu_scene;
extern Scene* selector_scene;
extern Scene* game_scene;

class SceneManager {
public:
	enum class SceneTpye {
		Menu,
		Game,
		Selector
	};

public:
	SceneManager() = default;
	~SceneManager() = default;

	void set_current_scene(Scene* scene) {
		current_scene = scene;
		current_scene->on_enter();
	}

	// 用枚举屏蔽管理器中的指针操作
	void switch_to(SceneTpye type) {
		current_scene->on_exit();
		switch (type)
		{
		case SceneManager::SceneTpye::Menu:
			current_scene = menu_scene;
			break;
		case SceneManager::SceneTpye::Game:
			current_scene = game_scene;
			break;
		case SceneManager::SceneTpye::Selector:
			current_scene = selector_scene;
			break;
		default:
			break;
		}

		current_scene->on_enter();
	}

	void on_update(int delta) {
		current_scene->on_update(delta);
	}

	void on_draw(const Camera& camera) {
		current_scene->on_draw(camera);
	}

	void on_input(const ExMessage& msg) {
		current_scene->on_input(msg);
	}
private:
	Scene* current_scene = nullptr;
};


#endif // !_SCCENE_MANAGER_H_
