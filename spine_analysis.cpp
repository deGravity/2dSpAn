
#include "mainwindow.h"
#include "ui_mainwindow.h"



void MainWindow::spine_DT(int id)
{
    int i,j,min,temp,change=0,maxDT=0, neck_len,len1,basex,basey;

    sM=spine[id].h+4;//for outside padding
    sN=spine[id].w+4;//for outside padding

    DT  = ( int **)calloc(sizeof( int *) , sM);
    DT1  = ( int **)calloc(sizeof( int *) , sM);
    DT2  = ( int **)calloc(sizeof( int *) , sM);

    for (int i = 0 ;  i < sM; i++) {
        DT[i] = ( int *)calloc(sizeof( int) , sN);
        DT1[i] = ( int *)calloc(sizeof( int) , sN);
        DT2[i] = ( int *)calloc(sizeof( int) , sN);
    }

    //for finding spine DT
    for(int ii=0,i=spine[id].y1;i<=spine[id].y2;ii++,i++)
        for(int jj=0,j=spine[id].x1;j<=spine[id].x2;jj++,j++)
        {
            if(i==spine[id].bCGy && j==spine[id].bCGx)
            {
                basex=jj;basey=ii;
            }
            if(symbol[i][j]=='p')
                DT[ii][jj]=9999;
        }

    std::cout<<"SpineID:"<<id<<std::endl;
    compute_DT(DT,2);

    int maxi,maxj;
    for(int ii=0;ii<sM;ii++)
        for(int jj=0;jj<sN;jj++)
        {
            if(DT[ii][jj]==9999) continue;
            if (DT[ii][jj]>maxDT)
            {
                maxDT=DT[ii][jj];
                maxi=ii;
                maxj=jj;
            }
        }

    int maxDTcnt=0, minDTx=999,minDTy=999, maxDTx=-999,maxDTy=-999, CGDTx=0,CGDTy=0;

    float maxDT_Th=ui->doubleSpinBox_DT_Th->value()*10;

    for(int ii=0;ii<sM;ii++)
        for(int jj=0;jj<sN;jj++)
        {
            if(DT[ii][jj]==9999) continue;

            if (abs(DT[ii][jj]-maxDT)<maxDT_Th)
            {
                if(ii>maxDTx) maxDTx=ii;
                if(jj>maxDTy) maxDTy=jj;

                if(ii<minDTx) minDTx=ii;
                if(jj<minDTy) minDTy=jj;

                CGDTx+=ii;
                CGDTy+=jj;
                maxDTcnt++;                
            }
        }

    maxi=(float)CGDTx/maxDTcnt;
    maxj=(float)CGDTy/maxDTcnt;

    int delx=(maxDTx-minDTx);
    int dely=(maxDTy-minDTy);
    float radial_dist=sqrt(delx*delx+dely*dely);

    //spine DT with true background
    for(int ii=0;ii<sM;ii++)
        for(int jj=0;jj<sN;jj++)
            DT2[ii][jj]=8888;

    for(int ii=0,i=spine[id].y1;i<=spine[id].y2;ii++,i++)
        for(int jj=0,j=spine[id].x1;j<=spine[id].x2;jj++,j++)

        {
            if(symbol[i][j]=='p')
                DT2[ii][jj]=9999;

            if(symbol[i][j]=='1')
                DT2[ii][jj]=0;
        }


    compute_DT(DT2,2);

    //for finding farthest spine point
    for(int ii=0;ii<sM;ii++)
        for(int jj=0;jj<sN;jj++)
        {
            DT1[ii][jj]=9999;
        }

    for(int ii=0,i=spine[id].y1;i<=spine[id].y2;ii++,i++)
        for(int jj=0,j=spine[id].x1;j<=spine[id].x2;jj++,j++)
        {            
            if(i==spine[id].bCGy && j==spine[id].bCGx)
            {
                DT1[ii][jj]=0;
            }
            else
            if(symbol[i][j]=='p')
            {
                //DT[ii][jj]=9998;
                DT1[ii][jj]=9998;
            }
        }
    compute_DT1(maxDT+10);

    int maxi1,maxj1;
    int maxDT1=0;
    for(int ii=0;ii<sM;ii++)
        for(int jj=0;jj<sN;jj++)
        {
            if(DT1[ii][jj]==9999 || DT1[ii][jj]==9998) continue;

            if (DT1[ii][jj]>maxDT1)
            {
                maxDT1=DT1[ii][jj];
                maxi1=ii;
                maxj1=jj;
            }
        }

    spine[id].headX=spine[id].x1+maxj; spine[id].headY=spine[id].y1+maxi;
    spine[id].headW=(float)DT[maxi][maxj]/5;
    spine[id].headWmax=spine[id].headW+radial_dist;

    std::cout<<" Head_W="<<spine[id].headW;//<<std::endl;
    trace_cnt=0;
    neck_len=0;
    spine[id].avg_neckW=0;
    spine[id].min_neckW=9999;//large value
    trace_back(id,maxi,maxj, 1,0);//center of head to base_CG
    neck_len=trace_cnt-(float)spine[id].headW/2;

    std::cout<<" Base_W="<<spine[id].bCnt<<" Neck_len="<<neck_len;//<<std::endl;

    if(neck_len==0)
    {
        spine[id].type='s';
        spine[id].neck_length=0;
        spine[id].avg_neckW=0;
        spine[id].min_neckW=0;
        spine[id].length=((float)DT[maxi][maxj]/10);
    }
    else
    {
        //Regular spines (Mushroom or Filopodia)
        spine[id].type=(unsigned char)'r';//SPINE HEAD PROTRUSION by default
        spine[id].neck_length=(float)neck_len;
        spine[id].avg_neckW=(float)spine[id].avg_neckW/(neck_len*5);
        spine[id].min_neckW=(float)spine[id].min_neckW/5;        
    }

        //Regular spines
        DT1[basey][basex]=1;
        trace_cnt=0;
        trace_back(id,maxi1,maxj1, 1,1);//Farthest point to base_CG
        spine[id].length=trace_cnt;

        float sp_type;
        sp_type=ui->doubleSpinBox_sp_type->value();//Spine type threshold
        if((spine[id].length>spine[id].headW) &&
                (spine[id].length>(float)spine[id].bCnt/2) &&
                neck_len>0)
        {
            spine[id].type='r';//SPINE HEAD PROTRUSION

            if (spine[id].length<sp_type*(spine[id].headW+neck_len))
                //upto 25% extension is allowed in MUSHROOM
                spine[id].type='m';//MUSHROOM

            if(radial_dist>spine[id].headW)//Tube like structure with many points with maxDT value
                spine[id].type='f';//FILOPODIA
        }
        else
        {
            //STUBBY
            spine[id].type='s';
        }
    std::cout<<" Spine Length:"<<spine[id].length<<std::endl;

    free(DT);
    free(DT1);
}


void MainWindow::compute_DT(int **D, int DT_Flag)
{
    int i,j,min,temp,change=0;

    do{

    change=0;

    //Positive DT
    for(int ii=1;ii<sM-1;ii++)
        for(int jj=1;jj<sN-1;jj++)
        {
            if(D[ii][jj]==0) continue;

            min=D[ii][jj];

            temp=D[ii-1][jj-1]+14;
            if(temp<min) {D[ii][jj]=temp; min=temp;change++;}

            temp=D[ii-1][jj]  +10;
            if(temp<min) {D[ii][jj]=temp; min=temp;change++;}

            temp=D[ii-1][jj+1]+14;
            if(temp<min) {D[ii][jj]=temp; min=temp;change++;}

            temp=D[ii][jj-1]  +10;
            if(temp<min) {D[ii][jj]=temp; min=temp;change++;}
        }

    //Negative DT
    for(int ii=sM-2;ii>0;ii--)
        for(int jj=sN-2;jj>0;jj--)
        {
            if(D[ii][jj]==0) continue;

            min=D[ii][jj];

            temp=D[ii+1][jj+1]+14;
            if(temp<min) {D[ii][jj]=temp; min=temp;change++;}

            temp=D[ii+1][jj]  +10;
            if(temp<min) {D[ii][jj]=temp; min=temp;change++;}

            temp=D[ii+1][jj-1]+14;
            if(temp<min) {D[ii][jj]=temp; min=temp;change++;}

            temp=D[ii][jj+1]  +10;
            if(temp<min) {D[ii][jj]=temp; min=temp;change++;}
        }
    }while(change);
}

void MainWindow::compute_DT1(int maxDT)
{
    int i,j,min,temp,change=0;
    do{
    change=0;

    //Positive DT
    for(int ii=1;ii<sM-1;ii++)
        for(int jj=1;jj<sN-1;jj++)
        {
            if(DT1[ii][jj]==0) continue;
            if(DT1[ii][jj]==9999) continue;

            min=DT1[ii][jj];

            temp=DT1[ii-1][jj-1]+maxDT-DT[ii-1][jj-1]+14;
            if(temp<min) {DT1[ii][jj]=temp; min=temp;change++;}

            temp=DT1[ii-1][jj]  +maxDT-DT[ii-1][jj]+10;
            if(temp<min) {DT1[ii][jj]=temp; min=temp;change++;}

            temp=DT1[ii-1][jj+1]+maxDT-DT[ii-1][jj+1]+14;
            if(temp<min) {DT1[ii][jj]=temp; min=temp;change++;}

            temp=DT1[ii][jj-1]  +maxDT-DT[ii][jj-1]+10;
            if(temp<min) {DT1[ii][jj]=temp; min=temp;change++;}

        }

    //Negative DT
    for(int ii=sM-2;ii>0;ii--)
        for(int jj=sN-2;jj>0;jj--)
        {
            if(DT1[ii][jj]==0) continue;
            if(DT1[ii][jj]==9999) continue;

            min=DT1[ii][jj];

            temp=DT1[ii+1][jj+1]+maxDT-DT[ii+1][jj+1]+14;
            if(temp<min) {DT1[ii][jj]=temp; min=temp;change++;}

            temp=DT1[ii+1][jj]  +maxDT-DT[ii+1][jj]+10 ;
            if(temp<min) {DT1[ii][jj]=temp; min=temp;change++;}

            temp=DT1[ii+1][jj-1]+maxDT-DT[ii+1][jj-1]+14;
            if(temp<min) {DT1[ii][jj]=temp; min=temp;change++;}

            temp=DT1[ii][jj+1]  +maxDT-DT[ii][jj+1]+10;
            if(temp<min) {DT1[ii][jj]=temp; min=temp;change++;}

        }    
    }while(change);
}


void MainWindow::trace_back(int id, int r, int c, int StopDT, int flag)
//searching minimum value around
{
    int minDT=0, mini,minj;

    if (DT1[r][c]<=1) return;
    if (DT1[r][c]==9999) return;
    if (DT1[r][c]==StopDT) return;
    if(r<1 || r>sM-2 || c<1 || c>sN-2) return;

    minDT=DT1[r][c];
    DT1[r][c]=minDT*2;//to avoid backtracking

    if(DT1[r-1][c-1]<=minDT) {minDT=DT1[r-1][c-1];mini=r-1;minj=c-1;}
    if(DT1[r-1][c]  <=minDT) {minDT=DT1[r-1][c]  ;mini=r-1;minj=c;  }
    if(DT1[r-1][c+1]<=minDT) {minDT=DT1[r-1][c+1];mini=r-1;minj=c+1;}
    if(DT1[r][c-1]  <=minDT) {minDT=DT1[r][c-1]  ;mini=r;  minj=c-1;}
    if(DT1[r][c+1]  <=minDT) {minDT=DT1[r][c+1]  ;mini=r;  minj=c+1;}
    if(DT1[r+1][c-1]<=minDT) {minDT=DT1[r+1][c-1];mini=r+1;minj=c-1;}
    if(DT1[r+1][c]  <=minDT) {minDT=DT1[r+1][c]  ;mini=r+1;minj=c;  }
    if(DT1[r+1][c+1]<=minDT) {minDT=DT1[r+1][c+1];mini=r+1;minj=c+1;}

    trace_cnt++;

    if(flag==0 && trace_cnt>=((float)spine[id].headW/2))//center of head to base_CG
    {
        symbol[spine[id].y1+mini][spine[id].x1+minj]='y';
        spine[id].avg_neckW+=DT2[mini][minj];
        if(DT2[mini][minj]<spine[id].min_neckW)
            spine[id].min_neckW=DT2[mini][minj];
    }

    std::cout<<"trace_cnt="<<trace_cnt<<std::endl;
    trace_back(id,mini,minj, StopDT,flag);
}

void MainWindow::write_skel(int id)
{
    int i,j;

    FILE *fp;
    char filename[50],ch='A';

    sprintf(filename,"spines\\skel%03d.txt",id);
    fp=fopen(filename,"w");
    if (!fp) {
        printf("\nERROR writing file");
        return; }
    fprintf(fp,"%d %d\n",spine[id].w,spine[id].h);
    for(i=0;i<sM;i++)
    {
        for(j=0;j<sN;j++)
        {
            if(DT[i][j]>=8888)
                fprintf(fp,"....");//for testing
            else
                fprintf(fp,"%4d",DT[i][j]);//for testing

        }
        fprintf(fp,"\n");
    }
    fclose(fp);
    QImage spine_img((int)sN,(int)sM,QImage::Format_RGB32);
    QByteArray qb = "spines\\spine";
    spine_img.save(qb.append(id).append(".png"),"PNG",-1);
}


