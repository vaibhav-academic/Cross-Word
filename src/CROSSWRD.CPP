//Cross Word Program
//Made by Vaibhav Jain & Iram Sheikh
//S.G.S.I.T.S Indore
#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <graphics.h>
#include <ctype.h>
#include <string.h>
#define LEFTBUTTON 1
#define RIGHTBUTTON 2

#pragma warn -inl

int exitFlag=0; // infors the main loop to exit
int winFlag=0;// waits when user has completed the puzzle

//*************************************************************************/
// just for the initilization of the graphics subsystem
class Dummy
{
public:
 Dummy()
	{int gd=DETECT,gm;
	 initgraph(&gd,&gm,"c:\\tc\\bgi");
	 randomize();
	}
}dmw;

//*************************************************************************/

struct Direction
	{ int dx,dy;
	  Direction(int ddx=0,int ddy=0){dx=ddx;dy=ddy;}
	  enum directions{NONE=-1,EAST,NORTHEAST,NORTH,NORTHWEST,WEST,SOUTHWEST,SOUTH,SOUTHEAST};
	  int operator ==(Direction & d)
		  { return dx==d.dx && dy==d.dy;}
	  Direction operator *(int i)
		  {return Direction (dx*i,dy*i);}
	  Direction(enum directions d)
	  {switch(d)
		{case NONE:dx=0,dy=0;break;
		 case EAST: dx=1,dy=0;break;
		 case NORTHEAST: dx=1,dy=-1;break;
		 case NORTH: dx=0,dy=-1;break;
		 case NORTHWEST: dx=-1,dy=-1;break;
		 case WEST: dx=-1,dy=0;break;
		 case SOUTHWEST:dx=-1,dy=1;break;
		 case SOUTH: dx=0,dy=+1;break;
		 case SOUTHEAST: dx=1,dy=1;break;
		}
	  }
	 int getdy(enum directions d)
		{switch(d)
		{case NONE:return 0;
		 case EAST: return 0;
		 case NORTHEAST: return -1;
		 case NORTH: return -1;
		 case NORTHWEST: return -1;
		 case WEST: return 0;
		 case SOUTHWEST:return  1;
		 case SOUTH: return 1;
		 case SOUTHEAST: return 1;
		}
		return 0;
		}

	 int getdx(enum directions d)
		{switch(d)
		{case NONE:return 0;
		 case EAST: return 1;
		 case NORTHEAST: return 1;
		 case NORTH: return 0;
		 case NORTHWEST: return -1;
		 case WEST: return -1;
		 case SOUTHWEST:return -1;
		 case SOUTH: return 0;
		 case SOUTHEAST:return 1;
		}
		 return 1;
		}

	 enum directions getDirection()
		{ if(dx<0)
			{if(dy<0) return NORTHWEST;
			 if(dy==0) return WEST;
			 if(dy>0) return SOUTHWEST;
			}
		  if(dx==0)
			{if(dy<0) return NORTH;
			 if(dy==0) return NONE;
			 if(dy>0) return SOUTH;
			}
		  if(dx>0)
			{if(dy<0) return NORTHEAST;
			 if(dy==0) return EAST;
			 if(dy>0) return SOUTHEAST;
			}
		  return NONE;
		}
	  Direction & operator++()
		{ enum directions i=(enum directions)(getDirection()+1);
		   if(i>SOUTHEAST) i=EAST;
		  dx=getdx(i); dy=getdy(i);
		 return *this;
		}
	};
//*************************************************************************/
struct Rect
{int x1,y1,x2,y2;
 Rect(){x1=y1=x2=y2=0;}
 Rect(int x1,int y1,int x2,int y2)
     {this->x1=x1;this->x2=x2;this->y1=y1;this->y2=y2;}
};

struct Point
{int x,y;
 int bx,by;
 Point(int xx=0,int yy=0){x=xx,y=yy;}
 int operator==(Point & p)
 {return p.x==x && p.y==y;}
 void setBounds(int bbx,int bby)
	{bx=bbx;by=bby;}
 Point & operator ++()
	{ x++;
	  if(x>=bx) x=0,y++;
	  if(y>=by) y=0;
	  return *this;
	}
};
//*************************************************************************/
class CharMatrix
{ char * arr;int row,col;
  public:
   CharMatrix(int row,int col)
   { this->row=row;this ->col=col;
     arr=new char[row*col];
     memset(arr,0,row*col);
   }
   char & getElement(int r,int c)//todo:: check for dimensions
   { return arr[r*col+c];}

   char & operator()(int r,int c)
   {return getElement(r,c);}

   int getRows(){return row;}
   int getCols(){return col;}

   void show()
	{cout<<endl;
	for(int i=0;i<row;i++)
		{for(int j=0;j<col;j++)
			cout<<getElement(i,j)<<" ";
		 cout<<endl;
		}
	}

 virtual ~CharMatrix()
	{delete arr;}
};
//*************************************************************************/
class Dictionary
{ char words[255][10];
  char selected[255];
  int count;
 public:
  Dictionary(char *fname)
  { ifstream fis(fname);
    memset(words,0,sizeof(words));
    memset(selected,0,sizeof(selected));
    for(count=0;count<255&&!fis.eof();count++)
	 {fis>>words[count];
	  if(isPresent(words[count])) count--;
	  else  strupr(words[count]);
	 }
  }

  int getCount(){return count;}
  char * getWord(int i)
  {  if(i<0 || i>=count) {cerr<<"Invalid Word index";return NULL;}
      return words[i];
  }

  int isPresent(char * wrd)
  { for(int i=0;i<count ;i++)
     if(strcmpi(wrd,words[i])==0) return 1;
     return 0;
  }
  int getIndex(char *wrd)
  {for(int i=0;i<count ;i++)
     if(strcmpi(wrd,words[i])==0) return i;
   return -1;
  }
  int countSelected()
  {int c=0;
  for(int i=0;i<count;i++)
   c+=selected[i];
   return c;
  }

  int isSelected(int i) {return selected[i];}
  void setSelected(int i){selected[i]=1;}
};
//*************************************************************************/
class PuzzleMatrix:public CharMatrix
{Dictionary &dic;
 char curstrng[11];
    Point pts[10];int nxtpt;
    Direction curdir;
  struct
  {Point p;
   Direction d;
   char word[11];
  }Words[255];int wrdcount;
 int lst;
 public:
 PuzzleMatrix(Dictionary & d):CharMatrix(15,15),dic(d)
 {memset(curstrng,0,sizeof(curstrng));
  memset(pts,0,sizeof(pts));nxtpt=0;
  wrdcount=0;lst=-1;
  for(int i=0;i<d.getCount();i++)
	{char * str=d.getWord(i);
	 Point p(random(15),random(15)),pi(p); // startting point to be random
	 pi.setBounds(15,15);
	 char brkpi=0,brkdi;
	 for(++pi;brkpi==0;++pi)
		{
		  Direction d(Direction::SOUTHEAST);
		  Direction di(d);
		  brkdi=0;
		  if(pi==p) brkpi=1;

		  for(++di;brkdi==0;++di)
		  { if(di==d) brkdi=1;
		    for(int i=0;i<strlen(str);i++)
			if(canBeAdjusted(str,i,pi,di))
				{copyString(str,i,pi,di);
				 brkdi=brkpi=1;
				 break;
		  }		}



		}
	}
 }
 int canBeAdjusted(char * str,int ii,Point p, Direction&  d)
 {   Direction dd=d*ii;
     p.x+=dd.dx;
     p.y+=dd.dy;
     for(int i=0;str[i]!=0;i++)
	{if( (p.x< 0 || p.x>=15 ) || (p.y< 0 || p.y>=15 ) ) return 0;
	 if(getElement(p.x,p.y)!=0 && str[i]!=getElement(p.x,p.y)) return 0;
	 p.x+=d.dx;
	 p.y+=d.dy;
	}
     return 1;
 }

 void copyString(char * str,int ii,Point p, Direction & d)
 {  Direction dd=d*ii;
    strcpy(Words[wrdcount].word,str);

    Words[wrdcount].d=d;


     p.x+=dd.dx;
     p.y+=dd.dy;
     Words[wrdcount].p=p;
     for(int i=0;str[i]!=0;i++)
	{getElement(p.x,p.y)=str[i];
	 p.x+=d.dx;
	 p.y+=d.dy;
	}
   wrdcount++;
 }

int disclose()
 {
  for(lst++ ; lst<=wrdcount; lst++)
   {if( dic.isSelected(dic.getIndex(Words[lst].word)) ) continue;
    Point p= Words[lst].p;
    Direction d=Words[lst].d;
    for(int i=0;Words[lst].word[i]!=0;i++)
     { select(p.x,p.y);
       p.x+=d.dx;
       p.y+=d.dy;
     }

    break;
   }
   return lst<wrdcount;
 }

 void clear()
 {nxtpt=0;
  curstrng[0]=0;
 }

 /* return 0 is not selected, 1 if selected , 2 if selected and tail*/
 int isSelected(int r,int c)
 {Point p(r,c);
  if(nxtpt==0) return 0;
  if(pts[nxtpt-1]==p ) return 2;
  for(int i=0;i<nxtpt;i++)
  if (pts[i]==p) return 1;
  return 0;
 }

 char *getCurString()
 {for(int i=0;i<nxtpt;i++)
   curstrng[i]=getElement(pts[i].x,pts[i].y);
  curstrng[i]=0;
  return curstrng;
 }


 void toggle(int r,int c)
 {Point p(r,c);
  if(getElement(r,c)==0) return;
   for(int i=0;i<nxtpt;i++)
	if(pts[i]==p) break;
   if(i==nxtpt) select(r,c);
   else unselect(r,c);
 }

 int unselect(int r,int c)
 {Point p(r,c);
  if( nxtpt>0&&p==pts[nxtpt-1] ) {nxtpt--; return 1;}
  return 0;
 }

 int select(int r,int c)
 {Point p(r,c);
  if(nxtpt==0) {pts[0]=p;nxtpt++;return 1;}
  Direction d=Direction(p.x-pts[nxtpt-1].x,p.y-pts[nxtpt-1].y);
  if(nxtpt==1) {pts[1]=p;nxtpt++; curdir=d;return 1;}
  if(d==curdir) {pts[nxtpt++]=p;return 1;}
  return 0;
 }

};
/**************************************************************************/
/**************************************************************************/
class Window
{protected:
Rect rect;
Window(Rect r):rect(r){}
public:
virtual void setRect(Rect & r){ rect=r;}
virtual Rect getRect(){ return rect;}
virtual void click(int,int){}
virtual void draw() {}
};
/**************************************************************************/
/**************************************************************************/
class CommandButton:public Window
{char caption[40];
 int colfg,colbg;
 public:

 CommandButton(Rect r=Rect(0,0,0,0),char *ch=NULL ):Window(r)
 {//cout<<"CommandButton";
  strcpy(caption,ch);
  colfg=BLACK;
  colbg=WHITE;
 }
 int getFG(){return colfg;}
 int getBG(){return colbg;}
 void setFG(int c){colfg=c;}
 void setBG(int c){colbg=c;}

 char * getCaption(){return caption;}
 void setCaption(char *cp){strcpy(caption,cp);}
 void draw()
 {int precol=getcolor();
  setfillstyle(SOLID_FILL,colbg);
  bar(rect.x1+1,rect.y1+1,rect.x2-1,rect.y2-1);
  setcolor(colfg);
  if(caption !=NULL)outtextxy((rect.x1+rect.x2 -textwidth(caption))/2,
	(rect.y1+rect.y2-textheight(caption))/2,caption );
  setcolor(precol);
 }

};

/**************************************************************************/
/**************************************************************************/
class TitleBar:public Window
{char txt[20];
 int colfg,colbg;
 class tagcmdclose:public CommandButton
	{public:
	 void click(int,int)
		 {exitFlag=1;}
	}cmdclose;
 public:
 TitleBar(int height,char *t=NULL):Window(Rect(0,0,getmaxx(),height))
 {//cout<<"Titlebar";
  strcpy(txt,t);
  colbg=GREEN;
  colfg=BLUE;
  cmdclose.setRect(Rect(getmaxx()-height-3,3,getmaxx()-3,height-3 ));
  cmdclose.setCaption("x");
 }
 void setText(char *t)
 {strcpy(txt,t);}
 char *getText()
 {return txt;}

 void draw()
 {setfillstyle(SOLID_FILL,colbg);
  bar(rect.x1,rect.y1,rect.x2,rect.y2);
  if(txt!=NULL) outtextxy(rect.x1, rect.y1 + textheight(txt)/2,txt);
  cmdclose.draw();
 }
 void click(int x,int y)
 {Rect r=cmdclose.getRect();
 if( x >= r.x1 && x<=r.x2 && y>=r.y1 && y<=r.y2)
	       cmdclose.click(x,y);
 }
}tbr(15,"Cross Word");
/**************************************************************************/
/**************************************************************************/
class CheckBox:public Window
{int r,c;
 PuzzleMatrix * mt;
public:
    CheckBox():Window(Rect(0,0,0,0))
	    {mt=NULL;r=c=0;}

    CheckBox(Rect r):Window(r)
	{}
    void setPuzzleElement(PuzzleMatrix *mmt,int rr,int cc)
	{r=rr;c=cc;mt=mmt;}
    void draw()
	{rectangle(rect.x1,rect.y1,rect.x2,rect.y2);
	 char buff[2]={0};
	 buff[0]=toupper(mt->getElement(r,c));
	 switch(mt->isSelected(r,c))
	 {case 0:setfillstyle(SOLID_FILL,BLACK);break;
	  case 1:setfillstyle(SOLID_FILL,GREEN);break;
	  case 2:setfillstyle(SOLID_FILL,RED);break;
	 }
	 bar(rect.x1+1,rect.y1+1,rect.x2-1,rect.y2-1);
	 outtextxy((rect.x1+rect.x2)/2,(rect.y1+rect.y2)/2,buff);
	}
   void click(int,int)
	{mt->toggle(r,c);
	 tbr.setText(mt->getCurString());
	}
};/**/
/**************************************************************************/
/**************************************************************************/

class PuzzleGrid:public Window
{PuzzleMatrix & pm;
 CheckBox boxes[15][15];
 public:
 PuzzleGrid(Rect r,PuzzleMatrix &pz):Window(r),pm(pz)
 { //rectangle(r.x1,r.y1,r.x2,r.y2);
   for(int i=0;i<15;i++)
    for(int j=0;j<15;j++)
     { //boxes[i][j].setText(pm(i,j));
       boxes[i][j].setRect( Rect( r.x1+ 28*j ,r.y1+ 28*i , r.x1+ 28*(j+1),
			    r.y1+ 28*(i+1)) );
       boxes[i][j].setPuzzleElement(&pm,i,j);
      // boxes[i][j].draw();
     }
 }
 void click(int x,int y)
 {for(int i=0;i<15;i++)
  for(int j=0;j<15;j++)
   { Rect r= boxes[i][j].getRect();
     if( x >= r.x1 && x<=r.x2 && y>=r.y1 && y<=r.y2)
	       {boxes[i][j].click(x,y);break;}
   }
 }

 void draw()
 {for(int i=0;i<15;i++)
  for(int j=0;j<15;j++)
   boxes[i][j].draw();
 }

};
/**************************************************************************/
/**************************************************************************/
class WordList:public Window
{Dictionary & dic;
int starti;
public:
 WordList(Dictionary & d , Rect &r):Window(r),dic(d)
	{starti=0;}
 void scrollDown()
 {starti++;
  starti%=dic.getCount();
 }
 void scrollUp()
 {if(starti>0)starti--;}

 void draw()
 {int y=rect.y1+10;
  setfillstyle(SOLID_FILL,BLACK);
  bar(rect.x1,rect.y1,rect.x2,rect.y2);
  rectangle(rect.x1,rect.y1,rect.x2,rect.y2);
  for(int i=starti;i<dic.getCount() && y+textheight(dic.getWord(i))
	 < rect.y2 ;i++)
	if(dic.isSelected(i))
	{outtextxy(rect.x1+5,y,dic.getWord(i));
	 line(rect.x1+5,y+textheight(dic.getWord(i))/2
	 ,rect.x2,y+textheight(dic.getWord(i))/2);
	 y+=textheight(dic.getWord(i))+5;
	}
 }
};
/**************************************************************************/
/**************************************************************************/
class ScrollBar:public Window
{WordList & wlst;
 class tagcmdDown:public CommandButton
	{ScrollBar & sc;
	 public:
	   tagcmdDown(ScrollBar & s):sc(s)
		{Rect r=sc.getRect();
		 setCaption("\0x1E");
		 setRect(Rect( r.x1,r.y2-(r.x2-r.x1),r.x2,r.y2));
		 setFG(BLACK);
		 setBG(WHITE);
		}
	}cmddown;

 class tagcmdUp:public CommandButton
	{ScrollBar & sc;
	 public:
	   tagcmdUp(ScrollBar & s):sc(s)
		{Rect r=sc.getRect();
		 setFG(BLACK);
		 setBG(WHITE);
		 setCaption("\0x1F");
		 setRect(Rect( r.x1,r.y1,r.x2,r.y1+(r.x2-r.x1)));
		}
	}cmdup;

public:
    ScrollBar(Rect & r,WordList & w):Window(r),wlst(w),cmdup(*this),cmddown(*this)
    {}
    void draw()
    {setfillstyle(HATCH_FILL,LIGHTGRAY);
     bar(rect.x1,rect.y1,rect.x2,rect.y2);
     rectangle(rect.x1,rect.y1,rect.x2,rect.y2);
     cmdup.draw();
     cmddown.draw();
    }

    void click(int x,int y)
    {Rect r=cmddown.getRect();
     if( x >= r.x1 && x<=r.x2 && y>=r.y1 && y<=r.y2)
	       {wlst.scrollDown();return;}
     r=cmdup.getRect();
     if( x >= r.x1 && x<=r.x2 && y>=r.y1 && y<=r.y2)
	       {wlst.scrollUp();return;}

    }
};
/**************************************************************************/
/**************************************************************************/
Dictionary d("g:\\cd\\words.txt");
PuzzleMatrix pm(d);
/**************************************************************************/
PuzzleGrid pg(Rect(0,20,420,430),pm);
WordList wls(d,Rect(440,20, getmaxx()-20, 430));
ScrollBar sbr(Rect(getmaxx()-17,20,getmaxx()-3,430), wls);
/**********************************************************************/
CommandButton distot(Rect(440,430,getmaxx()-20,450),"Matched:0");
/**********************************************************************/
class tagcmdClear:public CommandButton
{ public:
       tagcmdClear()
       {setCaption("Clear");
	setRect(Rect(170,450,250,470));
       }
      void click(int,int)
	{pm.clear();
	 tbr.setText("Cross Word");
	}
}cmdClear;
/**********************************************************************/
class tagcmdCheck:public CommandButton
{ public:
   char clearflag,nowinflag;
       tagcmdCheck()
       {setCaption("Check");
	setRect(Rect(65,450,145,470));
	clearflag=nowinflag=0;
       }
      void click(int,int)
      { char * curword=pm.getCurString();
	if(d.isPresent(curword))
	{int index= d.getIndex(curword);
	 if(d.isSelected(index))
		{
		tbr.setText("Already Selected");
		return;
		}
	 d.setSelected(index);
	 if(clearflag==0)cmdClear.click(0,0);
	 else clearflag=0;
	 char buffer[30];
	 sprintf(buffer,"Matched:%d/%d",d.countSelected(),d.getCount());
	 distot.setCaption(buffer);
	 if(d.countSelected()==d.getCount())
	   if(nowinflag==0)
		 {winFlag=1;exitFlag=1;tbr.setText("You Win!!");}
	   else  {nowinflag=0;
		  winFlag=1;exitFlag=1;
		  tbr.setText("All Words Disclosed");
		 }
	}
      }
}cmdCheck;
/**********************************************************************/
class tagcmdDisclose:public CommandButton
{ public:
       tagcmdDisclose()
       {setCaption("Disclose");
	setRect(Rect(275,450,355,470));
       }
      void click(int,int)
      { cmdClear.click(0,0);
       int ret=pm.disclose();
       if(ret==1) {cmdCheck.clearflag=1;
		   cmdCheck.nowinflag=1;
		   cmdCheck.click(0,0);
		   }
      }
}cmdDisclose;
/**********************************************************************/
/*************************************************************************/
Window * wnds [] ={& pg, &tbr, & cmdCheck, & cmdClear, & cmdDisclose,
		   & wls,&sbr,&distot};
/************************************************************************/
void initMouse()
{asm{ mov ax,0x0;
      int 0x33;
      mov ax,1;
      int 0x33;
    }
}
unsigned getMouseButtons()
{unsigned  a;
 asm{mov ax,0x3;
     int 0x33;
     mov word ptr a, bx;
    }
return a;
}

Point getMousePos()
{unsigned x,y;
 asm{mov ax,0x3;
    int 0x33;
    mov word ptr x,cx;
    mov word ptr y,dx;
    }
 return Point(x,y);
}

void drawWindows()
{for(int i=0;i<sizeof(wnds)/sizeof(wnds[0]);i++)
	wnds[i]->draw();
}
void showMouse()
{asm{mov ax,1;
     int 0x33;
    }
}

void hideMouse()
{asm{mov ax,2;
     int 0x33;
     }
}

void main()
{ initMouse();
  hideMouse();
  drawWindows();
  while(!exitFlag)
  {showMouse();
   delay(100);
   int but=getMouseButtons();
   if(but&LEFTBUTTON)
      {Point p=getMousePos();
       for(int i=0;i<sizeof(wnds)/sizeof(wnds[0]);i++)
	 { Rect r=wnds[i]->getRect();
	  if( p.x >= r.x1 && p.x<=r.x2 && p.y>=r.y1 && p.y<=r.y2)
	       {wnds[i]->click(p.x,p.y);break;}
	 }
       hideMouse();drawWindows();
      }
  }/**/
 if (winFlag) getch();
}
