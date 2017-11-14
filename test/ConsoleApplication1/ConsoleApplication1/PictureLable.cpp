// pic_label.cpp : 定义控制台应用程序的入口点。
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

//全局变量
bool is_drawing = false;
//最终的标志盒子
vector<CvRect> biaozhu_boxs;
//绘制时记录的临时框
CvRect drawing_box;
IplImage *img, *img1;

static void help();
static void onMouse(int event, int x, int y, int, void*);
void getFiles(string path, vector<string>& files);

//1.鼠标框定目标【可多个】
//2.按n，进入下一帧，保存当前框定目标坐标到txt文本【可多个】
//3.按c，清除当前帧所有已标定区域【人总有犯错的时候】或者上一帧遗留的区域
//文件保存格式：
//帧编号目标编号矩形左上角坐标矩形右下角坐标
//int _tmain(int argc, _TCHAR* argv[])
int main()
{
	CvFont font;
	CvScalar scalar;
	char text[10];

	// 初始化字体
	double hScale = 1;
	double vScale = 1;
	int lineWidth = 3;// 相当于写字的线条
	scalar = CV_RGB(255, 0, 0);
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, hScale, vScale, 0, lineWidth);//初始化字体，准备写到图片上的 

	int frame_counter = 0;
	int obj_id = 0;

	////读取视频
	//CvCapture *capture = cvCreateFileCapture("a.avi");
	////获取一帧
	//img = cvQueryFrame(capture);
	//img1 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 3);
	////保存到图像区域
	//cvCopy(img, img1);
	//读取所有图像文件名
	string strPath = "..\\LightImages";
	//cin >> strPath;   //也可以用此段代码直接在DOS窗口输入地址，此时只需正常的单反斜杠即可 
	vector<string> files;
	getFiles(strPath, files);  //files为返回的文件名构成的字符串向量组  
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
		//函数cvLoadImage()的第1 个参数是图像文件的路径.
		//第2 个参数是读取图像的方式:-1 表示按照图像本身的类型来读取,1 表示强制彩色化,0 表示
		//强制灰值化.
		if (img == NULL)
		{
			cout << "无法读取图像 " << files[0] << endl;

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
	//保存到图像区域
	cvCopy(img, img1);

	//输出文本文件
	ofstream outfile(strPath + "\\LightLable.txt");
	//帮助信息提示
	help();

	//绘制线框
	for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
	{
		cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
	}
	//显示原始图片
	cvShowImage("Image", img);

	//鼠标回调
	cvSetMouseCallback("Image", onMouse, 0);

	while (1)
	{
		//等待输入
		int c = cvWaitKey(0);
		//esc键退出
		if ((c & 255) == 27)
		{
			cout << "Exiting ...\n";
			break;
		}

		switch ((char)c)
		{
			//读取下一帧
		case 'n':
			//记录当前帧的所有标注信息
			for (vector<CvRect>::iterator it = biaozhu_boxs.begin();it != biaozhu_boxs.end();++it)
			{
				cvRectangle(img1, cvPoint((*it).x, (*it).y), cvPoint((*it).x + (*it).width, (*it).y + (*it).height), CV_RGB(0, 255, 0));
				//形成文本记录的对象名称
				_itoa(obj_id, text, 10);
				//在图片中输出字符，记录是当前帧的第几个框
				cvPutText(img1, text, cvPoint((*it).x, (*it).y), &font, CV_RGB(255, 255, 255));

				//输出到标记记录文本
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
			//获取下一帧
			while (1)
			{
				if (frame_counter >= iFileSize)
				{
					goto EndProgram;
				}
				img = cvLoadImage((files[frame_counter]).c_str(), -1);
				//函数cvLoadImage()的第1 个参数是图像文件的路径.
				//第2 个参数是读取图像的方式:-1 表示按照图像本身的类型来读取,1 表示强制彩色化,0 表示
				//强制灰值化.
				if (img == NULL)
				{
					cout << "无法读取图像 " << files[frame_counter] << endl;

					frame_counter++;
				}
				else
				{
					break;
				}
			}
			//保存到图像区域
			cvReleaseImage(&img1);
			//img1 = cvCreateImage(cvSize(img->width, img->height), IPL_DEPTH_8U, 3);
			img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
			//拷贝下一帧图像到操作区域
			cvCopy(img, img1);

			//每帧中的标注框索引清0
			obj_id = 0;
			//清除标注框列表
			biaozhu_boxs.clear();
			break;

		case 'c':
			//清除标注框列表
			biaozhu_boxs.clear();
			//重新拷贝当前帧图像到操作区域
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
		//左键按下
	case CV_EVENT_LBUTTONDOWN:
		//the left up point of the rect
		is_drawing = true;
		drawing_box.x = x;
		drawing_box.y = y;
		break;

		//鼠标移动
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

		//左键弹起
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
	//文件句柄  
	long long hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(inPath).append("\\*.*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			//如果是目录,迭代之  
			//如果不是,加入列表  
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