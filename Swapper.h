#pragma once
#include <opencv2\opencv.hpp>
#include <dlib\image_processing.h>
#include <vector>

using namespace cv;
using namespace std;

class Swapper
{
public:
	Swapper();
	~Swapper();
	void setFacialLandmarks(vector<vector<Point2f>>);
	void setOriginalFrame(Mat of);
	void testes();
	Mat getSwappedFrame();

private:
	Mat originalFrame;
	Mat originalFrameBackup;
	Mat cloneAux;
	vector<vector<Point2f>> facialLandmarks;
	vector<vector<Point2f>> getEqualizedFaces(vector<Point2f> &srcFace, vector<Point2f> &dstFace);
	void calculateDelaunayTriangles(vector<vector<Point2f>> &equalizedFaces, vector<vector<int>> &triangleIndexes);
	void warpEquivalentTriangles(vector<vector<Point2f>> &equalizedFaces, vector<vector<int>> &faceTriangleIndexes);
	void warpFaceTriangles(vector<Point2f> &srcFaceTriangles, vector<Point2f> &dstFaceTriangles);
	void smoothSwappedFaces(vector<Point2f> &dstFace);
};

