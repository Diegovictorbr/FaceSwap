#include "Swapper.h"

using namespace std;

Swapper::Swapper()
{
}

Swapper::~Swapper()
{
}

void Swapper::setFacialLandmarks(vector<vector<Point2f>> fls)
{
	facialLandmarks = fls;
}

void Swapper::setOriginalFrame(Mat of)
{
	originalFrame = of.clone();
	originalFrameBackup = originalFrame.clone();
}

void Swapper::testes()
{	
	//getEqualizedFaces retorna pontos externos igualados de dois rostos baseado em um deles. Os pontos igualados são importantes
	//para a triangulação de Delaunay ser feita a partir dos mesmos pontos externos
	vector<vector<Point2f>> equalizedFaces1 = getEqualizedFaces(facialLandmarks[0], facialLandmarks[1]);
	vector<vector<Point2f>> equalizedFaces2 = getEqualizedFaces(facialLandmarks[1], facialLandmarks[0]);

	//faceTriangleIndexes guardam vetores de 3 inteiros, onde cada posição guarda um índice 
	//de equalizedFaces (referenciando o ponto guardado com o índice)
	vector<vector<int>> firstFaceTriangleIndexes;
	vector<vector<int>> secondFaceTriangleIndexes;

	//Chamada à função que calcula os triângulos de Delaunay e preenche os índices em facetriangleIndexes
	//cada vetor com 3 índices em faceTriangleIndexes diz respeito a três pontos em equalizedFaces
	calculateDelaunayTriangles(equalizedFaces1, firstFaceTriangleIndexes);
	calculateDelaunayTriangles(equalizedFaces2, secondFaceTriangleIndexes);
	namedWindow("BEFORE", WINDOW_FREERATIO);
	imshow("BEFORE", originalFrame);

	//As atividades de warp e smooth 
	//warpEquivalentTriangles equivale as regiões triangulares faciais dos rostos e chama warpFaceTriangles passando os triângulos equivalentes.
	warpEquivalentTriangles(equalizedFaces1, firstFaceTriangleIndexes);	
	smoothSwappedFaces(equalizedFaces1[1]);
	
	cloneAux = originalFrame.clone();
	
	warpEquivalentTriangles(equalizedFaces2, secondFaceTriangleIndexes);
	
	originalFrameBackup = cloneAux.clone();
	
	
	smoothSwappedFaces(equalizedFaces2[1]);

	namedWindow("AFTER", WINDOW_FREERATIO);
	imshow("AFTER", originalFrame);
	waitKey();
}

//Retorna dois vetores point2f contendo pontos equivalentes para que um rosto seja colocado em outro.
//srcFace é o vetor de pontos do rosto a ser equalizado, em dstFace. Os dois vetores retornados
//devem ser usados nas transformações faciais com warpAffine.
vector<vector<Point2f>> Swapper::getEqualizedFaces(vector<Point2f> &srcFace, vector<Point2f> &dstFace)
{
	//Pontos faciais igualados do primeiro rosto no segundo rosto
	vector<vector<Point2f>> faceHulls(2);
	//Indices dos pontos faciais externos do rosto a ser igualado nos vetores
	vector<int> hullIndexes;

	//Capturando índices dos pontos faciais externos do primeiro rosto
	convexHull(srcFace, hullIndexes, false, false);

	for (int i = 0; i < hullIndexes.size(); i++)
	{
		//Igualando pontos externos do rosto 1 no rosto 2
		faceHulls[0].push_back(srcFace[hullIndexes[i]]);
		faceHulls[1].push_back(dstFace[hullIndexes[i]]);
	}

	return faceHulls;
}

void Swapper::calculateDelaunayTriangles(vector <vector<Point2f>> &equalizedFaces, vector<vector<int>> &triangleIndexes)
{
	Rect rect(boundingRect(equalizedFaces[0]));
	Subdiv2D sbdiv(rect);
	sbdiv.insert(equalizedFaces[0]);

	vector<Vec6f> tlist;
	sbdiv.getTriangleList(tlist);

	for (int i = 0; i < tlist.size(); i++)
	{
		Vec6f t = tlist[i];
		vector<Point2f> pts(3);
		vector<int> triangleIndex(3);

		pts[0] = Point2f(t[0], t[1]);
		pts[1] = Point2f(t[2], t[3]);
		pts[2] = Point2f(t[4], t[5]);


		if (rect.contains(pts[0]) && rect.contains(pts[1]) && rect.contains(pts[2]))
		{
			for (int j = 0; j < 3; j++)
				for (int k = 0; k < equalizedFaces[0].size(); k++)
					if (abs(pts[j].x - equalizedFaces[0][k].x) < 1.0 && abs(pts[j].y - equalizedFaces[0][k].y) < 1.0)
						triangleIndex[j] = k;

			triangleIndexes.push_back(triangleIndex);
		}
	}

}

void Swapper::warpEquivalentTriangles(vector<vector<Point2f>> &equalizedFaces, vector<vector<int>> &faceTriangleIndexes)
{
	for (auto const &ti : faceTriangleIndexes)
	{
		vector<Point2f> singleTriangle(3);
		vector<Point2f> singleTriangle2(3);

		for (int i = 0; i < 3; i++)
		{
			singleTriangle[i] = equalizedFaces[0][ti[i]];
			singleTriangle2[i] = equalizedFaces[1][ti[i]];
		}

		warpFaceTriangles(singleTriangle, singleTriangle2);
	}
}

void Swapper::warpFaceTriangles(vector<Point2f>& srcFaceTriangles, vector<Point2f>& dstFaceTriangles)
{	
	Rect triangle1BRect = boundingRect(srcFaceTriangles);
	Rect triangle2BRect = boundingRect(dstFaceTriangles);

	//Recuperando coordenadas reais dos triangulos dentro do bounding rect
	vector<Point2f> trueTriangle1, trueTriangle2;
	vector<Point> trueTriangle2Int;
	for (int i = 0; i < 3; i++)
	{
		trueTriangle1.push_back(Point2f(srcFaceTriangles[i].x - triangle1BRect.x, srcFaceTriangles[i].y - triangle1BRect.y));
		trueTriangle2.push_back(Point2f(dstFaceTriangles[i].x - triangle2BRect.x, dstFaceTriangles[i].y - triangle2BRect.y));
		trueTriangle2Int.push_back(Point(dstFaceTriangles[i].x - triangle2BRect.x, dstFaceTriangles[i].y - triangle2BRect.y));
	}

	//Get mask by filling triangle
	Mat mask = Mat::zeros(triangle2BRect.height, triangle2BRect.width, CV_8UC3);
	fillConvexPoly(mask, trueTriangle2Int, Scalar(1.0, 1.0, 1.0), 16);

	// Apply warpImage to small rectangular patches
	Mat img1Rect;
	originalFrameBackup(triangle1BRect).copyTo(img1Rect);

	Mat img2Rect = Mat::zeros(triangle2BRect.height, triangle2BRect.width, img1Rect.type());

	Mat warpMat = getAffineTransform(trueTriangle1, trueTriangle2);

	warpAffine(img1Rect, img2Rect, warpMat, img2Rect.size(), INTER_LINEAR, BORDER_REFLECT_101);
	
	multiply(img2Rect, mask, img2Rect);
	multiply(originalFrame(triangle2BRect), Scalar(1.0, 1.0, 1.0) - mask, originalFrame(triangle2BRect));
	originalFrame(triangle2BRect) = originalFrame(triangle2BRect) + img2Rect;
}

void Swapper::smoothSwappedFaces(vector<Point2f> &dstFace)
{
	//Calculando a mascara do rosto destino antes de ser trocado
	vector<Point> hull8U;
	for (int i = 0; i < dstFace.size(); i++)
		hull8U.push_back(Point(dstFace[i].x, dstFace[i].y));

	Mat mask = Mat::zeros(originalFrame.rows, originalFrame.cols, originalFrame.depth());
	fillConvexPoly(mask, &hull8U[0], hull8U.size(), Scalar(255, 255, 255));

	// Clone seamlessly.
	Rect r = boundingRect(hull8U);
	Point center = (r.tl() + r.br()) / 2;
	
	seamlessClone(originalFrame, originalFrameBackup, mask, center, originalFrame, NORMAL_CLONE);
}