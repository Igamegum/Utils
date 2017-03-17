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
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
	fmt.fmt.pix.width = mwidth;
	fmt.fmt.pix.height = mheight;
	ioctl(fd,VIDIOC_S_FMT,&fmt);


/////Check whether support 






	struct v4l2_fmtdesc fmtdesc;  
	fmtdesc.index=0;  
	fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;  
	while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)  
	{  
		if(fmtdesc.pixelformat & fmt.fmt.pix.pixelformat)  
		{  
			printf("format:%s\n",fmtdesc.description);  
			break;  
		}  
		fmtdesc.index++;  
	}  

/////Check whether support 




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

unsigned char *YUYV2BGR(const int width,const int height,unsigned char *data)
{
	unsigned char *bgr = (unsigned char *)malloc(height * width *3 *sizeof(unsigned char));
	int index = 0;
	int row; int col;
	for(int i=0;i<width*height*2;i+=4)
	{
		int y0 = data[i];
		int u = data[i+1];
		int y1 = data[i+2];
		int v = data[i+3];

		int R = y0 + 1.13983 *(v-128);
		int G = y0 - 0.39465 *(u-128) -0.58060*(v-128);
		int B = y0 + 2.03211 *(u-128);

		R = std::max(std::min(255,R),0); 
		G = std::max(std::min(255,G),0); 
		B = std::max(std::min(255,B),0); 

		row = index / (width*3);
		col = index % (width*3);
		bgr[index] = B;
		++index;

		row = index / (width*3);
		col = index % (width*3);
		bgr[index] = G;
		++index;

		row = index / (width*3);
		col = index % (width*3);
		bgr[index] = R;
		++index;

		R = y1 + 1.13983 *(v-128);
		G = y1 - 0.39465 *(u-128) -0.58060*(v-128);
		B = y1 + 2.03211 *(u-128);

		R = std::max(std::min(255,R),0); 
		G = std::max(std::min(255,G),0); 
		B = std::max(std::min(255,B),0); 

		row = index / (width*3);
		col = index % (width*3);
		bgr[index] = B;
		++index;

		row = index / (width*3);
		col = index % (width*3);
		bgr[index] = G;
		++index;

		row = index / (width*3);
		col = index % (width*3);
		bgr[index] = R;
		++index;

	}
	return bgr;
}


void BGR2MAT(const int width,const int height,unsigned char *data)
{

	cv::Mat src_img(height,width,CV_8UC3);
	int row;int col;
	for(int i=0;i<height * width *3;i++)
	{
		row = i/(width*3);
		col = i%(width*3);
		src_img.at<uchar>(row,col) = data[i];
	}
	cv::imshow("wemaefox",src_img);
	cv::waitKey(1);
}

unsigned char *Flip_BGR(const int width,const int height,unsigned char *data)
{
	unsigned char *ans = (unsigned char *) malloc(width * height *3*sizeof(unsigned char));
	int index = 0;

	for(int i=0;i<width;i++)
	{
		for(int j=height-1;j>=0;j--)
		{
			ans[index] = data[j*width*3+i*3]; index++;
			ans[index] = data[j*width*3+i*3+1]; index++;
			ans[index] = data[j*width*3+i*3+2]; index++;
		}
	}

	return ans;
}

unsigned char *Crop_Image(const int width,const int height,unsigned char *data,int sx,int sy,int endx,int endy)
{
	int n_width = endx - sx+1;
	int n_height = endy - sy+1;

	assert(n_width>0 && n_height>0 && sx>=0 && endx<width && sy>=0 && endy<height);

	unsigned char *ans = (unsigned char *)malloc(n_width * n_height *3*sizeof(unsigned char));

	int index = 0;
	for(int i=sy;i<=endy;i++)	
	{
		for(int j=sx;j<=endx;j++)
		{
			ans[index] = data[i*width*3+j*3];index++;
			ans[index] = data[i*width*3+j*3+1];index++;
			ans[index] = data[i*width*3+j*3+2];index++;
		}
	}

	return ans;
}
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

		unsigned char *ans = YUYV2BGR(640,480,data);
		BGR2MAT(640,480,ans);

//		unsigned char *wemadefox =Flip_BGR(640,480,ans);		
//		BGR2MAT(480,640,wemadefox);
//
//		unsigned char *crop = Crop_Image(640,480,ans,100,100,600,400);
//		BGR2MAT(501,301,crop);

		free(data);
	}
	

	return 0;
}
