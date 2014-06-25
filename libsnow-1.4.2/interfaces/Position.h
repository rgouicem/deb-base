#ifndef _POSITION_H_
#define _POSITION_H_

#include <string>
#include <utility>
using namespace std;


class Position {

  int x,y;
  double vx,vy;
public :

  Position (int xx=100,int yy=100): x(xx),y(yy) { vx=x/100 ; vy = y/100; }
  Position (double vvx,double vvy): vx(vvx),vy(vvy) { x =(int)  (vx*100) ; y = (int) (vy*100); }
  
  /* returns position vectorial style */
  pair<double,double> getposV () { return pair<double,double> (vx,vy); }
  pair<int,int> getposA () { return pair<int,int> (x,y); }

  /* returns position for printing */
  string getCamiPO(int num);
  string getCamiPT(int num,string tag);
  string getGspn();

};






#endif /* _POSITION_H_ */
