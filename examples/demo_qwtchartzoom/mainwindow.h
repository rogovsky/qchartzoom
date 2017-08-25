#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <qwt_plot_canvas.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include "qwtchartzoom.h"
#include "qwheelzoomsvc.h"
#include "qaxiszoomsvc.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QwtLegend *leg;             // легенда
    QwtPlotGrid *grid;          // координатная сетка
    QwtPlotCurve *curv1,*curv2; // кривые, отображаемые на графике

// Менеджер масштабирования и перемещения графика в стиле TChart
    QwtChartZoom *zoom;
// Интерфейс масштабирования графика вращением колеса мыши (дополнение к менеджеру)
    QWheelZoomSvc *whlzmsvc;
// Интерфейс изменения одной из границ графика (дополнение к менеджеру)
    QAxisZoomSvc *axzmsvc;
};

#endif // MAINWINDOW_H
