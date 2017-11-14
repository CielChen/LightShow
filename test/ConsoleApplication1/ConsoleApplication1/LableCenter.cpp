// pic_label.cpp : 定义控制台应用程序的入口点。
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

	//读取所有标注文件名
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
	//依次处理每个标注文件，将标注位置读取
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
	//根据需要读取图像并修改像素值
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
		//函数cvLoadImage()的第1 个参数是图像文件的路径.
		//第2 个参数是读取图像的方式:-1 表示按照图像本身的类型来读取,1 表示强制彩色化,0 表示
		//强制灰值化.
		if (img == NULL)
		{
			cout << "无法读取图像 " << strPathFile << endl;

			continue;
		}
		img1 = cvCreateImage(cvSize(img->width, img->height), img->depth, img->nChannels);
		//保存到图像区域
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

		//显示原始图片
		cvShowImage("Image", img);
		cvShowImage("ImageLable", img1);

		string strResultPath = "..\\LightImagesResult\\" + strFile;
		cvSaveImage(strResultPath.c_str(), img1);
		//保存到图像区域
		cvReleaseImage(&img1);
	}
	
	//帮助信息提示
	help();

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

vector<string> split(const string &s, const string &seperator) {
	vector<string> result;
	typedef string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		//找到字符串中首个不等于分隔符的字母；
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

		//找到又一个分隔符，将两个分隔符之间的字符串取出；
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
