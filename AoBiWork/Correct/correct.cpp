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
	std::string filename("depth_image/") ;
	filename += IntToString(index);
	filename += ".jpg";

	std::cout<<filename<<std::endl;

	/* std::ifstream ifs; */
	/* ifs.open(filename); */
	/*  int width; */
	/*  int height; */
	/* ifs>>height>>width; */
	/* cv::Mat src_img(height,width,CV_16UC1); */

	/* ifs.close(); */

	
	cv::Mat src_img = cv::imread(filename,cv::IMREAD_ANYDEPTH);

	cv::Mat src_img16(src_img.rows,src_img.cols,CV_16UC1);

//	std::cout<<src_img.elemSize()<<"  "<<src_img16.elemSize()<<std::endl;

	for(uint16_t i=0;i<src_img.rows;i++)
	{
		for(uint16_t j=0;j<src_img.cols;j++)
		{
			uint8_t * value = (uint8_t * ) src_img.ptr<uchar>(i)+(src_img.cols-1-j);

			/* uint8_t * depl = (uint8_t * ) src_img16.ptr<uchar>(i)+(j*2); */
			/* uint8_t * depr = (uint8_t * ) src_img16.ptr<uchar>(i)+(j*2+1); */
			/* *depl = *value; */
			/* *depr = *value; */

			 uint16_t * dep = (uint16_t * ) src_img16.ptr<uchar>(i)+(j);
			 *dep = *value*257;

		}
	}


	//再次翻转矫正后的深度图片
	//
//	cv::Mat deep_img = src_img16;
	cv::Mat deep_img = MappingDepth2Color(src_img16);

	cv::flip(deep_img,deep_img,1);

	cv::Mat src_img8(src_img16.rows,src_img16.cols,CV_8UC1);
	for(int i=0;i<src_img8.rows;i++)
	{
		for(int j=0;j<src_img8.cols;j++)
		{

			 uint16_t * dep = (uint16_t * ) deep_img.ptr<uchar>(i)+(j);
			 uint8_t * value = (uint8_t * ) src_img8.ptr<uchar>(i)+(j);
			 *value = *dep>>8;
		}
	}
	

	std::string savename("8dImage/");
	savename += IntToString(index);
	savename += ".jpg";
	cv::imwrite(savename,src_img8);
	return src_img8;
	
}
void Overlap(const int index)
{
	std::string filename("color_image/") ;
	filename += IntToString(index);
	filename += ".jpg";

	std::cout<<filename<<std::endl;

	/* std::ifstream ifs; */
	/* ifs.open(filename); */
	/*  int width; */
	/*  int height; */
	/* ifs>>height>>width; */
	/* cv::Mat src_img(height,width,CV_16UC3); */

	/* ifs.close(); */

	cv::Mat src_img = cv::imread(filename);
	cv::Mat src_img16(src_img.rows,src_img.cols,CV_16UC3);

//	std::cout<<src_img.elemSize()<<"  "<<src_img16.elemSize()<<std::endl;

	for(uint16_t i=0;i<src_img.rows;i++)
	{
		for(uint16_t j=0;j<src_img.cols;j++)
		{
			uint8_t * vb = (uint8_t * ) src_img.ptr<uchar>(i)+j*3;
			uint8_t * vg = (uint8_t * ) src_img.ptr<uchar>(i)+j*3+1;
			uint8_t * vr = (uint8_t * ) src_img.ptr<uchar>(i)+j*3+2;

			uint16_t * vb16 = (uint16_t * ) src_img16.ptr<uchar>(i)+j*3;
			uint16_t * vg16 = (uint16_t * ) src_img16.ptr<uchar>(i)+j*3+1;
			uint16_t * vr16 = (uint16_t * ) src_img16.ptr<uchar>(i)+j*3+2;

			*vb16 = *vb;
			*vg16 = *vg;
			*vr16 = *vr;
			
		}
	}

	cv::Mat deep_img = correct_deep(index);

	for(uint16_t i=0;i<src_img16.rows;i++)
	{
		for(uint16_t j=0;j<src_img16.cols;j++)
		{
			uint16_t * vb = (uint16_t * ) src_img16.ptr<uchar>(i)+j*3;
			uint16_t * vg = (uint16_t * ) src_img16.ptr<uchar>(i)+j*3+1;
			uint16_t * vr = (uint16_t * ) src_img16.ptr<uchar>(i)+j*3+2;


			uint8_t * vd = (uint8_t * ) deep_img.ptr<uchar>(i)+j;

			*vg += *vd;
			*vr += *vd;

		}
	}

	std::string savename("crImage/");
	savename += IntToString(index);
	savename += ".jpg";
	cv::imwrite(savename,src_img16);

}
int main()
{
	Init_Mat();
	for(int index = 0;index<600;index++)
	{
//		correct_deep(index);
		Overlap(index);
//		cv::waitKey(10000000);
	}
	return 0;
}
