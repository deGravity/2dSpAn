#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMouseEvent>
#include <QtCore/qmath.h>
#include <QPainter>
#include <QMessageBox>
#include <QDialog>
#include <QtGui>  //SS

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>

#define FORCE 0 //force threshold
#define A 1/*Flow Angle = tan-inv(1/A)

                    1 - 45   deg
                    2 - 26.6
                    3 - 18.4
                    4 - 14
                    5 - 11.3
                    6 - 9.5
*/
#define MAX 1000//maximum nummber of spines in an image

struct spine_struct
{
    unsigned short spine_flag;
    unsigned int sp_id,pCGx,pCGy,pArea,bCGx,bCGy,bCnt;
    unsigned short seedX,seedY,x1,y1,x2,y2,w,h,headX,headY,majA,minA;
    unsigned char **data,type;
    float length, headW,headWmax, neck_length,avg_neckW,min_neckW,tA;

};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void        mousePressEvent( QMouseEvent * );
    void        keyPressEvent(QKeyEvent *);

private slots:

    void on_Load_Image_clicked();

    void on_bth_valueChanged(int value);

    void on_binarize_clicked();

    void on_flow_to_right_clicked();

    void on_flow_to_left_clicked();

    void on_reset_flow_clicked();

    void on_segment_clicked();

    void on_segment27_clicked();

    void on_save_image_clicked();

    void on_mark_spines_clicked(bool checked);


    void on_pushButton_Reset_ROI_clicked();

    void on_checkBox_zoom_clicked();

     void on_pushButton_change_label_clicked();

     void on_pushButton_show_all_marked_sp_clicked();

     void on_checkBox_delSp_clicked(bool checked);

     void on_lineEdit_new_editingFinished();

     void on_checkBox_delSp_clicked();

     void on_pushButton_load_spine_annotation_clicked();

     void on_pushButton_delete_by_spine_number_clicked();

     void on_lineEdit_current_editingFinished();

     void on_pushButton_set_spid_clicked();

     void on_checkBox_manualSeg_clicked();

     void on_checkBox_joinSeg_clicked();

     void on_checkBox_clicked();

     void on_lneZoom_lostFocus();

     void on_lneZoom_returnPressed();

     void on_hsbZoom_valueChanged(int value);

     void on_bth_sliderReleased();

     void on_spinBox_valueChanged(int arg1);

     void on_bth_sliderPressed();

     void on_lineEdit_sp_no_returnPressed();


private:
    Ui::MainWindow *ui;

    unsigned char **symbol;
    unsigned char **symbol_bak;

    unsigned char **nodemat;
    unsigned int **intorg;
    unsigned int **orgmat;
    unsigned int **hmat;
    int **DT, **DT1, **DT2;

    QString filepath;
    QByteArray filename,only_filename;

    QPoint startP, endP, segStart, segEnd;
    QPoint  spineP,startROI, endROI,zoomROIs,zoomROIe,scrollPOS;
    QMessageBox msgBox;
    QImage save_img;

    struct spine_struct spine[MAX];
    unsigned int pID, trace_cnt;
    unsigned short maxpID;
    unsigned short usedPID[500];
    unsigned int delCGx,delCGy,delArea;
    unsigned short M,N, sM, sN;
    unsigned char SEflag,saveFlag, ROIflag,badSpine,step, manual_segment;
    short BinTh;
    int flow_angle;
    unsigned int flow_cnt;
    float scaleX,scaleY;
    float scaleSS, lastscaleSS;

    QString str;

    void draw_image();
    void writeinfile(int);
    void spine_analysis(int,int);
    void delete_spine(int,int);
    void draw_ROI();
    void FloodFill(int,int);
    void FloodFill1(int,int);
    void delFloodFill(int,int);
    int Bresenham(int,int,int,int,char);

    void flow_right(int,int,int);
    void flow_left(int,int,int);
    void flow_up(int,int,int);
    void flow_down(int,int,int);
    void flow_Dright(int,int,int);
    void flow_Dleft(int,int,int);


    void flow_right27(int,int,int);
    void flow_left27(int,int,int);
    void flow_up27(int,int,int);
    void flow_down27(int,int,int);
    void flow_Dright27(int,int,int);
    void flow_Dleft27(int,int,int);
    void flow_D1right27(int,int,int);
    void flow_D1left27(int,int,int);
    void flow_D2right27(int,int,int);
    void flow_D2left27(int,int,int);

    void polynomial_regression();

    void skel_main(int);
    void outline();
    void peeling();
    void gethor();
    void getver();
    void write_skel(int);
    int link_break(int,int);
    int connectivity(int, int);
    void create_node();

    void spine_DT(int);
    void compute_DT(int **, int);
    void compute_DT1(int);
    void trace_back(int, int, int,int, int);

};

#endif // MAINWINDOW_H
