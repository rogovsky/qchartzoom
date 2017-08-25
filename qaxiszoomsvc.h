/**********************************************************/
/*                                                        */
/*                   ����� QAxisZoomSvc                   */
/*                      ������ 1.2.2                      */
/*                                                        */
/* ������������ ��������� ��������� ����� �� ������       */
/* �������, �������� ����������� � ������ QwtChartZoom,   */
/* ������� � ������ 1.5.0.                                */
/*                                                        */
/* ���������� ��������� ������ ���������,                 */
/* �. �������-��������� ������������ ���., 2012 �.,       */
/* ��� ��������� �. �. ����������, �. �����������.        */
/*                                                        */
/* ����������� ��������� ������������� � ���������������. */
/* ���������� ������ �����������.                         */
/*                                                        */
/**********************************************************/

#ifndef QAXISZOOMSVC_H
#define QAXISZOOMSVC_H

#include "qwtchartzoom.h"

class QAxisZoomSvc : public QObject
{
    Q_OBJECT

public:
    // �����������
    explicit QAxisZoomSvc();

    // ������������ ���������� � ��������� ���������������
    void attach(QwtChartZoom *);

    // ���������/���������� ������� ������
    void setLightMode(bool);
    // ��������� ����� �������, ������������� ��������������� �����
    void setAxisRubberBandColor(QColor);
    // ���������/���������� ��������� ��������������� �����
    // (����� ������, ���� ������� ������ �����)
    void indicateAxisZoom(bool);

protected:
    // ���������� ���� �������
    bool eventFilter(QObject *,QEvent *);

private:
    QwtChartZoom *zoom;     // ��������� �������� ���������������
    bool light;             // ������ ����� (� ���������� ���������������)
    QWidget *zwid;          // ������ ��� ����������� ���������� ��������������� �����
    QColor awClr;           // ���� �������, ������������� ��������������� �����
    bool indiAxZ;           // ���� ��������� ��������������� �����
    QCursor tCursor;        // ����� ��� ���������� �������� �������

    double scb_xl,scb_xr;   // ������� ������� ������� �� ��� x � ������ ������ ��������������
    double scb_yb,scb_yt;   // ������� ������� ������� �� ��� y � ������ ������ ��������������
    double scb_wx,scb_hy;   // ������� ������ � ������ ������� � ������ ������ ��������������
    int scb_pxl,scb_pyt;    // ������� ����� � ������� �������� ������� � ������ ������ ��������������
                            // (� �������� ������������ �����)
    int scb_pw,scb_ph;      // ������� ������ � ������ ������� � ������ ������ ��������������
                            // (� ��������)
    int scp_x,scp_y;        // ��������� ������� � ������ ������ ��������������
                            // (� �������� ������������ ����� ������� �� ������� �������� �������)
    int sab_pxl,sab_pyt;    // ������� ����� � ������� �������� ������� � ������ ������ ��������������
                            // (� �������� ������������ ������� �����)

    // ����������� ������� ����������
    int limitSize(int,int);
    // ��������� ��������� ��������� ��������������� �����
    QRect *axisZoomRect(QPoint,int);
    // ���������� ����������� �������������� �����
    void showZoomWidget(QPoint,int);

    // ����������� ������ ������� �����
    double limitScale(double,double);
    // ���������� ����������� ����������� ������� �����
    void axisApplyMove(QPoint,int);

    // ���������� ������� �� ���� ��� �����
    void axisMouseEvent(QEvent *,int);

    // ���������� ������� �� ������ ���� ��� ������
    // (��������� ��������� �������� �����)
    void startAxisZoom(QMouseEvent *,int);
    // ���������� ����������� ����
    // (��������� ����� ������ �����)
    void procAxisZoom(QMouseEvent *,int);
    // ���������� ���������� ������ ����
    // (���������� ��������� �������� �����)
    void endAxisZoom(QMouseEvent *,int);
};

#endif // QAXISZOOMSVC_H
