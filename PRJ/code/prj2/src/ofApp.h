#ifndef _OFAPP_H_
#define _OFAPP_H_

#include "ofMain.h"
#include "polygon.h"
#include <windows.h>
using namespace std;

class ofApp : public ofBaseApp
{

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	int screenWidth = ofGetScreenWidth();
	int screenHeight = ofGetScreenHeight();
	int windowWidth = 1200;
	int windowHeight = 900;

	bool bAddingPointsMode; /* variable to track the point input mode status */
	bool prev_bAddingPointsMode;

	vector<POINT2D> currentPoints; /* vector to store the current input points */
	vector<POLYGON> polygons;	   /* vector to store all polygons */

	char field[90][120];		  /* block field */
	bool BFSMode;				  /* variable to track the BFS mode status */
	pair<int, int> prev[90][120]; /* vector to store previous path */
	int BFSX, BFSY;				  /* BFS starting point */
	int dstX, dstY;				  /* BFS destination point */

	void set_field();
	void print_field();
	void traceBack(int, int);

	ofSoundPlayer perc, notify, loop, score, coin; /* sounds */
};

#endif /* _OFAPP_H_ */