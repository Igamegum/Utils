#include <iostream>
#include <opencv2/opencv.hpp>
#include <nnpack.h>

const int THREAD_NUM = 4;

int main()
{
	enum nnp_status init_status = nnp_initialize();
	if(init_status != nnp_status_success)
	{
		std::cout<<"Init fail"<<std::endl;
		return 1;
	}
	
	// 由代码自己决定计算卷积的算法
	enum nnp_convolution_algorithm Algorithm = nnp_convolution_algorithm_auto;

	const int batch_size = 1;
	const int input_channels = 1;
	const struct nnp_size input_matrix_size = {320,240};
	const struct nnp_size input_padding_size = {0,0};

	const struct nnp_padding input_padding = { 0, 0, 0, 0 };  

	const struct nnp_size kernel_size = {3,3};
	const int output_channels = 10;//10个卷积核

	const struct nnp_size output_matrix_size = 
	{
		input_matrix_size.width - kernel_size.width+1,
		input_matrix_size.height - kernel_size.height+1
	};

	float *input = new float[batch_size * input_channels * input_matrix_size.width*input_matrix_size.height*sizeof(float)];

	float *kernel = new float[kernel_size.width * kernel_size.height*output_channels*sizeof(float)];

	float *output = new float[batch_size * output_channels * output_matrix_size.width * output_matrix_size.height*sizeof(float)];

	float *bias = new float[output_channels*sizeof(float)];

	
	//开启4个线程加速
	pthreadpool_t threadpool = nullptr;
//	threadpool = pthreadpool_create(THREAD_NUM<<1);

	long long st,ed;//计算时间

	//初始化输入矩阵
	
	std::cout<<"total:"<<input_matrix_size.height*input_matrix_size.width*input_channels*batch_size<<std::endl;

	for(int c = 0; c<input_channels;c++)
	{
		for(int i=0;i<input_matrix_size.height;i++)
		{
			for(int j=0;j<input_matrix_size.width;j++)
			{
				input[c*input_matrix_size.width*input_matrix_size.height
				 + i*input_matrix_size.width + j] = (i*j);
			}
		}
	}

	//初始化卷积核
	for(int c = 0; c<output_channels;c++)
	{
		for(int i=0;i<kernel_size.height;i++)
		{
			for(int j=0;j<kernel_size.width;j++)
			{
				kernel[c*kernel_size.width*kernel_size.height + i *kernel_size.width + j] = ((c*i*j)&1)+432159;
			}
		}
	}

	//初始化bias
	for(int c = 0; c<output_channels;c++)
	{
		bias[c] = 1;
	}



	struct nnp_profile computation_profile;//use for compute time;  


	st = cvGetTickCount();

	for(int i=0;i<1;i++)
	{  
		nnp_convolution_output(Algorithm,  
				batch_size,  
				input_channels,  
				output_channels,  
				input_matrix_size,  
				input_padding,  
				kernel_size,  
				input,  
				kernel,  
				bias,  
				output,  
				threadpool,  
				&computation_profile);  
	}


	ed = cvGetTickCount();

	std::cout<<"total cost :"<<(ed-st)/(cvGetTickFrequency()*1000)<<" ms"<<std::endl;

/* 	for(int i=0;i<output_channels;i++) */
/* 	{ */
/* 		for(int j=0;j<output_matrix_size.height*output_matrix_size.width;j++) */
/* 		{ */
/* 			std::cout<<output[i*output_matrix_size.width * output_matrix_size.height+j]<<" "; */
/* 		} */
/* 		std::cout<<std::endl; */
/* 	} */

	
	delete [] input;
	delete [] kernel;
	delete [] output;
	
	return 0;
}
