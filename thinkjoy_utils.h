#pragma once

#include "czmq.h"
#include "zmq.h"
#include "zhelpers.h"
#include "bits/stdc++.h"
#include "opencv2/opencv.hpp"
#include "mysqlDAO.h"
#include "thread"
#include "Property.h"
#include "jsoncpp/json/json.h"
//#include <jsoncpp/json.h>
#include <glog/logging.h>

std::time_t parseTime(const std::string time_stamp)
{
	struct tm  rtm;

	strptime(time_stamp.c_str(), "%Y-%m-%d %H:%M:%S", &rtm);

	std::time_t ans = mktime(&rtm);
	return ans;
}

std::string getTimeStamp() 
{
	std::time_t rawtime;
	long long currentTime = time(&rawtime);

	struct tm tm = *localtime((time_t *)&currentTime);
	char res[30];
	strftime(res, 29, "%Y-%m-%d %H:%M:%S", &tm);
	res[29] = '\0';

	std::string result = res;
	return result;
}

int get_return_code(zframe_t *self)
{

	if(self == nullptr) return -1;
	int   _length = zframe_size(self);
	if(_length<=0) return -1;
	byte *data = zframe_data(self);

	std::string str("");
	for(int i=0;i<_length;i++){
		str+= data[i];
	}
	Json::Value value;
	Json::Reader reader;
	if(reader.parse(str,value)){
		return value["return_code"].asInt();
	}
	return -1;
}

std::string& replace_all_distinct(std::string &str, const std::string &old_value, const std::string &new_value)  
{  
	for(std::string::size_type pos(0); pos!=std::string::npos; pos+=new_value.length())   {  
		if( (pos=str.find(old_value,pos))!=std::string::npos )  
			str.replace(pos,old_value.length(),new_value);  
		else break;  
	}  
	return str;  
} 


std::string Byte2Binary(byte num) {
	std::string res = "", one = "1", zero = "0";
	unsigned char temp = num;
	while (temp) {
		if (temp % 2)
			res =  one + res;
		else 
			res = zero + res;
		temp /= 2;
	}

	while (res.size() != 8) 
		res = std::string("0") + res;

	return res;
}

byte Binary2Byte(std::string num) {
	int res = 0;
	int w = 1;
	for (int i = num.size() - 1; i >= 0; i--) {
		res += w * (num[i] - '0') ;
		w *= 2;
	}
	byte rr = res;
	return rr;
}



int bytesToInt(const byte* bytes)  
{  
	int a = bytes[0] & 0xFF;  
	a |= ((bytes[1] << 8) & 0xFF00);  
	a |= ((bytes[2] << 16) & 0xFF0000);  
	a |= ((bytes[3] << 24) & 0xFF000000);  
	return a;  
}  

std::string IntToString(const int & number)
{
	std::stringstream stream;
	stream<<number;
	return  stream.str();
}



void base64_trans(std::string &s)
{
	char mp[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string bit;
	int n = s.size();
	for(int i=0;i<n;i++) bit += std::bitset<8>(s[i]).to_string();
	while (bit.length() % 6) bit += '0';
	n = 0; s = "";
	for (int i = 0; i < bit.length(); i += 6)
		s += mp[std::bitset<6>(bit.substr(i, 6)).to_ulong()];
	while (n % 4) s += '=';
	//        s[n] = 0;
}


byte* show_the_zframe(zframe_t* self)
{
	assert(self);
	byte *data =zframe_data(self);
	size_t  SIZE =zframe_size(self);

	// printf("The frame length is %d:\n",size);
	int is_bin = 0;
	int char_nbr;
	for (char_nbr = 0; char_nbr < SIZE; char_nbr++)
		if (data [char_nbr] < 9 || data [char_nbr] > 127)
			is_bin = 1;

	for (char_nbr = 0; char_nbr < SIZE; char_nbr++) {
		if (is_bin)
			printf ("%02X", (unsigned char) data [char_nbr]);
		else
			printf ("%c", data [char_nbr]);
	}
	puts("");
	return data;
}

zmsg_t * get_zmsg_new(const std::string &ID,const std::string &Flag,const std::string &Json)
{
	zmsg_t *message = zmsg_new();

	zframe_t *empty_ = zframe_new("",0);
	zframe_t *id = zframe_new(ID.c_str(),ID.size());
	zframe_t *json = zframe_new(Json.c_str(),Json.size());
	zframe_t *flag = zframe_new(Flag.c_str(),Flag.size());

	zmsg_push(message,json);
	zmsg_push(message,flag);
	zmsg_push(message,id);
	zmsg_push(message,empty_);

	return message;
}
zmsg_t * get_zmsg(const std::string &ID,const std::string &Json)
{
	zmsg_t *message = zmsg_new();

	zframe_t *empty_ = zframe_new("",0);
	zframe_t *id = zframe_new(ID.c_str(),ID.size());
	zframe_t *json = zframe_new(Json.c_str(),Json.size());

	zmsg_push(message,json);
	zmsg_push(message,id);
	zmsg_push(message,empty_);

	return message;
}
byte * get_zframe_data(zframe_t *self)
{
	//	assert(self);
	if(self == nullptr) return nullptr;
	byte *data = zframe_data(self);
	return data;
}

void zmsg_print (zmsg_t *self)
{
	assert (self);
	assert (zmsg_is (self));

	if (!self) {
		zsys_debug ("(NULL)");
		return;
	}
	zframe_t *frame = zmsg_first (self);
	while (frame) {
		show_the_zframe (frame);
		frame = zmsg_next (self);
	}
}

byte* matToBytes(cv::Mat image) {

	int SIZE = image.total() * image.elemSize();

	byte* bytes = new byte[SIZE];
	std::memcpy(bytes, image.data, SIZE * sizeof(byte));

	return bytes;
}

cv::Mat bytesToMat(byte* bytes,const  int &width,const  int &height) {
	cv::Mat image = cv::Mat(height, width, CV_8UC3, bytes).clone();
	return image;
}

