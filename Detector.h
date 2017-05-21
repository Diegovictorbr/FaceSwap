#pragma once
#include <opencv2\opencv.hpp>
#include <dlib\image_processing.h>
#include <dlib\image_processing\frontal_face_detector.h>
#include <dlib\image_processing\render_face_detections.h>
#include <dlib\image_processing\generic_image.h>
#include <dlib\image_processing\full_object_detection.h>
#include <dlib\opencv.h>
#include <dlib\geometry.h>
#include <dlib\image_transforms.h>
#include <dlib\serialize.h>
#include <dlib\gui_widgets.h>
#include <dlib\image_io.h>
#include <vector>

using namespace cv;
using namespace std;

class Detector
{
public:
	Detector(char * imagePath);
	vector<vector<Point2f>> getFacialLandmarks();
	Mat getOriginalFrame();
	~Detector();

private:
	Mat originalFrame;
	dlib::cv_image<dlib::bgr_pixel> dlibFrame;
	vector<dlib::rectangle> faces;
	vector<dlib::full_object_detection> shapes;
	dlib::frontal_face_detector faceDetector = dlib::get_frontal_face_detector();
	dlib::shape_predictor spredictor;
	void detectFacesAndShapes();
};

