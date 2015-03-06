TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH+=D:\OpenCV-2.4.9\include\opencv\
                    D:\OpenCV-2.4.9\include\opencv2\
                    D:\OpenCV-2.4.9\include

LIBS+=D:\OpenCV-2.4.9\lib\libopencv_calib3d249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_contrib249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_core249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_features2d249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_flann249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_gpu249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_highgui249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_imgproc249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_legacy249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_ml249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_objdetect249.dll.a\
        D:\OpenCV-2.4.9\lib\libopencv_video249.dll.a

include(deployment.pri)
qtcAddDeployment()

