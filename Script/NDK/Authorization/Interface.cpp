#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>


#include <functional>
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <tinyxml2.h>


#include <ThinkjoySDK.h>
#include <jnipack.h>

#define TAG "toroto"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

#ifdef __cplusplus
extern "C"{
#endif

static const char *classPathName = "com/thinkjoy/myapplication/MainActivity";


JNIEXPORT jstring JNICALL native_read_sdkxml(JNIEnv * env, jclass obj,jobject context) {

	jstring xmlname = GetXMLName(env,context) ;
	const std::string str_xmlname = env->GetStringUTFChars(xmlname,nullptr);

	if (IsExist(str_xmlname) == false) {
		return env->NewStringUTF("The sdkxml is not exist");
	}

	tinyxml2::XMLDocument doc;
	doc.LoadFile( str_xmlname.c_str() );

	tinyxml2::XMLPrinter printer;
	doc.Print(&printer);
	return env->NewStringUTF(printer.CStr());
}

JNIEXPORT jint JNICALL native_reset_sdkxml(JNIEnv * env, jclass obj,jobject context) {
	return Reset(env,context);
}


JNIEXPORT jint JNICALL native_set_license(JNIEnv * env,jclass obj,jobject context,jstring license) {
	return SetLicense(env,context,license);
}


JNIEXPORT jstring JNICALL native_get_proof(JNIEnv * env, jclass obj,jobject context,jstring app_key) {
	
	jstring secret;
	if (GetProof(env,context,app_key,secret) != TJ_RETURNCODE_OK) {
		return env->NewStringUTF("");
	}
	return secret;
}

JNIEXPORT jint JNICALL native_validate_authorization(JNIEnv * env,jclass obj,jobject context) {
	jstring  constKey;
	return CheckAuthorization(env,context,constKey);
}

static JNINativeMethod nMethods[] = { 
	{"read_sdkxml", "(Ljava/lang/Object;)Ljava/lang/String;", (void*)native_read_sdkxml },	
	{"reset_sdkxml", "(Ljava/lang/Object;)I", (void*)native_reset_sdkxml },	
	{"set_license", "(Ljava/lang/Object;Ljava/lang/String;)I", (void*)native_set_license},	
	{"get_proof", "(Ljava/lang/Object;Ljava/lang/String;)Ljava/lang/String;", (void*)native_get_proof},
	{"validate_authorization", "(Ljava/lang/Object;)I", (void*)native_validate_authorization},	
};

static int registerNativeMethods(JNIEnv* env, const char* className,
		JNINativeMethod* gMethods, int numMethods) {
	jclass clazz;
	clazz = env->FindClass(className);
	if (clazz == NULL) {
		return JNI_FALSE;
	}
	if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
		return JNI_FALSE;
	}
	return JNI_TRUE;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	JNIEnv* env;
	if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
		return -1;
	}
	if (!registerNativeMethods(env, classPathName, nMethods,
				sizeof(nMethods) / sizeof(nMethods[0]))) {
		return -1;
	}
	return JNI_VERSION_1_6;
}

#ifdef __cplusplus
}
#endif
