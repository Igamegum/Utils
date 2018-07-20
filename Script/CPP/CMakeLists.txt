#设置Cmake编译版本
cmake_minimum_required (VERSION 3.0)
project(ClassroomAttendance)


#配置OpenCV
if (ANDROID)
	message("android system")
	message(${ANDROID_ABI})
	set(OpenCV_DIR ${CMAKE_SOURCE_DIR}/third_party/OpenCV-android-sdk/sdk/native/jni)
	#set(OpenCV_INCLUDE_DIRS, ${CMAKE_SOURCE_DIR}/third_party/opencv)
	find_package(OpenCV REQUIRED )
else ()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
endif()


#配置其它第三方库
if (ANDROID)
    include_directories("${CMAKE_SOURCE_DIR}/third_party/curl/include")
    include_directories("${CMAKE_SOURCE_DIR}/third_party/jsoncpp/include")
    link_directories("${CMAKE_SOURCE_DIR}/third_party/curl/lib/armeabi-v7a")
    link_directories("${CMAKE_SOURCE_DIR}/third_party/jsoncpp/lib/armeabi-v7a")
endif()

#设置编译出的版本类型
set(CMAKE_BUILD_TYPE "Release")
if (ANDROID)
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 -O2 -DWITH_OPENSSL -D__ANDROID__ -fvisibility=hidden -fvisibility-inlines-hidden -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections -mfloat-abi=softfp -mfpu=neon")
set(CMAKE_C_FLAGS_RELEASE "$ENV{CFLAGS} -O2 -fvisibility=hidden -D__ANDROID__ -fomit-frame-pointer -fstrict-aliasing -ffunction-sections -fdata-sections  -mfloat-abi=softfp -mfpu=neon")
#set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -Wl,--gc-sections")
set(LD_FLAGS "-Wl,--gc-sections ")

find_package(OpenMP)
if(OpenMP_CXX_FOUND OR OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
else ()
	set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -std=c++11 -DWITH_OPENSSL")
## 设置编译器
set(CMAKE_CXX_COMPILER "g++-5")
endif()

#设置输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


include_directories(${CMAKE_SOURCE_DIR}/include/)

### Cmake The party

### aligned
include_directories(${CMAKE_SOURCE_DIR}/include/aligned/)

aux_source_directory(${CMAKE_SOURCE_DIR}/src/aligned aligned_src)

### detect
include_directories(${CMAKE_SOURCE_DIR}/include/detect)
include_directories(${CMAKE_SOURCE_DIR}/include/detect/ncnn)
include_directories(${CMAKE_SOURCE_DIR}/include/detect/ncnn/layer)
include_directories(${CMAKE_SOURCE_DIR}/include/detect/ncnn/layer/x86)
include_directories(${CMAKE_SOURCE_DIR}/include/detect/ncnn/layer/arm)

aux_source_directory(${CMAKE_SOURCE_DIR}/src/detect detect_src)
aux_source_directory(${CMAKE_SOURCE_DIR}/include/detect/ncnn src_ncnn)
aux_source_directory(${CMAKE_SOURCE_DIR}/include/detect/ncnn/layer src_ncnn_layer)
aux_source_directory(${CMAKE_SOURCE_DIR}/include/detect/ncnn/layer/x86 src_ncnn_layer_x86)
aux_source_directory(${CMAKE_SOURCE_DIR}/include/detect/ncnn/layer/arm src_ncnn_layer_arm)

### client
include_directories(${CMAKE_SOURCE_DIR}/include/client)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/client client_src)

### configuration
include_directories(${CMAKE_SOURCE_DIR}/include/configuration)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/configuration configuration_src)

### easylog
include_directories(${CMAKE_SOURCE_DIR}/include/easylog)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/easylog easylog_src)

### easythreadpool
include_directories(${CMAKE_SOURCE_DIR}/include/easythreadpool)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/easythreadpool easythreadpool_src)



### pack
include_directories(${CMAKE_SOURCE_DIR}/include/pack)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/pack  pack_src)


### recognition
include_directories(${CMAKE_SOURCE_DIR}/include/recognition)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/recognition recognition_src)


### frame
include_directories(${CMAKE_SOURCE_DIR}/include/frame)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/frame frame_src)


### task
include_directories(${CMAKE_SOURCE_DIR}/include/task)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/task task_src)


##ONVIF
include_directories(${CMAKE_SOURCE_DIR}/include/onvif/soap)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/onvif/soap soap_src)
include_directories(${CMAKE_SOURCE_DIR}/include/onvif/soap/client)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/onvif/soap/client soap_client_src)
include_directories(${CMAKE_SOURCE_DIR}/include/onvif/plugin)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/onvif/plugin plugin_src)
include_directories(${CMAKE_SOURCE_DIR}/include/onvif/onvifpack)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/onvif/onvifpack onvifpack_src)


### sourcecode
aux_source_directory(${CMAKE_SOURCE_DIR} sourcecode_root)



#编译可执行程序

if (ANDROID)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/jniInterface jni_src)
set(sourcecode
	${aligned_src}
	${detect_src}
	${src_ncnn}
	${src_ncnn_layer}
	${src_ncnn_layer_x86}
	${src_ncnn_layer_arm}
	${client_src}
	${easylog_src}
	${easythreadpool_src}
	${configuration_src}
	${frame_src}
	${task_src}
	${pack_src}
	${soap_src}
	${soap_client_src}
	${plugin_src}
	${onvifpack_src}
	${recognition_src}
		)
else ()
set(sourcecode
	${sourcecode_root}
	${aligned_src}
	${detect_src}
	${src_ncnn}
	${src_ncnn_layer}
	${src_ncnn_layer_x86}
	${src_ncnn_layer_arm}
	${client_src}
	${easylog_src}
	${easythreadpool_src}
	${configuration_src}
	${frame_src}
	${task_src}
	${pack_src}
	${soap_src}
	${soap_client_src}
	${plugin_src}
	${onvifpack_src}
	${recognition_src})

add_executable(Demo ${sourcecode} )

endif()


message(STATUS "All OpenCV INCLUDES is "${OpenCV_INCLUDE_DIRS})
message(WARNING "All OpenCV LIBS is "　${OpenCV_LIBS})
message(STATUS "All CPPFile is "　${sourcecode})



#链接动态库

if (ANDROID)
add_executable(Demo ${sourcecode} ${CMAKE_SOURCE_DIR}/Demo.cpp)
target_link_libraries(Demo jsoncpp)
target_link_libraries(Demo curl)
target_link_libraries(Demo opencv_core opencv_highgui)
target_link_libraries(Demo ssl crypto)

add_library(classroom SHARED ${sourcecode} ${jni_src})
target_link_libraries(classroom jsoncpp)
target_link_libraries(classroom curl opencv_core opencv_highgui)
target_link_libraries(classroom ssl crypto)
else ()
target_link_libraries(Demo pthread jsoncpp curl ${OpenCV_LIBS})
target_link_libraries(Demo ssl crypto)
endif()

