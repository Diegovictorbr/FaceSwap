#include "Detector.h"
#include "Swapper.h"
#include <vector>
#include <iostream>

using namespace std;

int main()
{
	VideoCapture videoFrames("C:\\Users\\Administrador\\Desktop\\testv3.mp4");
	Mat currentFrame;
	Detector d;
	Swapper s;

	while (videoFrames.read(currentFrame))
	{
		d.setOriginalFrame(currentFrame);
		vector<vector<cv::Point2f>> facialPoints = d.getFacialLandmarks();

		//Checa se existem pontos faciais nos dois vetores de facialPoints
		if (!(facialPoints[0].size() && facialPoints[1].size()))
			continue;

		s.setFacialLandmarks(facialPoints);
		s.setOriginalFrame(d.getOriginalFrame());
		s.testes();
		imshow("RESULT", s.getSwappedFrame());
		waitKey(1);
	}
	
	return 0;
}