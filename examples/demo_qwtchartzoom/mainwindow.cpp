#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // устанавливаем заголовок графика
    ui->myPlot->setTitle(QString::fromLocal8Bit("Зависимость U(t)"));

    // создаем легенду
    leg = new QwtLegend();
    // запрещаем ее редактировать
#if QWT_VERSION < 0x060099   // qwt-5.2.x + qwt-6.0.x
    leg->setItemMode(QwtLegend::ReadOnlyItem);
#else                        // qwt-6.1.x
    leg->setDefaultItemMode(QwtLegendData::ReadOnly);
#endif
    //  добавляем легенду в верхнюю часть графика
    ui->myPlot->insertLegend(leg,QwtPlot::TopLegend);

    // создаем сетку
    grid = new QwtPlotGrid;
    // разрешаем отображение линий сетки, соответствующих вспомогательным делениям нижней шкалы
    grid->enableXMin(true);
    // назначаем цвета для линий сетки:
#if QWT_VERSION < 0x060099   // qwt-5.2.x + qwt-6.0.x
    grid->setMajPen(QPen(Qt::black,0,Qt::DotLine)); // черный для основных делений
    grid->setMinPen(QPen(Qt::gray,0,Qt::DotLine));  // серый для вспомогательных
#else                        // qwt-6.1.x
    grid->setMajorPen(QPen(Qt::black,0,Qt::DotLine)); // черный для основных делений
    grid->setMinorPen(QPen(Qt::gray,0,Qt::DotLine));  // серый для вспомогательных
#endif
                                                    // линии сетки будут изображаться пунктирной линией
    // связываем созданную сетку с графиком
    grid->attach(ui->myPlot);

    // устанавливаем
    ui->myPlot->setAxisTitle(
        QwtPlot::xBottom,QString::fromLocal8Bit("t, мкс")); // наименование нижней шкалы
    ui->myPlot->setAxisScale(QwtPlot::xBottom,-0.25,8.25);  // минимальную и максимальную границы для нее
    ui->myPlot->setAxisTitle(
        QwtPlot::yLeft,QString::fromLocal8Bit("U, В"));     // наименование левой шкалы
    ui->myPlot->setAxisScale(QwtPlot::yLeft,-1.25,1.25);    // минимальную и максимальную границы для нее

    // создаем первую кривую с наименованием "U1(t)"
    curv1 = new QwtPlotCurve(QString("U1(t)"));
    // разрешаем для нее сглаживание при прорисовке
    curv1->setRenderHint(QwtPlotItem::RenderAntialiased);
    // назначаем цвет прорисовки - красный
    curv1->setPen(QPen(Qt::red));

    // создаем маркеры для точек первой кривой
#if QWT_VERSION < 0x060000
    // инструкции для библиотеки Qwt версии 5.x.x
    QwtSymbol symbol1;                      // объявляем экземпляр класса QwtSymbol
    symbol1.setStyle(QwtSymbol::Ellipse);   // назначаем маркерам стиль – эллипс
    symbol1.setPen(QColor(Qt::black));      // цвет прорисовки – черный
    symbol1.setSize(5);                     // размер – 5
    curv1->setSymbol(symbol1);              // прикрепляем его к кривой
#else
    // инструкции для библиотеки Qwt версии 6.x.x
    QwtSymbol *symbol1 = new QwtSymbol();
    symbol1->setStyle(QwtSymbol::Ellipse);
    symbol1->setPen(QColor(Qt::black));
    symbol1->setSize(4);
    curv1->setSymbol(symbol1);
#endif

    // создаем вторую кривую с наименованием "U2(t)"
    curv2 = new QwtPlotCurve(QString("U2(t)"));
    // сглаживание при прорисовке не разрешаем, т.к. точек на кривой будет много
    // назначаем цвет прорисовки - темно-зеленый
    curv2->setPen(QPen(Qt::darkGreen));

    // количество данных
    const int N1 = 128;     // для первой кривой
    const int N2 = 262144;  // для второй кривой

    // выделяем блок памяти под размещение данных Х, Y ((общий для всех массивов))
    double *X1 = (double *)malloc((2*N1+2*N2)*sizeof(double));
    double *Y1 = X1 + N1;
    double *X2 = Y1 + N1;
    double *Y2 = X2 + N2;

    // подготовка данных для первой кривой
    // X1 принимает значения от 0 до 8
    // шаг приращения по X1 для первой кривой
    // (зависит от количества точек N1)
    double h = 8./(N1-1);
    for (int k = 0; k < N1; k++)
    {
        X1[k] = k*h;
        // Y1(X1) – синусоида с заданными амплитудой, частотой и начальной фазой
        // амплитуда        1
        // частота          0.5
        // начальная фаза   -5*PI/12
        Y1[k] = cos(M_PI*X1[k]-5*M_PI/12);
    }
    // подготовка данных для второй кривой
    // X2 принимает значения от 0 до 8
    // шаг приращения по X2 для второй кривой
    // (зависит от количества точек N2)
    h = 8./(N2-1);
    for (int k = 0; k < N2; k++)
    {
        X2[k] = k*h;
        // Y2(X2) – синусоида с заданными амплитудой, частотой и начальной фазой
        // амплитуда        0.7
        // частота          4
        // начальная фаза   PI/9
        Y2[k] = 0.7 * cos(8*M_PI*X2[k]+M_PI/9);
    }

    // передаем кривым подготовленные данные
#if QWT_VERSION < 0x060000
    // инструкции для библиотеки Qwt версии 5.x.x
    curv1->setData(X1,Y1,N1);
    curv2->setData(X2,Y2,N2);
#else
    // инструкции для библиотеки Qwt версии 6.x.x
    curv1->setSamples(X1,Y1,N1);
    curv2->setSamples(X2,Y2,N2);
#endif

    // освобождаем выделенную память
    free((void *)X1);

    // помещаем кривые на график
    curv1->attach(ui->myPlot);
    curv2->attach(ui->myPlot);

    // перестраиваем график
    ui->myPlot->replot();

    // назначаем тип курсора для канвы графика
    ui->myPlot->canvas()->setCursor(Qt::ArrowCursor);

    // создаем менеджер масштабирования и перемещения графика
    zoom = new QwtChartZoom(ui->myPlot);
    // устанавливаем цвет рамки, задающей новый размер графика - белый
    zoom->setRubberBandColor(Qt::white);

    // создаем интерфейс изменения масштаба вращением колеса мыши
    whlzmsvc = new QWheelZoomSvc();
    // прикрепляем интерфейс к менеджеру масштабирования
    whlzmsvc->attach(zoom);

    // создаем интерфейс изменения одной из границ графика
    axzmsvc = new QAxisZoomSvc();
    // прикрепляем интерфейс к менеджеру масштабирования
    axzmsvc->attach(zoom);
}

MainWindow::~MainWindow()
{
    // удаляем все, что было создано в конструкторе:
    delete leg;     // легенду
    delete grid;    // координатную сетку
        // кривую
    delete curv1;   // первую
    delete curv2;   // вторую

    delete axzmsvc;
    delete whlzmsvc;
    delete zoom;    // менеджер масштабирования и перемещения графика

    delete ui;
}
