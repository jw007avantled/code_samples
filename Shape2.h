#ifndef SHAPE_H
#define SHAPE_H

#include "Vector.h"
#include "Parameter.h"
#include <algorithm>
#include "Shader.h"



unsigned char *tex;

int count = 0;
float A, B, C, tH;
Vector3d c, newP, newPC, npe, pH, nH, nLH, mH;
Vector3d Vview(0,0,1);
Vector3d Vup(0,1,0);
Vector3d n2 = Vview/Vview.norm();
Vector3d crossV = Vview % Vup;
Vector3d n0 = crossV/crossV.norm();
Vector3d n1 = n0 % n2;


Vector3d Vview2(0,0,1);
Vector3d Vup2(0,1,0);
Vector3d n02 = Vview2/Vview2.norm();
Vector3d crossV2 = Vup2 % Vview2;
Vector3d n00 = crossV2/crossV2.norm();
Vector3d n01 = n02 % n00;

param quadric(Vector3d a2, Vector3d a1, Vector3d s, int a0, Vector3d center, Vector3d Pe, Vector3d npe){

	param shape;
		tH = 0.0;
		pH  = 0.0;
	   
		Vector3d n[]  = {n00,n01,n02};
		Vector3d V;

        A = 0.0;
        B = 0.0;
        C = 0.0;
        V = 0.0;

		for(int i = 0; i < 3; i++){
			A += a2[i] * (((n[i] * npe) * (n[i] * npe)) / (s[i] * s[i]));
                        B += (a2[i] * (2 * (((n[i] * npe) * (n[i] * (Pe - center))) / (s[i] * s[i])))) + (a1[i] * ((n[i] * npe) / s[i]));
                        C += a2[i] * (((n[i] * (Pe - center)) * (n[i] * (Pe - center))) / (s[i] * s[i])) + a1[i] * (n[i] * (Pe - center) / s[i]);
			
		}
                C = C + a0;
               // if(count < 1){
               //         std::cout<<B<<std::endl;
                //        count++;
               // }
                float f = ((B * B) - (4 * A * C));
                float sq =  sqrt(f);
				if(f > 0){
					float t0 = (-B - sq) / (float)(2 * A);
					float t1 = (-B + sq) / (float)(2 * A);


					if(t0 < t1){
						tH = t0;
					}
					else{
						tH = t1;
					}

					pH = Pe + (npe * tH);


					for(int i = 0; i < 3; i++){
						V = V + (a2[i] * (2 * (n[i] / s[i]) * ((n[i] / s[i]) * (pH - center))) + a1[i] * (n[i] / s[i]));
					}
		}

				shape.normal = V/V.norm();
				shape.surfacePoint = pH;
				shape.tVal = tH;

				return shape;
}

class Face{
    public:
        Vector3d vertices, normal;

        Face(Vector3d v, Vector3d n){
            vertices = v;
            normal = n;
        }

        Face(void){
            vertices.set(0,0,0);
            normal.set(0,0,0);
        }

};

class Texture{

public:
	unsigned char *tex;
	int texXmax, texYmax;

	Texture(int newtexXmax, int newtexYmax, unsigned char *newtex){

		texXmax=newtexXmax;
		texYmax=newtexYmax;
		tex = new unsigned char[texXmax * texYmax * 3]; 
		tex = newtex;
	}

	Texture(void){
		texXmax=0;
		texYmax=0;
		tex = new unsigned char[texXmax * texYmax * 3]; 			
	}		
};


class Shape{

public:
	Vector3d ambient, diffuse, specular;
	float reflection;	
	int tex, shadeType;

	Shape(Vector3d Newambient, Vector3d Newdiffuse, Vector3d Newspecular, float Newreflection){

		ambient = Newambient;		

		diffuse = Newdiffuse;		

		specular = Newspecular;		

		reflection = Newreflection;		
	}

	Shape(void){
		ambient.set(0,0,0);		

		diffuse.set(0,0,0);		

		specular.set(0,0,0);	

		reflection = 0.0;	

		tex=0;

		shadeType=0;
	}	

    virtual param intersection(Vector3d Pe, Vector3d npe, int I, int J)=0;
    virtual Vector3d texture(Vector3d nH, Vector3d pH, Vector3d lightColor, Vector3d light, Texture t)=0;
	
};


class infinitePlane: public Shape{

public:
	Vector3d point, normal, texVup, texView;
	float a,b,c;
	int s0,s1;

	infinitePlane( Vector3d NewPoint, Vector3d NewNormal, float newa, float newb, float newc, Vector3d newtexview, Vector3d newTexup, int news0, int news1){
		point = NewPoint;
		normal = NewNormal;		
		a=newa;
		b=newb;
		c=newc;		
		texVup=newTexup;
		texView = newtexview;
		s0 = news0;
		s1 = news1;
	}

	infinitePlane(void){
		point.set(0,0,0);
		normal.set(0,0,0);	
		a = 0;
		b = 0;
		c = 0;		
		texView.set(0,0,0);
		texVup.set(0,0,0);
		s0=0;
		s1=0;
	}	

        virtual param intersection(Vector3d Pe, Vector3d npe, int I, int J){
		param plane;	
		//plane intersect
		float denom = this->normal * npe;
		//c.set(0,0,0);
		tH = 0.0;
		if(denom < 0){			
			tH = this->normal * (this->point - Pe)/denom;
			pH = Pe + npe * tH;			
			mH = this->diffuse;
			
			//c = lambert(this->normal,pH,mH);
		}

		if(this->normal * (Pe - this->point) < 0){
			//std::cout<<"uhuh"<<std::endl;
			//system("PAUSE");
		}
		
		/*param s = quadric(Vector3d(0,1,0),Vector3d(0,0,0),Vector3d(this->a,this->b,this->c),0,this->point,Pe,npe);		
		plane.tVal = s.tVal;
		plane.normal = s.normal;
		plane.surfacePoint = this->normal;       
		plane.shape = this;
		*(plane.shape) = *(this);
                */


		plane.surfacePoint = pH;	     
		plane.normal = this->normal;		
		plane.tVal = tH;
		//this->color = c;
		plane.shape = this;
		*(plane.shape) = *(this);

		return plane;
	}

	virtual Vector3d texture(Vector3d nH, Vector3d pH, Vector3d lightColor, Vector3d light, Texture t){
		Vector3d c;		

		Vector3d texN2 = texView/texView.norm();
		Vector3d texcrossV = texView % texVup;
		Vector3d texN0 = texcrossV/texcrossV.norm();
		Vector3d texN1 = texN0 % texN2;	

		float u = (pH - this->point) * texN0 / s0;
		float v = (pH - this->point) * texN1 / s1;

		u = u - (int)u;
		if(u < 0) u = 1 + u;
		v = v - (int)v;
		if(v < 0) v = 1 + v;

		int x = t.texXmax * u;
		int y = t.texYmax * v;

		int pixel = (y * t.texXmax + x) * 3; 
		
		c[0] = t.tex[pixel];
		pixel++;
		c[1] = t.tex[pixel];
		pixel++;
		c[2] = t.tex[pixel];		

		c[0] = c[0]/255; 
		c[1] = c[1]/255; 
		c[2] = c[2]/255; 

		return c;
	}

};



class Sphere: public Shape
{
public:
	Vector3d center, texVup, texView;
	float radius;	

	Sphere(Vector3d NewCenter, float NewRadius, Vector3d newtexview, Vector3d newTexup){
		center = NewCenter;		
		radius = NewRadius;
		texView = newtexview;
		texVup = newTexup;
	}

	Sphere(void){
		center.set(0,0,0);	
		texView.set(0,0,0);
		texVup.set(0,0,0);
		radius = 0.0;
	}

        virtual param intersection(Vector3d Pe, Vector3d npe, int I, int J){
		param sphere;	
		
		/*//sphere intersect
		newP =  Pe - this->center;
		newPC =  this->center - Pe;		
        B = npe * newPC;
		C = (newPC * newPC) - (this->radius * this->radius);      
		float delta = (B*B) - C;
		tH = 0.0;
		c.set(0,0,0);
		tH = B - sqrt(delta);
			std::cout<<tH<<std::endl;
        if(delta>=0.0 && B>=0.0){			
			tH = B - sqrt(delta);
			std::cout<<tH<<std::endl;
			pH = Pe + npe * tH;
			nH = (pH - this->center)/this->radius;				
			mH = this->diffuse;	
			this->intersected = 1;
			//c = phong(nH,pH,mH,npe);
			//c.set(1,1,1);
        }
		
        if((newP * newP) - (this->radius * this->radius)<=0.0){
			//std::cout<<"uhoh"<<std::endl;
            //system("PAUSE");
        }*/

		param s = quadric(Vector3d(1,1,1),Vector3d(0,0,0),Vector3d(this->radius,this->radius,this->radius),-1,this->center,Pe,npe);		
		sphere.tVal = s.tVal;
		sphere.normal = s.normal;
		sphere.surfacePoint = s.surfacePoint;       
		sphere.shape = this;
		*(sphere.shape) = *(this);

		/*sphere.surfacePoint = pH;
		sphere.normal = nH;
		sphere.tVal = tH;
		//this->color = c;
		sphere.shape = this;
		*(sphere.shape) = *(this);*/

		return sphere;
	}

	virtual Vector3d texture(Vector3d nH, Vector3d pH, Vector3d lightColor, Vector3d light, Texture t){
		Vector3d c;		

		Vector3d texN2 = texView / texView.norm();
		Vector3d texcrossV = texView % texVup;
		Vector3d texN0 = texcrossV / texcrossV.norm();
		Vector3d texN1 = texN0 % texN2;	

		float x = texN0 * (pH - center) / radius;
		float y = texN1 * (pH - center) / radius;
		float z = texN2 * (pH - center) / radius;
		float u,r;
		if(z!=1){
			 r = y / sqrt(1.0 - (z * z));
			 if(r>1) r = 1;
			 if(r<-1) r = -1;
			 u = acos(r) / (2.0 * PI);
		}
		float v = acos(z) / PI;

		if(x < 0) u = 1 - u;	

		//if(u < 0) std::cout<<"klasd"<<std::endl;

		int x2 = t.texXmax * u;
		int y2 = t.texYmax * v;

		int pixel = (y2 * t.texXmax + x2) * 3; 
		
		c[0] = t.tex[pixel];
		pixel++;
		c[1] = t.tex[pixel];
		pixel++;
		c[2] = t.tex[pixel];		

		c[0] = c[0] / 255; 
		c[1] = c[1] / 255; 
		c[2] = c[2] / 255; 

		return c;
	}
};

class infiniteCylinder: public Shape
{
public:
	Vector3d center, axis;
	float radius;

	infiniteCylinder( Vector3d NewCenter, Vector3d NewAxis, float NewRadius)
	{
		center=NewCenter;
		axis=NewAxis;
		radius=NewRadius;
	}

	infiniteCylinder(void)
	{
		center.set(0,0,0);
		axis.set(0,0,0);
		radius = 0;
	}

        param intersection(Vector3d Pe, Vector3d npe, int I, int J)
	{		
		/*//Ray newray;
		Vector3d ray = npe - (((npe - this->center) * this->axis) * this->axis);
		//Vector3d ray = subtractVect(r.direction,scaleVect(dotprod(subtractVect(r.direction,this->center),this->axis),this->axis));
		Vector3d newNpe = ray/ray.norm();
		//Vector3d newPe = subtractVect(r.position,scaleVect(dotprod(subtractVect(r.position,this->center),this->axis),this->axis));		 
		Vector3d newPe = Pe - (((Pe - this->center) * this->axis) * this->axis);
		Sphere* sphere = new Sphere(this->center,this->radius);
		param cylinder = sphere->intersection(newPe,newNpe);	
		
		cylinder.shape = this;
		*(cylinder.shape) = *(this);*/

		param cylinder;

		param s = quadric(Vector3d(1,0,1),Vector3d(0,0,0),Vector3d(this->radius,this->radius,this->radius),-1,this->center,Pe,npe);		
		cylinder.tVal = s.tVal;
		cylinder.normal = s.normal;
		cylinder.surfacePoint = s.surfacePoint;       
		cylinder.shape = this;
		*(cylinder.shape) = *(this);

		return cylinder;
	}

};


class Ellipsoid: public Shape
{
public:
	Vector3d center, texVup, texView;
	float a,b,c;

	Ellipsoid( Vector3d NewCenter, float NewA, float NewB, float NewC, Vector3d newtexview, Vector3d newTexup)
	{
		center=NewCenter;		
		a=NewA;
		b=NewB;
		c=NewC;
		texView = newtexview;
		texVup = newTexup;
	}

	Ellipsoid(void)
	{
		center.set(0,0,0);		
		a = 0;
		b = 0;
		c = 0;
		texView.set(0,0,0);
		texVup.set(0,0,0);
	}

        param intersection(Vector3d Pe, Vector3d npe, int I, int J)
	{		
		param ellipsoid;		
		param s = quadric(Vector3d(1,1,1),Vector3d(0,0,0),Vector3d(this->a,this->b,this->c),-1,this->center,Pe,npe);		
		ellipsoid.tVal = s.tVal;
		ellipsoid.normal = s.normal;
		ellipsoid.surfacePoint = s.surfacePoint;       
		ellipsoid.shape = this;
		*(ellipsoid.shape) = *(this);

		return ellipsoid;
	}

	virtual Vector3d texture(Vector3d nH, Vector3d pH, Vector3d lightColor, Vector3d light, Texture t){
		Vector3d col;		

		Vector3d texN2 = texView / texView.norm();
		Vector3d texcrossV = texView % texVup;
		Vector3d texN0 = texcrossV / texcrossV.norm();
		Vector3d texN1 = texN0 % texN2;	

		float x = texN0 * (pH - center) / a;
		float y = texN1 * (pH - center) / b;
		float z = texN2 * (pH - center) / c;
		float u,r;
		if(z!=1){
			 r = y / sqrt(1.0 - (z * z));
			 if(r>1) r = 1;
			 if(r<-1) r = -1;
			 u = acos(r) / (2.0 * PI);
		}
		float v = acos(z) / PI;

		if(x < 0) u = 1 - u;	

		//if(u < 0) std::cout<<"klasd"<<std::endl;

		int x2 = t.texXmax * u;
		int y2 = t.texYmax * v;

		int pixel = (y2 * t.texXmax + x2) * 3; 
		
		col[0] = t.tex[pixel];
		pixel++;
		col[1] = t.tex[pixel];
		pixel++;
		col[2] = t.tex[pixel];		

		col[0] = col[0] / 255; 
		col[1] = col[1] / 255; 
		col[2] = col[2] / 255; 

		return col;
	}
};

class Cone: public Shape
{
public:
        Vector3d center, texVup, texView;
	float a,b,c;

        Cone( Vector3d NewCenter, float NewA, float NewB, float NewC,  Vector3d newtexview, Vector3d newTexup)
	{
		center=NewCenter;		
		a=NewA;
		b=NewB;
		c=NewC;
                texView = newtexview;
                texVup = newTexup;
	}

	Cone(void)
	{
		center.set(0,0,0);		
		a = 0;
		b = 0;
		c = 0;
                texView.set(0,0,0);
                texVup.set(0,0,0);
	}

        param intersection(Vector3d Pe, Vector3d npe, int I, int J)
	{		
		param cone;		
		param s = quadric(Vector3d(1,-1,1),Vector3d(0,0,0),Vector3d(this->a,this->b,this->c),0,this->center,Pe,npe);		
		cone.tVal = s.tVal;
		cone.normal = s.normal;
		cone.surfacePoint = s.surfacePoint;       
		cone.shape = this;
		*(cone.shape) = *(this);

		return cone;
	}

        virtual Vector3d texture(Vector3d nH, Vector3d pH, Vector3d lightColor, Vector3d light, Texture t){
                Vector3d col;

                Vector3d texN2 = texView / texView.norm();
                Vector3d texcrossV = texView % texVup;
                Vector3d texN0 = texcrossV / texcrossV.norm();
                Vector3d texN1 = texN0 % texN2;

                float x = texN0 * (pH - center) / a;
                float y = texN1 * (pH - center) / b;
                float z = texN2 * (pH - center) / c;

                float r = y / z;
                if(r < -1) r = -1;
                if(r > 1) r = 1;

                float u = (1 / (2 * PI)) * acos(r);
                float v = z - (int)z;

                if(x >= 0) u = 1 - u;

                //u = u - (int)u;
                if(u < 0) u = 1 + u;
               // v = v - (int)v;
                if(v < 0) v = 1 + v;

                //if(u < 0) std::cout<<"klasd"<<std::endl;

                int x2 = t.texXmax * u;
                int y2 = t.texYmax * v;

                int pixel = (y2 * t.texXmax + x2) * 3;

                col[0] = t.tex[pixel];
                pixel++;
                col[1] = t.tex[pixel];
                pixel++;
                col[2] = t.tex[pixel];

                col[0] = col[0] / 255;
                col[1] = col[1] / 255;
                col[2] = col[2] / 255;

                return col;
        }
};

class Hyperboloid: public Shape
{
public:
        Vector3d center, texVup, texView;
	float a,b,c;
	int type;

        Hyperboloid( Vector3d NewCenter, float NewA, float NewB, float NewC, int NewType,  Vector3d newtexview, Vector3d newTexup)
	{
		center=NewCenter;		
		a=NewA;
		b=NewB;
		c=NewC;
		type = NewType;
                texView = newtexview;
                texVup = newTexup;
	}

	Hyperboloid(void)
	{
		center.set(0,0,0);		
		a = 0;
		b = 0;
		c = 0;
                type = 0;
                texView.set(0,0,0);
                texVup.set(0,0,0);
	}

        param intersection(Vector3d Pe, Vector3d npe, int I, int J)
	{		
		param hyper;	
		int ty;
		if(type == 0) ty = -1;
		if(type == 1) ty = 1;	
		param s = quadric(Vector3d(1,-1,1),Vector3d(0,0,0),Vector3d(this->a,this->b,this->c),ty,this->center,Pe,npe);		
		hyper.tVal = s.tVal;
		hyper.normal = s.normal;
		hyper.surfacePoint = s.surfacePoint;       
		hyper.shape = this;
		*(hyper.shape) = *(this);

		return hyper;
	}

        virtual Vector3d texture(Vector3d nH, Vector3d pH, Vector3d lightColor, Vector3d light, Texture t){
                Vector3d col;

                Vector3d texN2 = texView / texView.norm();
                Vector3d texcrossV = texView % texVup;
                Vector3d texN0 = texcrossV / texcrossV.norm();
                Vector3d texN1 = texN0 % texN2;

                float x = texN0 * (pH - center) / a;
                float y = texN1 * (pH - center) / b;
                float z = texN2 * (pH - center) / c;

                if(type == 0){
                    float phi;// = asinh(z);
                    float r = y / cosh(phi);
                    if(r < 0) r = 0;
                    if(r > .5) r = .5;

                    float u = (1 / (2 * PI)) * acos(r);
                    float v = phi - (int)phi;

                    if(x < 0) u = 1 - u;

                    u = u - (int)u;
                    if(u < 0) u = 1 + u;
                     v = v - (int)v;
                    if(v < 0) v = 1 + v;

                    //if(u < 0) std::cout<<"klasd"<<std::endl;

                    int x2 = t.texXmax * u;
                    int y2 = t.texYmax * v;

                    int pixel = (y2 * t.texXmax + x2) * 3;

                    col[0] = t.tex[pixel];
                    pixel++;
                    col[1] = t.tex[pixel];
                    pixel++;
                    col[2] = t.tex[pixel];

                    col[0] = col[0] / 255;
                    col[1] = col[1] / 255;
                    col[2] = col[2] / 255;
                }

                 if(type == 1){
//		    if(z < 1) z = 1;	
                   float phi;// = asinh(z);		    

                    float r = y / cosh(phi);
                    if(r < -1) r = -1;
                    if(r > 1) r = 1;

                    float u = (1 / (2 * PI)) * acos(r);
                    float v = phi - (int)phi;

                    if(x  < 0) u = 1 - u;

                    u = u - (int)u;
                    if(u < 0) u = 1 + u;
                     v = v - (int)v;
                    if(v < 0) v = 1 + v;

                   // std::cout<<"klasd"<<std::endl;

                    int x2 = t.texXmax * u;
                    int y2 = t.texYmax * v;
		    
                    int pixel = (y2 * t.texXmax + x2) * 3;

                    col[0] = t.tex[pixel];
                    pixel++;
                    col[1] = t.tex[pixel];
                    pixel++;
                    col[2] = t.tex[pixel];

                    //std::cout<<" sdk"<<std::endl;

                    col[0] = col[0] / 255;
                    col[1] = col[1] / 255;
                    col[2] = col[2] / 255;
                }

                return col;
        }
};

class Triangle: public Shape
{
public:
        Vector3d texVup, texView, A, a0, a1, a2, vert1, vert2, vert3, norm1, norm2, norm3;
        param tri, inter;
        Vector3d p0, p1, p2, v1, v2, Ai, v0,n;
        int nona1, nona2;
        float u,v,f;

        Triangle(Vector3d v1, Vector3d v2, Vector3d v3, Vector3d n1, Vector3d n2, Vector3d n3)
        {
                vert1 = v1;
                vert2 = v2;
                vert3 = v3;
                norm1= n1;
                norm2= n2;
                norm3= n3;
        }

        Triangle(void)
        {                
                vert1.set(0,0,0);
                vert2.set(0,0,0);
                vert3.set(0,0,0);
                norm1.set(0,0,0);
                norm2.set(0,0,0);
                norm3.set(0,0,0);
        }

        param intersection(Vector3d Pe, Vector3d npe, int I, int J)
        {



                p0 = vert1;
                p1 = vert2;
                p2 = vert3;
				
				//p0.print();std::cout<<" ";
				//p1.print();std::cout<<" ";
				//p2.print();std::cout<<std::endl;


                v1 = p1 - p0;
                v2 = p2 - p0;

                v0 = v1 % v2;
                A = v0;
                n = v0 / v0.norm();

                infinitePlane* p = new infinitePlane(p0, n, 1, 1, 1, Vector3d(0,1,0), Vector3d(0,0,1), 2, 2);
                inter = p->intersection(Pe,npe, I,J);
                tri.normal = n;
                tri.surfacePoint = inter.surfacePoint;               
                delete p;
                Vector3d ph = tri.surfacePoint;

                a0 = (ph - p1) % (ph - p2);
                a1 = (ph - p2) % (ph - p0);
                a2 = (ph - p0) % (ph - p1);



                if(a1[0] != 0 && A[0] != 0) nona1 = 0;
                else if (a1[1] != 0 && A[1] != 0) nona1 = 1;
                else nona1 = 2;

                if(a2[0] != 0 && A[0] != 0) nona2 = 0;
                else if (a2[1] != 0 && A[1] != 0) nona2 = 1;
                else nona2 = 2;               

                u = a1[nona1] / A[nona1];
                v = a2[nona2] / A[nona2];
                f = (1-u-v);

               /*  if(I==350 && J==300){
                    a0.print();std::cout<<std::endl;
                    a1.print();std::cout<<std::endl;
                    a2.print();std::cout<<std::endl;
                    tri.normal.print();std::cout<<std::endl;
                    tri.surfacePoint.print();std::cout<<std::endl;
                    std::cout<<u<<" "<<v<<" "<<1-u-v<<" "<<f<<std::endl;
                }*/

                if((0<=u) && (0<=v) && (0<=f))  tri.tVal = inter.tVal;
                else tri.tVal = -1;

                tri.shape = this;
                *(tri.shape) = *(this);

                return tri;
        }

        virtual Vector3d texture(Vector3d nH, Vector3d pH, Vector3d lightColor, Vector3d light, Texture t){
                Vector3d col;

                Vector3d texN2 = texView / texView.norm();
                Vector3d texcrossV = texView % texVup;
                Vector3d texN0 = texcrossV / texcrossV.norm();
                Vector3d texN1 = texN0 % texN2;

                //float x = texN0 * (pH - center) / a;
                //float y = texN1 * (pH - center) / b;
                //float z = texN2 * (pH - center) / c;

                float u = a1[0]/A[0];

                float v = a2[0]/A[0];

                if(u >= 0 && v >= 0 && (1-u-v) >= 0){
                    //ph is inside
                }

                u = u - (int)u;
                if(u < 0) u = 1 + u;
                v = v - (int)v;
                if(v < 0) v = 1 + v;

                // std::cout<<"klasd"<<std::endl;

                int x2 = t.texXmax * u;
                int y2 = t.texYmax * v;

                int pixel = (y2 * t.texXmax + x2) * 3;

                col[0] = t.tex[pixel];
                pixel++;
                col[1] = t.tex[pixel];
                pixel++;
                col[2] = t.tex[pixel];

                //std::cout<<" sdk"<<std::endl;

                col[0] = col[0] / 255;
                col[1] = col[1] / 255;
                col[2] = col[2] / 255;

                return col;
        }
};

#endif
