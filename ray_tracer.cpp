//raytracer

#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <GL/glut.h>
#include <stdio.h>
#include "Vector.h"
#include "Matrix.h"
#include "Shape.h"

using namespace std;

int count2 = 0;
int fractimes;

#define Xmax 700	
#define Ymax 600
//#define radius .3
unsigned char *color, *wpixmap;
float colors[Xmax][Ymax][3];

vector<float> tVals;
vector<param> paras;
vector<Shape*> shapes;
vector<Light> lights;
vector<Texture> textures;
vector<Vector3d> texcoors;
vector<Vector3d> vertices;
vector<Vector3d> normals;
vector<Face> faces;

/*float AreaSx = 2;
float AreaSy = 2;
//float AreaD = 8;
Vector3d AreaPc(0,2,0);*/

/*float AreaM = 2;//Area light samples
float AreaN = 2;//Area light samples
Vector3d AreaVview(0,-1,0);
Vector3d AreaVup(0,0,1);
Vector3d AreaN2 = AreaVview/AreaVview.norm();
Vector3d AreacrossV = AreaVview % AreaVup;
Vector3d AreaN0 = AreacrossV/AreacrossV.norm();
Vector3d AreaN1 = AreaN0 % AreaN2;

Vector3d Areav1 = AreaSx/2 *AreaN0;
Vector3d Areav2 = ((AreaSy/2) * AreaN1);
Vector3d AreaP00 = AreaPc - Areav1 - Areav2;*/

Vector3d Pe(.005,0,-8);

float Sx = 7;
float d = 8;
float m = 1;//samples
float n = 1;//samples
float X, Y, rx, ry, x, y;// AX, AY, Arx, Ary, Ax, Ay;
Vector3d  Pp;

float Sy = (Sx*Ymax)/Xmax;

Vector3d Pc = Pe + (n2 * d);

Vector3d v1 = Sx/2 * n0;
Vector3d v2 = ((Sy/2) * n1);
Vector3d P00 = Pc - v1 - v2;

int texXmax, texYmax, maxColor;
bool morecomm = false;


void OBJreader(){
    Vector3d temp, c1,c2,c3;
    Face f;
    vector<Vector3d> tmp;
    string s;
    float v1,v2,v3,n1,n2,n3,t1,t2,t3;
    char f1[5], f2[5], f3[5], f4[5], f5[5], f6[5], f7[5], f8[5], f9[5];

    ifstream fin;
    fin.open("012.obj");

    if(!fin){
     cout << "Cannot open file.\n";
    }

    while(!fin.eof()){
        fin>>s;
        if(s=="v"){

            fin>>v1>>v2>>v3;
            temp[0]=v1;
            temp[1]=v2;
            temp[2]=v3;
            vertices.push_back(temp);
        }
        if(s=="vn"){

            fin>>v1>>v2>>v3;
            temp[0]=v1;
            temp[1]=v2;
            temp[2]=v3;
            normals.push_back(temp);
        }
        if(s=="vt"){

            fin>>v1>>v2>>v3;
            temp[0]=v1;
            temp[1]=v2;
            temp[2]=v3;
            texcoors.push_back(temp);
        }
        if(s=="f"){        

            fin.getline(f1,5,'/');
            fin.getline(f2,5,'/');
            fin.getline(f3,5,' ');
            v1 = atoi(f1);
            t1 = atoi(f2);
            n1 = atoi(f3);      

            fin.getline(f4,5,'/');
            fin.getline(f5,5,'/');
            fin.getline(f6,5,' ');
            v2 = atoi(f4);
            t2 = atoi(f5);
            n2 = atoi(f6);  

            fin.getline(f7,5,'/');
            fin.getline(f8,5,'/');
            fin>>f9;
            v3 = atoi(f7);
            t3 = atoi(f8);
            n3 = atoi(f9);          

            f.vertices = Vector3d(v1,v2,v3);
            f.normal = Vector3d(n1,n2,n3);
            f.texcoors = Vector3d(t1,t2,t3);

            faces.push_back(f);
        }
    }
    fin.close();
}


//read images for textures
Texture readPPM(char *filename){
  //unsigned char *tex;
	Texture t;
  texXmax = texYmax = 100; // These values must be initialized for the OpenGL
                        // functions to work.
  FILE * infile;
  infile = fopen(filename, "rb");

  if (infile == NULL) {
    cerr << "File " << filename << " could not be opened." << endl;
	system("PAUSE");
    exit(1);
  }

  char magic[10];
  //get the magic number from file header
  fscanf(infile, "%s", magic);

  //check if it is P6
  if (magic[0] != 'P' || magic[1]!='6'){
    printf("Magic Number for input file is not P6.");
    exit(0);
  }
  fgetc(infile);
  //skip over comments
  unsigned char temp = fgetc(infile);
  do{
    if(temp=='#'){
      while(temp!='\n'){
        temp = fgetc(infile);
      }
	  unsigned char temp2 = fgetc(infile);
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

  fscanf (infile, "%d %d %d", &texXmax, &texYmax, &maxColor);
  
  tex = new unsigned char[texXmax * texYmax * 3];  

  int y, x, pixel;
  unsigned char red, green, blue;
  fgetc(infile);
  for(y = texYmax-1; y >= 0; y--) {
    for(x = 0; x < texXmax; x++) {
      red = green = blue = 255;

	  red = fgetc(infile);  
      green = fgetc(infile);  
      blue = fgetc(infile);     

      pixel = (y * texXmax + x) * 3; 

      tex[pixel] = red;
      pixel++;
      tex[pixel] = green;
      pixel++;
      tex[pixel] = blue;
    }
  } 
  t.texXmax=texXmax;
  t.texYmax=texYmax;
  t.tex=tex;
  textures.push_back(t);

  return t;
}


void writePPM(char *outfilename){

  wpixmap = new unsigned char[Xmax * Ymax * 3];
  int wheight = Ymax-1;
  int wpixel,pix;  

  for(int y = 0; y < Ymax; y++) {    
    for(int x = 0; x < Xmax; x++) {

      wpixel = (y * Xmax + x) * 3;
      pix = (wheight * Xmax + x) * 3;

      wpixmap[wpixel] = color[pix];
      wpixel++;
      pix++;
      wpixmap[wpixel] = color[pix];
      wpixel++;
      pix++;
      wpixmap[wpixel] = color[pix];
    }
    wheight--;
  }      

  FILE *outfile;
  outfile = fopen(outfilename, "wb");

 // int y,x;
 // unsigned char pixel;

  fprintf(outfile, "P6\n");
  fprintf(outfile, "%d %d\n", Xmax, Ymax);
  fprintf(outfile, "255\n");
  fwrite(wpixmap,Xmax*Ymax*3,sizeof(*wpixmap),outfile);
  fclose(outfile);
}

param paraTest, para;

param rayTrace (Vector3d Pe2, Vector3d npe, int I, int J){

		para.tVal = -1;
		paras.clear();
		tVals.clear();
		
        for(int k = 0; k<shapes.size(); k++){
            paraTest = shapes[k]->intersection(Pe2,npe,I,J);
			if(paraTest.tVal>0){
				paras.push_back(paraTest);
				tVals.push_back(paraTest.tVal);
			}
		}

		for(int i = 0; i<paras.size(); i++){
			if(paras.at(i).tVal == *min_element(tVals.begin(),tVals.end())){
				para = paras.at(i);
			}
                }




        return para;
}

param shadowIntersection;
Vector3d shade, shade2, shadowRay;
float max_tVal;

//shadows
float shadows(param para2, vector<Light> lights, int I, int J, int i){
	float shad;
	
    shad = 1;             
    shade = lights[i].position -  para2.surfacePoint;
    shade2 = lights[i].position -  para2.surfacePoint;
    shadowRay = shade/shade.norm();
					
	max_tVal = shade2.norm();
    shadowIntersection = rayTrace(para2.surfacePoint,shadowRay,I,J);
    // shadowIntersection.tVal = -1;
	if(shadowIntersection.tVal>0.001 && shadowIntersection.tVal<max_tVal){				
		shad = 0.5;
	}
	return shad;
}

Vector3d  reflect, c1;
int depth = 0;

//lighting
Vector3d calcLighting(param para2, int I, int J, Vector3d npe, vector<Light> lights){
	Vector3d sumlight;
	float shadow;
	param ref;
    sumlight = 0;
	//depth++;
	for(int i = 0; i<lights.size(); i++){
	//for(int I = 0;I<AreaSx;I++){
	//	for(int J = 0;J<AreaSy;J++){
		//	for(int i = 0;i<AreaM;i++){
			//	for(int j = 0;j<AreaN;j++){
					//Arx = (float)rand()/RAND_MAX;
					//Ary = (float)rand()/RAND_MAX;							
							
					//AX = I + (i/AreaM) + (Arx/AreaM);
					//AY = J + (j/AreaN) + (Ary/AreaN);
					//Ax = AX/Sx;	
					//Ay = AY/Sy;
					//Vector3d AreaPP = AreaP00 + (Ax*AreaN0*AreaSx) + (Ay*AreaN1*AreaSy);
					Vector3d rcol, colo, fracol;

					//refraction
					if(para2.shape->refraction > 0){
						Vector3d t;
						param fract;
						//fractimes++;
						

						//if(fractimes>1){
						//	para2.normal = -para2.normal;
						//	cout<<"here"<<endl;
						//}

						float sq = ((para2.normal * npe) * (para2.normal * npe) - 1.0) / (para2.shape->refraction * para2.shape->refraction) + 1.0;

						if(sq<0)
							t = npe - ((2.0 * (npe * para2.normal)) * para2.normal);
						else
							t = (1.0 / para2.shape->refraction) * npe + (((-para2.normal * npe) / para2.shape->refraction) - sqrt(sq)) * para2.normal;

						t = t/t.norm();
						para2.surfacePoint = para2.surfacePoint + (t * 0.0001);
						//Vector tmp = t;

						//for(int m1 = 0; m1 < 12; m1++){
						//t = tmp;
						//	for(int n1 = 0; n1<4; n1++){
						
						//Vector3d randvect((float)rand()/RAND_MAX,(float)rand()/RAND_MAX,(float)rand()/RAND_MAX);
						//randvect = randvect * 0.05;
						//float neg = (float)rand()/RAND_MAX;

						//if(neg<0.5){
						//	randvect = -randvect;
						//}
						
						//t = t + randvect;
												
						fract = rayTrace(para2.surfacePoint, t, I, J);

						//if(fract.tVal>0.0001 && depth<6)
						fracol = calcLighting(fract, I, J, t, lights);
						
						//	}
						//}

						//fracol = fracol / 12;
						//if(fracol[0]>1) fracol[0]=1;
						//if(fracol[1]>1) fracol[1]=1;
						//if(fracol[2]>1) fracol[2]=1;
					}

					//reflections
					if(para2.shape->reflection > 0){						

						reflect = npe - ((2 * (npe * para2.normal)) * para2.normal);
						reflect = reflect/reflect.norm();

						para2.surfacePoint = para2.surfacePoint + (reflect * 0.00001);
						Vector tmp = reflect;

						/*for(int m1 = 0; m1 < 16; m1++){
						reflect = tmp;						
						
						Vector3d randvect((float)rand()/RAND_MAX,(float)rand()/RAND_MAX,(float)rand()/RAND_MAX);
						randvect = randvect * 0.5;
						float neg = (float)rand()/RAND_MAX;

						if(neg<0.5){
							randvect = -randvect;
						}
						
						reflect = reflect + randvect;*/


						ref = rayTrace(para2.surfacePoint,reflect,I,J);	
						
						if(ref.tVal>0.0001){	
							rcol = rcol + calcLighting(ref,I,J,reflect,lights);							
							//para2.shape->reftimes++;
						}

						//}
						//rcol = rcol / 16;
						rcol = (para2.shape->reflection * rcol);
					}	
					
					shadow = shadows(para2,lights,I,J,i);				
				
					//sumlight = sumlight + (shadow * (phong(para2.normal,para2.surfacePoint,para2.shape->diffuse,npe, lights[i].color, lights[i].position))); // + (.1 * lightColor * para2.shape->diffuse);
					//sumlight = sumlight / (AreaM*AreaN);                

					if(para2.shape->text == true){//if there is a texture
						colo = para2.shape->texture(para2.normal,para2.surfacePoint,lights[i].color,lights[i].position,textures[para2.shape->tex]);
					}
					else 
						colo = para2.shape->diffuse;

					if(para2.shape->shadeType==0)
						sumlight = sumlight + fracol + rcol + (shadow * (lambert(para2.normal, para2.surfacePoint, colo * (1 - para2.shape->reflection) , lights[i].color, lights[i].position)));
					if(para2.shape->shadeType==1)
                                                sumlight = sumlight + fracol + rcol + (shadow * (phong(para2.normal, para2.surfacePoint, colo * (1 - para2.shape->reflection) , npe, lights[i].color, lights[i].position)));
				
				}
			//}
			//sumlight = sumlight / (AreaM*AreaN);
		//}
	//}
		if(sumlight[0]>1) sumlight[0] = 1;
		if(sumlight[1]>1) sumlight[1] = 1;
		if(sumlight[2]>1) sumlight[2] = 1;
		c1 = sumlight;// /  ((AreaM*AreaN) + (Sx*Sy));
		
        return c1;
}


void display(void){ 	
	color = new unsigned char[Xmax * Ymax * 3];  
    Vector3d col, v;
    int pixel;
    param r;

	 for(int I = 0; I<Xmax; I++){		 
                for(int J = 0; J<Ymax; J++){
                        pixel = (J * Xmax + I) * 3;

                    for(int i = 0; i<m; i++){						
                        for(int j = 0; j<n; j++){
                            rx = (float)rand()/RAND_MAX;
                            ry = (float)rand()/RAND_MAX;							
							
                            X = I + (i/m) + (rx/m);
                            Y = J + (j/n) + (ry/n);
                            x = X/Xmax;
                            y = Y/Ymax;
                            Pp = P00 + (x*n0*Sx) + (y*n1*Sy);
                            v = Pp - Pe;
                            npe = v/v.norm();
							
                            r = rayTrace(Pe,npe,I,J);

                            if(r.tVal > 0){
                                 param occ;
                                 Vector3d occol, ocraycol;
                                 int num = vertices.size();
                                 Vector3d surf = r.surfacePoint;

                                 for(int i = 0; i<vertices.size()-1; i++){

                                    Vector3d o = vertices[i] - r.surfacePoint;
                                    o = o / o.norm();

                                    if(o * r.normal > 0){
                                        surf = surf + (o * 0.0001);
                                        occ = rayTrace(surf,o,I,J);

                                        if(occ.tVal != -1)
                                            ocraycol = Vector3d(0,0,0);
                                        else
                                            ocraycol = Vector3d(1,1,1);
                                    }                                 

                                    occol = occol + ocraycol;                                    
                                }
                                 occol = occol / num;
                                 col = occol;
                                //col = calcLighting(r,I,J,npe, lights);
                                //depth=0;
                            }
                            else
                                col.set(0,0,0);

                            colors[I][J][0] = colors[I][J][0] + col[0];
                            colors[I][J][1] = colors[I][J][1] + col[1];
                            colors[I][J][2] = colors[I][J][2] + col[2];
                        }
                    }
		}
	} 	

	 //divide by samples
            for(int I = 0; I<Xmax; I++){
                for(int J = 0; J<Ymax; J++){
                   colors[I][J][0] = colors[I][J][0] / (m*n);
                   colors[I][J][1] = colors[I][J][1] / (m*n);
                   colors[I][J][2] = colors[I][J][2] / (m*n);
                 }
            }

     int pixel1;
	 for(int I = 0; I<Xmax; I++){		 
                for(int J = 0; J<Ymax; J++){
					pixel1 = (J * Xmax + I) * 3;				

                    color[pixel1] = colors[I][J][0] * 255;
                    pixel1++;
                    color[pixel1] = colors[I][J][1] * 255;
                    pixel1++;
                    color[pixel1] = colors[I][J][2] * 255;
				}
	 }
  
  glDrawPixels(Xmax, Ymax, GL_RGB, GL_UNSIGNED_BYTE, color); 
  writePPM("raytrace.ppm");
  glFlush();    
  int x;
  cin>>x;
}


void init(void){

/*Sphere* sphere = new Sphere(Vector3d(.3,-.2,-5),.4,Vector3d(0,-1,0),Vector3d(0,0,1));
sphere->ambient = Vector3d(.5,.3,.5);
sphere->diffuse = Vector3d(0,0,0);
sphere->specular = Vector3d(.6,.6,.6);	
//sphere->tex = 1;
sphere->text = false;
sphere->shadeType = 1;
sphere->refraction = 1.5;
sphere->reflection = .1;
shapes.push_back(sphere);*/

/*Sphere* sphere2 = new Sphere(Vector3d(.2,-.2,-3),.4,Vector3d(0,-1,0),Vector3d(0,0,1));
sphere2->ambient = Vector3d(.5,.3,.5);
sphere2->diffuse = Vector3d(1,0,1);
sphere2->specular = Vector3d(.6,.6,.6);	
//sphere2->text = true;
//sphere2->tex = 1;
sphere2->shadeType = 1;
sphere2->reflection = .5;
shapes.push_back(sphere2);*/

Sphere* sphere2 = new Sphere(Vector3d(0,-.2,-4),.4,Vector3d(0,-1,0),Vector3d(0,0,1));
sphere2->ambient = Vector3d(.5,.3,.5);
sphere2->diffuse = Vector3d(1,0,1);
sphere2->specular = Vector3d(.6,.6,.6);
//sphere2->text = true;
//sphere2->tex = 1;
sphere2->shadeType = 1;
//sphere2->reflection = .5;
shapes.push_back(sphere2);

infinitePlane* plane = new infinitePlane(Vector3d(0,-.6,0),Vector3d(0,1,0),1,1,1, Vector3d(0,-1,0),Vector3d(0,0,1),2,2);
plane->ambient = Vector3d(.2,.2,.2);
plane->diffuse = Vector3d(0,0,1);
plane->specular = Vector3d(.5,.5,.5);	
//plane->tex = 0;
plane->shadeType = 0;
//plane->text = true;
//plane->reflection = .1;
//plane->refraction = 1.5;
shapes.push_back(plane);

/*infinitePlane* plane3 = new infinitePlane(Vector3d(-2,0,0),Vector3d(1,0,0),1,1,1, Vector3d(-1,0,0),Vector3d(0,0,1),2,2);
plane3->ambient = Vector3d(.2,.2,.2);
plane3->diffuse = Vector3d(0,1,1);
plane3->specular = Vector3d(.5,.5,.5);	
plane3->tex = 0;
plane3->shadeType = 0;
//plane->text = true;
//plane->reflection = .5;
shapes.push_back(plane3);

infinitePlane* plane4 = new infinitePlane(Vector3d(2,0,0),Vector3d(-1,0,0),1,1,1, Vector3d(1,0,0),Vector3d(0,0,1),2,2);
plane4->ambient = Vector3d(.2,.2,.2);
plane4->diffuse = Vector3d(.5,0,1);
plane4->specular = Vector3d(.5,.5,.5);	
plane4->tex = 0;
plane4->shadeType = 0;
//plane4->text = true;
//plane4->reflection = .5;
shapes.push_back(plane4);

infinitePlane* plane5 = new infinitePlane(Vector3d(0,1.5,0),Vector3d(0,-1,0),1,1,1, Vector3d(0,1,0),Vector3d(0,0,1),2,2);
plane5->ambient = Vector3d(.2,.2,.2);
plane5->diffuse = Vector3d(.5,.5,1);
plane5->specular = Vector3d(.5,.5,.5);	
plane5->tex = 0;
plane5->shadeType = 0;
//plane4->text = true;
//plane4->reflection = .5;
shapes.push_back(plane5);

infiniteCylinder* cylin = new infiniteCylinder(Vector3d(-1,0,-5.1),Vector3d(0,1,0),0.2,Vector3d(0,1,0),Vector3d(0,0,1));
cylin->ambient = Vector3d(.2,.2,.2);
cylin->diffuse = Vector3d(0,0,1);
cylin->specular = Vector3d(.5,.5,.5);
cylin->reflection=.1;
cylin->refraction = 1.3;
//cylin->tex = 1;
cylin->shadeType=1;
shapes.push_back(cylin);*/

/*Ellipsoid* ellipsoid = new Ellipsoid(Vector3d(-.3,.1,-4),.3,.4,.3,Vector3d(1,0,0),Vector3d(0,1,0));
ellipsoid->ambient = Vector3d(.5,.3,.5);
ellipsoid->diffuse = Vector3d(0,0,0);
ellipsoid->specular = Vector3d(.6,.6,.6);
ellipsoid->reflection = .1;
ellipsoid->refraction = 1.5;
//ellipsoid->text = true;
ellipsoid->tex = 1;
ellipsoid->shadeType = 1;
shapes.push_back(ellipsoid);*/

/*Cone* cone = new Cone(Vector3d(.3,.5,-2),.9,.9,.9,Vector3d(0,-1,0),Vector3d(0,0,-1));
cone->ambient = Vector3d(.5,.3,.5);
cone->diffuse = Vector3d(0,1,0);
cone->specular = Vector3d(.6,.6,.6);
cone->reflection = .1;
cone->refraction = 1.5;
//cone->tex = 1;
cone->shadeType = 1;
shapes.push_back(cone);

Hyperboloid* hyper = new Hyperboloid(Vector3d(-.6,.5,-3.8),.08,.2,.1,0,Vector3d(0,-1,0),Vector3d(0,0,1));
hyper->ambient = Vector3d(.5,.3,.5);
hyper->diffuse = Vector3d(1, 0, 0);
hyper->specular = Vector3d(.6,.6,.6);
hyper->reflection=.1;
hyper->refraction=1.5;
//hyper->tex = 1;
hyper->type = 0;
hyper->shadeType = 1;
shapes.push_back(hyper);

Hyperboloid* hyper2 = new Hyperboloid(Vector3d(1,.5,-5),.08,.2,.1,1,Vector3d(0,-1,0),Vector3d(0,0,1));
hyper2->ambient = Vector3d(.5,.3,.5);
hyper2->diffuse = Vector3d(1,1,0);
hyper2->specular = Vector3d(.6,.6,.6);	
hyper2->reflection = .1;
hyper2->refraction=1.5;
//hyper2->tex = 1;
hyper2->type = 1;
hyper2->shadeType = 1;
shapes.push_back(hyper2);

infinitePlane* plane2 = new infinitePlane(Vector3d(0,0,1),Vector3d(0,0,-1),1,1,1,Vector3d(0,0,1),Vector3d(0,1,0),2,2);
plane2->ambient = Vector3d(.2,.2,.2);
plane2->diffuse = Vector3d(.4,.1,.1);
plane2->specular = Vector3d(.5,.5,.5);
//plane2->reflection = .5;
//plane2->text = true;
plane2->tex = 2;
plane2->shadeType = 0;
shapes.push_back(plane2);*/

//lights.push_back(Light(Vector3d(.2,.8, -1),Vector3d(.3,.3,.3)));	
lights.push_back(Light(Vector3d(0,1, -5),Vector3d(1.1,1.1,1.1)));
//lights.push_back(Light(Vector3d(-.3,-.3, -3),Vector3d(.4,.4,.4)));

/*Sphere* sphere2 = new Sphere(Vector3d(.4,0,.2),0.2);
sphere2->ambient = Vector3d(.5,.3,.5);
sphere2->diffuse = Vector3d(.5,1,0);
sphere2->specular = Vector3d(.6,.6,.6);				
shapes.push_back(sphere2);*/

string s = "norton.ppm";
readPPM((char*)s.c_str());
//string s2 = "checkers.ppm";
//readPPM((char*)s2.c_str());
//string s1 = "brickwall.ppm";
//readPPM((char*)s1.c_str());

OBJreader();

/*for(int i = 0; i<texcoors.size(); i++){
    //for(int j = 0;j < 3;j++){
    //faces[i].vertices.print();cout<<" ";
	texcoors[i].print(); cout<<endl;
    //}

}*/
cout<<vertices.size()<<endl;


/*for(int i = 0; i<faces.size()-1; i++){
	Triangle* tri1 = new Triangle();
	tri1->vert1 = vertices[faces[i].vertices[0]-1];
	tri1->vert2 = vertices[faces[i].vertices[1]-1];
	tri1->vert3 = vertices[faces[i].vertices[2]-1];

	tri1->norm1 = normals[faces[i].normal[0]-1];
	tri1->norm2 = normals[faces[i].normal[1]-1];
	tri1->norm3 = normals[faces[i].normal[2]-1];

	tri1->tex1 = texcoors[faces[i].texcoors[0]-1];
	tri1->tex2 = texcoors[faces[i].texcoors[1]-1];
	tri1->tex3 = texcoors[faces[i].texcoors[2]-1];
	//tri1->reflection = .1;
	tri1->refraction = 1.5;
    tri1->shadeType = 1;
	tri1->tex = 1;
	tri1->diffuse = Vector3d(0,0,.2);
    shapes.push_back(tri1);
	//delete tri1;
	//tri1 = NULL;
}
cout<<shapes.size()<<endl;*/



}

int main ( int argc, char** argv ){	
	srand ( time(NULL) );
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(Xmax, Ymax);
	glutInitWindowPosition(100,100);
    glutCreateWindow("Project 2");
	init();		
	glutDisplayFunc(display); 	
	glutMainLoop();		
	return 0;
}
