/**********************************************************/
/*                                                        */
/*            ���������� ������ QWheelZoomSvc             */
/*                      ������ 1.0.3                      */
/*                                                        */
/* ���������� ��������� ������ ���������,                 */
/* �. �������-��������� ������������ ���., 2012 �.,       */
/* ��� ��������� �. �. ����������, �. �����������.        */
/*                                                        */
/* ����������� ��������� ������������� � ���������������. */
/* ���������� ������ �����������.                         */
/*                                                        */
/**********************************************************/

#include "qwheelzoomsvc.h"

#ifdef Q_WS_WIN
//    #define L_SHIFT 42
    #define R_SHIFT 54
#endif

#ifdef Q_WS_X11
//    #define L_SHIFT 50
    #define R_SHIFT 62
#endif

// �����������
QWheelZoomSvc::QWheelZoomSvc() :
    QObject()
{
    // ��������� �����������, ������������ ��������� �������� �������
    // ��� �������� ������ ����
    sfact = 1.2;
}

// ������������ ���������� � ��������� ���������������
void QWheelZoomSvc::attach(QwtChartZoom *zm)
{
    // ���������� ��������� �� �������� ���������������
    zoom = zm;
    // ��������� ��� ������� ���������� ������� (������ �������)
    zoom->plot()->installEventFilter(this);
}

// ������� ������������ ��������������� �������
// ��� �������� ������ ���� (�� ��������� �� ����� 1.2)
void QWheelZoomSvc::setWheelFactor(double fact) {
    sfact = fact;
}

// ���������� ���� �������
bool QWheelZoomSvc::eventFilter(QObject *target,QEvent *event)
{
    // ���� ������� ��������� ��� �������, ��
    if (target == zoom->plot())
    {
        // ���� ��������� ���� �� ������� �� ����������, ��
        if (event->type() == QEvent::KeyPress ||
            event->type() == QEvent::KeyRelease)
            switchWheel(event); // �������� ��������������� ����������
        // ���� ������� ������� ��������� ������ ����, ��
        if (event->type() == QEvent::Wheel)
        {
            procWheel(event);   // �������� ��������������� ����������
            zoom->updatePlot(); // ��������� ������
        }
    }
    // �������� ���������� ������������ ����������� �������
    return QObject::eventFilter(target,event);
}

// ���������� �������/���������� ������� Ctrl ��� Shift
void QWheelZoomSvc::switchWheel(QEvent *event)
{
    // ������ ����� ���������������
    QwtChartZoom::QConvType ct = zoom->regim();
    // ������� ��������� �� ������� �� ����������
    QKeyEvent *kEvent = static_cast<QKeyEvent *>(event);
    // ����������� ����� � ����������� �� �������
    switch (kEvent->key())
    {
    // ������� Ctrl
    case Qt::Key_Control:
        // � ����������� �� �������
        switch (event->type())
        {
        // ������� ������
        case QEvent::KeyPress:
            // ���� �� ������� ������� ������ �����,
            if (ct == QwtChartZoom::ctNone)
                // �� �������� ����� Wheel
                zoom->setRegim(QwtChartZoom::ctWheel);
        // ������� ��������
        case QEvent::KeyRelease:
            // ���� ������� ����� Wheel,
            if (ct == QwtChartZoom::ctWheel)
                // �� ��������� ���
                zoom->setRegim(QwtChartZoom::ctNone);
            break;
        // ����� ������ �� ������
        default: ;
        }
        break;
    // ������� Shift
    case Qt::Key_Shift:
        // � ����������� �� �������
        switch (event->type())
        {
        // ������� ������
        case QEvent::KeyPress:

#ifdef R_SHIFT
// ��������� Win ��� X11
            // ���� �� ������� ������� ������ �����, ��
            if (ct == QwtChartZoom::ctNone)
            {
                // ���� ����� ������ Shift,
                if (kEvent->nativeScanCode() == R_SHIFT)
                    // �� �������� ����� ctHorWheel
                    zoom->setRegim(QwtChartZoom::ctHorWheel);
                // ����� (����� ����� Shift) �������� ����� ctVerWheel
                else zoom->setRegim(QwtChartZoom::ctVerWheel);
            }
#else
// ����������� ���������
            // ���� �� ������� ������� ������ �����,
            if (ct == QwtChartZoom::ctNone)
                // �� �������� ����� ctVerWheel
                zoom->setRegim(QwtChartZoom::ctVerWheel);
#endif

            break;
        // ������� ��������
        case QEvent::KeyRelease:

#ifdef R_SHIFT
// ��������� Win ��� X11
            // ���� ������� ������ Shift,
            if (kEvent->nativeScanCode() == R_SHIFT)
            {
                // ���� ������� ����� ctHorWheel,
                if (ct == QwtChartZoom::ctHorWheel)
                    // �� ��������� ���
                    zoom->setRegim(QwtChartZoom::ctNone);
            }
            // ����� (������� ����� Shift)
            // ���� ������� ����� ctVerWheel,
            else if (ct == QwtChartZoom::ctVerWheel)
                // �� ��������� ���
                zoom->setRegim(QwtChartZoom::ctNone);
#else
// ����������� ���������
            // ���� ������� ����� ctVerWheel,
            if (ct == QwtChartZoom::ctVerWheel)
                // �� ��������� ���
                zoom->setRegim(QwtChartZoom::ctNone);
#endif

            break;
        // ����� ������ �� ������
        default: ;
        }
        break;
        // ��� ��������� ������ �� ������
    default: ;
    }
}

// ���������� ��������� �� �������� ������ ����
void QWheelZoomSvc::applyWheel(QEvent *event,bool ax,bool ay)
{
    // �������� ��� QEvent � QWheelEvent
    QWheelEvent *wEvent = static_cast<QWheelEvent *>(event);
    // ���� ��������� ������������ ������ ����
    if (wEvent->orientation() == Qt::Vertical)
    {
        // ���������� ���� �������� ������ ����
        // (�������� 120 ������������� ���� �������� 15�)
        int wd = wEvent->delta();
        // ��������� �������������� ���������
        // (�� ������� ��� ����� ��������/�������� ������)
        double kw = sfact*wd/120;
        if (wd != 0)    // ���� ������ ���������, ��
        {
            // ��������� �������� ������� ������� (���� ����� ��� �� ���� �������)
            zoom->fixBounds();
            // �������� ��������� �� ������
            QwtPlot *plt = zoom->plot();
            if (ax) // ���� ������ ��������������� �� �����������
            {
                // �������� ����� �������� �������������� �����
                QwtScaleMap sm = plt->canvasMap(zoom->masterH());
                // ���������� ����� ������������� �� ����� x ���������
                double mx = (sm.s1()+sm.s2())/2;
                // � ���������� ���������
                double dx = (sm.s2()-sm.s1())/2;
                // � ����������� �� ����� ���� �������� ������ ����
                // ��������� ���������� ������������ ���������� � kw ���
                if (wd > 0) dx /= kw;
                // ��� ����������� ���������� ������������ ���������� � -kw ���
                else dx *= -kw;
                // ������������� ����� ����� � ������ ������� ����� ��� ��� x
                // (����� ������������ ����� ������� �������� �� �����,
                // � ������� ��������� �� ������, �.�. ����������� ������� �����������)
                zoom->isb_x->set(mx-dx,mx+dx);
            }
            if (ay) // ���� ������ ��������������� �� ���������
            {
                // �������� ����� �������� ������������ �����
                QwtScaleMap sm = plt->canvasMap(zoom->masterV());
                // ���������� ����� ������������� �� ����� y ���������
                double my = (sm.s1()+sm.s2())/2;
                // � ���������� ���������
                double dy = (sm.s2()-sm.s1())/2;
                // � ����������� �� ����� ���� �������� ������ ����
                // ��������� ���������� ������������ ���������� � kw ���
                if (wd > 0) dy /= kw;
                // ����������� ���������� ������������ ���������� � -kw ���
                else dy *= -kw;
                // ������������� ����� ������ � ������� ������� ������������ �����
                // (����� ������������ ����� ������� �������� �� �����,
                // � ������� ��������� �� ������, �.�. ����������� ������� �����������)
                zoom->isb_y->set(my-dy,my+dy);
            }
            // ������������� ������ (��������� � ����������)
            plt->replot();
        }
    }
}

// ���������� �������� ������ ����
void QWheelZoomSvc::procWheel(QEvent *event)
{
    // � ����������� �� ����������� ������ ��������
    // ��������������� � ���������������� �����������
    switch (zoom->regim())
    {
        // Wheel - ���� ������ ������� Ctrl -
            // ��������������� �� ����� ����
    case QwtChartZoom::ctWheel: applyWheel(event,true,true); break;
        //VerWheel - ���� ������ ����� ������� Shift -
            // ��������������� ������ �� ������������ ���
    case QwtChartZoom::ctVerWheel: applyWheel(event,false,true); break;
        // HorWheel - ���� ������ ������ ������� Shift -
            // ��������������� ������ �� �������������� ���
    case QwtChartZoom::ctHorWheel: applyWheel(event,true,false); break;
        // ��� ������ ������� ������ �� ������
    default: ;
    }
}
