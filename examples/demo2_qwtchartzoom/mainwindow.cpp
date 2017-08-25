#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // устанавливаем заголовок графика
    ui->myPlot->setTitle(QString::fromLocal8Bit("Суточные колебания температуры"));

    // создаем сетку
    grid = new QwtPlotGrid;
    // разрешаем отображение линий сетки, соответствующих вспомогательным делениям нижней шкалы
    grid->enableXMin(true);
    // назначаем цвета для линий сетки:
    grid->setMajPen(QPen(Qt::black,0,Qt::DotLine)); // черный для основных делений
    grid->setMinPen(QPen(Qt::gray,0,Qt::DotLine));  // серый для вспомогательных
                                                    // линии сетки будут изображаться пунктирной линией
    // связываем созданную сетку с графиком
    grid->attach(ui->myPlot);

    // устанавливаем
    ui->myPlot->setAxisTitle(
        QwtPlot::xBottom,QString::fromLocal8Bit("Время в часах"));  // наименование нижней шкалы
    ui->myPlot->setAxisScale(QwtPlot::xBottom,0,24);// минимальную и максимальную границы для нее
    ui->myPlot->enableAxis(QwtPlot::xTop,true);     // разрешаем верхнюю шкалу
    ui->myPlot->axisScaleDraw(QwtPlot::xTop)->enableComponent(
        QwtAbstractScaleDraw::Labels,false);        // запрещаем метки на ней
                                                    // устанавливаем
    ui->myPlot->setAxisScale(QwtPlot::xTop,0,24);   // минимальную и максимальную границы для нее
    ui->myPlot->setAxisTitle(
        QwtPlot::yLeft,QString::fromLocal8Bit("Шкала Цельсия")); // наименование левой шкалы
    ui->myPlot->setAxisScale(QwtPlot::yLeft,-17,3); // минимальную и максимальную границы для нее
    ui->myPlot->enableAxis(QwtPlot::yRight,true);   // разрешаем правую шкалу
    ui->myPlot->setAxisTitle(                                       // устанавливаем
        QwtPlot::yRight,QString::fromLocal8Bit("Шкала Фаренгейта"));// наименование правой шкалы
    ui->myPlot->setAxisScale(QwtPlot::yRight,-17*1.8+32,3*1.8+32);  // минимальную и максимальную границы

    // создаем кривую с наименованием "t"
    curv = new QwtPlotCurve(QString("t"));
    // разрешаем для нее сглаживание при прорисовке
    curv->setRenderHint(QwtPlotItem::RenderAntialiased);
    // назначаем цвет прорисовки - синий
    curv->setPen(QPen(Qt::darkBlue));

    // данные для отображения
    const double X[] = {  0,  2,  4,  6,  8, 10, 12, 14, 16, 18, 20, 22, 24};
    const double Y[] = {-10,-11,-14,-15,-14,-11, -7, -2,  1,  1, -2, -4, -5};

    // передаем кривой данные
#if QWT_VERSION < 0x060000
    // инструкции для библиотеки Qwt версии 5.x.x
    curv->setData(X,Y,13);
#else
    // инструкции для библиотеки Qwt версии 6.x.x
    curv->setSamples(X,Y,13);
#endif

    // помещаем кривую на график
    curv->attach(ui->myPlot);

    // перестраиваем график
    ui->myPlot->replot();

    // назначаем тип курсора для канвы графика
    ui->myPlot->canvas()->setCursor(Qt::ArrowCursor);

    // создаем менеджер масштабирования и перемещения графика
    zoom = new QwtChartZoom(ui->myPlot);
    // устанавливаем цвет рамки, задающей новый размер графика - серый
    zoom->setRubberBandColor(Qt::gray);
    // включаем легкий режим
    zoom->setLightMode(true);
    // выбираем подробную индикацию
    zoom->indicateDragBand(QwtChartZoom::disDetailed);
    // устанавливаем цвета виджета индикатора перемещения
    zoom->setDragBandColor(Qt::gray);

    // создаем интерфейс изменения одной из границ графика
    axzmsvc = new QAxisZoomSvc();
    // включаем ему легкий режим
    axzmsvc->setLightMode(true);
    // устанавливаем цвет виджета, индицирующего масштабирование шкалы
    axzmsvc->setAxisRubberBandColor(Qt::gray);
    // прикрепляем интерфейс к менеджеру масштабирования
    axzmsvc->attach(zoom);
}

MainWindow::~MainWindow()
{
    // удаляем все, что было создано в конструкторе:
    delete grid;    // координатную сетку
    delete curv;    // кривую

    delete axzmsvc;
    delete zoom;    // менеджер масштабирования и перемещения графика

    delete ui;
}
