// =============================================================================
// Jared Wright
// 

#include <cstdlib>
#include <iostream>
#include <math.h>
#include <vector>
#include <GL/glut.h>
#include "matrix.h"
#include <stdio.h>

using namespace std;

// =============================================================================
// These variables will store the input ppm image's width, height, and color
// depth values read in from the ppm header.
// =============================================================================
int width, height, maxColor;

unsigned char *pixmap, *wpixmap, *outPixmap, *commentsSize, temp, temp2;
vector<unsigned char> comments;
Matrix3D scaleM, translateM, rotateM, flipM, shearM, perspectM, m, inv;
Vector2D coor, coorout;
int outHeight, outWidth, offsetX, offsetY;
bool morecomm = false;

void shear(double hx, double hy){
	identity(shearM);
	shearM[0][1] = hx;
	shearM[1][0] = hy;
}

void translate(double dx, double dy){
	identity(translateM);
	translateM[0][2] = dx;
	translateM[1][2] = dy;
}

void scale(double sx, double sy){
	identity(scaleM);
	scaleM[0][0] = sx;
	scaleM[1][1] = sy;	
}

void rotate(double r){
	identity(rotateM);
	rotateM[0][0] = cos(r*PI/180);
	rotateM[0][1] = -sin(r*PI/180);
	rotateM[1][0] = sin(r*PI/180);
	rotateM[1][1] = cos(r*PI/180);
}

void flip(int xf, int yf){
	identity(flipM);
	if(xf == 0)
	 	xf = 1;		
	else if(xf == 1)
		xf = -1;		
	if(yf == 0)
		yf = 1;		
	else if(yf == 1)
		yf = -1;		
	flipM[0][0] = xf;
	flipM[1][1] = yf;			
}

void perspective(double px, double py){
	identity(perspectM);	
	perspectM[2][0] = px;
	perspectM[2][1] = py;
}

unsigned char getpix(int inpixel){
	unsigned char pix;	
	if(coorout.x<0 || coorout.y<0 || coorout.x>width-1 || coorout.y>height-1)
		pix = 0;
	else
		pix = pixmap[inpixel];	
	return pix;
}

void inverseMap(){
	outPixmap = new unsigned char[outWidth * outHeight * 3];
	unsigned char pix;
	int outpixel, inpixel;		
	if(offsetX<0){
		m[0][2] = m[0][2] - offsetX;
	}
	if(offsetY<0){
		m[1][2] = m[1][2] - offsetY;
	}
	
	for(int y = outHeight-1; y >= 0; y--){
		for(int x = 0; x < outWidth; x++){			
			coor.x = x; coor.y = y;			
				
    		inverse(m, inv); 
			transform(inv, coor, coorout);			
			
   	        outpixel = ((int)coor.y * outWidth + (int)coor.x) * 3; 			
			inpixel = ((int)round(coorout.y) * width + (int)round(coorout.x)) * 3;			
			outPixmap[outpixel] = getpix(inpixel);
			outpixel++;inpixel++;			
			outPixmap[outpixel] = getpix(inpixel);
			outpixel++;inpixel++;	
			outPixmap[outpixel] = getpix(inpixel);					
		}
	}
}

void readPPM(char *filename){
  width = height = 100; 

  FILE * infile;
  infile = fopen(filename, "rb");

  if (infile == NULL) {
    cerr << "File " << filename << " could not be opened." << endl;	
    exit(1);
  }

  char magic[2];
  //get the magic number from file header
  fscanf(infile, "%s", magic);

  //check if it is P6
  if (magic[0] != 'P' || magic[1]!='6'){
    printf("Magic Number for input file is not P6.");
    exit(0);
  } 
 
  fgetc(infile);
  //skip over comments  
  int sharp = 0;
  
  temp = fgetc(infile);
  do{
    if(temp=='#'){
      if(sharp<1){
        ungetc(temp,infile);
        sharp++;
      }
      while(temp!='\n'){
        temp = fgetc(infile);
	comments.push_back(temp);
      }
	  temp2 = fgetc(infile);
	  if(temp2=='#'){
	    morecomm=true;
	    temp='#';
	  }
	  else
	    morecomm=false;
	    ungetc(temp2,infile);
    }
    else{
      ungetc(temp,infile);
      morecomm=false;
    }
  }while(morecomm);

  fscanf (infile, "%d %d %d", &width, &height, &maxColor);  
   // [width * height * 3] is the number of color channels for rgb pixels;    
  int pixel;  
  unsigned char red, green, blue;
  fgetc(infile);
  
  outWidth = width; outHeight = height;
  offsetY = 0; offsetX = 0;
  
  for(int x = 0; x < height; x+=height-1){
	  for(int y = 0; y < width; y+=width-1){
		  
		coor.x = x;coor.y = y;	
	  	transform(m,coor,coorout);
		  
		if(coorout.x>outWidth)
			outWidth = (int)round(coorout.x);			
		if(coorout.y>outHeight)			  
			outHeight = (int)round(coorout.y);
		if(coorout.x<0){
			if(coorout.x<offsetX){
				int f = ((int)round(abs(coorout.x)) + width);
				if(f>outWidth)
					outWidth = f;	
				offsetX = (int)round(coorout.x);
			}
		}
		if(coorout.y<0){
			if(coorout.y<offsetY){
				int f = ((int)round(abs(coorout.y)) + height);
				if(f>outHeight)
					outHeight = f;			
				offsetY = (int)round(coorout.y);
			}
		}
	 }
  } 
  pixmap = new unsigned char[width * height * 3];

  for(int y = height-1; y >= 0; y--){
    for(int x = 0; x < width; x++){  
      red = green = blue = 0;

      red = fgetc(infile);  
      green = fgetc(infile);  
      blue = fgetc(infile);     

      pixel = (y * width + x) * 3; 

      pixmap[pixel] = red;
      pixel++;
      pixmap[pixel] = green;
      pixel++;
      pixmap[pixel] = blue;
    }
  }
}

void writePPM(char *outfilename){
  wpixmap = new unsigned char[width * height * 3];
  int wheight = height-1;
  int wpixel,pix;  

  for(int y = 0; y < height; y++) {    
    for(int x = 0; x < width; x++) {

      wpixel = (y * width + x) * 3;
      pix = (wheight * width + x) * 3;

      wpixmap[wpixel] = pixmap[pix];
      wpixel++;
      pix++;
      wpixmap[wpixel] = pixmap[pix];
      wpixel++;
      pix++;
      wpixmap[wpixel] = pixmap[pix];
    }
    wheight--;
  }      

  FILE *outfile;
  outfile = fopen(outfilename, "wb");    
  commentsSize = new unsigned char[comments.size()]; 

  for(int i = 0; i<comments.size(); i++){
    commentsSize[i] = comments.at(i);
  } 
  fprintf(outfile, "P6\n");
  fwrite(commentsSize,comments.size(),sizeof(*commentsSize),outfile);
  fprintf(outfile, "%d %d\n", width, height);
  fprintf(outfile, "255\n");
  fwrite(wpixmap,width*height*3,sizeof(*wpixmap),outfile);
  fclose(outfile); 
}

static void resize(int w, int h){   
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,(w/2),0,(h/2),0,1); 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity() ;
}
static void display(void){  
  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(0,0);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);   
  glDrawPixels(outWidth, outHeight, GL_RGB, GL_UNSIGNED_BYTE, outPixmap); 
  glFlush();
}
static void processMouse(int button, int state, int x, int y){
  if(state == GLUT_UP)
  exit(0);               // Exit on mouse click.
}
static void init(void){
  glClearColor(1,1,1,1); // Set background color.
}
void cinErr(bool fail){
	if(fail){
		cout<<endl;
		cout<<"Incorrect input type, must be of type double."<<endl;
		cout<<endl;
		exit(1);
	}
}
// =============================================================================
// main() Program Entry
// =============================================================================
int main(int argc, char *argv[]){ 

  string input;	
  double param1, param2;
  bool done = false;
 
  if(argv[1]!=NULL){}
  else{
    cerr << "No filename specified. Type: 'warper InFilename.ppm [OutFilename.ppm]' in command line. Where 'InFilename' is the name of the file you want to read and 'OutFilename' is an optional input to write the image out to file. Do not include quotes. " << endl;    
    exit(1);
  }    
  
  identity(m);
  identity(inv);    
  
  cout<<endl;
  cout<<"Type character to perform cooresponding warp(s):"<<endl;
  cout<<endl;
  cout<<"'r' - rotation (CCW)."<<endl;
  cout<<"'s' - scale."<<endl;
  cout<<"'t' - translate."<<endl;
  cout<<"'f' - flip."<<endl;
  cout<<"'h' - shear."<<endl;
  cout<<"'p' - perspective."<<endl;
  cout<<"'d' - done."<<endl;  
  
  while(!done){  
  cout<<endl;
  cout<<"Enter Warp: ";  
  cin >> input; 
	  
  	switch(input[0]){

		case 'r':
			double theta;
			cout<<"Input theta: ";
			cin>> theta;
			cinErr(cin.fail());	
			rotate(theta);
			product(rotateM,m,m);
			printmatrix(m);
		break;
		case 's':
			double sx,sy;
			cout<<"Input sx: ";
			cin>> sx;
			cinErr(cin.fail());	
			cout<<"Input sy: ";
			cin>> sy;
			cinErr(cin.fail());	
			scale(sx,sy);
			product(scaleM,m,m);
			printmatrix(m);		
		break;
		case 't':
			double dx,dy;
			cout<<"Input dx: ";
			cin>> dx;
			cinErr(cin.fail());	
			cout<<"Input dy: ";
			cin>> dy;
			cinErr(cin.fail());	
			translate(dx,dy);
			product(translateM,m,m);
			printmatrix(m);
		break;
		case 'f':
			cout<<"(1 = flip, 0 = no flip)"<<endl;
			int xf,yf;
			cout<<"Input xf: ";
			cin>> xf;
			cinErr(cin.fail());	
			cout<<"Input yf: ";
			cin>> yf;
			cinErr(cin.fail());	
			flip(xf,yf);
			product(flipM,m,m);
			printmatrix(m);
		break;
		case 'h':
			double hx,hy;
			cout<<"Input hx: ";
			cin>> hx;
			cinErr(cin.fail());	
			cout<<"Input hy: ";
			cin>> hy;
			cinErr(cin.fail());	
			shear(hx,hy);
			product(shearM,m,m);
			printmatrix(m);
		break;
		case 'p':
			double px,py;
			cout<<"Input px: ";
			cin>> px;
			cinErr(cin.fail());	
			cout<<"Input py: ";
			cin>> py;
			cinErr(cin.fail());	
			perspective(px,py);
			product(perspectM,m,m);
			printmatrix(m);
		break;
		case 'd':
			done = true;
		break;
		default:
			cout<<"Not a valid option"<<endl;
		break;
  	}    
  } 
  
  readPPM(argv[1]);
  inverseMap();
  if(argv[2]!=NULL){
    writePPM(argv[2]);
  }  
  cout<<outWidth<<" "<<outHeight<<endl;

  // OpenGL commands
  glutInit(&argc, argv);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(outWidth, outHeight);
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutCreateWindow("Warping");
  init();
  glutReshapeFunc(resize);
  glutDisplayFunc(display);
  glutMouseFunc(processMouse);
  glutMainLoop();
  return 0;
}
