
// HandWrittingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "HandWritting.h"
#include "HandWrittingDlg.h"
#include "afxdialogex.h"

#include <ctime>
#include <iostream>
#include<sstream>

#include <opencv2/opencv.hpp>  
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/ml/ml.hpp>  
#include <io.h> //查找文件相关函数

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace cv;
using namespace ml;
ostringstream oss;

const int classsum = 10;	//图片共有10类，可修改
int imagesSum = 500;	//每类有张图片，可修改
Mat trainingData;	//训练数据，每一行一个训练图片
Mat labels;			//训练样本标签
vector<vector<String>> allImage;//二维链表存储所有图片信息
Ptr<SVM> SVM_params = SVM::create();

int test_i = 0;
int test_j = imagesSum;
bool begin_predict = false;
bool begin_read = false;
bool begin_train = true;


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHandWrittingDlg 对话框



CHandWrittingDlg::CHandWrittingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HANDWRITTING_DIALOG, pParent)
	, m_show_predict(_T(""))
	, m_prompt_message(_T("准备就绪，请先读取测试数据，然后再进行其他操作！"))
	, m_train_sample(_T("500"))
	, m_test_name(_T("0"))//测试图库文件夹
	, m_test_sample_num(_T("0"))//文件夹图片数量
	, m_NPre(_T("0"))//可预测图片数量
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	initSVM();
}

void CHandWrittingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Picture, m_picture);
	DDX_Text(pDX, IDC_ShowPre, m_show_predict);
	DDX_Text(pDX, IDC_PromptMessage, m_prompt_message);
	DDX_Text(pDX, IDC_TrainSample, m_train_sample);
	DDX_Text(pDX, IDC_TestName, m_test_name);
	DDX_Text(pDX, IDC_TestSampleNum, m_test_sample_num);

	DDX_Control(pDX, IDC_showcor, m_CCheck);
	DDX_Control(pDX, IDC_Npre, m_Npre);
	DDX_Text(pDX, IDC_Npre, m_NPre);
}

BEGIN_MESSAGE_MAP(CHandWrittingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TrainBtn, &CHandWrittingDlg::OnBnTrainbtn)
	ON_BN_CLICKED(IDC_PredictBtn, &CHandWrittingDlg::OnBnPredictbtn)
	ON_BN_CLICKED(IDC_ReadDataBtn, &CHandWrittingDlg::OnBnReaddatabtn)
	ON_BN_CLICKED(IDC_ConfirmBtn, &CHandWrittingDlg::OnBnConfirmbtn)
	ON_BN_CLICKED(IDC_LastBtn, &CHandWrittingDlg::OnBnLastbtn)
	ON_BN_CLICKED(IDC_NextBtn, &CHandWrittingDlg::OnBnNextbtn)
	ON_BN_CLICKED(IDC_UseMode, &CHandWrittingDlg::OnBnUsemode)
END_MESSAGE_MAP()


// CHandWrittingDlg 消息处理程序

BOOL CHandWrittingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHandWrittingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHandWrittingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHandWrittingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// 读取数据并进行图片数据化
void CHandWrittingDlg:: readTrainData() {
	m_prompt_message = (_T("正在读取数据"));
	//////////////////////从指定文件夹下提取图片//////////////////
	for (int label = 0; label < classsum; label++)//依次提取0到9文件夹中的图片
	{
		oss << "mnist_train/";
		oss << label << "/*.png";//图片名字后缀，oss可以结合数字与字符串

		string pattern = oss.str();//oss.str()输出oss字符串，并且赋给pattern
		oss.str("");//每次循环后把oss字符串清空

		vector<String> input_images_name;	//存放符合条件的图片地址
		glob(pattern, input_images_name, false);	//为false时，仅仅遍历指定文件夹内符合模式的文件，当为true时，同时遍历文件夹的子文件夹

		allImage.push_back(input_images_name);

		vector<Mat> input_images;
		Mat gray_sample;
		Mat thresh_sample;
		for (int i = 0; i < imagesSum; i++)//依次循环遍历每个文件夹中的图片
		{
			cvtColor(imread(input_images_name[i]), gray_sample, COLOR_BGR2GRAY);//灰度变换
			threshold(gray_sample, thresh_sample, 0, 255, THRESH_OTSU);//二值化
			//循环读取每张图片并且依次放在vector<Mat> input_images内
			input_images.push_back(thresh_sample);
			Mat dealimage = input_images[i];

			//注意：我们简单粗暴将整个图的所有像素作为了特征，因为我们关注更多的是整个的训练过程，
			//所以选择了最简单的方式完成特征提取工作，除此中外，特征提取的方式有很多，比如LBP，HOG等等
			//我们利用reshape()函数完成特征提取,
			//eshape(1, 1)的结果就是原图像对应的矩阵将被拉伸成一个一行的向量，作为特征向量。 
			dealimage = dealimage.reshape(1, 1);//降维打散
			trainingData.push_back(dealimage);//序列化后的图片依次存入
			labels.push_back(label);//把每个图片对应的标签依次存入
		}
	}
}

// 创建分类器并设置参数
void CHandWrittingDlg:: initSVM() {
	
	SVM_params->setType(SVM::C_SVC);//C_SVC用于分类，C_SVR用于回归
	SVM_params->setKernel(SVM::LINEAR);  //LINEAR线性核函数。SIGMOID为高斯核函数
	SVM_params->setDegree(0);//核函数中的参数degree,针对多项式核函数;
	SVM_params->setGamma(1);//核函数中的参数gamma,针对多项式/RBF/SIGMOID核函数; 
	SVM_params->setCoef0(0);//核函数中的参数,针对多项式/SIGMOID核函数；
	SVM_params->setC(1);//SVM最优问题参数，设置C-SVC，EPS_SVR和NU_SVR的参数；
	SVM_params->setNu(0);//SVM最优问题参数，设置NU_SVC， ONE_CLASS 和NU_SVR的参数； 
	SVM_params->setP(0);//SVM最优问题参数，设置EPS_SVR 中损失函数p的值. 
	//结束条件，即训练1000次或者误差小于0.01结束
	SVM_params->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 1000, 0.01));
}

// 训练模型（xml文件）
void CHandWrittingDlg:: trainData() {
	Mat reslabel;		//最终的训练样本标签
	Mat traindata;		//最终的训练数据
	Mat(trainingData).copyTo(traindata);//复制
	traindata.convertTo(traindata, CV_32FC1);//更改图片数据的类型，必要，不然会出错
	Mat(labels).copyTo(reslabel);//复制

	initSVM();		//初始化SVM模型

	Ptr<TrainData> tData = TrainData::create(traindata, ROW_SAMPLE, reslabel);	//训练数据和标签的结合

	//训练分类器
	SVM_params->train(tData);//训练

	//保存模型
	SVM_params->save("svm.xml");
}

// 真正的预测函数
void CHandWrittingDlg::predictData(int i, int j) {
		//显示
	CPaintDC dc(this); // device context for painting
	CRect rect;
	GetClientRect(&rect);
	CImage image;
	CString imgName(allImage[i][j].c_str());
	image.Load(imgName);
	image.BitBlt(dc, 0, 0, rect.Width(), rect.Height(), 0, 0);
	if (image.IsNull())    //判断有无图像  
		return;
	// 取得客户区尺寸  
	CRect zcRect;
	GetDlgItem(IDC_Picture)->GetClientRect(&zcRect);
	// 将图像显示在界面之上  
	image.Draw(GetDlgItem(IDC_Picture)->GetDC()->m_hDC,
		zcRect.left,
		zcRect.top,
		zcRect.Width(),
		zcRect.Height());
	image.Destroy();

	Mat src = imread(allImage[i][j]);
	cvtColor(src, src, COLOR_BGR2GRAY);
	threshold(src, src, 0, 255, THRESH_OTSU);
	Mat input;
	src = src.reshape(1, 1);//输入图片序列化
	input.push_back(src);
	input.convertTo(input, CV_32FC1);//更改图片数据的类型，必要，不然会出错

	float preval = SVM_params->predict(input);   //对所有行进行预测
	stringstream dss;
	dss << preval;
	m_show_predict = dss.str().c_str();

	if (preval != (float)test_i)
		m_CCheck.SetWindowText(_T("错误"));
	else 
		m_CCheck.SetWindowText(_T("正确"));
}

// 更新提示框信息，完成训练
void CHandWrittingDlg::OnBnTrainbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (begin_read) {
		trainData();
		m_prompt_message = (_T("完成数据训练！"));
	}

	UpdateData(FALSE);
}

// 读数据完成，开始预测，里面调用了真正的预测函数
void CHandWrittingDlg::OnBnPredictbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (begin_read) {
		m_prompt_message = (_T("正在预测数据！"));
		begin_predict = true;

		stringstream dss;
		dss << allImage[test_i].size();
		m_test_sample_num = dss.str().c_str();

		int i = _ttoi(m_test_sample_num);
		int j= _ttoi(m_NPre);
		j = i - imagesSum;

		m_NPre.Format(_T("%d"),j);

		predictData(test_i, test_j);
	}

	UpdateData(FALSE);
}

// 提示框更新，完成数据的读取
void CHandWrittingDlg::OnBnReaddatabtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	readTrainData();
	m_prompt_message = (_T("完成数据读取！"));
	begin_read = true;

	UpdateData(FALSE);
}

// 确认修改信息，更新变量
void CHandWrittingDlg::OnBnConfirmbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	string train_sam;
	train_sam = CW2A(m_train_sample.GetString());
	imagesSum = stoi(train_sam);

	string test_name;
	test_name = CW2A(m_test_name.GetString());
	test_i = stoi(test_name);

	test_j = 500;

	predictData(test_i, test_j);

	UpdateData(FALSE);
}

// 上一张图片，里面有调用真正预测函数
void CHandWrittingDlg::OnBnLastbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_CCheck.SetWindowText(_T(""));
	UpdateData(TRUE);

	if (begin_predict) {
		if (test_j > 0) {
			--test_j;
		}
		predictData(test_i, test_j);
	}
	
	UpdateData(FALSE);
}

// 下一张图片，里面有调用真正预测函数
void CHandWrittingDlg::OnBnNextbtn()
{
	// TODO: 在此添加控件通知处理程序代码
	m_CCheck.SetWindowText(_T(""));
	UpdateData(TRUE);

	if (begin_predict) {
		if (test_j < allImage[test_i].size() - 1) {
			++test_j;
		}
		predictData(test_i, test_j);
	}

	UpdateData(FALSE);
}

// 更新提示框框信息，完成模型加载
void CHandWrittingDlg::OnBnUsemode()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (begin_read && begin_train) {
		SVM_params = SVM::load("svm.xml");
		m_prompt_message = (_T("训练模型加载完成！"));
	}

	UpdateData(FALSE);
}