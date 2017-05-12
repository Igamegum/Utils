#include <opencv2/opencv.hpp>
#include <iostream>
#include <zhelpers.h>
#include <czmq.h>
#include <string>

#include "ConsJson.h"
#include "thinkjoy_utils.h"



class Service
{
	private:
		ConsJson consjson;
		void *context;
		void *dealer;
	public:
		int  Init();
		void Clean();
		void OpenVideo(std::string );



		std::vector<std::string> Extract_FaceId( zframe_t *);
		std::vector<std::string> Detect_Frame(cv::Mat,const std::string,const bool,const bool);


		void AddToFacesetByImage(cv::Mat,const std::string,const std::string);
		void AddToFacesetByFaceId(const std::vector<std::string> ,const std::string,const std::string,const std::string);
};

int Service::Init()
{
	context = zmq_ctx_new();
	dealer = zmq_socket(context,ZMQ_DEALER);
	int rc = zmq_connect(dealer,Send_Address.c_str());
	
	if(rc != 0 ){
		std::cout<<"ZMQ_CONNECT FAIL"<<std::endl;
	}else{
		std::cout<<"ZMQ_CONNECT SUCCESS"<<std::endl;
	}
	return rc;
}
void Service::Clean()
{
	zmq_close(dealer);
	zmq_ctx_destroy(context);
}
void Service::OpenVideo(std::string filename)
{
	if(filename.size() <= 0 || filename.length() <=0){
		filename = "0";
	}
	cv::VideoCapture pCapture(filename);

	cv::Mat src_img;
	while(true)
	{
		pCapture >> src_img;
//		cv::resize(src_img,src_img,cv::Size(640,480));
		cv::imshow("thinkjoy",src_img);
		cv::waitKey(1);
		if( src_img.empty() || src_img.rows<=0 || src_img.cols<=0) break;

		std::vector<std::string> faceIds  = Detect_Frame(src_img,UserId,true,true);
		AddToFacesetByFaceId(faceIds,UserId,"",FacesetName);
	}
}

std::vector<std::string> Service::Extract_FaceId(zframe_t * frame)
{
	std::vector<std::string> ans;
	
	int length = zframe_size(frame);
	byte *data = get_zframe_data(frame);


	std::string message("");
	for(int i=0;i<length;i++){
		message += data[i];
	}

	Json::Value value;
	Json::Reader reader;
	if(reader.parse(message,value)) 
	{
		if(!value["faces"].isNull())
		{
			Json::Value face_array = value["faces"];
			for(int i=0; i<face_array.size(); i++)
			{
				ans.push_back(face_array[i]["faceId"].asString());
			}
		}
	}

	return ans;
}
std::vector<std::string>  Service::Detect_Frame(cv::Mat src_img,const std::string userId,const bool need_age,const bool need_sex)	
{
	
 	zmsg_t *message = zmsg_new();

	zframe_t *empty = zframe_new("",0);
	zmsg_append(message,&empty);

	std::string Json = consjson.detect_api(userId,1,need_age,need_sex);
	zframe_t *json = zframe_new(Json.c_str(),Json.size());
	zmsg_append(message,&json);

	std::vector<byte> src_img_vec;
	cv::imencode(".jpg",src_img,src_img_vec);


	
	zframe_t *byte_frame = zframe_new(src_img_vec.data(),src_img_vec.size());
	zmsg_append(message,&byte_frame);

	zmsg_send(&message,dealer);


	zmsg_t* reply = zmsg_recv(dealer);
	zmsg_print(reply);


	zframe_t *it_frame = zmsg_first(reply);
	it_frame  = zmsg_next(reply) ;

	std::vector<std::string> faceIds = Extract_FaceId(it_frame);

	for(int i=0 ; i<faceIds.size(); i++){
		std::cout<<faceIds[i]<<std::endl;
	}
	
	zmsg_destroy(&reply);

	return faceIds;

}
void Service::AddToFacesetByFaceId(const std::vector<std::string> faceIds,const std::string userId,const std::string facesetId,const std::string facesetName)
{
	zmsg_t *message = zmsg_new();

	zframe_t *empty = zframe_new("",0);
	zmsg_append(message,&empty);

	std::string Json = consjson.addface_api_faceids(faceIds,userId,facesetId,facesetName);

	std::cout<<Json<<std::endl;
	zframe_t *json = zframe_new(Json.c_str(),Json.size());
	zmsg_append(message,&json);

	zmsg_send(&message,dealer);


	zmsg_t* reply = zmsg_recv(dealer);
	zmsg_print(reply);
	
}
int main()
{

	Service se;
	se.Init();
	se.OpenVideo("SLL.MOV");


	/* cv::Mat src_img = cv::imread("mmp.jpg",cv::IMREAD_COLOR); */
	/* std::vector<std::string> faceIds = se.Detect_Frame(src_img,"9527",true,true); */
	/* se.AddToFacesetByFaceId(faceIds,"9527","","infi"); */

	se.Clean();
	return 0;
}
