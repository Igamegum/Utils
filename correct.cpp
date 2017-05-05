#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "New_Meta.h"

std::string IntToString(const int val)
{
	std::stringstream ss;
	ss<<val;
	return ss.str();
}

std::string Prefix(const int val)
{
	std::string num = IntToString(val);
	std::string zero;
	for(int i=0;i<5 - num.length();i++)zero+='0';
	zero+=num;
	return zero;
}
cv::Mat correct_deep(const int index)
{
	std::string filename("DEP/") ;
	filename += Prefix(index);
	filename += ".txt";

	std::cout<<filename<<std::endl;

	std::ifstream ifs;
	ifs.open(filename);
	 int width;
	 int height;
	ifs>>height>>width;
	cv::Mat src_img(height,width,CV_16UC1);

	for(uint16_t i=0;i<src_img.rows;i++)
	{
		for(uint16_t j=0;j<src_img.cols;j++)
		{
			int value;
			ifs>>value;
			value*=256;
			uint16_t * dep = (uint16_t * ) src_img.ptr<uchar>(i)+(src_img.cols-1-j);
			*dep = value;
		}
	}
	ifs.close();

	cv::Mat deep_img = MappingDepth2Color(src_img);
	std::string savename("cdImage/");
	savename += Prefix(index);
	savename += ".jpg";
	cv::imwrite(savename,deep_img);
	return deep_img;
	
}
void Overlap(const int index)
{
	std::string filename("RGB/") ;
	filename += Prefix(index);
	filename += ".txt";

	std::cout<<filename<<std::endl;

	std::ifstream ifs;
	ifs.open(filename);
	 int width;
	 int height;
	ifs>>height>>width;
	cv::Mat src_img(height,width,CV_16UC3);

	for(uint16_t i=0;i<src_img.rows;i++)
	{
		for(uint16_t j=0;j<src_img.cols;j++)
		{
			int r,g,b;
			ifs>>r>>g>>b;
			/* uint16_t * vb = (uint16_t * ) src_img.ptr<uchar>(i)+j*3; */
			/* uint16_t * vg = (uint16_t * ) src_img.ptr<uchar>(i)+j*3+1; */
			/* uint16_t * vr = (uint16_t * ) src_img.ptr<uchar>(i)+j*3+2; */

			uint16_t * vb = (uint16_t * ) src_img.ptr<uchar>(i)+(src_img.cols-1-j)*3;
			uint16_t * vg = (uint16_t * ) src_img.ptr<uchar>(i)+(src_img.cols-1-j)*3+1;
			uint16_t * vr = (uint16_t * ) src_img.ptr<uchar>(i)+(src_img.cols-1-j)*3+2;
			*vb = b;
			*vg = g;
			*vr = r;
			
		}
	}

	cv::Mat deep_img = correct_deep(index);

	for(uint16_t i=0;i<src_img.rows;i++)
	{
		for(uint16_t j=0;j<src_img.cols;j++)
		{
			uint16_t * vb = (uint16_t * ) src_img.ptr<uchar>(i)+j*3;
			uint16_t * vg = (uint16_t * ) src_img.ptr<uchar>(i)+j*3+1;
			uint16_t * vr = (uint16_t * ) src_img.ptr<uchar>(i)+j*3+2;


			uint16_t * vd = (uint16_t * ) deep_img.ptr<uchar>(i)+j;

			*vg = *vd;
			*vr = *vd;


		}
	}
	ifs.close();

	std::string savename("crImage/");
	savename += Prefix(index);
	savename += ".jpg";
	cv::imwrite(savename,src_img);

}
int main()
{
	Init_Mat();
	for(int index = 5;index<=2105;index+=5)
	{
	//	correct_deep(index);
		Overlap(index);
		cv::waitKey(1);
	}
	return 0;
}
