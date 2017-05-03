#ifndef WEMADEFOX
#define WEMADEFOX


#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

double Kmatrix[9] = {959.052,0,339.982,0,1001.21,253.55,0,0,1};
double Rmatrix[9] = {0.999695,0.00621777,0.0239136,-0.0059439,0.999916,-0.0115068,-0.0239831,0.0113611,0.999648};
double Tmatrix[3] = {-25.1959,0.35221,0.761359};

void ShowMat(const cv::Mat src)
{
	for(int i=0;i<src.rows;i++)
	{
		for(int j=0;j<src.cols;j++)
		{
			std::cout<<src.at<float>(i,j)<<"  ";
		}
		std::cout<<std::endl;
	}
	std::cout<<"----------------------"<<std::endl;
}
cv::Mat GetInvert()
{
	cv::Mat src(3,3,CV_32FC1);
	cv::Mat dst(3,3,CV_32FC1);

	for(int i=0;i<src.rows;i++)
	{
		for(int j=0;j<src.cols;j++)
		{
			src.at<float>(i,j) = Kmatrix[i*src.cols+j];
		}
	}

	IplImage ss = src;	IplImage *simg=&ss;
	IplImage dd = dst;	IplImage *dimg=&dd;


	cvInvert(simg,dimg);
	return dst;
}

cv::Mat MatMul(const cv::Mat &A, const cv::Mat &B, double scales)//A*scales*B
{
	cv::Mat ans(A.rows,B.cols,CV_32FC1);

	for(int i=0;i<A.rows;i++)
	{
		for(int j=0;j<B.cols;j++)
		{
			double sum = 0;
			for(int k=0;k<A.cols;k++)
			{
				sum += A.at<float>(i,k)*B.at<float>(k,j);

			}	

			ans.at<float>(i,j) = sum*scales;

		}
	}

	return ans;
}
cv::Mat MatAdd(const cv::Mat &A,const cv::Mat &B)
{

	cv::Mat ans(A.rows,A.cols,CV_32FC1);
	for(int i=0;i<A.rows;i++)
	{
		for(int j=0;j<B.cols;j++)
		{
			ans.at<float>(i,j) = A.at<float>(i,j) + B.at<float>(i,j);
		}
	}
	return ans;


}
cv::Mat KMatrix()
{
	cv::Mat src(3,3,CV_32FC1);

	for(int i=0;i<src.rows;i++)
	{
		for(int j=0;j<src.cols;j++)
		{
			src.at<float>(i,j) = Kmatrix[i*src.cols+j];
		}
	}
	return src;
}
cv::Mat RotateMatrix()
{
	cv::Mat src(3,3,CV_32FC1);

	for(int i=0;i<src.rows;i++)
	{
		for(int j=0;j<src.cols;j++)
		{
			src.at<float>(i,j) = Rmatrix[i*src.cols+j];
		}
	}
	return src;
}
cv::Mat TranslateMatrix()
{
	cv::Mat src(3,1,CV_32FC1);

	for(int i=0;i<src.rows;i++)
	{
		for(int j=0;j<src.cols;j++)
		{
			src.at<float>(i,j) = Tmatrix[i*src.cols+j];
		}
	}
	return src;
}
cv::Mat Step_One(const int u,const int v,const int z)
{
	cv::Mat K  = GetInvert();
	cv::Mat src(3,1,CV_32FC1);
	src.at<float>(0,0) = u;
	src.at<float>(1,0) = v;
	src.at<float>(2,0) = 1;


	cv::Mat ans = MatMul(K,src,z);

	return ans;
}

cv::Mat Step_Two(const int u,const int v,const int z)
{
	cv::Mat temp = MatMul(RotateMatrix(),Step_One(u,v,z),1);
	cv::Mat ans = MatAdd(temp,TranslateMatrix());

	return ans;
}
cv::Mat Step_Three(const int u,const int v,const int z)
{
	cv::Mat ans = MatMul(KMatrix(),Step_Two(u,v,z),1.0/(z*1.0));


	return ans;
}


#endif
