#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ������������� ��������� �������
    ui->myPlot->setTitle(QString::fromLocal8Bit("�������� ��������� �����������"));

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
        QwtPlot::xBottom,QString::fromLocal8Bit("����� � �����"));  // ������������ ������ �����
    ui->myPlot->setAxisScale(QwtPlot::xBottom,0,24);// ����������� � ������������ ������� ��� ���
    ui->myPlot->enableAxis(QwtPlot::xTop,true);     // ��������� ������� �����
    ui->myPlot->axisScaleDraw(QwtPlot::xTop)->enableComponent(
        QwtAbstractScaleDraw::Labels,false);        // ��������� ����� �� ���
                                                    // �������������
    ui->myPlot->setAxisScale(QwtPlot::xTop,0,24);   // ����������� � ������������ ������� ��� ���
    ui->myPlot->setAxisTitle(
        QwtPlot::yLeft,QString::fromLocal8Bit("����� �������")); // ������������ ����� �����
    ui->myPlot->setAxisScale(QwtPlot::yLeft,-17,3); // ����������� � ������������ ������� ��� ���
    ui->myPlot->enableAxis(QwtPlot::yRight,true);   // ��������� ������ �����
    ui->myPlot->setAxisTitle(                                       // �������������
        QwtPlot::yRight,QString::fromLocal8Bit("����� ����������"));// ������������ ������ �����
    ui->myPlot->setAxisScale(QwtPlot::yRight,-17*1.8+32,3*1.8+32);  // ����������� � ������������ �������

    // ������� ������ � ������������� "t"
    curv = new QwtPlotCurve(QString("t"));
    // ��������� ��� ��� ����������� ��� ����������
    curv->setRenderHint(QwtPlotItem::RenderAntialiased);
    // ��������� ���� ���������� - �����
    curv->setPen(QPen(Qt::darkBlue));

    // ������ ��� �����������
    const double X[] = {  0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22, 24};
    const double Y[] = {-10,-11,-14,-15,-14,-11, -7, -2,  1,  1, -2, -4, -5};

    // �������� ������ ������
#if QWT_VERSION < 0x060000
    // ���������� ��� ���������� Qwt ������ 5.x.x
    curv->setData(X,Y,13);
#else
    // ���������� ��� ���������� Qwt ������ 6.x.x
    curv->setSamples(X,Y,13);
#endif

    // �������� ������ �� ������
    curv->attach(ui->myPlot);

    // ������������� ������
    ui->myPlot->replot();

    // ��������� ��� ������� ��� ����� �������
    ui->myPlot->canvas()->setCursor(Qt::ArrowCursor);

    // ������� �������� ��������������� � ����������� �������
    zoom = new QwtChartZoom(ui->myPlot);
    // ������������� ���� �����, �������� ����� ������ ������� - �����
    zoom->setRubberBandColor(Qt::gray);
    // �������� ������ �����
    zoom->setLightMode(true);
    // �������� ��������� ���������
    zoom->indicateDragBand(QwtChartZoom::disDetailed);
    // ������������� ����� ������� ���������� �����������
    zoom->setDragBandColor(Qt::gray);

    // ������� ��������� ��������� ����� �� ������ �������
    axzmsvc = new QAxisZoomSvc();
    // �������� ��� ������ �����
    axzmsvc->setLightMode(true);
    // ������������� ���� �������, ������������� ��������������� �����
    axzmsvc->setAxisRubberBandColor(Qt::gray);
    // ����������� ��������� � ��������� ���������������
    axzmsvc->attach(zoom);
}

MainWindow::~MainWindow()
{
    // ������� ���, ��� ���� ������� � ������������:
    delete grid;    // ������������ �����
    delete curv;    // ������

    delete axzmsvc;
    delete zoom;    // �������� ��������������� � ����������� �������

    delete ui;
}
