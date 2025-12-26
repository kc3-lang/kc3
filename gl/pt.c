#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "../libkc3/kc3.h"
#include "../image/image.h"
#include "dvec3.h"
#include "pt.h"

s_dvec3 * pt_radiance (const s_list *scene, const s_dray *ray,
                       u32 depth, s_dvec3 *dest)
{
  (void) scene;
  (void) ray;
  (void) depth;
  (void) dest;
  return NULL;
  /*
  f64 t;                               // distance to intersection
  int id=0;                               // id of intersected object
  if (!intersect(r, t, id)) return s_dvec3(); // if miss, return black
  const Sphere &obj = spheres[id];        // the hit object
  s_dvec3 x=r.o+r.d*t, n=(x-obj.p).norm(), nl=n.dot(r.d)<0?n:n*-1, f=obj.c;
  f64 p = f.x>f.y && f.x>f.z ? f.x : f.y>f.z ? f.y : f.z; // max refl
  if (++depth>5) if (erand48()<p) f=f*(1/p); else return obj.e; //R.R.
  if (obj.refl == DIFF){                  // Ideal DIFFUSE reflection
    f64 r1=2*M_PI*erand48(), r2=erand48(), r2s=sqrt(r2);
    s_dvec3 w=nl, u=((fabs(w.x)>.1?s_dvec3(0,1):s_dvec3(1))%w).norm(), v=w%u;
    s_dvec3 d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).norm();
    return obj.e + f.mult(radiance(Ray(x,d),depth));
  } else if (obj.refl == SPEC)            // Ideal SPECULAR reflection
    return obj.e + f.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth));
  Ray reflRay(x, r.d-n*2*n.dot(r.d));     // Ideal dielectric REFRACTION
  bool into = n.dot(nl)>0;                // Ray from outside going in?
  f64 nc=1, nt=1.5, nnt=into?nc/nt:nt/nc, ddn=r.d.dot(nl), cos2t;
  if ((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)    // Total internal reflection
    return obj.e + f.mult(radiance(reflRay,depth));
  s_dvec3 tdir = (r.d*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).norm();
  f64 a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n));
  f64 Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
  return obj.e + f.mult(depth>2 ? (erand48()<P ?   // Russian roulette
    radiance(reflRay,depth)*RP:radiance(Ray(x,tdir),depth)*TP) :
    radiance(reflRay,depth)*Re+radiance(Ray(x,tdir),depth)*Tr);
  */
}

s_image * pt_render_image (s_image *image, const s_tag *tag_w,
                           const s_tag *tag_h, s_tag *tag_samples,
                           p_list *scene)
{
  s_dvec3 *c = NULL;
  s_dray cam;
  s_dvec3 cx;
  s_dvec3 cy;
  s_dvec3 d;
  s_dvec3 d_140;
  s_dvec3 du;
  s_dvec3 dv;
  f64 dx;
  f64 dy;
  uw h;
  s_dvec3 r;
  s_dvec3 r_rad;
  f64 r1;
  f64 r2;
  f64 random;
  s_dray ray;
  u32 s;
  u32 samples;
  u8 sx;
  u8 sy;
  const s_sym *sym_U32 = &g_sym_U32;
  const s_sym *sym_Uw = &g_sym_Uw;
  s_image tmp = {0};
  f64 u;
  f64 v;
  uw w;
  uw x;
  uw y;
  assert(image);
  assert(scene);
  u32_init_cast(&samples, &sym_U32, tag_samples);
  uw_init_cast(&w, &sym_Uw, tag_w);
  uw_init_cast(&h, &sym_Uw, tag_h);
  if (! image_init_alloc(&tmp, w, h, 3, 3))
    return NULL;
  cam.origin.x = 50;
  cam.origin.y = 52;
  cam.origin.z = 295.6;
  cam.direction.x = 0;
  cam.direction.y = -0.042612;
  cam.direction.z = -1;
  dvec3_normalize(&cam.direction, &cam.direction);
  cx.x = w * 0.5135 / h;
  cx.y = 0;
  cx.z = 0;
  dvec3_cross(&cx, &cam.direction, &cy);
  dvec3_normalize(&cy, &cy);
  cy.x *= 0.5135;
  cy.y *= 0.5135;
  cy.z *= 0.5135;
  if (! (c = alloc(w * h * sizeof(s_dvec3))))
    return NULL;
  y = 0;
  while (y < h) {
    fprintf(stderr,"\rRendering (%d spp) %5.2f%%",
            samples * 4, 100.0 * y / (h - 1));
    x = 0;
#pragma omp parallel for schedule(dynamic, 1) private(r)
    while (x < w) {
      sy = 0;
      int i = (h - y - 1) * w + x;
      while (sy < 2) {
        sx = 0;
        while (sx < 2) {
          s = 0;
          while (s < samples) {
            f64_random(&random);
            r1 = 2 * random;
            dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
            f64_random(&random);
            r2 = 2 * random;
            dy = r2 < 1 ? sqrt(r2)-1: 1-sqrt(2-r2);
            u = ((sx + 0.5 + dx) / 2.0 + x) / w - 0.5;
            v = ((sy + 0.5 + dy) / 2.0 + y) / h - 0.5;
            dvec3_mul(&cx, u, &du);
            dvec3_mul(&cy, v, &dv);
            dvec3_add(&du, &dv, &d);
            dvec3_add(&d, &cam.direction, &d);
            dvec3_mul(&d, 140, &d_140);
            dvec3_add(&cam.origin, &d_140, &ray.origin);
            dvec3_normalize(&d, &ray.direction);
            pt_radiance(*scene, &ray, 0, &r_rad);
            dvec3_mul(&r_rad, 1.0 / samples, &r_rad);
            dvec3_add(&r, &r_rad, &r);
            s++;
          }
          dvec3_clamp(&r, &r);
          dvec3_mul(&r, 0.25, &r);
          dvec3_add(c + i, &r, c + i);
          sx++;
        }
        sy++;
      }
      x++;
    }
    y++;
  }
  *image = tmp;
  return image;
}

// returns distance or 0 if miss
f64 pt_sphere_intersect (const s_pt_sphere *s, const s_dray *r)
{
  s_dvec3 rs;
  f64 t;
  f64 b;
  f64 det;
  f64 eps = 1e-4;
  dvec3_sub(&s->center, &r->origin, &rs);
  b = dvec3_dot(&rs, &r->direction);
  det = b * b - dvec3_dot(&rs, &rs) + s->radius * s->radius;
  if (det < 0)
    return 0;
  det = sqrt(det);
  t = b - det;
  if (t > eps)
    return t;
  t = b + det;
  if (t > eps)
    return t;
  return 0;
}

#if 0

struct Vec {        // Usage: time ./smallpt 5000 && xv image.ppm
  double x, y, z;                  // position, also color (r,g,b)
  Vec(double x_=0, double y_=0, double z_=0){ x=x_; y=y_; z=z_; }
  Vec operator+(const Vec &b) const { return Vec(x+b.x,y+b.y,z+b.z); }
  Vec operator-(const Vec &b) const { return Vec(x-b.x,y-b.y,z-b.z); }
  Vec operator*(double b) const { return Vec(x*b,y*b,z*b); }
  Vec mult(const Vec &b) const { return Vec(x*b.x,y*b.y,z*b.z); }
  Vec& norm(){ return *this = *this * (1/sqrt(x*x+y*y+z*z)); }
  double dot(const Vec &b) const { return x*b.x+y*b.y+z*b.z; } // cross:
  Vec operator%(Vec&b){return Vec(y*b.z-z*b.y,z*b.x-x*b.z,x*b.y-y*b.x);}
};
struct Ray { Vec o, d; Ray(Vec o_, Vec d_) : o(o_), d(d_) {} };
enum Refl_t { DIFF, SPEC, REFR };  // material types, used in radiance()
struct Sphere {
  double rad;       // radius
  Vec p, e, c;      // position, emission, color
  Refl_t refl;      // reflection type (DIFFuse, SPECular, REFRactive)
  Sphere(double rad_, Vec p_, Vec e_, Vec c_, Refl_t refl_):
    rad(rad_), p(p_), e(e_), c(c_), refl(refl_) {}
};
Sphere spheres[] = {//Scene: radius, position, emission, color, material
  Sphere(1e5, Vec( 1e5+1,40.8,81.6), Vec(),Vec(.75,.25,.25),DIFF),//Left
  Sphere(1e5, Vec(-1e5+99,40.8,81.6),Vec(),Vec(.25,.25,.75),DIFF),//Rght
  Sphere(1e5, Vec(50,40.8, 1e5),     Vec(),Vec(.75,.75,.75),DIFF),//Back
  Sphere(1e5, Vec(50,40.8,-1e5+170), Vec(),Vec(),           DIFF),//Frnt
  Sphere(1e5, Vec(50, 1e5, 81.6),    Vec(),Vec(.75,.75,.75),DIFF),//Botm
  Sphere(1e5, Vec(50,-1e5+81.6,81.6),Vec(),Vec(.75,.75,.75),DIFF),//Top
  Sphere(16.5,Vec(27,16.5,47),       Vec(),Vec(1,1,1)*.999, SPEC),//Mirr
  Sphere(16.5,Vec(73,16.5,78),       Vec(),Vec(1,1,1)*.999, REFR),//Glas
  Sphere(600, Vec(50,681.6-.27,81.6),Vec(12,12,12),  Vec(), DIFF) //Lite
};
inline double clamp(double x){ return x<0 ? 0 : x>1 ? 1 : x; }
inline int toInt(double x){ return int(pow(clamp(x),1/2.2)*255+.5); }
inline bool intersect(const Ray &r, double &t, int &id){
  double n=sizeof(spheres)/sizeof(Sphere), d, inf=t=1e20;
  for(int i=int(n);i--;) if((d=spheres[i].intersect(r))&&d<t){t=d;id=i;}
  return t<inf;
}
Vec radiance(const Ray &r, int depth){
  double t;                               // distance to intersection
  int id=0;                               // id of intersected object
  if (!intersect(r, t, id)) return Vec(); // if miss, return black
  const Sphere &obj = spheres[id];        // the hit object
  Vec x=r.o+r.d*t, n=(x-obj.p).norm(), nl=n.dot(r.d)<0?n:n*-1, f=obj.c;
  double p = f.x>f.y && f.x>f.z ? f.x : f.y>f.z ? f.y : f.z; // max refl
  if (++depth>5) if (erand48()<p) f=f*(1/p); else return obj.e; //R.R.
  if (obj.refl == DIFF){                  // Ideal DIFFUSE reflection
    double r1=2*M_PI*erand48(), r2=erand48(), r2s=sqrt(r2);
    Vec w=nl, u=((fabs(w.x)>.1?Vec(0,1):Vec(1))%w).norm(), v=w%u;
    Vec d = (u*cos(r1)*r2s + v*sin(r1)*r2s + w*sqrt(1-r2)).norm();
    return obj.e + f.mult(radiance(Ray(x,d),depth));
  } else if (obj.refl == SPEC)            // Ideal SPECULAR reflection
    return obj.e + f.mult(radiance(Ray(x,r.d-n*2*n.dot(r.d)),depth));
  Ray reflRay(x, r.d-n*2*n.dot(r.d));     // Ideal dielectric REFRACTION
  bool into = n.dot(nl)>0;                // Ray from outside going in?
  double nc=1, nt=1.5, nnt=into?nc/nt:nt/nc, ddn=r.d.dot(nl), cos2t;
  if ((cos2t=1-nnt*nnt*(1-ddn*ddn))<0)    // Total internal reflection
    return obj.e + f.mult(radiance(reflRay,depth));
  Vec tdir = (r.d*nnt - n*((into?1:-1)*(ddn*nnt+sqrt(cos2t)))).norm();
  double a=nt-nc, b=nt+nc, R0=a*a/(b*b), c = 1-(into?-ddn:tdir.dot(n));
  double Re=R0+(1-R0)*c*c*c*c*c,Tr=1-Re,P=.25+.5*Re,RP=Re/P,TP=Tr/(1-P);
  return obj.e + f.mult(depth>2 ? (erand48()<P ?   // Russian roulette
    radiance(reflRay,depth)*RP:radiance(Ray(x,tdir),depth)*TP) :
    radiance(reflRay,depth)*Re+radiance(Ray(x,tdir),depth)*Tr);
}
int main(int argc, char *argv[]){
  int w=1024, h=768, samps = argc==2 ? atoi(argv[1])/4 : 1; // # samples
  Ray cam(Vec(50,52,295.6), Vec(0,-0.042612,-1).norm()); // cam pos, dir
  Vec cx=Vec(w*.5135/h), cy=(cx%cam.d).norm()*.5135, r, *c=new Vec[w*h];
#pragma omp parallel for schedule(dynamic, 1) private(r)       // OpenMP
  for (int y=0; y<h; y++){                       // Loop over image rows
    fprintf(stderr,"\rRendering (%d spp) %5.2f%%",samps*4,100.*y/(h-1));
    for (unsigned short x=0, Xi[3]={0,0,y*y*y}; x<w; x++)   // Loop cols
      for (int sy=0, i=(h-y-1)*w+x; sy<2; sy++)     // 2x2 subpixel rows
        for (int sx=0; sx<2; sx++, r=Vec()){        // 2x2 subpixel cols
          for (int s=0; s<samps; s++){
            double r1=2*erand48(Xi), dx=r1<1 ? sqrt(r1)-1: 1-sqrt(2-r1);
            double r2=2*erand48(Xi), dy=r2<1 ? sqrt(r2)-1: 1-sqrt(2-r2);
            Vec d = cx*( ( (sx+.5 + dx)/2 + x)/w - .5) +
                    cy*( ( (sy+.5 + dy)/2 + y)/h - .5) + cam.d;
            r = r + radiance(Ray(cam.o+d*140,d.norm()),0,Xi)*(1./samps);
          } // Camera rays are pushed ^^^^^ forward to start in interior
          c[i] = c[i] + Vec(clamp(r.x),clamp(r.y),clamp(r.z))*.25;
        }
  }
  FILE *f = fopen("image.ppm", "w");         // Write image to PPM file.
  fprintf(f, "P3\n%d %d\n%d\n", w, h, 255);
  for (int i=0; i<w*h; i++)
    fprintf(f,"%d %d %d ", toInt(c[i].x), toInt(c[i].y), toInt(c[i].z));
}

#endif
