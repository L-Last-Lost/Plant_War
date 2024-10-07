#ifndef _TIMER_H
#define _TIMER_H

#include <functional>

//ʹ�ûص���������Ҫ��ζ����µ�������ʵ���µļ�ʱ��

class Timer {
public:
	Timer() = default;
	~Timer() = default;

	void restart() {
		pass_time = 0;
		shotted = false;
	}

	void set_wait_time(int val) {
		wait_time = val;
	}

	void set_one_shot(bool flag) {
		one_shot = flag;
	}

	void on_update(int delta) {
		if (paused)
			return;

		pass_time += delta;
		if (pass_time >= wait_time) {
			//���ǵ��δ������ǵ��δ�����û�д��������Ѿ�ʵ�ֻص�����
			if ((!one_shot || (one_shot && !shotted))&& callback)
				callback();
			shotted = true;
			pass_time = 0;
		}
	}

	void set_callback(std::function<void()> callback) {
		this->callback = callback;
	}

	void pause(){
		paused = true;
	}

	void resume() {
		paused = false;
	}

private:
	int pass_time = 0;							   // �ѹ�ʱ��
	int wait_time = 0;							   // �ȴ�ʱ��
	bool paused = false;						   // �Ƿ���ͣ
	bool shotted = false;						   // �Ƿ񴥷�
	bool one_shot = false;						   // ���δ���
	std::function<void()> callback;				   // �����ص�

};


#endif // !_TIMER_H
