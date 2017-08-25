/**********************************************************/
/*                                                        */
/*             ���������� ������ QAxisZoomSvc             */
/*                      ������ 1.2.2                      */
/*                                                        */
/* ���������� ��������� ������ ���������,                 */
/* �. �������-��������� ������������ ���., 2012 �.,       */
/* ��� ��������� �. �. ����������, �. �����������.        */
/*                                                        */
/* ����������� ��������� ������������� � ���������������. */
/* ���������� ������ �����������.                         */
/*                                                        */
/**********************************************************/

#include "qaxiszoomsvc.h"

#include <qwt_scale_widget.h>

// �����������
QAxisZoomSvc::QAxisZoomSvc() :
    QObject()
{
    // �� ��������� ������ ����� ��������
    light = false;
    // ������� ������, ���������� �� ����������� ����������
    zwid = 0;
    // � ��������� ���� �������, ������������� ��������������� �����
    awClr = Qt::black;
    // �� ��������� ��������������� ����� ������������
    // (�� ������ � ��� ������, ���� ������� ������ �����)
    indiAxZ = true;
}

// ������������ ���������� � ��������� ���������������
void QAxisZoomSvc::attach(QwtChartZoom *zm)
{
    // ���������� ��������� �� �������� ���������������
    zoom = zm;
    // ��� ���� ���� �������, �� ������� ��������� ��������,
    for (int ax=0; ax < QwtPlot::axisCnt; ax++)
        // ��������� ���������� ������� (������ �������)
        zoom->plot()->axisWidget(ax)->installEventFilter(this);
}

// ���������/���������� ������� ������
void QAxisZoomSvc::setLightMode(bool lm) {
    light = lm;
}

// ��������� ����� �������, ������������� ��������������� �����
void QAxisZoomSvc::setAxisRubberBandColor(QColor clr) {
    awClr = clr;
}

// ���������/���������� ��������� ��������������� �����
// (����� ������, ���� ������� ������ �����)
void QAxisZoomSvc::indicateAxisZoom(bool indi) {
    indiAxZ = indi;
}

// ���������� ���� �������
bool QAxisZoomSvc::eventFilter(QObject *target,QEvent *event)
{
    int ax = -1;    // ����� ���� �� �������
    // ������������� ������ ����
    for (int a=0; a < QwtPlot::axisCnt; a++)
        // ���� ������� ��������� ��� ������ �����, ��
        if (target == zoom->plot()->axisWidget(a))
        {
            ax = a;     // ���������� ����� �����
            break;      // ���������� �����
        }
    // ���� ����� ���� �������, ��
    if (ax >= 0)
        // ���� ��������� ���� �� ������� �� ����, ��
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseMove ||
            event->type() == QEvent::MouseButtonRelease)
            axisMouseEvent(event,ax);   // �������� ��������������� ����������
    // �������� ���������� ������������ ����������� �������
    return QObject::eventFilter(target,event);
}

// ����������� ������� ����������
int QAxisZoomSvc::limitSize(int sz,int bs)
{
    // �������
    int mn = floor(16*bs/31);
    // ����������� ������������ �������
    if (sz < mn) sz = mn;
    // ��������
    int mx = floor(31*bs/16);
    // ����������� ������������ �������
    if (sz > mx) sz = mx;
    return sz;
}

// ��������� ��������� ��������� ��������������� �����
QRect *QAxisZoomSvc::axisZoomRect(QPoint evpos,int ax)
{
    // �������� ��������� �� ������
    QwtPlot *plt = zoom->plot();
    // ���������� (��� ��������) ���������
    QRect gc = plt->canvas()->geometry();       // ����� �������
    QRect gw = plt->axisWidget(ax)->geometry(); // � ������� �����
    // ���������� ������� ��������� ������� ������������ ����� �������
    int x = evpos.x() + gw.x() - gc.x() - scb_pxl;
    int y = evpos.y() + gw.y() - gc.y() - scb_pyt;
    // ���������� (��� ��������)
    int wax = gw.width();   // ������ ������� �����
    int hax = gw.height();  // � ������
    // ������ ����� ���������������
    QwtChartZoom::QConvType ct = zoom->regim();
    // ��������� ��������� ������ �������� ����,
    int wl,wt,ww,wh;    // ������ � ������
    // ���� �������������� �������������� �����, ��
    if (ax == QwtPlot::xBottom ||
        ax == QwtPlot::xTop)
    {
        // ���� ���������� ������ �������, ��
        if (ct == QwtChartZoom::ctAxisHR)
        {
            // ����������� �� ��������� ������� �����
            int mn = floor(scb_pw/16);
            // ���� ������ ������� ������ � ����� �������, ��
            if (x < mn) x = mn;
            // ������ ��������������
            ww = floor(x * scb_pw / scp_x);
            // ��������� �����������
            ww = limitSize(ww,scb_pw);
            // ����� ������ ��������������
            wl = sab_pxl;
        }
        else    // ����� (���������� ����� �������)
        {
            // ����������� �� ��������� ������� ������
            int mx = floor(15*scb_pw/16);
            // ���� ������ ������� ������ � ������ �������, ��
            if (x > mx) x = mx;
            // ������ ��������������
            ww = floor((scb_pw - x) * scb_pw / (scb_pw - scp_x));
            // ��������� �����������
            ww = limitSize(ww,scb_pw);
            // ����� ������ ��������������
            wl = sab_pxl + scb_pw - ww;
        }
        // ������ ��������������
        wh = 4;
        // ������� ������ ��������������
        wt = 10;    // ��� ������ �����
        // ���� �� ���������� �� �����, ������������
        if (wt + wh > hax) wt = hax - wh;
        // ��� ������� ����� �����������
        if (ax == QwtPlot::xTop) wt = hax - wt - wh;
    }
    else    // ����� (�������������� ������������ �����)
    {
        // ���� ���������� ������ �������, ��
        if (ct == QwtChartZoom::ctAxisVB)
        {
            // ����������� �� ��������� ������� ������
            int mn = floor(scb_ph/16);
            // ���� ������ ������� ������ � ������� �������, ��
            if (y < mn) y = mn;
            // ������ ��������������
            wh = floor(y * scb_ph / scp_y);
            // ��������� �����������
            wh = limitSize(wh,scb_ph);
            // ������� ������ ��������������
            wt = sab_pyt;
        }
        else    // ����� (���������� ������� �������)
        {
            // ����������� �� ��������� ������� �����
            int mx = floor(15*scb_ph/16);
            // ���� ������ ������� ������ � ������ �������, ��
            if (y > mx) y = mx;
            // ������ ��������������
            wh = floor((scb_ph - y) * scb_ph / (scb_ph - scp_y));
            // ��������� �����������
            wh = limitSize(wh,scb_ph);
            // ������� ������ �������������� = �������� �������
            wt = sab_pyt + scb_ph - wh;
        }
        // ������ ��������������
        ww = 4;
        // ������� ������ ��������������
        wl = 10;    // ��� ������ �����
        // ���� �� ���������� �� �����, ������������
        if (wl + ww > wax) wl = wax - ww;
        // ��� ����� ����� �����������
        if (ax == QwtPlot::yLeft) wl = wax - wl - ww;
    }
    // ������� � ���������� ��������� �������
    // � ������������ ���������
    return new QRect(wl,wt,ww,wh);
}

// ���������� ������� �������������� �����
void QAxisZoomSvc::showZoomWidget(QPoint evpos,int ax)
{
    // �������� ��������� ��������� ��������������� �����
    QRect *zr = axisZoomRect(evpos,ax);
    // ��� �������� ����������
    int w = zr->width();    // ������
    int hw = floor(w/2);    // ����������
    int h = zr->height();   // ������
    int hh = floor(h/2);    // ����������
    // ������������� ��������� � ������� �������, �������������
    // ��������������� �����, � ������������ � �������� ����������
    zwid->setGeometry(*zr);
    // ������� ��������� ��������� ��������������� �����
    delete zr;
    // ������ ����� ���������������
    QwtChartZoom::QConvType ct = zoom->regim();
    // ��������� ����� ��� �������, ������������� ��������������� �����
    QRegion rw = QRegion(0,0,w,h);  // ������������ �������
    QRegion rs; // ��������� ���������� �������
    // ���� �������������� �������������� �����
    if (ax == QwtPlot::xBottom || ax == QwtPlot::xTop)
    {
        // ���� ���������� ������ ������� �����, ��
        if (ct == QwtChartZoom::ctAxisHR)
            // ���������� ������� ����� �����
            rs = QRegion(1,1,hw-1,h-2);
        // ����� (���������� ����� ������� �����)
        // ���������� ������� ����� ������
        else rs = QRegion(w-hw,1,hw-1,h-2);
    }
    else    // ����� (�������������� ������������ �����)
    {
        // ���� ���������� ������ ������� �����, ��
        if (ct == QwtChartZoom::ctAxisVB)
            // ���������� ������� ����� ������
            rs = QRegion(1,1,w-2,hh-1);
        // ����� (���������� ������� ������� �����)
        // ���������� ������� ����� �����
        else rs = QRegion(1,h-hh,w-2,hh-1);
    }
    // ������������� ����� ����� ��������� �� ������������ ������� ����������
    zwid->setMask(rw.subtracted(rs));
    // ������ ������, ������������ ��������������� �����, �������
    zwid->setVisible(true);
    // �������������� ������
    zwid->repaint();
}

// ����������� ������ ������� �����
double QAxisZoomSvc::limitScale(double sz,double bs)
{
    // ��������
    double mx = 16*bs;
    if (light)  // ���� ������ �����, ��
    {
        // ������ �������
        double mn = 16*bs/31;
        // ����������� ������������ �������
        if (sz < mn) sz = mn;
        // ������ ��������
        mx = 31*bs/16;
    }
    // ����������� ������������� �������
    if (sz > mx) sz = mx;
    return sz;
}

// ���������� ����������� ����������� ������� �����
void QAxisZoomSvc::axisApplyMove(QPoint evpos,int ax)
{
    // �������� ��������� �� ������
    QwtPlot *plt = zoom->plot();
    // ���������� (��� ��������) ���������
    QRect gc = plt->canvas()->geometry();       // ����� �������
    QRect gw = plt->axisWidget(ax)->geometry(); // � ������� �����
    // ���������� ������� ��������� ������� ������������ �����
    // (�� ������� �������� �������)
    int x = evpos.x() + gw.x() - gc.x() - scb_pxl;
    int y = evpos.y() + gw.y() - gc.y() - scb_pyt;
    bool bndCh = false; // ���� ������ �� ����������
    // ������ ����� ���������������
    QwtChartZoom::QConvType ct = zoom->regim();
    // � ����������� �� ����������� ������ ��������� ��������� ��������
    switch (ct)
    {
        // ����� ��������� ����� �������
    case QwtChartZoom::ctAxisHL:
    {
        // ����������� �� ��������� ������� ������
        if (x >= scb_pw) x = scb_pw-1;
        // ��������� ����� ������ �����
        double wx = scb_wx * (scb_pw - scp_x) / (scb_pw - x);
        // ��������� �����������
        wx = limitScale(wx,scb_wx);
        // ��������� ����� ����� �������
        double xl = scb_xr - wx;
        // ������������� �� ��� �������������� �����
        zoom->isb_x->set(xl,scb_xr);
        bndCh = true;   // ���������� �������
        break;
    }
        // ����� ��������� ������ �������
    case QwtChartZoom::ctAxisHR:
    {
        // ����������� �� ��������� ������� �����
        if (x <= 0) x = 1;
        // ��������� ����� ������ �����
        double wx = scb_wx * scp_x / x;
        // ��������� �����������
        wx = limitScale(wx,scb_wx);
        // ��������� ����� ������ �������
        double xr = scb_xl + wx;
        // ������������� �� ��� �������������� �����
        zoom->isb_x->set(scb_xl,xr);
        bndCh = true;   // ���������� �������
        break;
    }
        // ����� ��������� ������ �������
    case QwtChartZoom::ctAxisVB:
    {
        // ����������� �� ��������� ������� ������
        if (y <= 0) y = 1;
        // ��������� ����� ������ �����
        double hy = scb_hy * scp_y / y;
        // ��������� �����������
        hy = limitScale(hy,scb_hy);
        // ��������� ����� ������ �������
        double yb = scb_yt - hy;
        // ������������� �� ��� ������������ �����
        zoom->isb_y->set(yb,scb_yt);
        bndCh = true;   // ���������� �������
        break;
    }
        // ����� ��������� ������� �������
    case QwtChartZoom::ctAxisVT:
    {
        // ����������� �� ��������� ������� �����
        if (y >= scb_ph) y = scb_ph-1;
        // ��������� ����� ������ �����
        double hy = scb_hy * (scb_ph - scp_y) / (scb_ph - y);
        // ��������� �����������
        hy = limitScale(hy,scb_hy);
        // ��������� ����� ������� �������
        double yt = scb_yb + hy;
        // ������������� �� ��� ������������ �����
        zoom->isb_y->set(scb_yb,yt);
        bndCh = true;   // ���������� �������
        break;
    }
        // ��� ������ ������� ������ �� ������
    default: ;
    }
    // ���� ����-���� ������� ����������, �� ������������� ������
    if (bndCh) plt->replot();
}

// ���������� ������� �� ���� ��� �����
void QAxisZoomSvc::axisMouseEvent(QEvent *event,int a)
{
    // ������� ��������� �� ������� �� ����
    QMouseEvent *mEvent = static_cast<QMouseEvent *>(event);
    // � ����������� �� ���� ������� �������� ��������������� ����������
    switch (event->type())
    {
        // ������ ������ ����
    case QEvent::MouseButtonPress: startAxisZoom(mEvent,a); break;
        // ����������� ����
    case QEvent::MouseMove: procAxisZoom(mEvent,a); break;
        // �������� ������ ����
    case QEvent::MouseButtonRelease: endAxisZoom(mEvent,a); break;
        // ��� ������ ������� ������ �� ������
    default: ;
    }
}

// ���������� ������� �� ������ ���� ��� ������
// (��������� ��������� �������� �����)
void QAxisZoomSvc::startAxisZoom(QMouseEvent *mEvent,int ax)
{
    // ��������� �������� ������� ������� (���� ����� ��� �� ���� �������)
    zoom->fixBounds();
    // ���� � ������ ������ ��� �� ������� �� ���� �� �������
    if (zoom->regim() == QwtChartZoom::ctNone)
    {
        // ���� ������ ����� ������ ����, ��
        // �������� ���� �� ������� ���������������
        if (mEvent->button() == Qt::LeftButton)
        {
            // �������� ��������� ��
            QwtPlot *plt = zoom->plot();                // ������
            QwtScaleWidget *sw = plt->axisWidget(ax);   // ������ �����
            // �������� ����� �������� �������������� �����
            QwtScaleMap sm = plt->canvasMap(zoom->masterH());
            // ��� ���� ����� ����������� ��������� ����� � ������ �������
            scb_xl = sm.s1(); scb_xr = sm.s2(); scb_wx = sm.sDist();
            // ���������� �������� ����� �������� ������������ �����
            sm = plt->canvasMap(zoom->masterV());
            // ��� ���� ����� ����������� ��������� ������ � ������� �������
            scb_yb = sm.s1(); scb_yt = sm.s2(); scb_hy = sm.sDist();
            // ���������� (��� ��������) ���������
            QRect gc = plt->canvas()->geometry();   // ����� �������
            QRect gw = sw->geometry();              // � ������� �����
            // ������� ����� �������� ������� (� �������� ������������ �����)
            scb_pxl = plt->transform(zoom->masterH(),scb_xl);
            // ������� ������ ������� (� ��������)
            scb_pw = plt->transform(zoom->masterH(),scb_xr) - scb_pxl;
            // ������� ����� �������� �������
            // (� �������� ������������ ������� �����)
            sab_pxl = scb_pxl + gc.x() - gw.x();
            // ������� ������� �������� ������� (� �������� ������������ �����)
            scb_pyt = plt->transform(zoom->masterV(),scb_yt);
            // ������� ������ ������� (� ��������)
            scb_ph = plt->transform(zoom->masterV(),scb_yb) - scb_pyt;
            // ������� ������� �������� �������
            // (� �������� ������������ ������� �����)
            sab_pyt = scb_pyt + gc.y() - gw.y();
            // ���������� ������� ��������� ������� ������������ �����
            // (�� ������� �������� �������)
            scp_x = mEvent->pos().x() - sab_pxl;
            scp_y = mEvent->pos().y() - sab_pyt;
            // ���� �������������� �������������� �����
            if (ax == QwtPlot::xBottom ||
                ax == QwtPlot::xTop)
            {
                // ���� ����� ������� ������ ������,
                if (scb_wx > 0)
                    // ���� ������ ����� ������ ��������,
                    if (scb_pw > 36)
                    {
                        // � ����������� �� ��������� �������
                        // (������ ��� ����� �������� �����)
                        // �������� ��������������� ����� - ���������
                        if (scp_x >= floor(scb_pw/2))
                            zoom->setRegim(QwtChartZoom::ctAxisHR);     // ������ �������
                        else zoom->setRegim(QwtChartZoom::ctAxisHL);    // ��� �����
                    }
            }
            else    // ����� (�������������� ������������ �����)
            {
                // ���� ������ ������� ������ �������,
                if (scb_hy > 0)
                    // ���� ������ ����� ������ ��������,
                    if (scb_ph > 18)
                    {
                        // � ����������� �� ��������� �������
                        // (���� ��� ���� �������� �����)
                        // �������� ��������������� ����� - ���������
                        if (scp_y >= floor(scb_ph/2))
                            zoom->setRegim(QwtChartZoom::ctAxisVB);     // ������ �������
                        else zoom->setRegim(QwtChartZoom::ctAxisVT);    // ��� �������
                    }
            }
            // ���� ���� �� ������� ��� �������
            if (zoom->regim() != QwtChartZoom::ctNone)
            {
                // ���������� ������� ������
                tCursor = sw->cursor();
                // ������������� ������ PointingHand
                sw->setCursor(Qt::PointingHandCursor);
                // ���� ������ ����� � �������� ���������, ��
                if (light && indiAxZ)
                {
                    // ������� ������, ������������ ��������������� �����
                    zwid = new QWidget(plt->axisWidget(ax));
                    // ��������� ��� ����
                    zwid->setStyleSheet(QString(
                        "background-color:rgb(%1,%2,%3);").arg(
                        awClr.red()).arg(awClr.green()).arg(awClr.blue()));
                    // � �������������
                    showZoomWidget(mEvent->pos(),ax);
                }
            }
        }
    }
}

// ���������� ����������� ����
// (��������� ����� ������ �����)
void QAxisZoomSvc::procAxisZoom(QMouseEvent *mEvent,int ax)
{
    // ������ ����� ���������������
    QwtChartZoom::QConvType ct = zoom->regim();
    // �������, ���� �� ������� �� ���� �� ������� ��������� �����
    if (ct != QwtChartZoom::ctAxisHL &&
        ct != QwtChartZoom::ctAxisHR &&
        ct != QwtChartZoom::ctAxisVB &&
        ct != QwtChartZoom::ctAxisVT) return;
    if (light)  // ���� ������ �����, ��
    {
        // ���� �������� ���������, �� ������������� ������ ����������
        if (indiAxZ) showZoomWidget(mEvent->pos(),ax);
    }
    // ����� ��������� ���������� ����������� ������� �����
    else axisApplyMove(mEvent->pos(),ax);
}

// ���������� ���������� ������ ����
// (���������� ��������� �������� �����)
void QAxisZoomSvc::endAxisZoom(QMouseEvent *mEvent,int ax)
{
    // ������ ����� ���������������
    QwtChartZoom::QConvType ct = zoom->regim();
    // �������, ���� �� ������� �� ���� �� ������� ��������� �����
    if (ct != QwtChartZoom::ctAxisHL &&
        ct != QwtChartZoom::ctAxisHR &&
        ct != QwtChartZoom::ctAxisVB &&
        ct != QwtChartZoom::ctAxisVT) return;
    // ���� �������� ����� ������ ����, ��
    if (mEvent->button() == Qt::LeftButton)
    {
        if (light)  // ���� ������ �����, ��
        {
            // ���� �������� ���������, �� ������� ������ ����������
            if (indiAxZ) delete zwid;
            // ��������� ���������� ����������� ������� �����
            axisApplyMove(mEvent->pos(),ax);
        }
        // ��������������� ������
        zoom->plot()->axisWidget(ax)->setCursor(tCursor);
        // ��������� ����� ���������������
        zoom->setRegim(QwtChartZoom::ctNone);
    }
}
