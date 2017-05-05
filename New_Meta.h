
#ifndef NEW_META
#define NEW_META

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <fstream>
#include <string>


double LK_Matrix[16] = {959.052,0,339.982,0,0,1001.21,253.55,0,0,0,1,0,0,0,0,1};
double RK_Matrix[16] = {851.156,0,303.213,0,0,888.217,247.435,0,0,0,1,0,0,0,0,1};
double R2L_Matrix[16] = {0.999695,0.00621777,0.0239136,-25.1959,-0.0059439,0.999916,-0.0115068,0.35221,-0.0239831,0.0113611,0.999648,0.761359,0,0,0,1.0};


cv::Mat LK_Mat;
cv::Mat RK_Mat;
cv::Mat R2L_Mat;
cv::Mat T_Mat;

void Init_Mat()
{
	cv::Mat LK_Mat_temp(4,4,CV_32F);
	cv::Mat RK_Mat_temp(4,4,CV_32F);
	cv::Mat R2L_Mat_temp(4,4,CV_32F);

	for(int i=0;i<4;i++)	
	{
		for(int j=0;j<4;j++)
		{
			LK_Mat_temp.at<float>(i,j) = LK_Matrix[i*4+j];
			RK_Mat_temp.at<float>(i,j) = RK_Matrix[i*4+j];
			R2L_Mat_temp.at<float>(i,j) = R2L_Matrix[i*4+j];
		}

	}

	LK_Mat = LK_Mat_temp;
	RK_Mat = RK_Mat_temp;
	R2L_Mat = R2L_Mat_temp;

}
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
cv::Mat GetInvert(const cv::Mat &src)
{
	
	cv::Mat dst(src.rows,src.cols,CV_32FC1);
	IplImage ss = src;	IplImage *simg=&ss;
	IplImage dd = dst;	IplImage *dimg=&dd;

	cvInvert(simg,dimg);
	return dst;
}
cv::Mat MatMul(const cv::Mat &A, const cv::Mat &B, float scales)//A*scales*B
{
	cv::Mat ans(A.rows,B.cols,CV_32FC1);

	for(int i=0;i<A.rows;i++)
	{
		for(int j=0;j<B.cols;j++)
		{
			float sum = 0;
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
cv::Mat Calc_All_Mat()
{
	cv::Mat ans = MatMul(MatMul(RK_Mat,R2L_Mat,1.0),GetInvert(LK_Mat),1.0);
//	ShowMat(ans);
	return ans;
}

void MappingDepth2Color(const cv::Mat &src, cv::Mat &dst)
{
	cv::Mat all = Calc_All_Mat();
	double  z;
	int  u = 0, v = 0, d = 0;
	int  u_rgb = 0, v_rgb = 0;



	for (v = 0; v < src.rows; v++)
	{
		for (u = 0; u < src.cols; u++)
		{
			d = src.at<uchar>(v, u);

			z = (double)d;
			u_rgb = (int)(all.at<float>(0,0) * (double)u + all.at<float>(0,1) * (double)v + all.at<float>(0,2) + all.at<float>(0,3) / z);
			v_rgb = (int)(all.at<float>(1,0) * (double)u + all.at<float>(1,1) * (double)v + all.at<float>(1,2) + all.at<float>(1,3) / z);


			if (u_rgb < 0 || u_rgb >= dst.cols || v_rgb < 0 || v_rgb >= dst.rows) continue;

			dst.at<uchar>(v_rgb,u_rgb) = d;

		}
	}

}

cv::Mat MappingDepth2Color(cv::Mat &src)
{


	cv::Mat all = Calc_All_Mat();
	double  z;
	uint16_t u, v, d;
	uint16_t u_rgb = 0, v_rgb = 0;
	cv::Mat newdepth(src.rows, src.cols, CV_16UC1, cv::Scalar(0));
	for (v = 0; v < src.rows; v++)
	{
		for (u = 0; u < src.cols; u++)
		{
			d = src.at<uint16_t>(v, u);
			z = (double)d;

			u_rgb = (uint16_t)(all.at<float>(0,0) * (double)u + all.at<float>(0,1) * (double)v + all.at<float>(0,2) + all.at<float>(0,3) / z);
			v_rgb = (uint16_t)(all.at<float>(1,0) * (double)u + all.at<float>(1,1) * (double)v + all.at<float>(1,2) + all.at<float>(1,3) / z);

			if (u_rgb < 0 || u_rgb >= newdepth.cols || v_rgb < 0 || v_rgb >= newdepth.rows) continue;
			uint16_t *val = (uint16_t *)newdepth.ptr<uchar>(v_rgb)+u_rgb;
			*val = d;
		}
	}
	
	return newdepth;

}


#endif
