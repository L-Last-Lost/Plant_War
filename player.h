#ifndef _PLAYER_H
#define _PLAYER_H

#include "camera.h"
#include "vector2.h"
#include "animation.h"
#include "player_id.h"
#include "platform.h"
#include "bullet.h"
#include "particle.h"


#include <graphics.h>

extern bool is_debug;

extern IMAGE img_1P_cursor;
extern IMAGE img_2P_cursor;

extern std::vector<Platform> platform_list;
extern std::vector<Bullet*> bullet_list;

extern Atlas atlas_run_effect;
extern Atlas atlas_jump_effect;
extern Atlas atlas_land_effect;

class Player {
public: 
	Player(bool facing_right = true):is_face_right(facing_right) {
		current_animation = is_face_right? &animation_idle_right : &animation_idle_left;

		timer_attack_cd.set_wait_time(attack_cd);
		timer_attack_cd.set_one_shot(true);
		timer_attack_cd.set_callback(
			[&]() {
				can_attack = true;
			}
		);

		timer_invulnerable.set_wait_time(750);
		timer_invulnerable.set_one_shot(true);
		timer_invulnerable.set_callback(
			[&]() {
				is_invulnerable = false;
			}
		);

		timer_invulnerable_blink.set_wait_time(75);
		timer_invulnerable_blink.set_callback(
			[&]() {
				is_showing_sketch_frame = !is_showing_sketch_frame;
			}
		);

		timer_run_effect_generation.set_wait_time(75);
		timer_run_effect_generation.set_callback(
			[&]() {
				Vector2 particle_position;
				IMAGE* frame = atlas_run_effect.get_image(0);
				particle_position.x = position.x + (size.x - frame->getwidth()) / 2;
				particle_position.y = position.y + size.y - frame->getheight();
				particle_list.emplace_back(Particle(particle_position, &atlas_run_effect, 45));
			}
		);

		timer_die_effect_generation.set_wait_time(35);
		timer_die_effect_generation.set_callback(
			[&]() {
				Vector2 particle_position;
				IMAGE* frame = atlas_run_effect.get_image(0);
				particle_position.x = position.x + (size.x - frame->getwidth()) / 2;
				particle_position.y = position.y + size.y - frame->getheight();
				particle_list.emplace_back(Particle(particle_position, &atlas_run_effect, 150));
			}
		);
	
		animation_jump_effect.set_atlas(&atlas_jump_effect);
		animation_jump_effect.set_interval(25);
		animation_jump_effect.set_loop(false);
		animation_jump_effect.set_callback(
			[&]() {
				is_jump_effect_visible = false;
			}
		);

		animation_land_effect.set_atlas(&atlas_land_effect);
		animation_land_effect.set_interval(50);
		animation_land_effect.set_loop(false);
		animation_land_effect.set_callback(
			[&]() {
				is_land_effect_visible = false;
			}
		);

		timer_cursor_visibility.set_wait_time(2500);
		timer_cursor_visibility.set_one_shot(true);
		timer_cursor_visibility.set_callback(
			[&]() {
				is_cursor_visible = false;
			}
		);

		timer_can_fall_platform.set_wait_time(50);
		timer_can_fall_platform.set_callback(
			[&]() {
				is_can_fall_platform = false;
			}
		);
	}

	~Player() = default;

	virtual void on_run(float distance) {
		if (is_attacking_ex)
			return;
		
		position.x += distance;
		timer_run_effect_generation.resume();
	}
	
	virtual void on_jump() {
		if (velocity.y != 0 || is_attacking_ex) return;
		velocity.y += jump_velocity;

		is_jump_effect_visible = true;
		animation_jump_effect.reset();

		IMAGE* effect_frame = animation_jump_effect.get_frame();
		position_jump_effect.x = position.x + (size.x - effect_frame->getwidth()) / 2;
		position_jump_effect.y = position.y + size.y - effect_frame->getheight();
		
	}

	virtual void on_down() {
		if (position.y + size.y > 400)return;
		if (velocity.y != 0 || is_attacking_ex) return;
		is_can_fall_platform = true;
		timer_can_fall_platform.restart();
	}

	virtual void on_land() {
		is_land_effect_visible = true;
		animation_land_effect.reset();

		IMAGE* effect_frame = animation_land_effect.get_frame();
		position_land_effect.x = position.x + (size.x - effect_frame->getwidth()) / 2;
		position_land_effect.y = position.y + size.y - effect_frame->getheight();

	}


	virtual void on_attack() {

	}

	virtual void on_attack_ex() {

	}


	virtual void on_update(int delta) {
		int direction = is_right_key_down - is_left_key_down; 

		if (direction != 0)
		{
			if(!is_attacking_ex)
				is_face_right = direction > 0;
			current_animation = is_face_right ? &animation_run_right : &animation_run_left;
			float distance = direction * delta * run_velocity;   //一帧移动的距离
			on_run(distance);
		}
		else {
			current_animation = is_face_right ? &animation_idle_right : &animation_idle_left;
			timer_run_effect_generation.pause();
		}

		if (is_attacking_ex)
			current_animation = is_face_right ? &animation_attack_ex_right : &animation_attack_ex_left;

		if (hp <= 0)
			current_animation = last_hurt_direction.x < 0 ? &animation_die_left : &animation_die_right;

		current_animation->on_update(delta);	
		animation_jump_effect.on_update(delta);
		animation_land_effect.on_update(delta);

		timer_attack_cd.on_update(delta);
		timer_invulnerable.on_update(delta);
		timer_invulnerable_blink.on_update(delta);
		timer_run_effect_generation.on_update(delta);
		timer_cursor_visibility.on_update(delta);
		timer_can_fall_platform.on_update(delta);

		if (hp <= 0)
			timer_die_effect_generation.on_update(delta);

		particle_list.erase(std::remove_if(
			particle_list.begin(), particle_list.end(),
			[](const Particle& particle) {
				return !particle.check_valid();
			}),
			particle_list.end());
		for (Particle& particle : particle_list)
			particle.on_update(delta);

		if (is_showing_sketch_frame)
			sketch_image(current_animation->get_frame(), &img_sketch);

		move_and_collide(delta);
	}

	virtual void on_draw(const Camera& camera) {
		if (is_jump_effect_visible)
			animation_jump_effect.on_draw(camera, (int)position_jump_effect.x, (int)position_jump_effect.y);

		if (is_land_effect_visible)
			animation_land_effect.on_draw(camera, (int)position_land_effect.x, (int)position_land_effect.y);


		for (const Particle& particle : particle_list)
			particle.on_draw(camera);

		if (hp > 0 && is_invulnerable && is_showing_sketch_frame)
			putimage_alpha(camera, (int)position.x, (int)position.y, &img_sketch);
		else
			current_animation->on_draw(camera, (int)position.x, (int)position.y);	
	
		if (is_cursor_visible) {
			switch (id)
			{
			case PlayerID::p1:
				putimage_alpha(camera, (int)(position.x + (size.x - img_1P_cursor.getwidth()) / 2),
					(int)(position.y - img_1P_cursor.getheight()), &img_1P_cursor);

				break;
			case PlayerID::p2:
				putimage_alpha(camera, (int)(position.x + (size.x - img_2P_cursor.getwidth()) / 2),
					(int)(position.y - img_2P_cursor.getheight()), &img_2P_cursor);

				break;
			default:
				break;
			}
		}

		if (is_debug) {
			setlinecolor(RGB(0, 125, 255));
			rectangle((int)position.x, (int)position.y, (int)(position.x + size.x), (int)(position.y + size.y));
		}
	}

	virtual void on_input(const ExMessage& msg) {
		switch (msg.message)
		{
		case WM_KEYDOWN:
			switch (id)
			{
			case PlayerID::p1:
				switch (msg.vkcode)
				{
				case 0x41:
					is_left_key_down = true;
					break;
				case 0x44:
					is_right_key_down = true;
					break;
				case 0x57:
					on_jump();
					break;
				case 0x53:
					on_down();
					break;
				case 0x4A:
					if (can_attack) {
						on_attack();
						timer_attack_cd.restart();
						can_attack = false;
					}
					break;
				case 0x4B:
					if (mp>=100) {
						on_attack_ex();
						mp = 0;
					}
					break;
				default:
					break;
				}

				// std::cout << msg.vkcode << std::endl;
				break;
			case PlayerID::p2:
				switch (msg.vkcode)
				{
				case VK_LEFT:
					is_left_key_down = true;
					break;
				case VK_RIGHT:
					is_right_key_down = true;
					break;
				case VK_UP:
					on_jump();
					// std::cout << VK_UP;
					break;
				case VK_DOWN:
					on_down();
					break;
				case VK_NUMPAD1:
					// std::cout << "hi\n";
					if (can_attack) {
						on_attack();
						timer_attack_cd.restart();
						can_attack = false;
					}
					break;
				case VK_NUMPAD2:
					if (mp >= 100) {
						on_attack_ex();
						mp = 0;
					}
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			break; 

		case WM_KEYUP:
			switch (id)
			{
			case PlayerID::p1:
				switch (msg.vkcode)
				{
				case 0x41:
					is_left_key_down = false;
					break;
				case 0x44:
					is_right_key_down = false;
					break;
				default:
					break;
				}
				break;
			case PlayerID::p2:
				switch (msg.vkcode)
				{
				case VK_LEFT:
					is_left_key_down = false;
					break;
				case VK_RIGHT:
					is_right_key_down = false;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	void set_id(PlayerID id) {
		this->id = id;
	}

	void set_position(float x, float y) {
		position.x = x;
		position.y = y;
	}

	int get_hp()const {
		return hp;
	}
	
	int get_mp()const {
		return mp;
	}

	void set_hp(int val) {
		hp = val;
	}
	
	const Vector2& get_position()const {
		return position;
	}

	const Vector2& get_size()const {
		return size;
	}

	void make_invulnerable() {
		is_invulnerable = true;
		timer_invulnerable.restart();
	}

protected:
	void move_and_collide(int delta){
		float last_velocity_y = velocity.y;

		velocity.y += gravity * delta;						// 速度改变量
		position += velocity * (float)delta;				// 根据速度改变位置
	
		if (hp <= 0)
			return;

		if(!is_can_fall_platform){
			if (velocity.y > 0) {
				for (const Platform& platform : platform_list) {
					const Platform::CollisionShape& shape = platform.shape;
					bool is_collide_x = (max(position.x + size.x, shape.right) - min(position.x, shape.left)
						<= size.x + (shape.right - shape.left));
					bool is_collide_y = (shape.y >= position.y && shape.y <= position.y + size.y);

					if (is_collide_x && is_collide_y) {
						float delta_pos_y = velocity.y * delta;
						float last_tick_foot_pos_y = position.y + size.y - delta_pos_y;
						if (last_tick_foot_pos_y <= shape.y) {
							position.y = shape.y - size.y;
							velocity.y = 0;

							if (last_velocity_y != 0)
								on_land();

							break;
						}
					}

				}
			}
		}

		if(!is_invulnerable){
			for (Bullet* bullet : bullet_list) {
				if (!bullet->get_valid() || bullet->get_collide_target() != id)
					continue;

				if (bullet->check_collsion(position, size)) {
					make_invulnerable();
					bullet->on_collide();
					bullet->set_valid(false);
					hp -= bullet->get_damage();
					last_hurt_direction = bullet->get_position() - position;
					if (hp <= 0) {
						velocity.x = last_hurt_direction.x < 0 ? 0.35f : -0.35f;
						velocity.y = -1.0f;
					}
				}
			}
		}

	}

protected:
	int mp = 0;
	int hp = 100;

	const float run_velocity = 0.55f;
	const float gravity = 1.6e-3f;
	const float jump_velocity = -0.85f;


	Vector2 size;
	Vector2 position;
	Vector2 velocity;

	Animation animation_idle_left;
	Animation animation_idle_right;
	Animation animation_run_left;
	Animation animation_run_right;

	Animation animation_attack_ex_left;
	Animation animation_attack_ex_right;

	Animation animation_jump_effect;
	Animation animation_land_effect;

	Animation animation_die_left;
	Animation animation_die_right;

	bool is_jump_effect_visible = false;
	bool is_land_effect_visible = false;

	Vector2 position_jump_effect;
	Vector2 position_land_effect;

	Animation* current_animation = nullptr;

	PlayerID id = PlayerID::p1;

	bool is_left_key_down = false;
	bool is_right_key_down = false;

	bool is_face_right = true;

	bool can_attack = true;

	Timer timer_attack_cd;
	int attack_cd = 500;

	bool is_attacking_ex = false;

	bool is_invulnerable = false;
	bool is_showing_sketch_frame = false;

	IMAGE img_sketch;
	Timer timer_invulnerable;
	Timer timer_invulnerable_blink;

	std::vector<Particle> particle_list;

	Timer timer_run_effect_generation;
	Timer timer_die_effect_generation;

	bool is_cursor_visible = true;
	Timer timer_cursor_visibility;

	Vector2 last_hurt_direction;

	Timer timer_can_fall_platform;
	bool is_can_fall_platform = false;
};


#endif // !_PLAYER_H
