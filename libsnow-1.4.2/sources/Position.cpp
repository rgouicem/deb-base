#include "Position.h"

#include <cstring>
#include <cstdio>



/* returns position for printing */
string Position::getCamiPO(int num) {
  char buff[256];
  sprintf(buff,"PO(%d,%d,%d)",num,x,y);
  return buff;
}

string Position::getCamiPT(int num,string tag) {
  char buff[256];
  sprintf(buff,"PT(%d,%zu:%s,%d,%d)",num,tag.size(),tag.c_str(),x,y);
  return buff;
}

string Position::getGspn() {
  char buff[256];
  sprintf(buff,"%f %f ",vx,vy);
  return buff;
}


