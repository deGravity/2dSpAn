#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->scrollArea_ROI->setWidget(ui->label_img);
    scaleSS=lastscaleSS=1;
    ui->lneZoom->setVisible(false);
    ui->hsbZoom->setVisible(false);
    ui->lneZoom->setEnabled(false);
    ui->hsbZoom->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Load_Image_clicked()
{
    int t;
    filepath=QFileDialog::getOpenFileName(this,tr("Open File"), "G:\\ICM\\2dSpAn\\Data",
                                      tr("Images (*.*)"));
    ui->scrollArea_ROI->setVisible(true);
    ui->lne_bth_temp->setVisible(false);

    if (filepath!=NULL);

    else
        return;

    filename=filepath.toLatin1();

    short c1=filename.lastIndexOf("/");
    short c2=filename.lastIndexOf(".");
    only_filename=filename.mid(c1+1,c2-c1-1);

    ui->label_imgpath->setText(only_filename);
    setWindowTitle("2dSpAn: "+only_filename);

    QImage img(filepath);
    img.load(filename);
    save_img=img;
    ui->label_img->setPixmap(QPixmap::fromImage(img));
    scaleSS=lastscaleSS=1;

    symbol  = (unsigned char **)calloc(sizeof(unsigned char *) , img.height());
    symbol_bak  = (unsigned char **)calloc(sizeof(unsigned char *) , img.height());

    for (int i = 0 ;  i < img.height(); i++) {
        symbol[i] = (unsigned char *)calloc(sizeof(unsigned char) , img.width());
        symbol_bak[i] = (unsigned char *)calloc(sizeof(unsigned char) , img.width());
    }

    ui->test1->setNum(img.height());
    ui->test2->setNum(img.width());

    M=img.height();
    N=img.width();

    for(int i=0;i<img.height();i++)
        for(int j=0;j<img.width();j++)
            symbol_bak[i][j]='i';

    for(t=0;t<500;t++)
        usedPID[t]=0;

    pID=maxpID=1;
    manual_segment=0;
    on_pushButton_Reset_ROI_clicked();
}


void MainWindow::on_pushButton_load_spine_annotation_clicked()
{
    QString temp1=QFileDialog::getOpenFileName(this,tr("Open File"), "",
                                      tr("Annotation Files (*.*)"));

    QByteArray qb=temp1.toLatin1();
    const char *filename1 =  qb.data();

    ui->label_annotation_path->setText(filename1);

    FILE *fp;
    fp=fopen(filename1,"r");
    if(fp==NULL) return;

    fscanf(fp,"%d\n",&pID);
    maxpID=pID;
    for(int i=1;i<pID;i++)
    {        
        fscanf(fp,"%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %f %f %c\n",
               &spine[i].spine_flag, &spine[i].sp_id,&spine[i].seedX,&spine[i].seedY,
               &spine[i].pCGx,&spine[i].pCGy,&spine[i].pArea,&spine[i].x1,&spine[i].y1,&spine[i].x2,&spine[i].y2,
               &spine[i].bCGx,&spine[i].bCGy,&spine[i].bCnt,
               &spine[i].headX,&spine[i].headY,&spine[i].headW,&spine[i].headWmax,&spine[i].type);
    }

    fscanf(fp,"%d %d\n",&M,&N);

    for(int i=0;i<M;i++)
    {
        for(int j=0;j<N;j++)
        {
            fscanf(fp,"%c",&symbol_bak[i][j]);
        }
        fscanf(fp,"\n");
    }
    fclose(fp);
    on_pushButton_show_all_marked_sp_clicked();
}


void MainWindow::draw_image()
{
    char buffer[10];
    QImage img(filepath);
    img.load(filename);
    QImage out_img(img.size(),QImage::Format_RGB32);
    unsigned char intensity;
    float f;

    for(int i=0;i<N;i++)
        for(int j=0;j<M;j++)
        {
            intensity=qGray(img.pixel(i,j));
            f=(float)intensity/255;
            switch(symbol[j][i])
            {
                case 'i': out_img.setPixel(i,j,qRgb(intensity,intensity,intensity)); break;//original intensity

                case 's': out_img.setPixel(i,j,qRgb(0,255,0)); break;//start pixel
                case 'e': out_img.setPixel(i,j,qRgb(0,0,255)); break;//end pixel                
                case '0': out_img.setPixel(i,j,qRgb(255,255,255)); break;//potential flow region..myRed
                case '>': out_img.setPixel(i,j,qRgb(255,255,255)); break;//flow to right
                case '<': out_img.setPixel(i,j,qRgb(255,255,255)); break;//flow to left

                case '=': out_img.setPixel(i,j,qRgb(242*f,115*f,77*f));//,qRgb(219,60,43));//RED
                          flow_cnt++;
                          break;//flow intersection..
                case 'p': out_img.setPixel(i,j,qRgb(76*f,250*f,0)); break;//segmented spines..
                case 'R': out_img.setPixel(i,j,qRgb(255,204,153)); break;//ROI..
                case 'x': out_img.setPixel(i,j,qRgb(0,0,255)); break;//potential flow region..myRed
                case 'y': out_img.setPixel(i,j,qRgb(255,0,255)); break;//Head-Base line
                default: out_img.setPixel(i,j,qRgb(intensity,intensity,intensity)); break;
            }
    }



    ////////////////////////////Overlay text////////////////////////////////
    {
        QPainter painter(&out_img);
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 8,75));

        for(int i=1;i<=maxpID;i++)
        {
            QPointF p;

            if(spine[i].spine_flag=='1')
            {
                sprintf(buffer,"%d",spine[i].sp_id);
                painter.setPen(Qt::red);

                int dist1=abs(spine[i].x1-spine[i].bCGx)+abs(spine[i].y1-spine[i].bCGy);
                int dist2=abs(spine[i].x2-spine[i].bCGx)+abs(spine[i].y2-spine[i].bCGy);

                if(dist1>dist2)
                    painter.drawText(spine[i].x1,spine[i].y1,buffer);
                else
                    painter.drawText(spine[i].x2,spine[i].y2,buffer);

                p.setX(spine[i].headX);
                p.setY(spine[i].headY);

                if(spine[i].type=='r') painter.setPen(QPen(Qt::cyan, 1, Qt::SolidLine, Qt::RoundCap));
                if(spine[i].type=='m') painter.setPen(QPen(Qt::yellow, 1, Qt::SolidLine, Qt::RoundCap));
                if(spine[i].type=='f') painter.setPen(QPen(Qt::blue, 1, Qt::SolidLine, Qt::RoundCap));
                if(spine[i].type=='s') painter.setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap));

                painter.drawEllipse(p, (qreal)(spine[i].headW/2) , (qreal)(spine[i].headW/2) );
            }
        }
        if(scaleSS>1)            
            ui->label_img->setPixmap(QPixmap::fromImage(out_img).scaled(out_img.width()*scaleSS,out_img.height()*scaleSS));
        else
            ui->label_img->setPixmap(QPixmap::fromImage(out_img));
        save_img=out_img;
    }

    //Saving the current image
    if(saveFlag==1)
    {
        QByteArray qb = only_filename;
        out_img.save(qb.append(".png"),"PNG",-1);
    }
}


void MainWindow::mousePressEvent( QMouseEvent *e)
{
    QPoint pos;
    double x,y;
    int seg_len;

    if (filepath==NULL) return;
    int q;
    q=(e->x()-ui->scrollArea_ROI->x()+ui->scrollArea_ROI->horizontalScrollBar()->value())/scaleSS;

    str.setNum(q);
    x=q;
    q=(e->y()-ui->scrollArea_ROI->y()-ui->mainToolBar->height()+ui->scrollArea_ROI->verticalScrollBar()->value())/scaleSS;
    str.setNum(q);
    y=q;

    if(e->buttons() == Qt::RightButton)
    {
        ui->checkBox_delSp->setChecked(true);
        ui->mark_spines->setChecked(false);
        ui->checkBox_manualSeg->setChecked(false);
    }

    ui->test1->setNum(x);
    ui->test2->setNum(y);

    if(y<=1||y>=M-2||x<=1||x>=N-2) return;

    if(ui->checkBox_delSp->isChecked()==true)//delete spine
    {
        if (symbol_bak[(int)y][(int)x]!='p') return;
        delete_spine((int)y,(int)x);
        draw_image();
        return;
    }

    if(ui->mark_spines->isChecked()==true)//select spine
    {
         if (symbol_bak[(int)y][(int)x]=='p')
             QMessageBox::information(this, "2dSpAn", "Already marked spine. Delete first to re-annotate the spine.", QMessageBox::Ok);
         else
         if (symbol[(int)y][(int)x]=='0' || symbol[(int)y][(int)x]=='>'|| symbol[(int)y][(int)x]=='<')
         {
             badSpine=0;
             spine_analysis((int)y,(int)x);
             if(pID>=maxpID)
                maxpID=pID;
         }
        draw_image();        
        return;
    }

    if(ui->checkBox_manualSeg->isChecked()==true// Manually segment touching spines
            || ui->checkBox_joinSeg->isChecked()==true)//Manually join disconnected parts of one spines
    {
        if (manual_segment==0)
        {
            segStart.setX(x);
            segStart.setY(y);
            symbol[(int)y][(int)x]='x';            
            manual_segment=1;
        }
        else
            if(manual_segment==1)
            {
                segEnd.setX(x);
                segEnd.setY(y);
                symbol[(int)y][(int)x]='x';                
                manual_segment=0;
                if (ui->checkBox_joinSeg->isChecked()==true)
                {
                    seg_len= Bresenham(segStart.x(),segStart.y(),segEnd.x(),segEnd.y(), '0');
                    seg_len= Bresenham(segStart.x()-1,segStart.y(),segEnd.x()-1,segEnd.y(), '0');
                }
                else
                    seg_len= Bresenham(segStart.x(),segStart.y(),segEnd.x(),segEnd.y(), 'x');
                ui->mark_spines->setChecked(true);
                ui->checkBox_manualSeg->setChecked(false);
                ui->checkBox_joinSeg->setChecked(false);
            }
        draw_image();
        return;
    }

    if(SEflag==0)
    {
        on_pushButton_Reset_ROI_clicked();
        startP.setX(x);
        startP.setY(y);
        symbol[(int)y][(int)x]='s';
        symbol[(int)y][(int)x+1]='s';
        symbol[(int)y][(int)x-1]='s';
        symbol[(int)y-1][(int)x]='s';
        symbol[(int)y+1][(int)x]='s';
        SEflag=1;
    }
    else
    {
        endP.setX(x);
        endP.setY(y);
        symbol[(int)y][(int)x]='e';
        symbol[(int)y][(int)x+1]='e';
        symbol[(int)y][(int)x-1]='e';
        symbol[(int)y-1][(int)x]='e';
        symbol[(int)y+1][(int)x]='e';
        SEflag=0;
        draw_ROI();
    }
    draw_image();
}



void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
         {
            for(int i=(int)startROI.x();i<=endROI.x();i++)
                for(int j=(int)startROI.y();j<=endROI.y();j++)
                {                    
                    symbol[j][i]=symbol_bak[j][i];
                }
            draw_ROI();
        }
        event->accept();
    }
    else
        if (event->key() == Qt::Key_M)
        {
            if(ui->checkBox_manualSeg->isChecked()==false)
            {                
                ui->mark_spines->setChecked(false);
                ui->checkBox_manualSeg->setChecked(true);
                ui->checkBox_joinSeg->setChecked(false);
            }
            event->accept();
        }
        else
            if (event->key() == Qt::Key_J)
            {
                if(ui->checkBox_joinSeg->isChecked()==false)
                {                    
                    ui->mark_spines->setChecked(false);
                    ui->checkBox_joinSeg->setChecked(true);
                    ui->checkBox_manualSeg->setChecked(false);
                }
                event->accept();
            }
            else if((event->key()==Qt::Key_Up))
            {
                ui->hsbZoom->setValue(ui->hsbZoom->value()+1);
            }
            else if((event->key()==Qt::Key_Down))
            {
                ui->hsbZoom->setValue(ui->hsbZoom->value()-1);
            }        
}


void MainWindow::draw_ROI()
{
    int minTh=255,maxTh=0,cnt=0;

    QImage img(filepath);
    img.load(filename);

    unsigned char intensity;

    int histogram[256];
    int img_size=0;

    for (int i=0;i<256;i++)
        histogram[i]=0;

    int w, h;
    QPoint temp;

    if(startP.x()>endP.x())//swap points
    {
        temp=startP;
        startP=endP;
        endP=temp;
    }

    w= abs(endP.x()-startP.x());
    h= abs(endP.y()-startP.y());

    flow_angle=(int)(qAtan((double)h/w)*57.2957795131);

    ui->test3->setNum(flow_angle);
    if (w<=0.25*N) w=0.25*N;
    if (h<=0.25*M) h=0.25*M;

    startROI.setX(startP.x()-(w/2));
    if (startROI.x()<0) startROI.setX(0);

    endROI.setX(endP.x()+(w/2));
    if (endROI.x()>N-1) endROI.setX(N-1);

    if (startP.y()>endP.y())
    {
        startROI.setY(endP.y()-(h/2));
        endROI.setY(startP.y()+(h/2));
    }
    else
    {
        startROI.setY(startP.y()-(h/2));
        endROI.setY(endP.y()+(h/2));
    }

    if (startROI.y()<0) startROI.setY(0);
    if (endROI.y()>M-1) endROI.setY(M-1);

    for(int i=(int)startROI.y();i<=endROI.y();i++)
        for(int j=(int)startROI.x();j<=endROI.x();j++)
        {
            intensity=qGray(img.pixel(j,i));
            histogram[intensity]++;
            img_size++;
            if(intensity>maxTh) maxTh=intensity;
            if(intensity<minTh) minTh=intensity;
    }

    short ROIw,ROIh;

    ROIw= abs(endROI.x()-startROI.x())+1;
    ROIh= abs(endROI.y()-startROI.y())+1;

    scaleX=(float)ui->label_img->width()/ROIw;
    scaleY=(float)ui->label_img->height()/ROIh;


    ///OTSU START
    // NOTE: Creation of histogram[256] not shown
    float  ww = 0;                // first order cumulative
    float  u = 0;                // second order cumulative
    float  uT = 0;               // total mean level
    int    k = 255;              // maximum histogram index
    int    threshold = 0;        // optimal threshold value
    float  histNormalized[256];  // normalized histogram values
    float  work1, work2;		// working variables
    double work3 = 0.0;

    // Create normalised histogram values
    // (size=image width * image height)
    for (int I=1; I<=k; I++)
        histNormalized[I-1] = histogram[I-1]/(float)img_size;

    // Calculate total mean level
    for (int I=1; I<=k; I++)
    uT+=(I*histNormalized[I-1]);

    // Find optimal threshold value
    for (int I=1; I<k; I++) {
        ww+=histNormalized[I-1];
        u+=(I*histNormalized[I-1]);
        work1 = (uT * ww - u);
        work2 = (work1 * work1) / ( ww * (1.0f-ww) );
        if (work2>work3) work3=work2;
    }

    // Convert the final value to an integer
    threshold = (int)sqrt(work3);
    ////OTSU END////

    BinTh=(float)(maxTh+minTh)/2;

    std::cout<<"OTSU Th="<<threshold<<" Max-Min Th="<<BinTh;

    if(ui->checkBox->isChecked()==true)
        BinTh=(float)(BinTh+threshold)/2;

    std::cout<<"Final Th="<<BinTh<<std::endl;

    if(ui->bth->value()!=BinTh)
    {
        ui->bth->setValue(BinTh);
        on_bth_sliderReleased();
        return;
    }

     if(ui->checkBox_manualSeg->isChecked()==false)
     {
         ui->mark_spines->setChecked(true);
     }
    on_segment27_clicked();
    if(flow_cnt==0)
    {
        on_segment_clicked();
    }
}



void MainWindow::on_checkBox_clicked()
{
    draw_ROI();
}


void MainWindow::on_bth_valueChanged(int value)
{
    ui->lne_bth_temp->setText(str.setNum(value));
}

void MainWindow::on_bth_sliderReleased()
{
    ui->mark_spines->setChecked(true);
    ui->lne_bth_temp->setVisible(false);
    ui->spinBox->setValue(ui->bth->value());
    on_segment27_clicked();

    if(flow_cnt==0)
    {
        on_segment_clicked();
    }
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    ui->bth->setValue(arg1);
    on_bth_sliderReleased();
}

void MainWindow::on_bth_sliderPressed()
{
    ui->lne_bth_temp->setVisible(true);
}

void MainWindow::on_binarize_clicked()
{
    QImage img(filepath);
    img.load(filename);
    unsigned char intensity;
    BinTh=ui->bth->value();

    for(int i=(int)startROI.x();i<=endROI.x();i++)
        for(int j=(int)startROI.y();j<=endROI.y();j++)
    {
            intensity=qGray(img.pixel(i,j));
            if (symbol[j][i]=='s' || symbol[j][i]=='e') //retain the start end labels
                   continue;
            if (intensity>BinTh)
                symbol[j][i]='0';//dendrite
            else
                symbol[j][i]='1';//background
    }
    SEflag=0;

    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            if(symbol_bak[j][i]=='p')
                symbol[j][i]='p';
        }
    draw_image();
}

void MainWindow::on_segment_clicked()
{
    SEflag=0;
    flow_cnt=0;
    on_binarize_clicked();

    if(flow_angle>=0 && flow_angle<=23)
    {
        flow_right(startP.y(),startP.x(),0);
        flow_left(endP.y(),endP.x(),0);
    }
    else
    if(flow_angle>23 && flow_angle<67)
    {
        flow_Dright(startP.y(),startP.x(),0);
        flow_Dleft(endP.y(),endP.x(),0);
    }
    else
    if(flow_angle>=67 && flow_angle<=90)
    {
        QPoint temp;

        if(startP.y()<endP.y())//swap points
        {
            temp=startP;
            startP=endP;
            endP=temp;
        }

        flow_up(startP.y(),startP.x(),0);
        flow_down(endP.y(),endP.x(),0);
    }
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            if(symbol_bak[j][i]=='p')
                symbol[j][i]='p';
        }
    draw_image();
}

void MainWindow::on_segment27_clicked()
{    
    SEflag=0;
    flow_cnt=0;

    on_binarize_clicked();

    if(flow_angle>=0 && flow_angle<=11)
    {
        flow_right27(startP.y(),startP.x(),0);
        flow_left27(endP.y(),endP.x(),0);
    }
    else
    if(flow_angle>11 && flow_angle<=34)
    {
        flow_D1right27(startP.y(),startP.x(),0);
        flow_D1left27(endP.y(),endP.x(),0);
    }
    else
    if(flow_angle>34 && flow_angle<=56)
    {
        flow_Dright27(startP.y(),startP.x(),0);
        flow_Dleft27(endP.y(),endP.x(),0);
    }
    else
    if(flow_angle>56 && flow_angle<=79)
    {
        flow_D2right27(startP.y(),startP.x(),0);
        flow_D2left27(endP.y(),endP.x(),0);
    }
    else
    if(flow_angle>79 && flow_angle<=90)
    {
        QPoint temp;

        if(startP.y()<endP.y())//swap points
        {
            temp=startP;
            startP=endP;
            endP=temp;
        }
        flow_up27(startP.y(),startP.x(),0);
        flow_down27(endP.y(),endP.x(),0);
    }

    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            if(symbol_bak[j][i]=='p')
                symbol[j][i]='p';
        }
    draw_image();
}


void MainWindow::on_flow_to_right_clicked()
{
    flow_right(startP.y(),startP.x(),0);
    draw_image();
}


void MainWindow::on_flow_to_left_clicked()
{
    flow_left(endP.y(),endP.x(),0);
    draw_image();
}


void MainWindow::spine_analysis(int r,int c)
{
    int i;

    spine[pID].pArea=0;
    spine[pID].pCGx=0;//spine CG
    spine[pID].pCGy=0;//spine CG
    spine[pID].bCGx=0;//base CG
    spine[pID].bCGy=0;//base CG
    spine[pID].bCnt=0;//base pixel count
    spine[pID].x1=N;
    spine[pID].y1=M;
    spine[pID].x2=0;
    spine[pID].y2=0;

    step=1;
    badSpine=0;
    FloodFill(r,c);

    if (spine[pID].bCnt==0)
        badSpine=3;//isolated blob

    if(!badSpine)
    {
       FloodFill(r,c);
       for(int i=(int)startROI.x();i<=endROI.x();i++)
           for(int j=(int)startROI.y();j<=endROI.y();j++)
        {
               if (symbol[j][i]=='p')
                   symbol_bak[j][i]='p';
        }

    spine[pID].pCGx=(float)spine[pID].pCGx/spine[pID].pArea;
    spine[pID].pCGy=(float)spine[pID].pCGy/spine[pID].pArea;

    spine[pID].bCGx=(float)spine[pID].bCGx/spine[pID].bCnt;
    spine[pID].bCGy=(float)spine[pID].bCGy/spine[pID].bCnt;

    int tx=spine[pID].bCGx;
    int ty=spine[pID].bCGy;
    {
        spine[pID].x1-=2;
        spine[pID].y1-=2;
        spine[pID].x2+=2;
        spine[pID].y2+=2;
    }

    symbol[ty][tx]='y';
    spine[pID].spine_flag='1';//good spine
    spine[pID].seedX=c;
    spine[pID].seedY=r;
    spine[pID].sp_id=pID;
    spine[pID].w=abs(spine[pID].x1-spine[pID].x2+1);
    spine[pID].h=abs(spine[pID].y1-spine[pID].y2+1);

    //connecting base and spine CG, when separated
    int base_len=Bresenham(spine[pID].bCGx,spine[pID].bCGy, spine[pID].pCGx,spine[pID].pCGy, 'p');

    //analysing spine data
    spine_DT(pID);

    usedPID[pID]=1;     //occupy the usedPID position
    i=pID;
    while(spine[++i].spine_flag=='1');
    pID=i;
    bool ok;
    i=ui->lineEdit_spid->text().toInt(&ok);
    if(pID>i)
        ui->lineEdit_spid->setText("");    
    }
    else
    if(badSpine==1)
    {
        spine[pID].spine_flag='0';//deleted spine
        usedPID[pID]=0; //mark as unused pID
        QMessageBox::critical(this, "2dSpAn", "Wrong selection of spine. Try again!", QMessageBox::Ok);

        for(int i=0;i<M;i++)
            for(int j=0;j<N;j++)
            {
                if(symbol[j][i]=='p' && symbol_bak[j][i]=='i')
                    symbol[j][i]='0';
            }
    }
   else
        if(badSpine==2)
        {
           QMessageBox::critical(this, "2dSpAn", "Touching with another spine. Use manual segmentation to separate the boundary or delete the existing spine.", QMessageBox::Ok);

           for(int i=0;i<M;i++)
               for(int j=0;j<N;j++)
               {
                   if(symbol[j][i]=='p' && symbol_bak[j][i]=='i')
                       symbol[j][i]='0';
               }           
        }
   else
   if(badSpine==3)
   {
      QMessageBox::critical(this, "2dSpAn", "Isolated blob. Not touching with dendrite.", QMessageBox::Ok);

      for(int i=0;i<M;i++)
          for(int j=0;j<N;j++)
          {
              if(symbol[j][i]=='p' && symbol_bak[j][i]=='i')
                  symbol[j][i]='0';
          }     
   }
}

void MainWindow::FloodFill(int r,int c)
{
    if (r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x())
    {
        badSpine=1;
        return;
    }

    if(symbol_bak[r][c]=='p')
    {
        badSpine=2;
        return;
    }

    unsigned char label=symbol[r][c];

    if(label=='0'||label=='>'||label=='<')
    {
        symbol[r][c]='p';

        spine[pID].pArea++;
        spine[pID].pCGx+=c;
        spine[pID].pCGy+=r;

        if(c<spine[pID].x1) spine[pID].x1=c;
        if(r<spine[pID].y1) spine[pID].y1=r;
        if(c>spine[pID].x2) spine[pID].x2=c;
        if(r>spine[pID].y2) spine[pID].y2=r;

        FloodFill(r+1,c);
        FloodFill(r-1,c);
        FloodFill(r,c+1);
        FloodFill(r,c-1);
    }
    else
    if(label=='=')
    {
        spine[pID].bCnt++;
        spine[pID].bCGx+=c;
        spine[pID].bCGy+=r;
        //symbol[r][c]='y';
    }
}


void MainWindow::FloodFill1(int r,int c)
{
    if (r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x())
    {
        return;
    }
    unsigned char label=symbol[r][c];
    if(label=='p')
    {
        symbol[r][c]='0';

        FloodFill1(r+1,c);
        FloodFill1(r-1,c);
        FloodFill1(r,c+1);
        FloodFill1(r,c-1);
    }
}

void MainWindow::delete_spine(int r,int c)
{

    int reply=QMessageBox::warning(this, "Warning", "Are you sure to delete this Spine entry?", QMessageBox::Cancel, QMessageBox::Ok);
    if (reply == QMessageBox::Cancel)
        return;
    delCGx=delCGy=delArea=0;
    delFloodFill(r,c);
    delCGx=(float)delCGx/delArea;
    delCGy=(float)delCGy/delArea;

    for(int i=1;i<pID;i++)
    {
        // Match the CG also deleting the spine
        if(abs(spine[i].pArea-delArea)<=5 && abs(spine[i].pCGx-delCGx)<=5 && abs(spine[i].pCGy-delCGy)<=5)//Sometimes the spine area may grow due to base_CG connection
        {                                 //assuming 5 pixel is maximum increase
                spine[i].spine_flag='0';//deleted spine
        }
    }
    //resetting the spine count
    unsigned short maxID=0;
    for(int i=1;i<pID;i++)
    {
        if(spine[i].sp_id>maxID && spine[i].spine_flag=='1')
        {
            maxID=spine[i].sp_id;
        }
    }
    pID=maxID+1;
    maxpID=pID;

    //resetting the checkboxes
    ui->mark_spines->setChecked(true);
    ui->checkBox_delSp->setChecked(false);
    ui->checkBox_manualSeg->setChecked(false);
}


void MainWindow::delFloodFill(int r,int c)
{
    if (r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x())
        return;

    unsigned char label=symbol_bak[r][c];

    if(label=='p')
    {
        symbol[r][c]='0';
        symbol_bak[r][c]='i';

        delArea++;
        delCGx+=c;
        delCGy+=r;

        delFloodFill(r+1,c);
        delFloodFill(r-1,c);
        delFloodFill(r,c+1);
        delFloodFill(r,c-1);
    }
}


void MainWindow::on_mark_spines_clicked(bool checked)
{
    SEflag=9;
    if (checked==true)
    {
        ui->checkBox_delSp->setChecked(false);
        ui->checkBox_manualSeg->setChecked(false);
    }
}



void MainWindow::on_pushButton_Reset_ROI_clicked()
{    
    ROIflag=0;
    SEflag=0;

    startROI.setX(0);
    startROI.setY(0);
    endROI.setX(N-1);
    endROI.setY(M-1);
    scaleX=scaleY=1;

    for(int i=0;i<N;i++)
        for(int j=0;j<M;j++)
        {            
            symbol[j][i]='i';
        }

    ui->checkBox_zoom->setChecked(false);
    ui->lneZoom->setVisible(false);
    ui->hsbZoom->setVisible(false);
    scaleSS=lastscaleSS=1;

    ui->mark_spines->setChecked(false);
    ui->checkBox_delSp->setChecked(false);
    ui->checkBox_manualSeg->setChecked(false);

    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            if(symbol_bak[j][i]=='p')
                symbol[j][i]='p';
        }
    draw_image();
}


void MainWindow::on_reset_flow_clicked()
{
    SEflag=0;
    flow_cnt=0;

    ui->mark_spines->setChecked(false);
    ui->checkBox_delSp->setChecked(false);

    on_binarize_clicked();
}


void MainWindow::on_checkBox_zoom_clicked()
{
    if (ui->checkBox_zoom->isChecked()==true)
    {
        ui->lneZoom->setText("1.0");
        ui->hsbZoom->setValue(10);
        ui->hsbZoom->setValue(0);
        ui->lneZoom->setVisible(true);
        ui->hsbZoom->setVisible(true);
        ui->lneZoom->setEnabled(true);
        ui->hsbZoom->setEnabled(true);
        ui->lneZoom->setFocus();

        draw_image();
    }
    else
    {
        ui->lneZoom->setText("1.0");
        ui->hsbZoom->setValue(10);
        ui->lneZoom->setVisible(false);
        ui->hsbZoom->setVisible(false);
        ui->lneZoom->setEnabled(false);
        ui->hsbZoom->setEnabled(false);
    }
}

void MainWindow::on_pushButton_change_label_clicked()
{
    int val1,val2;
    char buffer[200];

    val1=ui->lineEdit_current->text().toInt();
    val2=ui->lineEdit_new->text().toInt();

    for(int i=1;i<pID;i++)
    {
        if(spine[i].sp_id==val2 && spine[i].spine_flag=='1')
        {
            sprintf(buffer,"Spine entry %d already exists.",val2);
            QMessageBox::warning(this, "Warning", buffer, QMessageBox::Ok);
            ui->lineEdit_new->setText("");
            return;
        }

        if(spine[i].sp_id==val1 && spine[i].spine_flag=='0')
        {
            sprintf(buffer,"Spine entry %d has already been deleted, Enter a valid spine ID",val1);
            QMessageBox::warning(this, "Warning", buffer, QMessageBox::Ok);
            ui->lineEdit_current->setText("");
            return;
        }        
    }

    for(int i=1;i<pID;i++)
    {
        if(spine[i].spine_flag=='1')
        {

            if(spine[i].sp_id==val1)
                spine[i].sp_id=val2;
        }

    }
    unsigned short maxID=0;
    for(int i=1;i<pID;i++)
    {
        if(spine[i].sp_id>maxID && spine[i].spine_flag=='1')
        {
            maxID=spine[i].sp_id;
        }
    }
    pID=maxID+1;
    maxpID=pID;
    on_pushButton_show_all_marked_sp_clicked();
}

void MainWindow::on_pushButton_show_all_marked_sp_clicked()
{
    on_pushButton_Reset_ROI_clicked();
    for(int i=0;i<M;i++)
        for(int j=0;j<N;j++)
        {
            if(symbol_bak[j][i]=='p')
                symbol[j][i]='p';
        }
    draw_image();
}


void MainWindow::on_checkBox_delSp_clicked(bool checked)
{
    if (checked==true)
    {
        ui->mark_spines->setChecked(false);
        ui->checkBox_manualSeg->setChecked(false);
    }
}

void MainWindow::on_checkBox_delSp_clicked()
{
    //Not used
}


void MainWindow::on_lineEdit_current_editingFinished()
{
    // Not used
}

void MainWindow::on_lineEdit_new_editingFinished()
{
    //Not used
}

void MainWindow::on_save_image_clicked()
{
    unsigned int h,w;

    FILE *fp;
    char * temp;
    QByteArray qb = only_filename;

    ///////////////////Saving an image///////////////////////
    saveFlag=1;
    on_pushButton_show_all_marked_sp_clicked();
    saveFlag=0;

    fp=fopen(qb.append(".csv"),"w");
    if(fp==NULL) return;

    fprintf(fp,"Spine_ID,Spine_Type,Area(micron^2),Length(micron),Neck_Length(micron),Avg_Neck_Width(micron),Min_Neck_Width(micron),Min_Head_Width(micron),Max_Head_Width(micron)\n");
    for(int i=1;i<pID;i++)
    {
        w=spine[i].w;
        h=spine[i].h;

        if(w>h) {int temp=w;w=h;h=temp;}

        if(spine[i].spine_flag=='1')
            fprintf(fp,"%d,%c,%f,%f,%f,%f,%f,%f,%f\n",spine[i].sp_id,spine[i].type,spine[i].pArea*0.0049,spine[i].length*0.07,
                    spine[i].neck_length*0.07,spine[i].avg_neckW*0.07,spine[i].min_neckW*0.07,spine[i].headW*0.07,spine[i].headWmax*0.07);
    }
    fclose(fp);


    /////////Writing annotation file/////////
    qb = only_filename;
    fp=fopen(qb.append(".txt"),"w");//modify for different files
    if(fp==NULL) return;

    fprintf(fp,"%d\n",pID);
    for(int i=1;i<pID;i++)
    {        
        fprintf(fp,"%c %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %f %f %c\n",
                spine[i].spine_flag, spine[i].sp_id,spine[i].seedX,spine[i].seedY,
                spine[i].pCGx,spine[i].pCGy,spine[i].pArea,spine[i].x1,spine[i].y1,spine[i].x2,spine[i].y2,
                spine[i].bCGx,spine[i].bCGy,spine[i].bCnt,
                spine[i].headX,spine[i].headY,spine[i].headW,spine[i].headWmax,spine[i].type);
    }

    fprintf(fp,"%d %d\n",M,N);
    for(int i=0;i<M;i++)
    {
        for(int j=0;j<N;j++)
        {
            fprintf(fp,"%c",symbol_bak[i][j]);
        }

        fprintf(fp,"\n");
        ui->progressBar->setValue((float)(100*i)/M);

    }
    fclose(fp);

    ui->progressBar->setValue(100);
}


void MainWindow::on_pushButton_delete_by_spine_number_clicked()
{
    int val1;
    char buffer[200];
    bool flag=false;

    val1=ui->lineEdit_sp_no->text().toInt();
    ui->lineEdit_sp_no->setText("");

    for(int i=1;i<pID;i++)
    {
        if(spine[i].sp_id==val1 && spine[i].spine_flag=='1')
        {
            sprintf(buffer,"Are you sure to delete the Spine entry %d?",val1);
            int reply=QMessageBox::warning(this, "Warning", buffer, QMessageBox::Cancel, QMessageBox::Ok);

            if (reply == QMessageBox::Cancel)  return;

            spine[i].spine_flag='0';//deleted spine
            delFloodFill(spine[i].seedY,spine[i].seedX);
            flag=true;
        }
        else if(spine[i].sp_id==val1 && spine[i].spine_flag=='0')
        {
            sprintf(buffer,"The Spine number %d has already been deleted",val1);
            QMessageBox::warning(this, "Warning", buffer, QMessageBox::Cancel);
            return;
        }
    }
    if(flag==false)
    {
        sprintf(buffer,"Wrong Spine number %d. Enter correct number",val1);
        QMessageBox::warning(this, "Warning", buffer, QMessageBox::Cancel);
        return;
    }

    unsigned short maxID=0;
    for(int i=1;i<pID;i++)
    {
        if(spine[i].sp_id>maxID && spine[i].spine_flag=='1')
        {
            maxID=spine[i].sp_id;
        }
    }
    pID=maxID+1;
    maxpID=pID;
    on_pushButton_show_all_marked_sp_clicked();
}

void MainWindow::on_lineEdit_sp_no_returnPressed()
{
     on_pushButton_delete_by_spine_number_clicked();
}

void MainWindow::on_pushButton_set_spid_clicked()
{
    unsigned int temp;
    int i;
    char buffer[256];
    bool flag=0;
    temp=ui->lineEdit_spid->text().toInt();
    for(i=1;i<maxpID;i++)
    {
        if(spine[i].sp_id==temp && spine[i].spine_flag=='1')
            flag=1;
    }
    if(flag==0)
    {
        if(spine[temp].spine_flag!='1')
            pID=temp;
        else
        {
            sprintf(buffer,"The Spine entry %d already used earlier, Enter a new ID",temp);
            QMessageBox::warning(this, "Warning", buffer, QMessageBox::Cancel);
        }
    }
    else
    {
        sprintf(buffer,"The Spine entry %d already exits, Enter a new ID",temp);
        QMessageBox::warning(this, "Warning", buffer, QMessageBox::Cancel);
        ui->lineEdit_spid->setText("");
    }
}

void MainWindow::on_checkBox_manualSeg_clicked()
{
    if (ui->checkBox_manualSeg->isChecked())
    {
        ui->mark_spines->setChecked(false);
        ui->checkBox_delSp->setChecked(false);
    }

}

void MainWindow::on_checkBox_joinSeg_clicked()
{
    if (ui->checkBox_joinSeg->isChecked())
    {
        ui->mark_spines->setChecked(false);
        ui->checkBox_delSp->setChecked(false);
        ui->checkBox_manualSeg->setChecked(false);
    }

}


/////////////Line Drawing Algorithm//////////////
int MainWindow::Bresenham(int x1,int y1,int x2,int y2, char ch)
{
        int lcnt=0;
        int delta_x(x2 - x1);

        signed char const ix((delta_x > 0) - (delta_x < 0));
        delta_x = std::abs(delta_x) << 1;

        int delta_y(y2 - y1);

        signed char const iy((delta_y > 0) - (delta_y < 0));
        delta_y = std::abs(delta_y) << 1;

        symbol[(int)y1][(int)x1]=ch;
        lcnt++;

        if (delta_x >= delta_y)
        {
            // error may go below zero
            int error(delta_y - (delta_x >> 1));

            while (x1 != x2)
            {
                if ((error >= 0) && (error || (ix > 0)))
                {
                    error -= delta_x;
                    y1 += iy;
                }
                // else do nothing
                error += delta_y;
                x1 += ix;

                symbol[(int)y1][(int)x1]=ch;
                lcnt++;

            }
        }
        else
        {
            // error may go below zero
            int error(delta_x - (delta_y >> 1));

            while (y1 != y2)
            {
                if ((error >= 0) && (error || (iy > 0)))
                {
                    error -= delta_y;
                    x1 += ix;
                }
                // else do nothing

                error += delta_x;
                y1 += iy;

                symbol[(int)y1][(int)x1]=ch;
                lcnt++;

            }
        }
        return lcnt;
}

void MainWindow::on_lneZoom_lostFocus()
{
    bool ok;
    scaleSS=ui->lneZoom->text().toFloat(&ok);
    ui->hsbZoom->setValue((int)(scaleSS*10));
}

void MainWindow::on_lneZoom_returnPressed()
{
    bool ok;
    int t;
    float f;
    scaleSS=ui->lneZoom->text().toFloat(&ok);
    ui->hsbZoom->setValue((int)(scaleSS*10));    
}

void MainWindow::on_hsbZoom_valueChanged(int value)
{
    scaleSS=(float)value/10;
    ui->lneZoom->setText(str.setNum(scaleSS));

    QImage img(filepath);
    img.load(filename);
    ui->label_img->setPixmap(QPixmap::fromImage(save_img).scaled(ui->label_img->width()*scaleSS/lastscaleSS,ui->label_img->height()*scaleSS/lastscaleSS));
    lastscaleSS=scaleSS;
}



