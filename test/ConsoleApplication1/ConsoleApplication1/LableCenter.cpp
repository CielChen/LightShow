// pic_label.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "stdio.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <io.h>
using namespace std;

//ȫ�ֱ���
bool is_drawing = false;
//���յı�־����
vector<CvRect> biaozhu_boxs;
//����ʱ��¼����ʱ��
CvRect drawing_box;
IplImage *img, *img1;

static void help();
static void onMouse(int event, int x, int y, int, void*);
void getFiles(string path, vector<string>& files);
vector<string> split(const string &s, const string &seperator);

typedef struct SLablePosition
{
	int iPosX;
	int iPosY;
}SLablePosition;
typedef multimap<string, SLablePosition> MapNameLable;
typedef multimap<string, SLablePosition>::iterator IterMapNameLable;
typedef pair<string, SLablePosition> PairNameLable;
MapNameLable mapNameLable;
typedef pair<IterMapNameLable, IterMapNameLable> RangeNameLable;

//1.����Ŀ�꡾�ɶ����
//2.��n��������һ֡�����浱ǰ��Ŀ�����굽txt�ı����ɶ����
//3.��c�������ǰ֡�����ѱ궨���������з����ʱ�򡿻�����һ֡����������
//�ļ������ʽ��
//֡���Ŀ���ž������Ͻ�����������½�����
//int _tmain(int argc, _TCHAR* argv[])
int main()
{
	CvFont font;
	CvScalar scalar;
	char text[10];

	// ��ʼ������
	double hScale = 1;
	double vScale = 1;
	int lineWidth = 3;// �൱��д�ֵ�����
	scalar = CV_RGB(255, 0, 0);
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hScale, vScale, 0, lineWidth);//��ʼ�����壬׼��д��ͼƬ�ϵ� 

	int frame_counter = 0;
	int obj_id = 0;

	//��ȡ���б�ע�ļ���
	string strLableTextPath = "..\\LableText";
	vector<string> lableTextFiles;
	getFiles(strLableTextPath, lableTextFiles);
	int iLableFileSize = lableTextFiles.size();
	if (iLableFileSize <= 0)
	{
		cout << "No Lable Text." << endl;
		cout << "Press any key quit..." << endl;
		char cTmp;
		cin >> cTmp;
		return 0;
	}
	//���δ���ÿ����ע�ļ�������עλ�ö�ȡ
	for (int iLableFile = 0; iLableFile < iLableFileSize; iLableFile++)
	{
		string strTextFile = lableTextFiles[iLableFile];
		int iPos = strTextFile.find_last_of(".");
		if (iPos == -1)
		{
			continue;
		}
		string strPostfix = strTextFile.substr(iPos + 1);
		if (strPostfix != "txt")
		{
			continue;
		}

		ifstream infile(strTextFile);
		string strEachLine;
		while (getline(infile, strEachLine))
		{
			vector<string> strSplit = split(strEachLine, " ");
			if (strSplit.size() >= 3)
			{
				SLablePosition lablePos;
				lablePos.iPosX = atoi(strSplit[1].c_str());
				lablePos.iPosY = atoi(strSplit[2].c_str());
				if (lablePos.iPosX >= 0 && lablePos.iPosY >= 0)
				{
					mapNameLable.insert(make_pair(strSplit[0], lablePos));
				}
			}
		}
	}

	////��ȡ��Ƶ
	//CvCapture *capture = cvCreateFileCapture("a.avi");
	////��ȡһ֡
	//img = cvQueryFrame(capture);
	//img1 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 3);
	////���浽ͼ������
	//cvCopy(img, img1);
	//��ȡ����ͼ���ļ���
	string strPath = "..\\LightImages";
	//cin >> strPath;   //Ҳ�����ô˶δ���ֱ����DOS���������ַ����ʱֻ�������ĵ���б�ܼ��� 
	vector<string> files;
	getFiles(strPath, files);  //filesΪ���ص��ļ������ɵ��ַ���������  
	int iFileSize = files.size();
	if (iFileSize <= 0)
	{
		cout << "No Valid Texture." << endl;
		cout << "Press any key quit..." << endl;
		char cTmp;
		cin >> cTmp;
		return 0;
	}
	//������Ҫ��ȡͼ���޸�����ֵ
	for (int iFile = 0; iFile < iFileSize; iFile++)
	{
		string strPathFile = files[iFile];
		int iPos = strPathFile.find_last_of("/");
		if (iPos == -1)
		{
			iPos = strPathFile.find_last_of("\\");
		}
		string strFile = strPathFile.substr(iPos + 1);
		if (mapNameLable.count(strFile) <= 0)
		{
			continue;
		}

		img = cvLoadImage(strPathFile.c_str(), -1);
		//����cvLoadImage()�ĵ�1 ��������ͼ���ļ���·��.
		//��2 �������Ƕ�ȡͼ��ķ�ʽ:-1 ��ʾ����ͼ�������������ȡ,1 ��ʾǿ�Ʋ�ɫ��,0 ��ʾ
		//ǿ�ƻ�ֵ��.
		if (img == NULL)
		{
			cout << "�޷���ȡͼ�� " << strPathFile << endl;

			continue;
		}
		img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
		//���浽ͼ������
		cvCopy(img, img1);

		int iWidth = img1->width;
		int iHeight = img1->height;

		RangeNameLable range = mapNameLable.equal_range(strFile);
		for (IterMapNameLable iterMove = range.first; iterMove != range.second; iterMove++)
		{
			int iSetX = (*iterMove).second.iPosX;
			int iSetY = (*iterMove).second.iPosY;
			if (iSetX < iWidth && iSetY < iHeight)
			{
				CvRect setRect(iSetX-10, iSetY-10, 20, 20);
				cvSetImageROI(img1, setRect);
				cvSet(img1, scalar);
				cvResetImageROI(img1);

				//cvSet2D(img1, iSetX, iSetY, scalar);
				//if (iSetX - 1 >= 0)
				//{
				//	cvSet2D(img1, iSetX - 1, iSetY, scalar);
				//}
				//if (iSetX + 1 < iWidth)
				//{
				//	cvSet2D(img1, iSetX + 1, iSetY, scalar);
				//}
				//if (iSetY - 1 >= 0)
				//{
				//	cvSet2D(img1, iSetX, iSetY - 1, scalar);
				//}
				//if (iSetY + 1 < iHeight)
				//{
				//	cvSet2D(img1, iSetX, iSetY + 1, scalar);
				//}
			}
		}

		//��ʾԭʼͼƬ
		cvShowImage("Image", img);
		cvShowImage("ImageLable", img1);

		string strResultPath = "..\\LightImagesResult\\" + strFile;
		cvSaveImage(strResultPath.c_str(), img1);
		//���浽ͼ������
		cvReleaseImage(&img1);
	}
	
	//������Ϣ��ʾ
	help();

	//���ص�
	cvSetMouseCallback("Image", onMouse, 0);

	while (1)
	{
		//�ȴ�����
		int c = cvWaitKey(0);
		//esc���˳�
		if ((c & 255) == 27)
		{
			cout << "Exiting ...\n";
			break;
		}

		switch ((char)c)
		{
			//��ȡ��һ֡
		case 'n':
			break;

		case 'c':	
			break;
		}
	}

EndProgram:
	cvNamedWindow("Image", 0);
	cvReleaseImage(&img);
	cvReleaseImage(&img1);
	cvDestroyWindow("Image");

	cout << "Done!" << endl;
	cout << "Press any key quit..." << endl;
	char cTmp;
	cin >> cTmp;

	return 0;
}

static void help()
{
	cout << "This program designed for labeling image \n"
		<< "Coded by Chen Dali on 7/25/2017\n" << endl;

	cout << "Use the mouse to draw rectangle on the image for labeling.\n" << endl;

	cout << "Hot keys: \n"
		"\tESC - quit the program\n"
		"\tn - next image\n"
		"\tc - clear all the labels\n"
		<< endl;
}

static void onMouse(int event, int x, int y, int, void*)
{
	switch (event)
	{
		//�������
	case CV_EVENT_LBUTTONDOWN:
		//the left up point of the rect
		is_drawing = true;
		drawing_box.x = x;
		drawing_box.y = y;
		break;

		//����ƶ�
	case CV_EVENT_MOUSEMOVE:
		//adjust the rect (use color blue for moving)
		if (is_drawing) {
			drawing_box.width = x - drawing_box.x;
			drawing_box.height = y - drawing_box.y;
			cvCopy(img, img1);
			for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
			{
				cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
			}
			cvRectangle(img1, cvPoint(drawing_box.x, drawing_box.y), cvPoint(drawing_box.x + drawing_box.width, drawing_box.y + drawing_box.height), CV_RGB(255, 0, 0));
		}
		break;

		//�������
	case CV_EVENT_LBUTTONUP:
		//finish drawing the rect (use color green for finish)
		if (is_drawing) {
			drawing_box.width = x - drawing_box.x;
			drawing_box.height = y - drawing_box.y;
			cvCopy(img, img1);
			for (vector<CvRect>::iterator it = biaozhu_boxs.begin();
				it != biaozhu_boxs.end();++it) {
				cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
			}
			cvRectangle(img1, cvPoint(drawing_box.x, drawing_box.y), cvPoint(drawing_box.x + drawing_box.width, drawing_box.y + drawing_box.height), CV_RGB(255, 0, 0));
			biaozhu_boxs.push_back(drawing_box);
		}
		is_drawing = false;
		break;
	}
	cvShowImage("Image", img1);
	return;
}

void getFiles(string inPath, vector<string>& inFiles)
{
	//�ļ����  
	long long hFile = 0;
	//�ļ���Ϣ  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(inPath).append("\\*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//�����Ŀ¼,����֮  
			//�������,�����б�  
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
					getFiles(p.assign(inPath).append("\\").append(fileinfo.name), inFiles);
			}
			else
			{
				inFiles.push_back(p.assign(inPath).append("\\").append(fileinfo.name));
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		
		_findclose(hFile);
	}
}

vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//�ҵ��ַ������׸������ڷָ�������ĸ��
		int flag = 0;
		while (i != s.size() && flag == 0) {
			flag = 1;
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[i] == seperator[x]) {
					++i;
					flag = 0;
					break;
				}
		}

		//�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
		flag = 0;
		string_size j = i;
		while (j != s.size() && flag == 0) {
			for (string_size x = 0; x < seperator.size(); ++x)
				if (s[j] == seperator[x]) {
					flag = 1;
					break;
				}
			if (flag == 0)
				++j;
		}
		if (i != j) {
			result.push_back(s.substr(i, j - i));
			i = j;
		}
	}
	return result;
}
