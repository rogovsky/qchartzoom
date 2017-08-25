#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ������������� ��������� �������
    ui->myPlot->setTitle(QString::fromLocal8Bit("����������� U(t)"));

    // ������� �������
    leg = new QwtLegend();
    // ��������� �� �������������
    leg->setItemMode(QwtLegend::ReadOnlyItem);
    //  ��������� ������� � ������� ����� �������
    ui->myPlot->insertLegend(leg,QwtPlot::TopLegend);

    // ������� �����
    grid = new QwtPlotGrid;
    // ��������� ����������� ����� �����, ��������������� ��������������� �������� ������ �����
    grid->enableXMin(true);
    // ��������� ����� ��� ����� �����:
    grid->setMajPen(QPen(Qt::black,0,Qt::DotLine)); // ������ ��� �������� �������
    grid->setMinPen(QPen(Qt::gray,0,Qt::DotLine));  // ����� ��� ���������������
                                                    // ����� ����� ����� ������������ ���������� ������
    // ��������� ��������� ����� � ��������
    grid->attach(ui->myPlot);

    // �������������
    ui->myPlot->setAxisTitle(
        QwtPlot::xBottom,QString::fromLocal8Bit("t, ���")); // ������������ ������ �����
    ui->myPlot->setAxisScale(QwtPlot::xBottom,-0.25,8.25);  // ����������� � ������������ ������� ��� ���
    ui->myPlot->setAxisTitle(
        QwtPlot::yLeft,QString::fromLocal8Bit("U, �"));     // ������������ ����� �����
    ui->myPlot->setAxisScale(QwtPlot::yLeft,-1.25,1.25);    // ����������� � ������������ ������� ��� ���

    // ������� ������ ������ � ������������� "U1(t)"
    curv1 = new QwtPlotCurve(QString("U1(t)"));
    // ��������� ��� ��� ����������� ��� ����������
    curv1->setRenderHint(QwtPlotItem::RenderAntialiased);
    // ��������� ���� ���������� - �������
    curv1->setPen(QPen(Qt::red));

    // ������� ������� ��� ����� ������ ������
#if QWT_VERSION < 0x060000
    // ���������� ��� ���������� Qwt ������ 5.x.x
    QwtSymbol symbol1;                      // ��������� ��������� ������ QwtSymbol
    symbol1.setStyle(QwtSymbol::Ellipse);   // ��������� �������� ����� � ������
    symbol1.setPen(QColor(Qt::black));      // ���� ���������� � ������
    symbol1.setSize(5);                     // ������ � 5
    curv1->setSymbol(symbol1);              // ����������� ��� � ������
#else
    // ���������� ��� ���������� Qwt ������ 6.x.x
    QwtSymbol *symbol1 = new QwtSymbol();
    symbol1->setStyle(QwtSymbol::Ellipse);
    symbol1->setPen(QColor(Qt::black));
    symbol1->setSize(4);
    curv1->setSymbol(symbol1);
#endif

    // ������� ������ ������ � ������������� "U2(t)"
    curv2 = new QwtPlotCurve(QString("U2(t)"));
    // ����������� ��� ���������� �� ���������, �.�. ����� �� ������ ����� �����
    // ��������� ���� ���������� - �����-�������
    curv2->setPen(QPen(Qt::darkGreen));

    // ���������� ������
    const int N1 = 128;     // ��� ������ ������
    const int N2 = 262144;  // ��� ������ ������

    // �������� ���� ������ ��� ���������� ������ �, Y ((����� ��� ���� ��������))
    double *X1 = (double *)malloc((2*N1+2*N2)*sizeof(double));
    double *Y1 = X1 + N1;
    double *X2 = Y1 + N1;
    double *Y2 = X2 + N2;

    // ���������� ������ ��� ������ ������
    // X1 ��������� �������� �� 0 �� 8
    // ��� ���������� �� X1 ��� ������ ������
    // (������� �� ���������� ����� N1)
    double h = 8./(N1-1);
    for (int k = 0; k < N1; k++)
    {
        X1[k] = k*h;
        // Y1(X1) � ��������� � ��������� ����������, �������� � ��������� �����
        // ���������        1
        // �������          0.5
        // ��������� ����   -5*PI/12
        Y1[k] = cos(M_PI*X1[k]-5*M_PI/12);
    }
    // ���������� ������ ��� ������ ������
    // X2 ��������� �������� �� 0 �� 8
    // ��� ���������� �� X2 ��� ������ ������
    // (������� �� ���������� ����� N2)
    h = 8./(N2-1);
    for (int k = 0; k < N2; k++)
    {
        X2[k] = k*h;
        // Y2(X2) � ��������� � ��������� ����������, �������� � ��������� �����
        // ���������        0.7
        // �������          4
        // ��������� ����   PI/9
        Y2[k] = 0.7 * cos(8*M_PI*X2[k]+M_PI/9);
    }

    // �������� ������ �������������� ������
#if QWT_VERSION < 0x060000
    // ���������� ��� ���������� Qwt ������ 5.x.x
    curv1->setData(X1,Y1,N1);
    curv2->setData(X2,Y2,N2);
#else
    // ���������� ��� ���������� Qwt ������ 6.x.x
    curv1->setSamples(X1,Y1,N1);
    curv2->setSamples(X2,Y2,N2);
#endif

    // ����������� ���������� ������
    free((void *)X1);

    // �������� ������ �� ������
    curv1->attach(ui->myPlot);
    curv2->attach(ui->myPlot);

    // ������������� ������
    ui->myPlot->replot();

    // ��������� ��� ������� ��� ����� �������
    ui->myPlot->canvas()->setCursor(Qt::ArrowCursor);

    // ������� �������� ��������������� � ����������� �������
    zoom = new QwtChartZoom(ui->myPlot);
    // ������������� ���� �����, �������� ����� ������ ������� - �����
    zoom->setRubberBandColor(Qt::white);

    // ������� ��������� ��������� �������� ��������� ������ ����
    whlzmsvc = new QWheelZoomSvc();
    // ����������� ��������� � ��������� ���������������
    whlzmsvc->attach(zoom);

    // ������� ��������� ��������� ����� �� ������ �������
    axzmsvc = new QAxisZoomSvc();
    // ����������� ��������� � ��������� ���������������
    axzmsvc->attach(zoom);
}

MainWindow::~MainWindow()
{
    // ������� ���, ��� ���� ������� � ������������:
    delete leg;     // �������
    delete grid;    // ������������ �����
        // ������
    delete curv1;   // ������
    delete curv2;   // ������

    delete axzmsvc;
    delete whlzmsvc;
    delete zoom;    // �������� ��������������� � ����������� �������

    delete ui;
}
