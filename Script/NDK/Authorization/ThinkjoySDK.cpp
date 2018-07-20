#include <string>
#include <authorization/ThinkjoySDK.h>
#include <authorization/jnipack.h>
#include <authorization/commontype.h>
#include <authorization/property.h>
#include <tinyxml2.h>
#include <json/json.h>
#include <util/opensslpack.h>
#include <util/mix.h>
#include <util/xmlpack.h>

#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#define TAG "toroto"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

//重置所有操作
TJ_RETURNCODE Reset(JNIEnv * env,jobject context) {
	jstring xmlname = GetXMLName(env,context) ;
	const std::string str_xmlname = env->GetStringUTFChars(xmlname,nullptr);


	const std::string declaration = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>";
	tinyxml2::XMLDocument doc;

	tinyxml2::XMLElement * mpa = doc.NewElement("map");
	doc.InsertEndChild(mpa);

	tinyxml2::XMLElement * root = doc.RootElement();
	tinyxml2::XMLElement * license = doc.NewElement("string");
	license->SetAttribute("name",APP_UUID.c_str());

	jstring machineId = UniquePsuedoID(env);
	const char * mid = env->GetStringUTFChars(machineId,nullptr);
	tinyxml2::XMLText * mid_text = doc.NewText(mid);
	license->InsertEndChild(mid_text);

	root->InsertEndChild(license);

	return doc.SaveFile( str_xmlname.c_str() ) ? TJ_RETURNCODE_FAILED : TJ_RETURNCODE_OK;

}

TJ_RETURNCODE SetLicense(JNIEnv * env,jobject context,jstring input_license) {
	jstring xmlname = GetXMLName(env,context);
	const std::string str_xmlname = env->GetStringUTFChars(xmlname,nullptr);

	if (IsExist(str_xmlname) == false) {
		Reset(env,context);
	}
	
	const std::string origin_license = Jstring2String(env,input_license);
	const std::size_t split_pos = origin_license.find(SPLIT_SYMBOL);
	const std::string license = origin_license.substr(0,split_pos);
	const std::string app_key = origin_license.substr(split_pos + 1);

	if (app_key.length() <= 0 || license.length() <= 0) {
		return TJ_RETURNCODE_FAILED;
	}

	if (SetXMLNode(str_xmlname,APP_LICENSE,license) && 
		SetXMLNode(str_xmlname,APP_KEY,app_key)) {
		return TJ_RETURNCODE_OK;
	}

	return TJ_RETURNCODE_FAILED;
}

TJ_RETURNCODE CheckAuthorization(JNIEnv * env,jobject context,jstring & constKey) {

	jstring xmlname = GetXMLName(env,context);
	const std::string str_xmlname = env->GetStringUTFChars(xmlname,nullptr);
	if (IsExist(str_xmlname) == false) {
		return TJ_RETURNCODE_LACK_ACTIVATIONCODE;
	}

	std::string app_key;
	if (ReadXMLNode(str_xmlname,APP_KEY,app_key) == false) {
		return TJ_RETURNCODE_LACK_ACTIVATIONCODE;
	}

	std::string app_uuid = env->GetStringUTFChars(UniquePsuedoID(env),nullptr);

	std::string app_license;
	if (ReadXMLNode(str_xmlname,APP_LICENSE,app_license) == false) {
		return TJ_RETURNCODE_LACK_LICENSE;
	}


	const std::string app_sha512 = Composite_Key(app_key,app_uuid);
	const std::string app_const = Xor_Key(app_sha512,app_license);
	LOGD("The app_sha512 is:%s\n",app_sha512.c_str());
	LOGD("The app_const  is:%s\n",app_const.c_str());

	OpenSSLPack OPS;
	LOGD("The 512appconstis:%s\n",OPS.SHA512_Encrypt(app_const).c_str());

	if (ValidateConstD(app_const) == true) {
		constKey = env->NewStringUTF(app_const.c_str());
		return TJ_RETURNCODE_OK;
	}

	return TJ_RETURNCODE_FAILED;


}
//获取用于发送给服务端的信息
TJ_RETURNCODE GetProof(JNIEnv * env,jobject context,jstring activation_key,jstring & result) {
	
	jstring xmlname = GetXMLName(env,context);
	const std::string str_xmlname = env->GetStringUTFChars(xmlname,nullptr);
	if (IsExist(str_xmlname) == false) {
		Reset(env,context);
//		return TJ_RETURNCODE_LACK_ACTIVATIONCODE;
	}

	std::string app_key = env->GetStringUTFChars(activation_key,nullptr);

	std::string app_uuid = env->GetStringUTFChars(UniquePsuedoID(env),nullptr);

	if (app_key.length() <= 0 || app_uuid.length() <= 0) {
		return TJ_RETURNCODE_FAILED;
	}

	const std::string proof = GenerateProof(app_key,app_uuid);
	LOGD("The proof is :%s\n",proof.c_str());
	result = env->NewStringUTF(proof.c_str());
	return TJ_RETURNCODE_OK;
}



