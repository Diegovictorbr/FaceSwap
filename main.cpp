#include "Detector.h"
#include "Swapper.h"
#include <vector>
#include <iostream>

using namespace std;



//N vou usar dessa vez pra entender a neessidade. A execução é mais rápida com ponto flutuante?
//originalFrame.convertTo(originalFrame, CV_32F);
//img1Warped.convertTo(img1Warped, CV_32F);

int main()
{
	Detector d("C:\\Users\\Administrador\\Desktop\\Dev\\Projetos\\Test data\\Images\\FSwap\\FF (12).jpg");
	Swapper s;
	
	vector<vector<cv::Point2f>> facialPoints = d.getFacialLandmarks();

	//Checa se existem pontos faciais nos dois vetores de facialPoints
	if (!(facialPoints[0].size() && facialPoints[0].size()))
	{
		cout << "NÃO DETECTOU DOIS ROSTOS!" << endl;
		system("pause");
		return 0;
	}

	s.setFacialLandmarks(facialPoints);
	s.setOriginalFrame(d.getOriginalFrame());
	s.testes();

	return 0;
}