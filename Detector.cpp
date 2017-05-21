#include "Detector.h"


Detector::Detector(char * imagePath)
{
	originalFrame = imread(imagePath);
	dlibFrame = originalFrame;
	dlib::deserialize("sp68fl.dat") >> spredictor;
}

Detector::~Detector()
{
	faces.clear();
	shapes.clear();
}

void Detector::detectFacesAndShapes()
{
	faces = faceDetector(dlibFrame);

	for (auto const &rects : faces)
		shapes.push_back(spredictor(dlibFrame, rects));
}

//Retorna os pontos faciais dos dois primeiros rostos da imagem
vector<vector<Point2f>> Detector::getFacialLandmarks()
{	
	detectFacesAndShapes();

	vector<vector<Point2f>> facialLandmarks(2);

	if (shapes.size() < 2)
		return facialLandmarks;

	for (int i = 0; i < 68; i++)
	{
		facialLandmarks[0].push_back(Point2f(shapes[0].part(i).x(), shapes[0].part(i).y()));
		facialLandmarks[1].push_back(Point2f(shapes[1].part(i).x(), shapes[1].part(i).y()));
	}
	
	return facialLandmarks;
}

Mat Detector::getOriginalFrame()
{
	return originalFrame;
}
