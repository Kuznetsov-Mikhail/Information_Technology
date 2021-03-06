﻿
// Signal_decodingDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "Signal_decoding.h"
#include "Signal_decodingDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// Диалоговое окно CSignaldecodingDlg



CSignaldecodingDlg::CSignaldecodingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIGNAL_DECODING_DIALOG, pParent)
	, bits_count(10)
	, sampling(96000)
	, bitrate(9600)
	, snr(20)
	, input_data("")
	, output_data("")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSignaldecodingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC1, viewer1);
	//DDX_Control(pDX, IDC_PIC2, viewer2);
	//DDX_Control(pDX, IDC_PIC3, viewer3);
	DDX_Text(pDX, IDC_EDIT1, bits_count);
	DDX_Text(pDX, IDC_EDIT2, sampling);
	DDX_Text(pDX, IDC_EDIT3, bitrate);
	DDX_Text(pDX, IDC_EDIT4, snr);
	DDX_Text(pDX, IDC_EDIT5, input_data);
	DDX_Text(pDX, IDC_EDIT6, output_data);
}

BEGIN_MESSAGE_MAP(CSignaldecodingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CSignaldecodingDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON2, &CSignaldecodingDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CSignaldecodingDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CSignaldecodingDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// Обработчики сообщений CSignaldecodingDlg

BOOL CSignaldecodingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CSignaldecodingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CSignaldecodingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSignaldecodingDlg::OnBnClickedCancel()
{
	// TODO: добавьте свой код обработчика уведомлений
	CDialogEx::OnCancel();
}

//code
void CSignaldecodingDlg::OnBnClickedButton2()
{
	UpdateData(1);
	decoder._sp = { bits_count, sampling, bitrate, snr };
	decoder.generate(_s, input_data);
	draw.clear();
	draw.resize(2);
	draw[0].resize(_s.size());
	draw[1].resize(_s.size());
	for (int i = 0; i < _s.size(); i++)
	{
		draw[0][i]= _s[i].real();
		draw[1][i] = _s[i].imag();
	}
	ViewerDraw(draw, 0, _s.size(), viewer1, "coding.png", false);
	UpdateData(0);
}
//decode
void CSignaldecodingDlg::OnBnClickedButton3()
{
	UpdateData(1);
	_ccfWithGC.clear(); _ccfWithGC.resize(4);
	decoder.ccf(_s, _ccfWithGC[0], _ccfWithGC[1], _ccfWithGC[2], _ccfWithGC[3], output_data);
	ViewerDraw(_ccfWithGC, 0, _ccfWithGC[0].size(), viewer1, "decoding.png", false);
	UpdateData(0);
}
void CSignaldecodingDlg::ViewerDraw(vector<vector<double>>& data, 
									double Xmin, double Xmax, CChartViewer& viewer_num,
									string PathPic, bool podpisi)
{
	if (data.empty())return;
	// In this example, we simply use random data for the 3 data series.
	vector<DoubleArray> Arr_dataReal; Arr_dataReal.resize(data.size());
	for (int i = 0; i < data.size(); i++)
	{
		Arr_dataReal[i] = vectorToArray(data[i]);
	}

	vector<double>Datatime;

	double OXstep = (Xmax - Xmin) / (data[0].size() - 1);
	for (double i = Xmin; i <= Xmax; i += OXstep)Datatime.push_back(i);
	DoubleArray timeStamps = vectorToArray(Datatime);

	// Create a XYChart object of size 600 x 400 pixels
	XYChart* c = new XYChart(850, 240);

	// Add a title box using grey (0x555555) 20pt Arial font
	//c->addTitle("", "arial.ttf", 20, 0x555555);

	// Set the plotarea at (70, 70) and of size 500 x 300 pixels, with transparent background and
	// border and light grey (0xcccccc) horizontal grid lines
	c->setPlotArea(70, 50, 700, 120, Chart::Transparent, -1, Chart::Transparent, 0xcccccc);

	// Add a legend box with horizontal layout above the plot area at (70, 35). Use 12pt Arial font,
	// transparent background and border, and line style legend icon.
	LegendBox* b = c->addLegend(20, 5, false, "arial.ttf", 12);
	b->setBackground(Chart::Transparent, Chart::Transparent);
	b->setLineStyleKey();

	// Set axis label font to 12pt Arial
	c->xAxis()->setLabelStyle("arial.ttf", 12);
	c->yAxis()->setLabelStyle("arial.ttf", 12);

	// Set the x and y axis stems to transparent, and the x-axis tick color to grey (0xaaaaaa)
	c->xAxis()->setColors(Chart::TextColor, Chart::TextColor, Chart::TextColor, 0xaaaaaa);
	c->yAxis()->setColors(Chart::TextColor);

	// Set the major/minor tick lengths for the x-axis to 10 and 0.
	c->xAxis()->setTickLength(10, 0);

	// For the automatic axis labels, set the minimum spacing to 80/40 pixels for the x/y axis.
	c->xAxis()->setTickDensity(80);
	c->yAxis()->setTickDensity(40);

	// Add a title to the y axis using dark grey (0x555555) 14pt Arial font
	c->yAxis()->setTitle("", "arial.ttf", 14, 0x555555);

	// Add a line layer to the chart with 3-pixel line width
	LineLayer* layer = c->addLineLayer();
	layer->setLineWidth(3);
	//
	if (podpisi) layer->setDataLabelFormat("{value|2} ");
	// Add 3 data series to the line layer
	for (int i = 0; i < Arr_dataReal.size(); i++)
	{
		stringstream ss;
		ss << "Data " << i;
		string name = ss.str();
		layer->addDataSet(Arr_dataReal[i], -1, name.c_str());
	}
	// The x-coordinates for the line layer
	layer->setXData(timeStamps);
	viewer_num.setChart(c);
	const char* chPathPic = PathPic.c_str();
	c->makeChart(chPathPic);
	delete c;
}



void CSignaldecodingDlg::OnBnClickedButton1()
{
	UpdateData(1);
	draw.clear();
	draw.resize(1);
	int try_size = 50;
	for (int i = -15; i <= -5; i+=1)
	{
		double error=0;
		for (int j = 0; j < try_size; j++)
		{
			double buffer=0;
			decoder._sp = { bits_count, sampling, bitrate, (double)i };
			decoder.generate(_s, input_data);
			_ccfWithGC.clear(); _ccfWithGC.resize(4);
			decoder.ccf(_s, _ccfWithGC[0], _ccfWithGC[1], _ccfWithGC[2], _ccfWithGC[3], output_data);
			for (int b = 0; b < bits_count; b++)
			{
				if (decoder._in_bits[b] == decoder._out_bits[b]) buffer += 1;
			}
			buffer /= bits_count;
			error += buffer;
		}
		error /= try_size;
		draw[0].push_back(error);
	}
	ViewerDraw(draw, -15, -5, viewer1, "errors.png", false);
	UpdateData(0);
}
