#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::flow_right(int r,int c,int f)
{

    if(symbol[r][c]=='1'||
        symbol[r][c]=='>'||
        symbol[r][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||c>endP.x())
        return;

    if(symbol[r][c]=='<')
        symbol[r][c]='=';
    else
        symbol[r][c]='>';

    flow_right(r,c+1,0);
    flow_right(r-1,c+1,0);
    flow_right(r+1,c+1,0);
}

void MainWindow::flow_left(int r,int c,int f)
{
    if(symbol[r][c]=='1'||
        symbol[r][c]=='<'||
        symbol[r][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()|c<startP.x())
        return;

    if(symbol[r][c]=='>')
        symbol[r][c]='=';
    else
        symbol[r][c]='<';

    flow_left(r,c-1,0);
    flow_left(r-1,c-1,0);
    flow_left(r+1,c-1,0);
}


void MainWindow::flow_up(int r,int c,int f)
{

    if(symbol[r][c]=='1'||
        symbol[r][c]=='>'||
        symbol[r][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||r<endP.y())
        return;

    if(symbol[r][c]=='<')
        symbol[r][c]='=';
    else
        symbol[r][c]='>';

    flow_up(r-1,c,0);
    flow_up(r-1,c-1,0);
    flow_up(r-1,c+1,0);
}

void MainWindow::flow_down(int r,int c,int f)
{
    if(symbol[r][c]=='1'||
        symbol[r][c]=='<'||
        symbol[r][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||r>startP.y())
        return;

    if(symbol[r][c]=='>')
        symbol[r][c]='=';
    else
        symbol[r][c]='<';

    flow_down(r+1,c,0);
    flow_down(r+1,c-1,0);
    flow_down(r+1,c+1,0);
}


void MainWindow::flow_Dright(int r,int c,int f)
{

    if(symbol[r][c]=='1'||
        symbol[r][c]=='>'||
        symbol[r][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||c>endP.x())
        return;

    if(symbol[r][c]=='<')
        symbol[r][c]='=';
    else
        symbol[r][c]='>';

    flow_Dright(r,c+1,0);//D right

    if (startP.y()>endP.y())
    {
        flow_Dright(r-1,c+1,0);//D right and up
        flow_Dright(r-1,c,0);//D right and up
    }
    else
    {
        flow_Dright(r+1,c+1,0);//D right and up
        flow_Dright(r+1,c,0);//D right and up
    }
}



void MainWindow::flow_Dleft(int r,int c,int f)
{
    if(symbol[r][c]=='1'||
        symbol[r][c]=='<'||
        symbol[r][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||c<startP.x())
        return;

    if(symbol[r][c]=='>')
        symbol[r][c]='=';
    else
        symbol[r][c]='<';

    flow_Dleft(r,c-1,0);//D left

    if (startP.y()>endP.y())
    {
        flow_Dleft(r+1,c-1,0);//D left and down
        flow_Dleft(r+1,c,0);//D left and down
    }
    else
    {
        flow_Dleft(r-1,c-1,0);//D left and down
        flow_Dleft(r-1,c,0);//D left and down
    }
}

//=======================
//FLOW-27
//=======================

void MainWindow::flow_right27(int r,int c,int f)
{

    if(symbol[r][c]=='1'||symbol[r][c+1]=='1'||
        symbol[r][c]=='>'||symbol[r][c+1]=='>'||
        symbol[r][c]=='='||symbol[r][c+1]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||c>endP.x())
        return;

    if(symbol[r][c]=='<')
    {
        symbol[r][c]='=';
        symbol[r][c+1]='=';

    }
    else
    {
        symbol[r][c]='>';
        symbol[r][c+1]='>';
    }

    flow_right27(r,c+2,0);
    flow_right27(r-1,c+2,0);
    flow_right27(r+1,c+2,0);
}

void MainWindow::flow_left27(int r,int c,int f)
{
    if(symbol[r][c]=='1'||symbol[r][c-1]=='1'||
        symbol[r][c]=='<'||symbol[r][c-1]=='<'||
        symbol[r][c]=='='||symbol[r][c-1]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||c<startP.x())
        return;

    if(symbol[r][c]=='>')
    {
        symbol[r][c]='=';
        symbol[r][c-1]='=';
    }
    else
    {
        symbol[r][c]='<';
        symbol[r][c-1]='<';
    }

    flow_left27(r,c-2,0);
    flow_left27(r-1,c-2,0);
    flow_left27(r+1,c-2,0);
}


void MainWindow::flow_up27(int r,int c,int f)
{

    if(symbol[r][c]=='1'||symbol[r-1][c]=='1'||
        symbol[r][c]=='>'||symbol[r-1][c]=='>'||
        symbol[r][c]=='='||symbol[r-1][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||r<endP.y())
        return;

    if(symbol[r][c]=='<')
    {
        symbol[r][c]='=';
        symbol[r-1][c]='=';
    }
    else
    {
        symbol[r][c]='>';
        symbol[r-1][c]='>';
    }

    flow_up27(r-2,c,0);
    flow_up27(r-2,c+1,0);
    flow_up27(r-2,c-1,0);
}

void MainWindow::flow_down27(int r,int c,int f)
{
    if(symbol[r][c]=='1'||symbol[r+1][c]=='1'||
        symbol[r][c]=='<'||symbol[r+1][c]=='<'||
        symbol[r][c]=='='||symbol[r+1][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||r>startP.y())
        return;

    if(symbol[r][c]=='>')
    {
        symbol[r][c]='=';
        symbol[r+1][c]='=';
    }
    else
    {
        symbol[r][c]='<';
        symbol[r+1][c]='<';
    }

    flow_down27(r+2,c,0);
    flow_down27(r+2,c-1,0);
    flow_down27(r+2,c+1,0);
}


void MainWindow::flow_Dright27(int r,int c,int f)
{

    if(symbol[r][c]=='1'||
        symbol[r][c]=='>'||
        symbol[r][c]=='='||
        r<=startROI.y()+1||r>=endROI.y()-1||c<=startROI.x()+1||c>=endROI.x()-1||c>endP.x())
        return;

    if(symbol[r][c]=='<')
    {
        symbol[r][c]='=';
    }
    else
    {
        symbol[r][c]='>';
    }

    if (startP.y()>endP.y())
    {
        flow_Dright27(r-1,c+1,0);//D right and up
        flow_Dright27(r-2,c+1,0);//D right and up
        flow_Dright27(r-1,c+2,0);//D right and up
    }
    else
    {
        flow_Dright27(r+1,c+1,0);//D right and up
        flow_Dright27(r+2,c+1,0);//D right and up
        flow_Dright27(r+1,c+2,0);//D right and up
    }
}



void MainWindow::flow_Dleft27(int r,int c,int f)
{
    if(symbol[r][c]=='1'||
        symbol[r][c]=='<'||
        symbol[r][c]=='='||
        r<=startROI.y()+1||r>=endROI.y()-1||c<=startROI.x()+1||c>=endROI.x()-1||c<=startP.x())
        return;

    if(symbol[r][c]=='>')
    {
        symbol[r][c]='=';
    }
    else
    {
        symbol[r][c]='<';
    }

    if (startP.y()>endP.y())
    {
        flow_Dleft27(r+1,c-1,0);//D left and down
        flow_Dleft27(r+1,c-2,0);//D left and down
        flow_Dleft27(r+2,c-1,0);//D left and down
    }
    else
    {
        flow_Dleft27(r-1,c-1,0);//D left and down
        flow_Dleft27(r-1,c-2,0);//D left and down
        flow_Dleft27(r-2,c-1,0);//D left and down
    }
}


void MainWindow::flow_D1right27(int r,int c,int f)
{

    if(symbol[r][c]=='1'||
        symbol[r][c]=='>'||
        symbol[r][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||c>endP.x())
        return;

    if(symbol[r][c]=='<')
    {
        symbol[r][c]='=';
    }
    else
    {
        symbol[r][c]='>';
    }

    if (startP.y()>endP.y())
    {
        flow_D1right27(r-1,c+1,0);//D right and up
        flow_D1right27(r,c+1,0);//D right and up
    }
    else
    {
        flow_D1right27(r+1,c+1,0);//D right and up
        flow_D1right27(r,c+1,0);//D right and up

    }
}

void MainWindow::flow_D1left27(int r,int c,int f)
{
    if(symbol[r][c]=='1'||
        symbol[r][c]=='<'||
        symbol[r][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||c<startP.x())
        return;

    if(symbol[r][c]=='>')
    {
        symbol[r][c]='=';
    }
    else
    {
        symbol[r][c]='<';
    }

    if (startP.y()>endP.y())
    {
        flow_D1left27(r+1,c-1,0);//D left and down
        flow_D1left27(r,c-1,0);//D left and down

    }
    else
    {
        flow_D1left27(r-1,c-1,0);//D left and down
        flow_D1left27(r,c-1,0);//D left and down

    }
}


void MainWindow::flow_D2right27(int r,int c,int f)
{

    if(symbol[r][c]=='1'||
        symbol[r][c]=='>'||
        symbol[r][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||c>endP.x())
        return;

    if(symbol[r][c]=='<')
    {
        symbol[r][c]='=';
    }
    else
    {
        symbol[r][c]='>';
    }

    if (startP.y()>endP.y())
    {
        flow_D2right27(r-1,c,0);//D right and up
        flow_D2right27(r-1,c+1,0);//D right and up

    }
    else
    {
        flow_D2right27(r+1,c,0);//D right and up
        flow_D2right27(r+1,c+1,0);//D right and up
    }
}

void MainWindow::flow_D2left27(int r,int c,int f)
{
    if(symbol[r][c]=='1'||
        symbol[r][c]=='<'||
        symbol[r][c]=='='||
        r<=startROI.y()||r>=endROI.y()||c<=startROI.x()||c>=endROI.x()||c<startP.x())
        return;

    if(symbol[r][c]=='>')
    {
        symbol[r][c]='=';
    }
    else
    {
        symbol[r][c]='<';
    }

    if (startP.y()>endP.y())
    {
        flow_D2left27(r+1,c,0);//D left and down
        flow_D2left27(r+1,c-1,0);//D left and down
    }
    else
    {
        flow_D2left27(r-1,c,0);//D left and down
        flow_D2left27(r-1,c-1,0);//D left and down

    }
}
