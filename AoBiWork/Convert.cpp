#include <iostream>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <bits/stdc++.h>
#include <iostream>

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

void solveR(const int index)
{

	 int width;
	 int height;
	std::ifstream is;
	std::string filename("RGB/");
	filename += Prefix(index);
	filename += ".txt";
	is.open(filename);

	is>>height>>width;
	cv::Mat src_img(height,width,CV_8UC3);

	for(int i=0;i<src_img.rows;i++)
	{
		for(int j=0;j<src_img.cols;j++)
		{
			int r,g,b;
			is>>r>>g>>b;
			src_img.at<uchar>(i,j*3) = b;
			src_img.at<uchar>(i,j*3+1) = g;
			src_img.at<uchar>(i,j*3+2) = r;
		}
	}

	cv::imshow("thinkjoyR",src_img);

	std::string savename("rImage/");
	savename += Prefix(index);
	savename += ".jpg";
	cv::imwrite(savename,src_img);
}
void solveD(const int index)
{

	 int width;
	 int height;
	std::ifstream is;
	std::string filename("DEP/");
	filename += Prefix(index);
	filename += ".txt";
	is.open(filename);

	is>>height>>width;
	cv::Mat src_img(height,width,CV_8UC1);

	for(int i=0;i<src_img.rows;i++)
	{
		for(int j=0;j<src_img.cols;j++)
		{
			int dep;
			is>>dep;
			src_img.at<uchar>(i,j) = dep;
		}
	}

	cv::imshow("thinkjoyD",src_img);

	std::string savename("dImage/");
	savename += Prefix(index);
	savename += ".jpg";
	cv::imwrite(savename,src_img);
}
int main()
{


	 for(int index = 5;index<=1775;index+=5)
	 {
			solveD(index);
			solveR(index);
			cv::waitKey(20);
	 }
	return 0;
}
