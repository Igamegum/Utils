#include <jnipack.h> 
#include <string>
#include <sstream>
#include <fstream>
#include <authorization/property.h>


bool IsExist(const std::string filename) {
	std::ifstream ifs;
	ifs.open(filename,std::ios::in);
	return !ifs ? false : true;
}

jstring GetPackageName(JNIEnv * env,jobject context) {

	jclass contextClass = (jclass)env->NewGlobalRef(env->FindClass("android/content/Context"));
	jmethodID getPackageNameId = env->GetMethodID(contextClass, "getPackageName","()Ljava/lang/String;");
	jstring packNameString =  (jstring)env->CallObjectMethod(context, getPackageNameId);
	return packNameString;
}

jstring GetXMLName(JNIEnv * env,jobject context) {
	jstring packagename = GetPackageName(env,context);
	const std::string pkname = env->GetStringUTFChars(packagename,nullptr);
	std::string xmlname = "/data/data/" + pkname + "/" + SDKNAME;
	return env->NewStringUTF(xmlname.c_str());
}

int JstringLength(JNIEnv * env,jstring js) {
	const char * char_str = env->GetStringUTFChars(js,nullptr);
	const std::string str_str(char_str);
	env->ReleaseStringUTFChars(js,char_str);
	return str_str.length();
}

std::string Jstring2String(JNIEnv * env,jstring js) {
	const char * char_str = env->GetStringUTFChars(js,nullptr);
	const std::string str_str(char_str);
	return str_str;
}

jstring BuildField(JNIEnv * env,std::string name) {
	jclass buildClass = (jclass)env->NewGlobalRef(env->FindClass("android/os/Build"));
	jfieldID paramId = env->GetStaticFieldID(buildClass,name.c_str(),"Ljava/lang/String;");
	jstring param = (jstring)env->GetStaticObjectField(buildClass,paramId);
	return param;
}

jint JstringHashcode(JNIEnv * env,jstring param) {
	jclass stringClass = (jclass)env->FindClass("java/lang/String");
	jmethodID hashId = env->GetMethodID(stringClass,"hashCode","()I");
	jint hash = (jint)env->CallIntMethod(param,hashId);
	return hash; 
}



jstring UniquePsuedoID(JNIEnv *env) {

	std::stringstream ss;

	ss << Jstring2String(env,BuildField(env,"BOARD"));
	ss << Jstring2String(env,BuildField(env,"BRAND"));
	ss << Jstring2String(env,BuildField(env,"CPU_ABI"));
	ss << Jstring2String(env,BuildField(env,"DEVICE"));
	ss << Jstring2String(env,BuildField(env,"DISPLAY"));
	ss << Jstring2String(env,BuildField(env,"HOST"));
	ss << Jstring2String(env,BuildField(env,"ID"));
	ss << Jstring2String(env,BuildField(env,"MANUFACTURER"));
	ss << Jstring2String(env,BuildField(env,"MODEL"));
	ss << Jstring2String(env,BuildField(env,"PRODUCT"));
	ss << Jstring2String(env,BuildField(env,"TAGS"));
	ss << Jstring2String(env,BuildField(env,"TYPE"));
	ss << Jstring2String(env,BuildField(env,"USER"));

	jstring m_szDevIDShort = env->NewStringUTF(ss.str().c_str());
	jstring serial = BuildField(env,"SERIAL");

	jlong m_szDevIDShort_hash = (jlong)JstringHashcode(env,m_szDevIDShort);
	jlong serial_hash = (jlong)JstringHashcode(env,serial);


	jclass uuidClass = (jclass)env->FindClass("java/util/UUID");
	jmethodID consID = env->GetMethodID(uuidClass,"<init>","(JJ)V");
	jmethodID tostringID = env->GetMethodID(uuidClass,"toString","()Ljava/lang/String;");

	jobject uuid = (jobject)env->NewObject(uuidClass,consID,m_szDevIDShort_hash,serial_hash);
	jstring result = (jstring)env->CallObjectMethod(uuid,tostringID);

	return result;
}

