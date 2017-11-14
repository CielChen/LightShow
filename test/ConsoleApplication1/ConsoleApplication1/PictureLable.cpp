// pic_label.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "stdio.h"
#include <iostream>
#include <string>
#include <vector>
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
	while (1)
	{
		img = cvLoadImage((files[frame_counter]).c_str(), -1);
		//����cvLoadImage()�ĵ�1 ��������ͼ���ļ���·��.
		//��2 �������Ƕ�ȡͼ��ķ�ʽ:-1 ��ʾ����ͼ�������������ȡ,1 ��ʾǿ�Ʋ�ɫ��,0 ��ʾ
		//ǿ�ƻ�ֵ��.
		if (img == NULL)
		{
			cout << "�޷���ȡͼ�� " << files[0] << endl;

			frame_counter++;
			if (frame_counter >= iFileSize)
			{
				cout << "Done!" << endl;
				cout << "Press any key quit..." << endl;
				char cTmp;
				cin >> cTmp;
				return 0;
			}
		}
		else
		{
			break;
		}
	}
	//img1 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 3);
	img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
	//���浽ͼ������
	cvCopy(img, img1);

	//����ı��ļ�
	ofstream outfile(strPath + "\\LightLable.txt");
	//������Ϣ��ʾ
	help();

	//�����߿�
	for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
	{
		cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
	}
	//��ʾԭʼͼƬ
	cvShowImage("Image", img);

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
			//��¼��ǰ֡�����б�ע��Ϣ
			for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
			{
				cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
				//�γ��ı���¼�Ķ�������
				_itoa(obj_id, text, 10);
				//��ͼƬ������ַ�����¼�ǵ�ǰ֡�ĵڼ�����
				cvPutText(img1, text, cvPoint((*it).x, (*it).y), &font, CV_RGB(255, 255, 255));

				//�������Ǽ�¼�ı�
				//outfile << frame_counter << " " << obj_id << " " << (*it).x << " "
				//	<< (*it).y << " " << (*it).width << " "
				//	<< (*it).height << endl;
				string strOutFilePath = files[frame_counter];
				int iPos = strOutFilePath.find_last_of("/");
				if (iPos == -1)
				{
					iPos = strOutFilePath.find_last_of("\\");
				}
				outfile << files[frame_counter].substr(iPos + 1) << " " << obj_id << " " << (*it).x << " "
					<< (*it).y << " " << (*it).width << " "
					<< (*it).height << endl;
				obj_id++;
			}

			//read the next frame
			++frame_counter;
			//��ȡ��һ֡
			while (1)
			{
				if (frame_counter >= iFileSize)
				{
					goto EndProgram;
				}
				img = cvLoadImage((files[frame_counter]).c_str(), -1);
				//����cvLoadImage()�ĵ�1 ��������ͼ���ļ���·��.
				//��2 �������Ƕ�ȡͼ��ķ�ʽ:-1 ��ʾ����ͼ�������������ȡ,1 ��ʾǿ�Ʋ�ɫ��,0 ��ʾ
				//ǿ�ƻ�ֵ��.
				if (img == NULL)
				{
					cout << "�޷���ȡͼ�� " << files[frame_counter] << endl;

					frame_counter++;
				}
				else
				{
					break;
				}
			}
			//���浽ͼ������
			cvReleaseImage(&img1);
			//img1 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 3);
			img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
			//������һ֡ͼ�񵽲�������
			cvCopy(img, img1);

			//ÿ֡�еı�ע��������0
			obj_id = 0;
			//�����ע���б�
			biaozhu_boxs.clear();
			break;

		case 'c':
			//�����ע���б�
			biaozhu_boxs.clear();
			//���¿�����ǰ֡ͼ�񵽲�������
			cvCopy(img, img1);
		}
		cvShowImage("Image", img1);
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