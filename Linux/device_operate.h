#pragma once

#include <stdio.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/time.h>
#include <cstring>
#include <unistd.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/ioctl.h>
#include<linux/fb.h>
#include<Xlib.h>


//按下按键
void simulate_key_push(int fd,int kval)
{
	struct input_event event;

	gettimeofday(&event.time, 0);

	//按下kval键

	event.type = EV_KEY;

	event.value = 1;

	event.code = kval;

	write(fd, &event, sizeof(event));

	//同步，也就是把它报告给系统

	event.type = EV_SYN;

	event.value = 0;

	event.code = SYN_REPORT;

	write(fd, &event, sizeof(event));

}
//放开按键
void release_key(int fd,int kval)
{
	struct input_event event;

	gettimeofday(&event.time, 0);
	//松开kval键

	event.type = EV_KEY;

	event.value = 0;

	event.code = kval;

	write(fd, &event, sizeof(event));

	//同步，也就是把它报告给系统

	event.type = EV_SYN;

	event.value = 0;

	event.code = SYN_REPORT;

	write(fd, &event, sizeof(event));


}



//按键模拟，按键包含按下和松开两个环节

std::pair<int,int> getScreenInfo()
{
	char *display_name = NULL;
	Display *display = XOpenDisplay(display_name);
	
	Screen *screen = ScreenOfDisplay(display,0);
	int rows = HeightOfScreen(screen);
	int cols = WidthOfScreen(screen);


	XFlush(display);
	XCloseDisplay(display);
	return std::make_pair(rows,cols);

}

std::pair<double,double> calc_ratio(int _rows,int _cols)
{
	std::pair<int ,int > screen_info = getScreenInfo();

	return std::make_pair(screen_info.first * 2.0 / _rows, 
			screen_info.second * 2.0 / _cols
			);
}
void simulate_key(int fd, int kval)
{

	struct input_event event;
	gettimeofday(&event.time, 0);
	//按下kval键

	event.type = EV_KEY;
	event.value = 1;
	event.code = kval;
	write(fd, &event, sizeof(event));

	//同步，也就是把它报告给系统

	event.type = EV_SYN;
	event.value = 0;
	event.code = SYN_REPORT;
	write(fd, &event, sizeof(event));



	memset(&event, 0, sizeof(event));
	gettimeofday(&event.time, 0);

	//松开kval键

	event.type = EV_KEY;
	event.value = 0;
	event.code = kval;
	write(fd, &event, sizeof(event));

	//同步，也就是把它报告给系统

	event.type = EV_SYN;
	event.value = 0;
	event.code = SYN_REPORT;
	write(fd, &event, sizeof(event));

}



//鼠标移动模拟

void simulate_mouse(int fd, int rel_x, int rel_y)

{

	struct input_event event;
	gettimeofday(&event.time, 0);

	//x轴坐标的相对位移

	event.type = EV_REL;
	event.value = rel_x;
	event.code = REL_X;
	write(fd, &event, sizeof(event));

	//同步

	event.type = EV_SYN;
	event.value = 0;
	event.code = SYN_REPORT;
	write(fd, &event, sizeof(event));

	//y轴坐标的相对位移

	event.type = EV_REL;
	event.value = rel_y;
	event.code = REL_Y;
	write(fd, &event, sizeof(event));

	//同步

	event.type = EV_SYN;
	event.value = 0;
	event.code = SYN_REPORT;
	write(fd, &event, sizeof(event));

}

std::pair<int,int> GetCursoPos()
{
	Display *display;
	Window root;

	if ((display = XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "Cannot open local X-display.\n");
		return std::make_pair(0,0);
	}
	root = DefaultRootWindow(display);

	int x,y;
	int tmp;unsigned int tmp2;
	Window fromroot, tmpwin;
	XQueryPointer(display, root, &fromroot, &tmpwin, &x, &y, &tmp, &tmp, &tmp2);

	XFlush(display);
	XCloseDisplay(display);
	return std::make_pair(x,y);
}
void  SetCursoPos(const int _x,const int _y)
{
	Display *display;
	Window root;

	if ((display = XOpenDisplay(NULL)) == NULL) {
		fprintf(stderr, "Cannot open local X-display.\n");
		return ;
	}
	root = DefaultRootWindow(display);

	int tmp;
	XWarpPointer(display, None, root, 0, 0, 0, 0, _x, _y);

	XFlush(display);
	XCloseDisplay(display);
	return ;
}

