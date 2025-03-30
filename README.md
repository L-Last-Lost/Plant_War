
项目总结：

首先，提出场景，以此展现不同的内容
	而因为多场景的原因，我们定义**场景类**来充当父类
		其中最关键的三个要素，<font color="#ff0000">on_input, on_draw, on_update</font>
		分别用来<font color="#ff0000">获取输入，进行绘图，更新数据</font>
	要管理多场景，我们便定义了一个**场景管理器**来对场景进行更换

在main中，我们进行了资源加载和入口函数的调用

根据动画的理念，我们设计了**Atlas类**，<font color="#ff0000">批量加载和存储一套动画所需的图片素材</font>（不用每次需要的时候再加载）
同时设计了**Animation类**，去<font color="#ff0000">记录当前动画播放进度</font>（on_draw哪一张图片）

因为需要屏幕抖动，引入摄像机**Camera类**，充当绘图时的<font color="#ff0000">相对定位锚点</font>，因此on_draw方法必须都要包含Camera

需要一些有关时间的功能，因此引入**Timer类**，当时间到达指定的<font color="#ff0000">那一帧</font>时进行某个函数调用

因为有重力模拟和位置系统，引入**Vector2类**，进行一些数学运算

游戏截图：
![image](https://github.com/user-attachments/assets/bf574390-aafd-4901-948a-7d641e9c1799)

![image](https://github.com/user-attachments/assets/97f95c95-aa2b-452d-918f-6e7d41d08c6e)
