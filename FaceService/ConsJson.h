#ifndef CONS_JSON
#define CONS_JSON

#include <jsoncpp/json.h>


class ConsJson
{
public:
	std::string detect_api(const std::string ,const int ,const bool,const bool);
	std::string addface_api_faceids(const std::vector<std::string> ,const std::string ,const std::string,const std::string);
};
std::string ConsJson::detect_api(const std::string userId,const int img_num,const bool need_age,const bool need_sex)
{

	Json::Value value;
	value["userId"] = userId;
	value["opType"] = "detect";

	Json::Value param;
	param["imgNum"] = img_num;

	std::string attribute("");
	if(need_age && need_sex){
		attribute = "age,gender";
	}else if(need_age && need_sex == false){
		attribute = "age";
	}else if(need_age && need_sex == false){
		attribute = "gender";
	}

	param["attribute"] = attribute;
	value["param"] = param;

	return value.toStyledString();

}

std::string ConsJson::addface_api_faceids(const std::vector<std::string> faceIds,const std::string userId,const std::string facesetId,const std::string facesetName)
{

	Json::Value value;
	value["userId"] = userId;
	value["opType"] = "faceset_addface";

	Json::Value param;
	if( facesetId.length() > 0 ) {
		param["facesetId"] = facesetId;
	}else if( facesetName.length() > 0) {
		param["facesetName"] = facesetName;
	}else {
		return "";
	}
	
	Json::Value faceId_array;
	for(int i=0; i<faceIds.size(); i++){
		faceId_array.append(faceIds[i]);
	}

	param["faceId"] = faceId_array;

	value["param"] = param;


	return value.toStyledString();

}

#endif
