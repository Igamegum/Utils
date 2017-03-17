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

void DrawRect(const int width,const int height,unsigned char *data,int sx,int sy,int endx,int endy)
{
	
	int n_width = endx - sx+1;
	int n_height = endy - sy+1;

	assert(n_width>0 && n_height>0 && sx>=0 && endx<width && sy>=0 && endy<height);

	for(int i=sy;i<=endy;i++)
	{
		data[i*width*3+sx*3] = 113;
		data[i*width*3+sx*3+1] = 179;
		data[i*width*3+sx*3+2] = 60;

		data[i*width*3+endx*3] = 113;
		data[i*width*3+endx*3+1] = 179;
		data[i*width*3+endx*3+2] = 60;

	}
	for(int i=sx;i<=endx;i++) 
	{
		data[sy*width*3+i*3] = 113;
		data[sy*width*3+i*3+1] = 179;
		data[sy*width*3+i*3+2] = 60;

		data[endy*width*3+i*3] = 113;
		data[endy*width*3+i*3+1] = 179;
		data[endy*width*3+i*3+2] = 60;
	}
	
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

typedef struct
{
	int B;int R;int G;
}Color;

////////////////////////////////////////////////////////////////////////////////////////////////////
inline int Fast_MulI( int a, int b)
{
	int res = 0;
	while(b) 
	{
		if(b&1) res = res + a;
		b>>=1;
		a<<=1; 
	}
	return res; 
}

inline double Fast_MulD(double a,int b)
{
	double res;
	while(b)
	{
		if(b&1) res = res + a;
		b>>=1;
		a*=2.0;
	}
	return res;
}
Color FastGetColor(unsigned char *data, int x, int y,const int width,const int height)
{
	x = std::max(std::min(x,width-1),0);
	y = std::max(std::min(y,height-1),0);

	Color ans;
	int index = Fast_MulI(y,width);
//	int index = y*width;
//	index = Fast_MulI(index,3);
	index *=3;

//	int baseR = Fast_MulI(x,3);
	int baseR = x*3;

	ans.B = data[index+baseR];
	ans.G = data[index+baseR+1];
	ans.R = data[index+baseR+2];

	return ans;
	
}
Color FastInsertValue(unsigned char *data,const int x,const int y,const double w_rate,const double h_rate,const int width,const int height)
{
	Color ans;

	Color color00 = FastGetColor(data,x,y,width,height); 
	Color color10 = FastGetColor(data,x+1,y,width,height); 
	Color color01 = FastGetColor(data,x,y+1,width,height); 
	Color color11 = FastGetColor(data,x+1,y+1,width,height); 

	double p1 = (1-w_rate)*(1-h_rate);
	double p2 = (1-w_rate)*(h_rate);
	double p3 = (w_rate)*(1-h_rate);
	double p4 = (w_rate)*(h_rate);

	ans.B = Fast_MulD(p1,color00.B) + Fast_MulD(p2,color01.B)+
			Fast_MulD(p3,color10.B)+ Fast_MulD(p4,color11.B);

	ans.G = Fast_MulD(p1,color00.G) + Fast_MulD(p2,color01.G)+
			Fast_MulD(p3,color10.G)+ Fast_MulD(p4,color11.G);

	ans.R = Fast_MulD(p1,color00.R) + Fast_MulD(p2,color01.R)+
			Fast_MulD(p3,color10.R)+ Fast_MulD(p4,color11.R);
	return ans;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//

Color GetColor(unsigned char *data, int x, int y,const int width,const int height)
{
	x = std::max(std::min(x,width-1),0);
	y = std::max(std::min(y,height-1),0);

	Color ans;
	ans.B = data[y*width*3+x*3];
	ans.G = data[y*width*3+x*3+1];
	ans.R = data[y*width*3+x*3+2];

	return std::move(ans);
	
}
Color InsertValue(unsigned char *data,const int x,const int y,const double w_rate,const double h_rate,const int width,const int height)
{
	Color ans;

	Color color00 = GetColor(data,x,y,width,height); 
	Color color10 = GetColor(data,x+1,y,width,height); 
	Color color01 = GetColor(data,x,y+1,width,height); 
	Color color11 = GetColor(data,x+1,y+1,width,height); 

	ans.B = (1-w_rate)*(1-h_rate)*color00.B + (1-w_rate)*h_rate*color01.B+
			w_rate*(1-h_rate)*color10.B+w_rate*h_rate*color11.B;

	ans.G = (1-w_rate)*(1-h_rate)*color00.G + (1-w_rate)*h_rate*color01.G+
			w_rate*(1-h_rate)*color10.G+w_rate*h_rate*color11.G;

	ans.R = (1-w_rate)*(1-h_rate)*color00.R + (1-w_rate)*h_rate*color01.R+
			w_rate*(1-h_rate)*color10.R+w_rate*h_rate*color11.R;
	return ans;
}

unsigned char * Resize(const int width,const int height,unsigned char *data,const int re_width,const int re_height)
{

	double w_rate = re_width*1.0/width;
	double h_rate = re_height*1.0/height;

	unsigned char * wemadefox = (unsigned char *)malloc(re_height * re_width *3 *sizeof(unsigned char));

	int index = 0;
	for(int i=0;i<re_height;i++)
	{
		for(int j=0;j<re_width;j++)
		{
			int origin_x = (j/w_rate);
			int origin_y = (i/h_rate);

			Color ans = InsertValue(data,origin_x,origin_y,w_rate,h_rate,width,height); 

			wemadefox[index] = ans.B; index++;
			wemadefox[index] = ans.G; index++;
			wemadefox[index] = ans.R; index++;
		}
	}

	return wemadefox;
}

int main()
{


	Device_Camera dc;

	dc.Open_Camera();
	dc.Init_Camera(Width,Height,10);
	dc.Steam_On();


	std::cout<<"Frame Size:"<<dc.v4l2_buf.length<<std::endl;
	
	long long st,ed;

	while(true)
	{
		TYPE *data = (TYPE *)malloc(sizeof(TYPE)*Width*Height*Per);

		dc.Pop_Frame(data);
		
		dc.Push_Frame((dc.v4l2_buf.index));

		unsigned char *ans = YUYV2BGR(640,480,data);
//		BGR2MAT(640,480,ans);

//		DrawRect(640,480,ans,100,100,600,400);
		
		st = cvGetTickCount();
		unsigned char *we = Resize(640,480,ans,1920,1080);
		ed = cvGetTickCount();

		std::cout<<"Cost Time is"<< (ed - st)/(cvGetTickFrequency()*1000)<<"ms"<<std::endl;

		BGR2MAT(1920,1080,we);

//		unsigned char *wemadefox =Flip_BGR(640,480,ans);		
//		BGR2MAT(480,640,wemadefox);
//
//		unsigned char *crop = Crop_Image(640,480,ans,100,100,600,400);
//		BGR2MAT(501,301,crop);
	
		free(we);
		free(ans);
		free(data);
	}
	
	return 0;
}
