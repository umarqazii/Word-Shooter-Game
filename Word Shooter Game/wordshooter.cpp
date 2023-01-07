//============================================================================
// Name        : cookie-crush.cpp
// Author      : Sibt ul Hussain
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game of Cookie  Crush...
//============================================================================
/*
Name: Umar Qazi
Roll no: 20i-0968
Section: E
*/
#ifndef WORD_SHOOTER_CPP
#define WORD_SHOOTER_CPP

#include <GL/gl.h>
#include <GL/glut.h>
#include <iostream>
#include<string>
#include<cmath>
#include<fstream>
#include<cstdlib>
#include<ctime>
#include "util.h"
using namespace std;
#define MAX(A,B) ((A) > (B) ? (A):(B)) // defining single line functions....
#define MIN(A,B) ((A) < (B) ? (A):(B))
#define ABS(A) ((A) < (0) ? -(A):(A))
#define FPS 10

string * dictionary;
int dictionarysize = 370099;
#define KEY_ESC 27 // A

// 20,30,30
const int bradius = 30; // ball radius in pixels...

int width = 930, height = 660;
int byoffset = bradius;


int nxcells = (width - bradius) / (2 * bradius);
int nycells = (height - byoffset /*- bradius*/) / (2 * bradius);
int nfrows = 2; // initially number of full rows //
float score = 0;
int **board; // 2D-arrays for holding the data...
int bwidth = 130;
int bheight = 10;
int bsx, bsy;
const int nalphabets = 26;
enum alphabets {
	AL_A, AL_B, AL_C, AL_D, AL_E, AL_F, AL_G, AL_H, AL_I, AL_J, AL_K, AL_L, AL_M, AL_N, AL_O, AL_P, AL_Q, AL_R, AL_S, AL_T, AL_U, AL_W, AL_X, AL_y, AL_Z
};
GLuint texture[nalphabets];
GLuint tid[nalphabets];
string tnames[] = { "a.bmp", "b.bmp", "c.bmp", "d.bmp", "e.bmp", "f.bmp", "g.bmp", "h.bmp", "i.bmp", "j.bmp",
"k.bmp", "l.bmp", "m.bmp", "n.bmp", "o.bmp", "p.bmp", "q.bmp", "r.bmp", "s.bmp", "t.bmp", "u.bmp", "v.bmp", "w.bmp",
"x.bmp", "y.bmp", "z.bmp" };
GLuint mtid[nalphabets];
int awidth = 60, aheight = 60; // 60x60 pixels cookies...


//USED THIS CODE FOR WRITING THE IMAGES TO .bin FILE
void RegisterTextures_Write()
//Function is used to load the textures from the
// files and display
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);
	vector<unsigned char> data;
	ofstream ofile("image-data.bin", ios::binary | ios::out);
	// now load each cookies data...

	for (int i = 0; i < nalphabets; ++i) {

		// Read current cookie

		ReadImage(tnames[i], data);
		if (i == 0) {
			int length = data.size();
			ofile.write((char*)&length, sizeof(int));
		}
		ofile.write((char*)&data[0], sizeof(char) * data.size());

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ofile.close();

}
void RegisterTextures()
/*Function is used to load the textures from the
* files and display*/
{
	// allocate a texture name
	glGenTextures(nalphabets, tid);

	vector<unsigned char> data;
	ifstream ifile("image-data.bin", ios::binary | ios::in);

	if (!ifile) {
		cout << " Couldn't Read the Image Data file ";
		//exit(-1);
	}
	// now load each cookies data...
	int length;
	ifile.read((char*)&length, sizeof(int));
	data.resize(length, 0);
	for (int i = 0; i < nalphabets; ++i) {
		// Read current cookie
		//ReadImage(tnames[i], data);
		/*if (i == 0) {
		int length = data.size();
		ofile.write((char*) &length, sizeof(int));
		}*/
		ifile.read((char*)&data[0], sizeof(char)* length);

		mtid[i] = tid[i];
		// select our current texture
		glBindTexture(GL_TEXTURE_2D, tid[i]);

		// select modulate to mix texture with color for shading
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		// when texture area is small, bilinear filter the closest MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		// when texture area is large, bilinear filter the first MIP map
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if wrap is true, the texture wraps over at the edges (repeat)
		//       ... false, the texture ends at the edges (clamp)
		bool wrap = true;
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
			wrap ? GL_REPEAT : GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
			wrap ? GL_REPEAT : GL_CLAMP);

		// build our texture MIP maps
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, awidth, aheight, GL_RGB,
			GL_UNSIGNED_BYTE, &data[0]);
	}
	ifile.close();
}
void DrawAlphabet(const alphabets &cname, int sx, int sy, int cwidth = 60,
	int cheight = 60)
	/*Draws a specfic cookie at given position coordinate
	* sx = position of x-axis from left-bottom
	* sy = position of y-axis from left-bottom
	* cwidth= width of displayed cookie in pixels
	* cheight= height of displayed cookiei pixels.
	* */
{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mtid[cname]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
int GetAlphabet() {
	return GetRandInRange(1, 26);
}

void Pixels2Cell(int px, int py, int & cx, int &cy) {
}
void Cell2Pixels(int cx, int cy, int & px, int &py)
// converts the cell coordinates to pixel coordinates...
{
}
void DrawShooter(int sx, int sy, int cwidth = 60, int cheight = 60)

{
	float fwidth = (float)cwidth / width * 2, fheight = (float)cheight
		/ height * 2;
	float fx = (float)sx / width * 2 - 1, fy = (float)sy / height * 2 - 1;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, -1);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(fx, fy);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(fx + fwidth, fy);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(fx + fwidth, fy + fheight);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(fx, fy + fheight);
	glEnd();

	glColor4f(1, 1, 1, 1);

	//	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	//glutSwapBuffers();
}
/*
* Main Canvas drawing function.
* */


int num1;
int num2;
int num3;
int num4;
int num5;
int num6;
int num7;
int num8;
int num9;
int num10;
int num11;
int num12;
int num13;
int num14;
int num15;
int num16;
int num17;
int num18;
int num19;
int num20;
int num21;
int num22;
int num23;
int num24;
int num25;
int num26;
int num27;
int num29;
int num28;
int num30;
int num31;
int num32;
int num33;
int num34;
int num35;
int num36;
int num37;
int num38;
int num39;
int num40;
int num41;
int num42;
int num43;
int num44;
int num45;
int num46;
int num47;
int num48;
int num49;
int num50;
int timee=1200;
int loc1y=18;
int loc1x=465;
int loc2y=18;
int loc2x=465;
int loc3y=18;
int loc3x=465;
int loc4y=18;
int loc4x=465;
int loc5y=18;
int loc5x=465;
int loc6y=18;
int loc6x=465;
int loc7y=18;
int loc7x=465;
int loc8y=18;
int loc8x=465;
int loc9y=18;
int loc9x=465;
int loc10y=18;
int loc10x=465;
int loc11y=18;
int loc11x=465;
int loc12y=18;
int loc12x=465;
int loc13y=18;
int loc13x=465;
int loc14y=18;
int loc14x=465;
int loc15y=18;
int loc15x=465;
int loc16y=18;
int loc16x=465;
int startdisplay=0;
void DisplayFunction() {
	// set the background color using function glClearColor.
	// to change the background play with the red, green and blue values below.
	// Note that r, g and b values must be in the range [0,1] where 0 means dim red and 1 means pure red and so on.
	//#if 0
	srand(time(0));
	
	glClearColor(1/*Red Component*/, 1.0/*Green Component*/,
		1.0/*Blue Component*/, 0 /*Alpha component*/); // Red==Green==Blue==1 --> White Colour
	glClear(GL_COLOR_BUFFER_BIT); //Update the colors

	//write your drawing commands here or call your drawing functions...
	if(startdisplay==0){
	//Game Intro
	//word
	DrawAlphabet((alphabets)22, 10, height-120, awidth, aheight);
	DrawAlphabet((alphabets)14, 70, height -120, awidth, aheight);
	DrawAlphabet((alphabets)17, 130, height -120, awidth, aheight);
	DrawAlphabet((alphabets)3, 190, height -120, awidth, aheight);
	//shooter
	DrawAlphabet((alphabets)18, 250, height-180, awidth, aheight);
	DrawAlphabet((alphabets)7, 310, height -180, awidth, aheight);
	DrawAlphabet((alphabets)14, 370, height -180, awidth, aheight);
	DrawAlphabet((alphabets)14, 430, height -180, awidth, aheight);
	DrawAlphabet((alphabets)19, 490, height-180, awidth, aheight);
	DrawAlphabet((alphabets)4, 550, height -180, awidth, aheight);
	DrawAlphabet((alphabets)17, 610, height -180, awidth, aheight);
	//game
	DrawAlphabet((alphabets)6, 670, height -240, awidth, aheight);
	DrawAlphabet((alphabets)0, 730, height-240, awidth, aheight);
	DrawAlphabet((alphabets)12, 790, height -240, awidth, aheight);
	DrawAlphabet((alphabets)4, 850, height -240, awidth, aheight);
	
	//designing
	DrawAlphabet((alphabets)4, 115,115 , 20, 20);
	DrawAlphabet((alphabets)13, 890,600 , 30, 30);
	DrawAlphabet((alphabets)8, 850,90 , 25, 25);
	DrawAlphabet((alphabets)25, 650,200 , 40, 40);
	DrawAlphabet((alphabets)9, 600, 400 , 35, 35);
	DrawAlphabet((alphabets)22, 300, 620 , 25, 25);
	DrawAlphabet((alphabets)2, 350, 375 , 15, 15);
	DrawAlphabet((alphabets)0, -70,330 , 150, 150);
	DrawAlphabet((alphabets)5, 870,-20 , 100, 100);
	
	
	//DrawString(350, 400,  "Instructions:", colors[BLUE_VIOLET]);
	DrawString(250, 350, width, height + 5, "Instructions:" , colors[BROWN]);
	DrawString(235, 320, width, height + 5, "- Two rows of alphabets are already given" , colors[LIME_GREEN]);
	DrawString(235, 290, width, height + 5, "- Shooter will shoot an alphabet upwards" , colors[ROYAL_BLUE]);
	DrawString(235, 260, width, height + 5, "- Use arrow keys to move the alphabet left and right" , colors[PLUM]);
	DrawString(235, 230, width, height + 5, "- Goal is to make words using alphabets" , colors[ORANGE]);
	DrawString(235, 200, width, height + 5, "- Each word will result in a point" , colors[DARK_GOLDEN_ROD]);
	DrawString(200, 120, width, height + 5, ">>>>CLICK ANYWHERE TO START THE GAME<<<<" , colors[RED]);
	DrawString(235, 50, width, height + 5, "-" , colors[WHITE]);
	}
	
	//Game board
	if(startdisplay>0 && timee!=0){
	//First row of Alphabets
	DrawAlphabet((alphabets)num1, 10, height-120, awidth, aheight);
	DrawAlphabet((alphabets)num2, 70, height -120, awidth, aheight);
	DrawAlphabet((alphabets)num3, 130, height -120, awidth, aheight);
	DrawAlphabet((alphabets)num4, 190, height -120, awidth, aheight);
	DrawAlphabet((alphabets)num5, 250, height-120, awidth, aheight);
	DrawAlphabet((alphabets)num6, 310, height -120, awidth, aheight);
	DrawAlphabet((alphabets)num7, 370, height -120, awidth, aheight);
	DrawAlphabet((alphabets)num8, 430, height -120, awidth, aheight);
	DrawAlphabet((alphabets)num9, 490, height-120, awidth, aheight);
	DrawAlphabet((alphabets)num10, 550, height -120, awidth, aheight);
	DrawAlphabet((alphabets)num11, 610, height -120, awidth, aheight);
	DrawAlphabet((alphabets)num12, 670, height -120, awidth, aheight);
	DrawAlphabet((alphabets)num13, 730, height-120, awidth, aheight);
	DrawAlphabet((alphabets)num14, 790, height -120, awidth, aheight);
	DrawAlphabet((alphabets)num15, 850, height -120, awidth, aheight);
	
	//Second row of Alphabets
	DrawAlphabet((alphabets)num16, 10, height-180, awidth, aheight);
	DrawAlphabet((alphabets)num17, 70, height -180, awidth, aheight);
	DrawAlphabet((alphabets)num18, 130, height -180, awidth, aheight);
	DrawAlphabet((alphabets)num19, 190, height -180, awidth, aheight);
	DrawAlphabet((alphabets)num20, 250, height-180, awidth, aheight);
	DrawAlphabet((alphabets)num21, 310, height -180, awidth, aheight);
	DrawAlphabet((alphabets)num22, 370, height -180, awidth, aheight);
	DrawAlphabet((alphabets)num23, 430, height -180, awidth, aheight);
	DrawAlphabet((alphabets)num24, 490, height-180, awidth, aheight);
	DrawAlphabet((alphabets)num25, 550, height -180, awidth, aheight);
	DrawAlphabet((alphabets)num26, 610, height -180, awidth, aheight);
	DrawAlphabet((alphabets)num27, 670, height -180, awidth, aheight);
	DrawAlphabet((alphabets)num28, 730, height-180, awidth, aheight);
	DrawAlphabet((alphabets)num29, 790, height -180, awidth, aheight);
	DrawAlphabet((alphabets)num30, 850, height -180, awidth, aheight);
	
	
	
	//shooting alphabet
	//first shoot
	if(loc1y<415){
	loc1y=loc1y+8;
	}
	DrawAlphabet((alphabets)num31, loc1x, loc1y, awidth, aheight);
	
	//second shoot
	if(loc2y<415 && loc1y>400){
	loc2y=loc2y+8;
	}
	if(loc1y>400){
	DrawAlphabet((alphabets)num32, loc2x, loc2y, awidth, aheight);
	}
	
	//third shoot
	if(loc3y<415 && loc2y>400){
	loc3y=loc3y+8;
	}
	if(loc2y>400){
	DrawAlphabet((alphabets)num33, loc3x, loc3y, awidth, aheight);
	}
	
	//forth shoot
	if(loc4y<415 && loc3y>400){
	loc4y=loc4y+8;
	}
	if(loc3y>400){
	DrawAlphabet((alphabets)num34, loc4x, loc4y, awidth, aheight);
	}
	
	//fifth shoot
	if(loc5y<415 && loc4y>400){
	loc5y=loc5y+8;
	}
	if(loc4y>400){
	DrawAlphabet((alphabets)num35, loc5x, loc5y, awidth, aheight);
	}
	
	//sixth shoot
	if(loc6y<415 && loc5y>400){
	loc6y=loc6y+8;
	}
	if(loc5y>400){
	DrawAlphabet((alphabets)num36, loc6x, loc6y, awidth, aheight);
	}
	
	//seventh shoot
	if(loc7y<415 && loc6y>400){
	loc7y=loc7y+8;
	}
	if(loc6y>400){
	DrawAlphabet((alphabets)num37, loc7x, loc7y, awidth, aheight);
	}
	
	//eighth shoot
	if(loc8y<415 && loc7y>400){
	loc8y=loc8y+8;
	}
	if(loc7y>400){
	DrawAlphabet((alphabets)num38, loc8x, loc8y, awidth, aheight);
	}
	
	//ninth shoot
	if(loc9y<415 && loc8y>400){
	loc9y=loc9y+8;
	}
	if(loc8y>400){
	DrawAlphabet((alphabets)num39, loc9x, loc9y, awidth, aheight);
	}
	
	//tenth shoot
	if(loc10y<415 && loc9y>400){
	loc10y=loc10y+8;
	}
	if(loc9y>400){
	DrawAlphabet((alphabets)num40, loc10x, loc10y, awidth, aheight);
	}
	//eleventh shoot
	if(loc11y<415 && loc10y>400){
	loc11y=loc11y+8;
	}
	if(loc10y>400){
	DrawAlphabet((alphabets)num41, loc11x, loc11y, awidth, aheight);
	}
	//twelveth shoot
	if(loc12y<415 && loc11y>400){
	loc12y=loc12y+8;
	}
	if(loc11y>400){
	DrawAlphabet((alphabets)num42, loc12x, loc12y, awidth, aheight);
	}
	//thirteenth shoot
	if(loc13y<415 && loc12y>400){
	loc13y=loc13y+8;
	}
	if(loc12y>400){
	DrawAlphabet((alphabets)num43, loc13x, loc13y, awidth, aheight);
	}
	
	
	DrawString(60, height - 20, width, height + 5, "Score " + Num2Str(score), colors[BLUE_VIOLET]);
	
	DrawString(670, height - 25, width, height,"Time Left:" + Num2Str(timee) + " mili secs", colors[RED]);
	
	// #----------------- Write your code till here ----------------------------#
	//DO NOT MODIFY THESE LINES
	DrawShooter((width / 2) - 35, 0, bwidth, bheight);
	}
	
	
	//end of game
	if(timee==0 && startdisplay>=1){
	DrawAlphabet((alphabets)6, 250, height-300, awidth, aheight);
	DrawAlphabet((alphabets)0, 310, height -300, awidth, aheight);
	DrawAlphabet((alphabets)12, 370, height -300, awidth, aheight);
	DrawAlphabet((alphabets)4, 430, height -300, awidth, aheight);
	DrawAlphabet((alphabets)14, 490, height-300, awidth, aheight);
	DrawAlphabet((alphabets)21, 550, height -300, awidth, aheight);
	DrawAlphabet((alphabets)4, 610, height -300, awidth, aheight);
	DrawAlphabet((alphabets)17, 670, height -300, awidth, aheight);
	DrawString(425, 310, width, height + 5, "Final Score: " + Num2Str(score), colors[BLUE_VIOLET]);
	DrawString(235, 50, width, height + 5, "-" , colors[WHITE]);
	}
	glutSwapBuffers();
	//DO NOT MODIFY THESE LINES..
	if(timee>0 && startdisplay>=1){
	timee--;
	}
	
	
}

/* Function sets canvas size (drawing area) in pixels...
*  that is what dimensions (x and y) your game will have
*  Note that the bottom-left coordinate has value (0,0) and top-right coordinate has value (width-1,height-1)
* */
void SetCanvasSize(int width, int height) {
	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.*/
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
* is pressed from the keyboard
*
* You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
*
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
*
* */

void NonPrintableKeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT /*GLUT_KEY_LEFT is constant and contains ASCII for left arrow key*/) {
		// what to do when left key is pressed...
		//first shoot
		if(loc1y<415){
		loc1x=loc1x-15;
		}
		//second shoot
		if(loc1y>400 && loc2y<415){
		loc2x=loc2x-15;
		}
		//third shoot
		if(loc2y>400 && loc3y<415){
		loc3x=loc3x-15;
		}
		//forth shoot
		if(loc3y>400 && loc4y<415){
		loc4x=loc4x-15;
		}
		//fifth shoot
		if(loc4y>400 && loc5y<415){
		loc5x=loc5x-15;
		}
		//sixth shoot
		if(loc5y>400 && loc6y<415){
		loc6x=loc6x-15;
		}
		//seventh shoot
		if(loc6y>400 && loc7y<415){
		loc7x=loc7x-15;
		}
		//eighth shoot
		if(loc7y>400 && loc8y<415){
		loc8x=loc8x-15;
		}
		//ninth shoot
		if(loc8y>400 && loc9y<415){
		loc9x=loc9x-15;
		}
		//tenth shoot
		if(loc9y>400 && loc10y<415){
		loc10x=loc10x-15;
		}
		//eleventh shoot
		if(loc10y>400 && loc11y<415){
		loc11x=loc11x-15;
		}
		//twelveth shoot
		if(loc11y>400 && loc12y<415){
		loc12x=loc12x-15;
		}
		//thirteenth shoot
		if(loc12y>400 && loc13y<415){
		loc13x=loc13x-15;
		}
		
	}
	else if (key == GLUT_KEY_RIGHT /*GLUT_KEY_RIGHT is constant and contains ASCII for right arrow key*/) {
		//first shoot
		if(loc1y<415){
		loc1x=loc1x+15;
		}
		//second shoot
		if(loc1y>400 && loc2y<415){
		loc2x=loc2x+15;
		}
		//third shoot
		if(loc2y>400 && loc3y<415){
		loc3x=loc3x+15;
		}
		//forth shoot
		if(loc3y>400 && loc4y<415){
		loc4x=loc4x+15;
		}
		//fifth shoot
		if(loc4y>400 && loc5y<415){
		loc5x=loc5x+15;
		}
		//sixth shoot
		if(loc5y>400 && loc6y<415){
		loc6x=loc6x+15;
		}
		//seventh shoot
		if(loc6y>400 && loc7y<415){
		loc7x=loc7x+15;
		}
		//eighth shoot
		if(loc7y>400 && loc8y<415){
		loc8x=loc8x+15;
		}
		//ninth shoot
		if(loc8y>400 && loc9y<415){
		loc9x=loc9x+15;
		}
		//tenth shoot
		if(loc9y>400 && loc10y<415){
		loc10x=loc10x+15;
		}
		//eleventh shoot
		if(loc10y>400 && loc11y<415){
		loc11x=loc11x+15;
		}
		//twelveth shoot
		if(loc11y>400 && loc12y<415){
		loc12x=loc12x+15;
		}
		//thirteenth shoot
		if(loc12y>400 && loc13y<415){
		loc13x=loc13x+15;
		}
		
	}
	else if (key == GLUT_KEY_UP/*GLUT_KEY_UP is constant and contains ASCII for up arrow key*/) {
	}
	else if (key == GLUT_KEY_DOWN/*GLUT_KEY_DOWN is constant and contains ASCII for down arrow key*/) {
	}

	/* This function calls the Display function to redo the drawing. Whenever you need to redraw just call
	* this function*/
	/*
	glutPostRedisplay();
	*/
}
/*This function is called (automatically) whenever your mouse moves witin inside the game window
*
* You will have to add the necessary code here for finding the direction of shooting
*
* This function has two arguments: x & y that tells the coordinate of current position of move mouse
*
* */

void MouseMoved(int x, int y) {
	//If mouse pressed then check than swap the balls and if after swaping balls dont brust then reswap the balls

}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
*
* You will have to add the necessary code here for shooting, etc.
*
* This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
* x & y that tells the coordinate of current position of move mouse
*
* */

void MouseClicked(int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON) // dealing only with left button
	{
		if (state == GLUT_UP)
		{
		startdisplay++;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON) // dealing with right button
	{

	}
	glutPostRedisplay();
}
/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
* is pressed from the keyboard
* This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
* program coordinates of mouse pointer when key was pressed.
* */
void PrintableKeys(unsigned char key, int x, int y) {
	if (key == KEY_ESC /* Escape key ASCII*/) {
		exit(1); // exit the program when escape key is pressed.
	}
	
}

/*
* This function is called after every 1000.0/FPS milliseconds
* (FPS is defined on in the beginning).
* You can use this function to animate objects and control the
* speed of different moving objects by varying the constant FPS.
*
* */
void Timer(int m) {

	glutPostRedisplay();
	glutTimerFunc(1000.0/FPS, Timer, 0);
}

/*
* our gateway main function
* */


int main(int argc, char*argv[]) {
	InitRandomizer(); // seed the random number generator...

	//Dictionary for matching the words. It contains the 370099 words.
	dictionary = new string[dictionarysize]; 
	ReadWords("words_alpha.txt", dictionary); // dictionary is an array of strings
	//print first 5 words from the dictionary
	for(int i=0; i < 5; ++i)
		cout<< " word "<< i << " =" << dictionary[i] <<endl;

	//Write your code here for filling the canvas with different Alphabets. You can use the Getalphabet function for getting the random alphabets
num1=rand()%26;
num2=rand()%26;
num3=rand()%26;
num4=rand()%26;
num5=rand()%26;
num6=rand()%26;
num7=rand()%26;
num8=rand()%26;
num9=rand()%26;
num10=rand()%26;
num11=rand()%26;
num12=rand()%26;
num13=rand()%26;
num14=rand()%26;
num15=rand()%26;

num16=rand()%26;
num17=rand()%26;
num18=rand()%26;
num19=rand()%26;
num20=rand()%26;
num21=rand()%26;
num22=rand()%26;
num23=rand()%26;
num24=rand()%26;
num25=rand()%26;
num26=rand()%26;
num27=rand()%26;
num28=rand()%26;
num29=rand()%26;
num30=rand()%26;
num31=rand()%26;
num32=rand()%26;
num33=rand()%26;
num34=rand()%26;
num35=rand()%26;
num36=rand()%26;
num37=rand()%26;
num38=rand()%26;
num39=rand()%26;
num40=rand()%26;
num41=rand()%26;
num42=rand()%26;
num43=rand()%26;
num44=rand()%26;
num45=rand()%26;
num46=rand()%26;
num47=rand()%26;
num48=rand()%26;
num49=rand()%26;
num50=rand()%26;



	glutInit(&argc, argv); // initialize the graphics library...
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
	glutInitWindowPosition(50, 50); // set the initial position of our window
	glutInitWindowSize(width, height); // set the size of our window
	glutCreateWindow("ITCs Word Shooter"); // set the title of our game window
	//SetCanvasSize(width, height); // set the number of pixels...

	// Register your functions to the library,
	// you are telling the library names of function to call for different tasks.
	RegisterTextures();
	glutDisplayFunc(DisplayFunction); // tell library which function to call for drawing Canvas.
	glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
	glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
	glutMouseFunc(MouseClicked);
	glutPassiveMotionFunc(MouseMoved); // Mouse

	//// This function tells the library to call our Timer function after 1000.0/FPS milliseconds...
	glutTimerFunc(1000.0/FPS, Timer, 0);

	//// now handle the control to library and it will call our registered functions when
	//// it deems necessary...

	glutMainLoop();

	return 1;
}
#endif /* */
