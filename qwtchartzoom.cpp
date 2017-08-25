/**********************************************************/
/*                                                        */
/*             ���������� ������ QwtChartZoom             */
/*                      ������ 1.5.2                      */
/*                                                        */
/* ���������� ��������� ������ ���������,                 */
/* �. �������-��������� ������������ ���., 2012 �.,       */
/* ��� ��������� �. �. ����������, �. �����������.        */
/*                                                        */
/* ����������� ��������� ������������� � ���������������. */
/* ���������� ������ �����������.                         */
/*                                                        */
/**********************************************************/

#include "qwtchartzoom.h"

#include <qwt_scale_widget.h>

// �����������
QwtChartZoom::QwtChartZoom(QwtPlot *qp) :
    QObject(qp)
{
    // �������� ������� ����
    mwin = generalParent(qp);
    // � ��������� ���������� ������� (������ �������)
    mwin->installEventFilter(this);

    // ���������� ���� ��� ����, ����� ����� ������ ���������� ��������
    // ������� ������� ������� ���� ������������� � �������� ��������
    isbF = false;
    // ���������� ������� ������
    convType = ctNone;

    // �������� ��������� QwtPlot, ��� ������� ����� ������������� ��� ��������������
    qwtp = qp;
    // ������������� ��� ��������, ����������� ������������ ������� �� ����������
    qp->setFocusPolicy(Qt::StrongFocus);

    qp->replot();   // ������������� ������
    // ������������ �����
    QwtPlotGrid *grid = NULL;
    // ���, � ������� ��� �����������
    int xAx;    // ��������������
    int yAx;    // ������������
    // �������� ������ ��������� �������
    QwtPlotItemList pil = qp->itemList();
    // ���������� ������ ���������
    for (int k=0; k < pil.count(); k++)
    {
        // �������� ��������� �� �������
        QwtPlotItem *pi = pil.at(k);
        // ���� ��� ������������ �����, ��
        if (pi->rtti() == QwtPlotItem::Rtti_PlotGrid)
        {
            // ���������� ��������� �� ���
            grid = (QwtPlotGrid *)pi;
            // �������� � ����� ��� ��� �����������
            xAx = grid->xAxis();    // �� ���� ��������������
            yAx = grid->yAxis();    // � ���� ������������
            // ���������� �������� ������ ���������
            break;
        }
    }
    // ���� ������������ ����� ���� �������, ��
    if (grid != NULL)
    {
        // ��������� �������� � �������������� �����, ������� ������������
        // ���, � ������� ����������� �����
            // ��������������
        allocAxis(xAx,QwtPlot::xBottom + QwtPlot::xTop - xAx,&masterX,&slaveX);
            // ������������
        allocAxis(yAx,QwtPlot::yLeft + QwtPlot::yRight - yAx,&masterY,&slaveY);
    }
    else    // ����� (������������ ����� �����������)
    {
        // ��������� �������� � �������������� �����, ������� ������������
            // ������ �� ��������������
        allocAxis(QwtPlot::xBottom,QwtPlot::xTop,&masterX,&slaveX);
            // � ����� �� ������������
        allocAxis(QwtPlot::yLeft,QwtPlot::yRight,&masterY,&slaveY);
    }
    // ���������� ���������� ������� �� �������������� �����
    mstHorDiv = qp->axisMaxMajor(masterX);
    slvHorDiv = qp->axisMaxMajor(slaveX);
/*    // �������� ����� �������� �������������� �����
    QwtScaleMap sm = qp->canvasMap(masterX);
    // � ������������� ����� �� ������� ��� ��������������
    qp->setAxisScale(slaveX,sm.s1(),sm.s2());*/
    // ���������� ���������� ������� �� ������������ �����
    mstVerDiv = qp->axisMaxMajor(masterY);
    slvVerDiv = qp->axisMaxMajor(slaveY);
    // ������� ���������� ������ �����
    isb_x = new QScaleBounds(qp,masterX,slaveX);    // ��������������
    isb_y = new QScaleBounds(qp,masterY,slaveY);    // � ������������
    qp->replot();   // ������������� ������

    // ������������� ���������� ���� �������
    qwtp->installEventFilter(this);
    // ��� ���� ���� �������
    for (int ax=0; ax < QwtPlot::axisCnt; ax++)
        // ��������� ���������� ������� (������ �������)
        qwtp->axisWidget(ax)->installEventFilter(this);

    // ������� ��������� ��������������� �������
    mnzmsvc = new QMainZoomSvc();
    // � ����������� ��� � ���������
    mnzmsvc->attach(this);

    // ������� ��������� ������������ �������
    drzmsvc = new QDragZoomSvc();
    // � ����������� ��� � ���������
    drzmsvc->attach(this);
}

// ����������
QwtChartZoom::~QwtChartZoom()
{
    // ������� ��������� ������������ �������
    delete drzmsvc;
    // ������� ��������� ��������������� �������
    delete mnzmsvc;
    // ������� ���������� ������ �����
    delete isb_x;    // ��������������
    delete isb_y;    // � ������������
}

// ����������� �������� ��������
QObject *QwtChartZoom::generalParent(QObject *p)
{
    // ����� � �������� ����������� �������� ������
    // (�������� � ������ ������ � ��������� �������)
    QObject *gp = p;
    // ���������� �������� �� ������� ������
    QObject *tp = gp->parent();
    // ���� �������� �� ������� ������ �� NULL
    while (tp != NULL)
    {
        // �������� �������:
        // ���������� � �������� ����������� �������� �������
        gp = tp;
        // ���������� �������� �� ��������� ������
        tp = gp->parent();
    }
    // ���������� � �������� �������� �������� ����������
    return gp;
}

// ���������� �������� � �������������� �����
void QwtChartZoom::allocAxis(int pre,int alt,
    QwtPlot::Axis *master,QwtPlot::Axis *slave)
{
    // �������� ����� ���������������� �����
    QwtScaleMap smp = qwtp->canvasMap(pre); // ���������������� �����
    QwtScaleMap sma = qwtp->canvasMap(alt); // � ��������������
    // ���� ���������������� ����� �������� ���
    // �������������� ����� ���������� � ��� ����
    // ������� ���������������� ����� �� ��������� ���
    // ������� �������������� ����� ���������, ��
    if ((qwtp->axisEnabled(pre) ||
        !qwtp->axisEnabled(alt)) &&
        (smp.s1() != smp.s2() ||
         sma.s1() == sma.s2()))
    {
        // ��������� ���������������� ����� ��������,
        *master = (QwtPlot::Axis)pre;
        // � �������������� ��������������
        *slave = (QwtPlot::Axis)alt;
    }
    else    // �����
            // (���������������� ����� ���������� �
            // �������������� ����� �������� ���
            // ������� ���������������� ����� ��������� �
            // ������� �������������� ����� �� ���������)
    {
        // ��������� �������������� ����� ��������,
        *master = (QwtPlot::Axis)alt;
        // � ���������������� ��������������
        *slave = (QwtPlot::Axis)pre;
    }
}

// ������� ����� ���������������
QwtChartZoom::QConvType QwtChartZoom::regim() {
    return convType;
}

// ������������ ������ ���������������
void QwtChartZoom::setRegim(QwtChartZoom::QConvType ct) {
    convType = ct;
}

// ��������� �� ��������� ��������� QwtPlot
QwtPlot *QwtChartZoom::plot() {
    return qwtp;
}

// �������� �������������� �����
QwtPlot::Axis QwtChartZoom::masterH() {
    return masterX;
}

// �������������� �������������� �����
QwtPlot::Axis QwtChartZoom::slaveH() {
    return slaveX;
}

// �������� ������������ �����
QwtPlot::Axis QwtChartZoom::masterV() {
    return masterY;
}

// �������������� ������������ �����
QwtPlot::Axis QwtChartZoom::slaveV() {
    return slaveY;
}

// ��������� ����� �����, �������� ����� ������ �������
void QwtChartZoom::setRubberBandColor(QColor clr) {
    mnzmsvc->setRubberBandColor(clr);
}

// ���������/���������� ������� ������
void QwtChartZoom::setLightMode(bool lm)
{
    light = lm; // ���������� ��������
    // � ������������� ��� ��� ���������� QDragZoomSvc
    drzmsvc->setLightMode(lm);
}

// ���������/���������� ��������� ������������ ������� �������
// (����� ������, ���� ������� ������ �����)
void QwtChartZoom::indicateDragBand(QDragIndiStyle indi) {
    drzmsvc->setIndicatorStyle(indi);
}

// ��������� ����� ������� ���������� �����������
void QwtChartZoom::setDragBandColor(QColor clr) {
    drzmsvc->setDragBandColor(clr);
}

// �������� ������� ������ ������� � �������� ��������
void QwtChartZoom::fixBoundaries() {
    // ����� ������ ������������ ���� � ��� �����
    // ����������� �� ������������� ����������� �������
    isbF = false;
    // ����������� �������� ������ ���������� � ������ ������
    // ������-���� �������������� ��� ������ fixBounds()
}

// ���������/���������� ������������� ��������������
// �������������� ����� �������
void QwtChartZoom::setHorSync(bool hs)
{
    // ���� ��������� ������������� ����������, ��
    // ������������� ������
    if (setHSync(hs)) qwtp->replot();
}

// ���������/���������� ������������� ��������������
// ������������ ����� �������
void QwtChartZoom::setVerSync(bool vs)
{
    // ���� ��������� ������������� ����������, ��
    // ������������� ������
    if (setVSync(vs)) qwtp->replot();
}

// ���������/���������� ������������� ��������������
// �������������� � ������������ ����� �������
void QwtChartZoom::setSync(bool s)
{
    // ��������/��������� ������������� �������������� �����
    bool repF = setHSync(s);
    // � ������������
    repF |= setVSync(s);
    // ���� ���������� ��������� ������������� �����-���� �����,
    // �� ������������� ������
    if (repF) qwtp->replot();
}

// ���������������� ���������/���������� �������������
// �������������� �������������� ����� �������
bool QwtChartZoom::setHSync(bool hs)
{
    // �������, ���� ��������� ��������� �������������
    // �������������� �������������� ����� ��� �����������
    if (isb_x->sync == hs) return false;
    // ���������� ����� ��������� �������������
    isb_x->sync = hs;
    // ����������������� ������� �������������� �����
    isb_x->dup();
    // ���������� �������, ��� ���������
    return true;    // ������������� ����������
}

// ���������������� ���������/���������� �������������
// �������������� ������������ ����� �������
bool QwtChartZoom::setVSync(bool vs)
{
    // �������, ���� ��������� ��������� �������������
    // �������������� ������������ ����� ��� �����������
    if (isb_y->sync == vs) return false;
    // ���������� ����� ��������� �������������
    isb_y->sync = vs;
    // ����������������� ������� �������������� �����
    isb_y->dup();
    // ���������� �������, ��� ���������
    return true;    // ������������� ����������
}

// �������� ������ ������ ����� �������������� �����
QList<int> *QwtChartZoom::getLabelWidths(int xAx)
{
    // �������� �����, �������������� �� �������������� �����
    QFont fnt = qwtp->axisFont(xAx);

    // �������� ������ �������� ����� �������������� �����
#if QWT_VERSION < 0x060000
    QwtValueList vl = qwtp->axisScaleDiv(xAx)->ticks(QwtScaleDiv::MajorTick);
#else
    QList<double> vl = qwtp->axisScaleDiv(xAx)->ticks(QwtScaleDiv::MajorTick);
#endif

    // ������� ������ ������ �����
    QList<int> *res = new QList<int>();
    // ���������� ��� �����
    for (int k=0; k < vl.count(); k++)
        // � ��������� ������ ������ �����
        res->append(qwtp->axisScaleDraw(xAx)->labelSize(fnt,vl.at(k)).width());
    if (res->count() > 2)   // ���� � ������ ������ 2-� �����
    {
        // ��������������
        int mn = res->at(0);    // ����������� ������
        int mx = mn;            // � ������������
        // ���������� ���������� �����
        for (int k=1; k < res->count(); k++)
        {
            // ����� ������ �����
            int wk = res->at(k);
            // ��������� ��������
            if (wk < mn) mn = wk;   // ����������� ������
            if (wk > mx) mx = wk;   // � ������������
        }
        // ������� � ������ ����������� ������
        int i = res->indexOf(mn);
        // � ���� �����, �� ������� �� �� ������
        if (i >= 0) res->removeAt(i);
        // ������� � ������ ������������ ������
        i = res->indexOf(mx);
        // � ���� �����, �� ������� �� �� ������
        if (i >= 0) res->removeAt(i);
    }
    // ���������� ������ ������ �����
    return res;
}

// ����������� ������� ������ ����� �������������� �����
int QwtChartZoom::meanLabelWidth(int xAx)
{
    // �������������� ������� ������ ������� �� ���� �����
    int res = 0;
    // �������� ������ ������ �����
    QList<int> *lbwds = getLabelWidths(xAx);
    // ��������� ������ �������� ���� �����
    for (int k=0; k < lbwds->count(); k++) res += lbwds->at(k);
    // � ���������� ������� ������
    if (lbwds->count() > 0) res = floor(res / lbwds->count());
    // ������� ������ ������ �����
    delete lbwds;
    // ���������� ������� ������ ������� �� ���� �����
    return res;
}

// ���������� �������������� ����� �������
bool QwtChartZoom::updateHorAxis(int xAx,int *hDiv)
{
    // ���������� ������� ������ ������� �� ���� �����
    int mwd = meanLabelWidth(xAx);
    // ���������� �������� ������ �����
    int mnw = qwtp->canvas()->size().width();
    const int dw = 48;  // ����������� ���������� ����� �������
    // ����������� ���������� ����� �������� ����� �� ����� �����
    // ��������� ������ ����� � ������� ������ ������� �� ����
    // ����� (� ��������� �������)
    int dv = floor(mnw/(mwd+dw));
    bool setF = false;  // ���������� ����� ���� ���
    // ���� ����������� ���������� ����� �������� �����
    // �� ����� ���������� � ������� �������, ��
    if (dv > *hDiv)
    {
        // ���� ����� ���������� ��������� ��������, ���
        // ������� ������ ����� ����������� �� 1 � ��� ���� �������
        // �� ���������� �� ����� � ��������� ����������, ��
        // �������������� ����������� ���������� ����� �������� �����
        if (dv*(mwd+dw+1) > mnw) dv--;
        // ���� ���������� ����� ���-���� �����������, ��
        // ������������� ���� - ��������� ���������� �����
        if (dv > *hDiv) setF = true;
    }
    // ���� ����������� ���������� ����� �������� �����
    // �� ����� ���������� � ������� �������, ��
    if (dv < *hDiv)
    {
        // ���� ����� ���������� ��������� ��������, ���
        // ������� ������ ����� ����������� �� 1 � ��� ���� �������
        // � ��������� ���������� �������� ������� ���� ����� �� �����, ��
        // �������������� ����������� ���������� ����� �������� �����
        if (dv*(mwd+dw-1) < mnw) dv++;
        // ���� ���������� ����� ���-���� �����������, ��
        // ������������� ���� - ��������� ���������� �����
        if (dv < *hDiv) setF = true;
    }
    if (setF)   // ���� ��������� ���������� �����, ��
    {
        // ������������� ����������� ����������� ���������� �����
        // �������� ����� ��� �������������� �����
        qwtp->setAxisMaxMajor(xAx,dv);
        *hDiv = dv;   // ���������� ���
    }
    // ���������� ���� ����������
    return setF;
}

// ���������� ������������ ����� �������
bool QwtChartZoom::updateVerAxis(int yAx,int *vDiv)
{
    // �������� �����, �������������� �� ������������ �����
    QFont fnt = qwtp->axisFont(yAx);
    // ������ �������� ������� ������� ������������ �����
    double mxl = qwtp->axisScaleDiv(yAx)->upperBound();

    // ���������� ������ �������, ��������������� ����� �������� ��� �������� ������
#if QWT_VERSION < 0x060000
    QSize szlb = qwtp->axisScaleDraw(yAx)->labelSize(fnt,mxl);
#else
    QSizeF szlb = qwtp->axisScaleDraw(yAx)->labelSize(fnt,mxl);
#endif

    // ����������� ���������� ����� �������� ����� �� ����� �����
    // ��������� ������ ����� � ������ ������� (� ��������� �������)
    int dv = floor(qwtp->canvas()->size().height()/(szlb.height()+8));
    // ���� ����������� �������� �� ��������� � �������������, ��
    if (dv != *vDiv)
    {
        // ������������� ����������� ����������� ���������� �����
        // �������� ����� ��� ������������ �����
        qwtp->setAxisMaxMajor(yAx,dv);
        *vDiv = dv;     // �������� ��� � ���������� ���������
        return true;    // � ���������� ���� - ����� ����������
    }
    // ����� ���������� ���� - ����� �� �����������
    else return false;
}

// ���������� �������
void QwtChartZoom::updatePlot()
{
    // ��������� �������������� �����
    bool repF = updateHorAxis(masterX,&mstHorDiv);
    repF |= updateHorAxis(slaveX,&slvHorDiv);
    // ��������� ������������ �����
    repF |= updateVerAxis(masterY,&mstVerDiv);
    repF |= updateVerAxis(slaveY,&slvVerDiv);
// ���� �����-���� �� ���� ������������� ����������
// (�.�. ���������� ������������ ���������� ����� �� �����),
// �� ��������������� ������
    if (repF) qwtp->replot();
}

// ���������� ���� �������
bool QwtChartZoom::eventFilter(QObject *target,QEvent *event)
{
    // ���� ������� ��������� ��� �������� ����,
    if (target == mwin)
        // ���� ���� ���� ���������� �� ������, ��� ���������� ��� �������, ��
        if (event->type() == QEvent::Show ||
            event->type() == QEvent::Resize)
            updatePlot();   // ��������� ������
    // ���� ������� ��������� ��� �������, ��
    if (target == qwtp)
        // ���� ���������� ������� �������, ��
        if (event->type() == QEvent::Resize)
            updatePlot();   // ��������� ������
    // �������� ���������� ������������ ����������� �������
    return QObject::eventFilter(target,event);
}

// ����������� �������� ������� ������ �������
// � �������� �������� (���� ���� isbF �������)
void QwtChartZoom::fixBounds()
{
    // ���� ����� ��� �� ���� �������
    if (!isbF)
    {
        // ��������� �������
        isb_x->fix();   // ��������������
        isb_y->fix();   // � ������������
        // ������������� ������ �������� ������ �������
        isbF = true;
    }
}

// �������������� �������� ������ �������
void QwtChartZoom::restBounds()
{
    // ������������� ����������� ����� �������
    isb_x->rest();  // �������������� �����
    isb_y->rest();  // � ������������
    // ������������� ������
    qwtp->replot();
}

    /**************************************************/
    /*         ���������� ������ QScaleBounds         */
    /*                  ������ 1.0.1                  */
    /**************************************************/

// �����������
QwtChartZoom::QScaleBounds::
    QScaleBounds(QwtPlot *plt,QwtPlot::Axis mst,QwtPlot::Axis slv) {
    // ����������
    plot = plt;     // ��������� ������
    master = mst;   // �������� �����
    slave = slv;    // � ��������������
    fixed = false;  // ������� ��� �� �����������
    sync = false;   // ������������� �� ��������� ���
}

// �������� �������� ������ �����
void QwtChartZoom::QScaleBounds::fix()
{
    // �������� ����� �������� �����
    QwtScaleMap sm = plot->canvasMap(master);
    // � ���������� ������� ����� � ������ ������� �����
    min = sm.s1(); max = sm.s2();
    // �������� ����� �������������� �������������� �����
    sm = plot->canvasMap(slave);
    // ������ �������������� �������� ����� � �������������� � ����
    //     s = a * m + b, ���:
    // ���� ������� �������� ����� �� ���������, ��
    if (min != max)
    {
        // a = (s2 - s1) / (m2 - m1)
        ak = (sm.s2() - sm.s1()) / (max - min);
        // b = (m2*s1 - m1*s2) / (m2 - m1)
        bk = (max * sm.s1() - min * sm.s2()) / (max - min);
    }
    else    // ����� (������� �������� ����� ���������,
            // ������ � �������������� ����)
    {
        // a = 0
        ak = 0;
        // b = s1
        bk = sm.s1();
    }
    fixed = true;   // ������� �����������
}

// ��������� �������� ������ �����
void QwtChartZoom::QScaleBounds::set(double mn,double mx)
{
    // ���� ������� ��� �� �����������, ��������� ��
    if (!fixed) fix();
    // ������������� ������ � ������� ������� �����
    plot->setAxisScale(master,mn,mx);   // ��������
                                        // � ��������������
    if (sync) plot->setAxisScale(slave,mn,mx);
    else plot->setAxisScale(slave,ak*mn+bk,ak*mx+bk);
}

// �������������� �������� ������ �����
void QwtChartZoom::QScaleBounds::rest() {
    // ���� ������� ��� �����������, �� ��������������� ��������
    if (fixed) set(min,max);
}

// ������������� ������ �������������� �����
void QwtChartZoom::QScaleBounds::dup()
{
    // ���� ������� ��� �� �����������, ��������� ��
    if (!fixed) fix();
    // �������� ����� �������� �����
    QwtScaleMap sm = plot->canvasMap(master);
    // � ������������� ������� ��� ��������������
    if (sync) plot->setAxisScale(slave,sm.s1(),sm.s2());
    else plot->setAxisScale(slave,ak*sm.s1()+bk,ak*sm.s2()+bk);
}

// ����������� ������ �� ��������� ����� �� ������
bool QwtChartZoom::QScaleBounds::affected(QwtPlot::Axis ax)
{
    // ���� ����� ��������, �� ������
    if (ax == master) return true;
    // ���� ����� �������������� � �������� �������������, �� ������
    if (ax == slave && sync) return true;
    // ����� (�������������� � ������������� ���������) �� ������
    return false;
}

// ��������� ���������� ������� �� ����� � �������
void QwtChartZoom::QScaleBounds::setDiv(QwtScaleDiv *sdv)
{
    // ������������� ���������� ������� ��� �������� �����,
    plot->setAxisScaleDiv(master,*sdv);
    // � ���� ����� ����� �������������, �� � ��� ��������������
    if (sync) plot->setAxisScaleDiv(slave,*sdv);
}

/**********************************************************/
/*                                                        */
/*             ���������� ������ QMainZoomSvc             */
/*                      ������ 1.0.1                      */
/*                                                        */
/* ���������� ��������� ������ ���������,                 */
/* �. �������-��������� ������������ ���., 2012 �.,       */
/* ��� ��������� �. �. ����������, �. �����������.        */
/*                                                        */
/* ����������� ��������� ������������� � ���������������. */
/* ���������� ������ �����������.                         */
/*                                                        */
/**********************************************************/

// �����������
QMainZoomSvc::QMainZoomSvc() :
    QObject()
{
    // ������� ������, ���������� �� ����������� ���������� �������
    zwid = 0;
    // � ��������� ��� ���� (�� ��������� - ������)
    zwClr = Qt::black;
}

// ������������ ���������� � ��������� ���������������
void QMainZoomSvc::attach(QwtChartZoom *zm)
{
    // ���������� ��������� �� �������� ���������������
    zoom = zm;
    // ��������� ��� ������� ���������� ������� (������ �������)
    zm->plot()->installEventFilter(this);
}

// ��������� ����� �����, �������� ����� ������ �������
void QMainZoomSvc::setRubberBandColor(QColor clr) {
    zwClr = clr;
}

// ���������� ���� �������
bool QMainZoomSvc::eventFilter(QObject *target,QEvent *event)
{
    // ���� ������� ��������� ��� �������, ��
    if (target == zoom->plot())
        // ���� ��������� ���� �� ������� �� ����, ��
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseMove ||
            event->type() == QEvent::MouseButtonRelease)
            procMouseEvent(event);  // �������� ��������������� ����������
    // �������� ���������� ������������ ����������� �������
    return QObject::eventFilter(target,event);
}

// ���������� ������� ���������� �������
void QMainZoomSvc::showZoomWidget(QRect zr)
{
    // ������������� ��������� � ������� �������, ������������� ���������� �������
    zwid->setGeometry(zr);
    // ���������� ��� ��������
    int dw = zr.width();    // ������ �������
    int dh = zr.height();   // � ������
    // ��������� ����� ��� �������, ������������� ���������� �������
    QRegion rw(0,0,dw,dh);      // ������������ �������
    QRegion rs(1,1,dw-2,dh-2);  // ���������� �������
    // ������������� ����� ����� ��������� �� ������������ ������� ����������
    zwid->setMask(rw.subtracted(rs));
    // ������ ������, ������������ ���������� �������, �������
    zwid->setVisible(true);
    // �������������� ������
    zwid->repaint();
}

// ���������� ������� ������� �� ����
void QMainZoomSvc::procMouseEvent(QEvent *event)
{
    // ������� ��������� �� ������� �� ����
    QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
    // � ����������� �� ���� ������� �������� ��������������� ����������
    switch (event->type())
    {
        // ������ ������ ����
    case QEvent::MouseButtonPress: startZoom(mEvent); break;
        // ����������� ����
    case QEvent::MouseMove: selectZoomRect(mEvent); break;
        // �������� ������ ����
    case QEvent::MouseButtonRelease: procZoom(mEvent); break;
        // ��� ������ ������� ������ �� ������
    default: ;
    }
}

// ���������� ������� �� ������ ����
// (��������� ��������� ��������)
void QMainZoomSvc::startZoom(QMouseEvent *mEvent)
{
    // ��������� �������� ������� ������� (���� ����� ��� �� ���� �������)
    zoom->fixBounds();
    // ���� � ������ ������ ��� �� ������� �� ���� �� �������
    if (zoom->regim() == QwtChartZoom::ctNone)
    {
        // �������� ��������� ��
        QwtPlot *plt = zoom->plot();        // ������
        QwtPlotCanvas *cv = plt->canvas();  // � �����
        // �������� ��������� ����� �������
        QRect cg = cv->geometry();
        // ���������� ������� ��������� ������� (������������ ����� �������)
        scp_x = mEvent->pos().x() - cg.x();
        scp_y = mEvent->pos().y() - cg.y();
        // ���� ������ ��������� ��� ������ �������
        if (scp_x >= 0 && scp_x < cg.width() &&
            scp_y >= 0 && scp_y < cg.height())
            // ���� ������ ����� ������ ����, ��
            if (mEvent->button() == Qt::LeftButton)
            {
                // ����������� ��������������� ������� ������
                zoom->setRegim(QwtChartZoom::ctZoom);
                // ���������� ������� ������
                tCursor = cv->cursor();
                // ������������� ������ Cross
                cv->setCursor(Qt::CrossCursor);
                // ������� ������, ������� ����� ���������� ���������� �������
                // (�� ����� ��������������� �� ��� �� �������, ��� � ������)
                zwid = new QWidget(plt->parentWidget());
                // � ��������� ��� ����
                zwid->setStyleSheet(QString(
                    "background-color:rgb(%1,%2,%3);").arg(
                    zwClr.red()).arg(zwClr.green()).arg(zwClr.blue()));
            }
    }
}

// ���������� ����������� ����
// (��������� ����� ������ �������)
void QMainZoomSvc::selectZoomRect(QMouseEvent *mEvent) {
    // ���� ������� ����� ��������� ��������, ��
    if (zoom->regim() == QwtChartZoom::ctZoom)
    {
        // �������� ��������� �� ������
        QwtPlot *plt = zoom->plot();
        // �������� ��������� �������
        QRect pg = plt->geometry();
        // � ��������� ����� �������
        QRect cg = plt->canvas()->geometry();
        // scp_x - ���������� ������� � �������� �� �������������� ���
        //     � ��������� ������ ������� (����� ���� ������ ����� ������ ����)
        // mEvent->pos().x() - cg.x() - ���������� ������� � ��������
        //     �� �������������� ��� � ������� ������ �������
        // mEvent->pos().x() - cg.x() - scp_x - �������� ������� � ��������
        //     �� �������������� ��� �� ���������� ��������� � ��������������
        //     ������ dx ���������� �������
        int dx = mEvent->pos().x() - cg.x() - scp_x;
        // pg.x() - ��������� ������� �� �������������� ���
        //     ������������ �������, ��� �����������
        // pg.x() + cg.x() - ��������� ����� ������� �� �������������� ���
        //     ������������ �������, ��� �����������
        // pg.x() + cg.x() + scp_x - ��������� gx0 ��������� ����� �� �������������� ���
        //     ������������ �������, ����������� ������, ��� ����� � �������� �����
        //     ��� ����������� ���������� �������
        int gx0 = pg.x() + cg.x() + scp_x;
        // ���� ������ ���������� ������� ������������, �� ������� ����� ��������� ����� ���������,
        //     � ����� ������ �� �� ���������� � �������� ����� ��� ����������� ���������� �������
        if (dx < 0) {dx = -dx; gx0 -= dx;}
        // ����� ���� ������ ����� ����, �� ��� ���� ����� ���������� ������� ���-���� �����������,
        //     ������������� ������� �� ������ �������
        else if (dx == 0) dx = 1;
        // ���������� ���������� ������ dy ���������� �������
        int dy = mEvent->pos().y() - cg.y() - scp_y;
        // � ��������� gy0 ��������� ����� �� ������������ ���
        int gy0 = pg.y() + cg.y() + scp_y;
        // ���� ������ ���������� ������� ������������, �� ������� ����� ��������� ���� ���������,
        //     � ����� ������ �� �� ���������� � �������� ����� ��� ����������� ���������� �������
        if (dy < 0) {dy = -dy; gy0 -= dy;}
        // ����� ���� ������ ����� ����, �� ��� ���� ����� ���������� ������� ���-���� �����������,
        //     ������������� ������� �� ������ �������
        else if (dy == 0) dy = 1;
        // ���������� ���������� �������
        showZoomWidget(QRect(gx0,gy0,dx,dy));
    }
}

// ���������� ���������� ������ ����
// (���������� ��������� ��������)
void QMainZoomSvc::procZoom(QMouseEvent *mEvent)
{
    // ���� ������� ����� ��������� �������� ��� ����� ����������� �������
    if (zoom->regim() == QwtChartZoom::ctZoom)
        // ���� �������� ����� ������ ����, ��
        if (mEvent->button() == Qt::LeftButton)
        {
            // �������� ��������� ��
            QwtPlot *plt = zoom->plot();        // ������
            QwtPlotCanvas *cv = plt->canvas();  // � �����
            // ��������������� ������
            cv->setCursor(tCursor);
            // ������� ������, ������������ ���������� �������
            delete zwid;
            // �������� ��������� ����� �������
            QRect cg = cv->geometry();
            // ���������� ��������� �������, �.�. ���������� xp � yp
            // �������� ����� ���������� ������� (� �������� ������������ ����� QwtPlot)
            int xp = mEvent->pos().x() - cg.x();
            int yp = mEvent->pos().y() - cg.y();
            // ���� ��������� ������������� ������ ������ ��� ����� �����,
            // �� ��������������� �������� ������� ������� (�������� ����������)
            if (xp < scp_x || yp < scp_y) zoom->restBounds();
            // ����� ���� ������ ���������� ������� ����������, �� �������� �������
            else if (xp - scp_x >= 8 && yp - scp_y >= 8)
            {
                QwtPlot::Axis mX = zoom->masterH();
                // ���������� ����� ������� �������������� ����� �� ��������� �����
                double lf = plt->invTransform(mX,scp_x);
                // ���������� ������ ������� �������������� ����� �� �������� �����
                double rg = plt->invTransform(mX,xp);
                // ������������� ������ � ������� ������� ������������ �����
                zoom->isb_x->set(lf,rg);
                // �������� �������� ������������ �����
                QwtPlot::Axis mY = zoom->masterV();
                // ���������� ������ ������� ������������ ����� �� �������� �����
                double bt = plt->invTransform(mY,yp);
                // ���������� ������� ������� ������������ ����� �� ��������� �����
                double tp = plt->invTransform(mY,scp_y);
                // ������������� ������ � ������� ������� ������������ �����
                zoom->isb_y->set(bt,tp);
                // ������������� ������ (��������� � ����������)
                plt->replot();
            }
            // ������� ������� ������
            zoom->setRegim(QwtChartZoom::ctNone);
        }
}

/**********************************************************/
/*                                                        */
/*             ���������� ������ QDragZoomSvc             */
/*                      ������ 1.0.1                      */
/*                                                        */
/* ���������� ��������� ������ ���������,                 */
/* �. �������-��������� ������������ ���., 2012 �.,       */
/* ��� ��������� �. �. ����������, �. �����������.        */
/*                                                        */
/* ����������� ��������� ������������� � ���������������. */
/* ���������� ������ �����������.                         */
/*                                                        */
/**********************************************************/

// �����������
QDragZoomSvc::QDragZoomSvc() :
    QObject()
{
    // �� ��������� ������ ����� ��������
    light = false;
    // ������� ������, ���������� �� ����������� ���������� �����������
    zwid = 0;
    // � ��������� ��� ���� (�� ��������� ������)
    dwClr = Qt::black;
    // �� ��������� ������� ������� ����� ��������� ����������� �������
    // (�� ������ � ��� ������, ���� ������� ������ �����)
    indiDrB = QwtChartZoom::disSimple;
}

// ������������ ���������� � ��������� ���������������
void QDragZoomSvc::attach(QwtChartZoom *zm)
{
    // ���������� ��������� �� �������� ���������������
    zoom = zm;
    // ��������� ��� ������� ���������� ������� (������ �������)
    zm->plot()->installEventFilter(this);
}

// ���������/���������� ������� ������
void QDragZoomSvc::setLightMode(bool lm) {
    light = lm;
}

// ��������� ����� ������� ���������� �����������
void QDragZoomSvc::setDragBandColor(QColor clr) {
    dwClr = clr;
}

// ���������/���������� ��������� ������������ �������
// (����� ������, ���� ������� ������ �����)
void QDragZoomSvc::setIndicatorStyle(QwtChartZoom::QDragIndiStyle indi) {
    indiDrB = indi;
}

// ���������� ���� �������
bool QDragZoomSvc::eventFilter(QObject *target,QEvent *event)
{
    // ���� ������� ��������� ��� �������, ��
    if (target == zoom->plot())
        // ���� ��������� ���� �� ������� �� ����, ��
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseMove ||
            event->type() == QEvent::MouseButtonRelease)
            dragMouseEvent(event);  // �������� ��������������� ����������
    // �������� ���������� ������������ ����������� �������
    return QObject::eventFilter(target,event);
}

// ���������� � ����� ���������� ������������ ����� ����� ��� ����� �������������� �����
QRegion QDragZoomSvc::addHorTicks(QRegion rw,QwtScaleDiv::TickType tt)
{
    // �������� ��������� �� ������
    QwtPlot *plt = zoom->plot();
    // �������� ������ �������� ����� �������������� �����
#if QWT_VERSION < 0x060000
    QwtValueList vl = plt->axisScaleDiv(zoom->masterH())->ticks(tt);
#else
    QList<double> vl = plt->axisScaleDiv(zoom->masterH())->ticks(tt);
#endif

    // ���������� ��� ����� �������������� �����
    for (int k=0; k < vl.count(); k++)
    {
        // ��������� �������� ����� ������������ �����
        int x = plt->transform(zoom->masterH(),vl.at(k));
        // ��������� ������������ ����� �����
        QRegion rs(x-1,1,1,rw.boundingRect().height()-2);
        // ��������� �� � �����
        rw = rw.united(rs);
    }
    // ���������� ���������� �����
    return rw;
}

// ���������� � ����� ���������� �������������� ����� ����� ��� ����� ������������ �����
QRegion QDragZoomSvc::addVerTicks(QRegion rw,QwtScaleDiv::TickType tt)
{
    // �������� ��������� �� ������
    QwtPlot *plt = zoom->plot();
    // �������� ������ �������� ����� ������������ �����
#if QWT_VERSION < 0x060000
    QwtValueList vl = plt->axisScaleDiv(zoom->masterV())->ticks(tt);
#else
    QList<double> vl = plt->axisScaleDiv(zoom->masterV())->ticks(tt);
#endif

    // ���������� ��� ����� ������������ �����
    for (int k=0; k < vl.count(); k++)
    {
        // ��������� �������� ����� ������������ �����
        int y = plt->transform(zoom->masterV(),vl.at(k));
        // ��������� �������������� ����� �����
        QRegion rs(1,y-1,rw.boundingRect().width()-2,1);
        // ��������� �� � �����
        rw = rw.united(rs);
    }
    // ���������� ���������� �����
    return rw;
}

// ���������� ����������� ���������� �����������
void QDragZoomSvc::showDragWidget(QPoint evpos)
{
    // �������� ��������� �� ������
    QwtPlot *plt = zoom->plot();
    // �������� ��������� �������
    QRect pg = plt->geometry();             // �������
    QRect cg = plt->canvas()->geometry();   // � ����� �������
    // ���������� ��� ��������
    int ww = cg.width() - 2;    // ������ �����
    int wh = cg.height() - 2;   // � ������
    // ��������� ��������� � ������ ����� �����
    QRect wg(pg.x()+1+evpos.x()-scp_x,pg.y()+1+evpos.y()-scp_y,ww,wh);
    // ������������� ��������� � ������� ������� ����������
    zwid->setGeometry(wg);
    // ��������� ������������ ����� ��� ������� ����������
    QRegion rw(0,0,ww,wh);      // ������������ �������
    QRegion rs(1,1,ww-2,wh-2);  // ���������� �������
    // ��������� ����� ����� ��������� �� ������������ ������� ����������
    rw = rw.subtracted(rs);
    // ���� ������� ��������� ����� ���������, ��
    if (indiDrB == QwtChartZoom::disDetailed)
    {
        // ��������� � ����� ������������ ����� ����� ��� ������� �������������� �����
        rw = addHorTicks(rw,QwtScaleDiv::MajorTick);    // ��������
        rw = addHorTicks(rw,QwtScaleDiv::MediumTick);   // �������
        rw = addHorTicks(rw,QwtScaleDiv::MinorTick);    // �����������
        // ��������� � ����� �������������� ����� ����� ��� �������� �������
        rw = addVerTicks(rw,QwtScaleDiv::MajorTick);    // ������������ �����
    }
    // ������������� �����
    zwid->setMask(rw);
    // ������ ������, ������������ ����������� �������, �������
    zwid->setVisible(true);
    // �������������� ������
    zwid->repaint();
}

// ���������� ����������� ����������� �������
void QDragZoomSvc::applyDrag(QPoint evpos)
{
    // �������� ��������� �� ������
    QwtPlot *plt = zoom->plot();
    // �������� ��������� ����� �������
    QRect cg = plt->canvas()->geometry();
    // scp_x - ���������� ������� � �������� �� �������������� ���
    //     � ��������� ������ ������� (����� ���� ������ ������ ������ ����)
    // evpos.x() - cg.x() - ���������� �������
    //     � �������� �� �������������� ��� � ������� ������ �������
    // evpos.x() - cg.x() - scp_x - �������� ������� � ��������
    //     �� �������������� ��� �� ���������� ���������
    // (evpos.x() - cg.x() - scp_x) * cs_kx -  ��� �� ��������,
    //     �� ��� � �������� �������������� �����
    // dx - �������� ������ �� �������������� ��� ������� � �������� ������
    //     (����� ������ ������������ ������ ������������ ������, ���� ������� ������� �������� �����)
    double dx = -(evpos.x() - cg.x() - scp_x) * cs_kx;
    // ������������� ����� ����� � ������ ������� ����� ��� �������������� ���
    //     ����� ������� = ��������� ������� + ��������
    zoom->isb_x->set(scb_xl + dx,scb_xr + dx);
    // ���������� ���������� dy - �������� ������ �� ������������ ���
    double dy = -(evpos.y() - cg.y() - scp_y) * cs_ky;
    // ������������� ����� ������ � ������� ������� ������������ �����
    zoom->isb_y->set(scb_yb + dy,scb_yt + dy);
    // ������������� ������ (��������� � ����������)
    plt->replot();
}

// ���������� ������� �� ����
void QDragZoomSvc::dragMouseEvent(QEvent *event)
{
    // ������� ��������� �� ������� �� ����
    QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
    // � ����������� �� ���� ������� �������� ��������������� ����������
    switch (event->type())
    {
        // ������ ������ ����
    case QEvent::MouseButtonPress: startDrag(mEvent); break;
        // ����������� ����
    case QEvent::MouseMove: procDrag(mEvent); break;
        // �������� ������ ����
    case QEvent::MouseButtonRelease: endDrag(mEvent); break;
        // ��� ������ ������� ������ �� ������
    default: ;
    }
}

// ���������� ������� �� ������ ����
// (��������� ����������� �������)
void QDragZoomSvc::startDrag(QMouseEvent *mEvent)
{
    // ��������� �������� ������� ������� (���� ����� ��� �� ���� �������)
    zoom->fixBounds();
    // ���� � ������ ������ ��� �� ������� �� ���� �� �������
    if (zoom->regim() == QwtChartZoom::ctNone)
    {
        // �������� ��������� ��
        QwtPlot *plt = zoom->plot();        // ������
        QwtPlotCanvas *cv = plt->canvas();  // � �����
        // �������� ��������� ����� �������
        QRect cg = cv->geometry();
        // ���������� ������� ��������� ������� (������������ ����� �������)
        scp_x = mEvent->pos().x() - cg.x();
        scp_y = mEvent->pos().y() - cg.y();
        // ���� ������ ��������� ��� ������ �������
        if (scp_x >= 0 && scp_x < cg.width() &&
            scp_y >= 0 && scp_y < cg.height())
            // ���� ������ ������ ������ ����, ��
            if (mEvent->button() == Qt::RightButton)
            {
                // ����������� ��������������� ������� ������
                zoom->setRegim(QwtChartZoom::ctDrag);
                // ���������� ������� ������
                tCursor = cv->cursor();
                // ������������� ������ OpenHand
                cv->setCursor(Qt::OpenHandCursor);
                // ���������� ������� �������������� ��������� �� �������������� ���
                // (�.�. ������ �� ������� ���������� ���������� �� ����� x
                // ��� ����������� ������� ������ �� ���� ������)
                cs_kx = plt->invTransform(zoom->masterH(),scp_x + 1) -
                    plt->invTransform(zoom->masterH(),scp_x);
                // �������� �������� ������������ �����
                QwtPlot::Axis mY = zoom->masterV();
                // ���������� ������� �������������� ��������� �� ������������ ���
                // (����������)
                cs_ky = plt->invTransform(mY,scp_y + 1) -
                    plt->invTransform(mY,scp_y);
                // �������� ����� �������� �������������� �����
                QwtScaleMap sm = plt->canvasMap(zoom->masterH());
                // ��� ���� ����� ����������� ��������� ����� � ������ �������
                scb_xl = sm.s1(); scb_xr = sm.s2();
                // ���������� �������� ����� �������� ������������ �����
                sm = plt->canvasMap(mY);
                // ��� ���� ����� ����������� ��������� ������ � ������� �������
                scb_yb = sm.s1(); scb_yt = sm.s2();
                // ���� ������ ����� � �������� ���������, ��
                if (light and indiDrB != QwtChartZoom::disNone)
                {
                    // ������� ������, ������������ ����������� �������
                    zwid = new QWidget(plt->parentWidget());
                    // ��������� ��� ����
                    zwid->setStyleSheet(QString(
                        "background-color:rgb(%1,%2,%3);").arg(
                        dwClr.red()).arg(dwClr.green()).arg(dwClr.blue()));
                    // ������������� ����������� ���������� �����������
                    showDragWidget(mEvent->pos());
                }
            }
    }
}

// ���������� ����������� ����
// (���������� ����������� ��� ����� ������ ��������� �������)
void QDragZoomSvc::procDrag(QMouseEvent *mEvent)
{
    // ���� ������� ����� ����������� �������, ��
    if (zoom->regim() == QwtChartZoom::ctDrag)
    {
        // ������������� ������ ClosedHand
        zoom->plot()->canvas()->setCursor(Qt::ClosedHandCursor);
        if (light)  // ���� ������ �����, ��
        {
            // ���� �������� ���������, ��
            if (indiDrB != QwtChartZoom::disNone)
                // ������������� ����������� ���������� �����������
                showDragWidget(mEvent->pos());
        }
        // ����� ��������� ���������� ����������� �������
        else applyDrag(mEvent->pos());
    }
}

// ���������� ���������� ������ ����
// (���������� ����������� �������)
void QDragZoomSvc::endDrag(QMouseEvent *mEvent)
{
    // ���� ������� ����� ��������� �������� ��� ����� ����������� �������
    if (zoom->regim() == QwtChartZoom::ctDrag)
        // ���� �������� ������ ������ ����, ��
        if (mEvent->button() == Qt::RightButton)
        {
            if (light)  // ���� ������ �����, ��
            {
                // ���� �������� ���������, �� ������� ������ ����������
                if (indiDrB != QwtChartZoom::disNone) delete zwid;
                // ��������� ���������� ����������� �������
                applyDrag(mEvent->pos());
            }
            // ��������������� ������
            zoom->plot()->canvas()->setCursor(tCursor);
            zoom->setRegim(QwtChartZoom::ctNone);  // � ������� ������� ������
        }
}
