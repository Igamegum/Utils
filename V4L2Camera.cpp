#include <bits/stdc++.h>

#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <cstdio>

#include <sys/types.h>      
#include <sys/stat.h>   
#include <sys/mman.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>


typedef struct fimc_buffer
{
	unsigned char * start;
	size_t length;

}fimc_buffer;

typedef unsigned char  TYPE;

class Device_Camera
{
	private:
		int fd;
		int mwidth,mheight;
		struct fimc_buffer	*buffers;
		int bufnum;
	public:
		struct v4l2_buffer	v4l2_buf;
		void Open_Camera();
		void Init_Camera(const int ,const int ,const int);
		Device_Camera();
		void Steam_On();
		int Pop_Frame(TYPE *);
		void Push_Frame(const int);


};
Device_Camera::Device_Camera()
{
	fd = -1;   bufnum = 1;
	buffers = nullptr;

}
void Device_Camera::Open_Camera()
{
	fd = open("/dev/video0",O_RDWR);

	std::cout<<__FUNCTION__<<" "<<fd<<std::endl;
}
void Device_Camera::Init_Camera(const int _width,const int _height,const int _buffernum)
{
	bufnum = _buffernum;
	mwidth = _width;
	mheight = _height;

	struct v4l2_format fmt;
	struct v4l2_capability cap;

	int rc = ioctl(fd,VIDIOC_QUERYCAP,&cap);
	assert(rc>=0);


	memset(&fmt,0,sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR666;
	fmt.fmt.pix.width = mwidth;
	fmt.fmt.pix.height = mheight;
	ioctl(fd,VIDIOC_S_FMT,&fmt);

	struct v4l2_requestbuffers req;
	req.count = _buffernum;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	ioctl(fd,VIDIOC_REQBUFS,&req);


	buffers = (fimc_buffer*)calloc(req.count,sizeof(*buffers));
	assert(buffers != nullptr);

	for(int i=0;i<bufnum;i++)
	{
		memset(&v4l2_buf,0,sizeof(v4l2_buf));
		v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v4l2_buf.memory = V4L2_MEMORY_MMAP;
		v4l2_buf.index = i;

		int rc = ioctl(fd,VIDIOC_QUERYBUF,&v4l2_buf);
		assert(rc>=0);

		buffers[i].length = v4l2_buf.length;
		buffers[i].start = (unsigned char *) mmap(0, v4l2_buf.length,
				PROT_READ | PROT_WRITE, MAP_SHARED,
				fd, v4l2_buf.m.offset);

		assert(buffers[i].start>=0);

	}


}
void Device_Camera::Steam_On()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	for(int i=0;i<bufnum;i++)
	{
		memset(&v4l2_buf,0,sizeof(v4l2_buf));
		v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v4l2_buf.memory = V4L2_MEMORY_MMAP;
		v4l2_buf.index = i;
		
		int rc = ioctl(fd,VIDIOC_QBUF,&v4l2_buf);
		assert(rc>=0);
	}

	int rc = ioctl(fd,VIDIOC_STREAMON,&type);
	assert(rc>=0);

}

int Device_Camera::Pop_Frame(TYPE * data)
{
	v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2_buf.memory = V4L2_MEMORY_MMAP;

	int rc = ioctl(fd,VIDIOC_DQBUF,&v4l2_buf);
	assert(rc>=0);
	

	memcpy(data,buffers[v4l2_buf.index].start,buffers[v4l2_buf.index].length);

	return v4l2_buf.index;

}
void Device_Camera::Push_Frame(const int index)
{
		v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		v4l2_buf.memory = V4L2_MEMORY_MMAP;
		v4l2_buf.index = index;
		int rc = ioctl(fd,VIDIOC_QBUF,&v4l2_buf);
		assert(rc>=0);
}



std::string IntToString(const int num)
{
	std::stringstream ss;
	ss<<num;
	return ss.str();
}


const int Width = 640;
const int Height = 480;
const int Per = 2;

int main()
{


	Device_Camera dc;


	dc.Open_Camera();
	dc.Init_Camera(Width,Height,10);
	dc.Steam_On();


	std::cout<<"Frame Size:"<<dc.v4l2_buf.length<<std::endl;

	while(true)
	{
		TYPE *data = (TYPE *)malloc(sizeof(TYPE)*Width*Height*Per);
		

		dc.Pop_Frame(data);
		
		dc.Push_Frame((dc.v4l2_buf.index));

		cv::Mat src_img(Height,Width,CV_8UC3);


		for(int i=0;i<Height;i++)
		{
			for(int j=0;j<Width;j++)
			{
				src_img.at<uchar>(i,j*3) = data[i*Width*Per+j*Per];	
				src_img.at<uchar>(i,j*3+1) = data[i*Width*Per+j*Per];	
				src_img.at<uchar>(i,j*3+2) = data[i*Width*Per+j*Per];	
			}
		}
		
		cv::imshow("wemaefox",src_img);
		
		cv::waitKey(1);
		/* std::string filename(""); */
		/* filename += "weamdefox"; */
		/* filename += IntToString(i); */
		/* filename += ".jpg"; */

		/* std::ofstream on; */
		/* on.open(filename); */
		/* for(int i=0;i<640*480*2;i++) */
		/* { */
		/* 	on<<data[i]; */
		/* } */

		/* on.close(); */

		free(data);
	}
	
	std::cout<<"Final"<<std::endl;

	return 0;
}
