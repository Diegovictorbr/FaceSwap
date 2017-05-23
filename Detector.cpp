#include "Detector.h"

Detector::Detector()
{
	dlib::deserialize("sp68fl.dat") >> spredictor;
}

Detector::~Detector()
{
	faces.clear();
	shapes.clear();
}

void Detector::detectFacesAndShapes()
{
	shapes.clear();
	faces = faceDetector(dlibFrame);
	
	if (faces.size() < 2)
		return;

	for (int i = 0; i < 2; i++)
		shapes.push_back(spredictor(dlibFrame, faces[i]));
}

//Retorna os pontos faciais dos dois primeiros rostos da imagem
vector<vector<Point2f>> Detector::getFacialLandmarks()
{	
	detectFacesAndShapes();

	vector<vector<Point2f>> facialLandmarks(2);

	if (!shapes.size())
		return facialLandmarks;
	
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 26; j++)
			facialLandmarks[i].push_back(Point2f(shapes[i].part(j).x(), shapes[i].part(j).y()));
			
	return facialLandmarks;
}

Mat Detector::getOriginalFrame()
{
	return originalFrame;
}

void Detector::setOriginalFrame(Mat of)
{
	originalFrame = of.clone();
	dlibFrame = originalFrame;
}