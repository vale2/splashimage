#ifndef GRAPHUTILMOD
#define GRAPHUTILMOD
#define NOREPEAT 1
void *__gxx_personality_v0;
int msgid;


#include"SDL/SDL.h"

#include"SDL/SDL.h"
#include "SDL/SDL_image.h"

#include"stdlib.h"
#include"time.h"
#include"math.h"
#include <GL/glew.h>
#include <GL/glu.h>

#ifndef MYGLMOD
#define MYGLMOD
void init_GL(){
    GLenum err = glewInit();
    if (GLEW_OK != err){
        printf("Error: %s\n",(char*) glewGetErrorString(err));
    }else{
       printf( "Status: Using GLEW %s\n",(char*) glewGetString(GLEW_VERSION));
    }
}
#endif


int GLH,GLW;
int GLX0,GLY0;
int H,W;
GLuint imgs[4094];
SDL_Surface *sdl_imgs[4094];
double aspect[4094];
int img_renu[4096];
int n_imgs=0;

void shuffleImages(){
    int i,j,t;
    for(i=0;i<n_imgs;i++){
        img_renu[i]=i;
    }
    if(n_imgs)
        for(i=0;i<n_imgs;i++){
            j=rand()%n_imgs;
            t=img_renu[i];
            img_renu[i]=img_renu[j];
            img_renu[j]=t;
        }

}

GLuint getTex(double a){
        if(n_imgs < 4093){
            glGenTextures(1, imgs + n_imgs++);
        }
    aspect[n_imgs-1] = a;
    return imgs[ n_imgs-1 ];
    
}


int getNtex(double a){
        if(n_imgs < 4093){
            n_imgs++;
        }
    aspect[n_imgs-1] = a;
    return  n_imgs-1 ;
    
}

GLuint getGlFormat(SDL_PixelFormat*f){
    if (f==NULL)
        return -1;
    if(f->BytesPerPixel == 3){
        if(f-> Rmask == 0xFF && f-> Gmask == 0xFF00 && f-> Bmask == 0xFF0000){    
            return GL_RGB;
        }
        if(f-> Bmask == 0xFF && f-> Gmask == 0xFF00 && f-> Rmask == 0xFF0000){    
            return GL_BGR;
        }
    }
    
    if(f->BytesPerPixel == 4){
        if(f-> Rmask == 0xFF && f-> Gmask == 0xFF00 && f-> Bmask == 0xFF0000){    
            return GL_RGBA;
        }
        if(f-> Bmask == 0xFF && f-> Gmask == 0xFF00 && f-> Rmask == 0xFF0000){    
            return GL_BGRA;
        }
    }
    return -1;
}
void image2gl(SDL_Surface *im){
    if(im->h == 0)
        return;
    GLuint  tex = getTex(double(im->w)/im->h);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, im->w, im->h,
            0, getGlFormat(im->format), GL_UNSIGNED_BYTE, im->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void image2cache(SDL_Surface *im){
    if(im->h == 0)
        return;
    int  ntex = getNtex(double(im->w)/im->h);
    sdl_imgs[ntex] = im;
}
/*

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, im->w, im->h,
            0, getGlFormat(im->format), GL_UNSIGNED_BYTE, im->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


*/

void initGl(){
    const SDL_VideoInfo* myPointer;
    SDL_Init( SDL_INIT_EVERYTHING );
    IMG_Init( IMG_INIT_JPG |  IMG_INIT_PNG);
    

    myPointer = SDL_GetVideoInfo();
    srand (time(NULL));
    char buf[1024];
    sprintf(buf,"SDL_VIDEO_WINDOW_POS=%d,%d",GLX0,GLY0);
    SDL_putenv((char*)buf);
    printf("Set sdlENV (%s)\n",buf);

     if(GLW<=0 || GLH<=0){
         W=myPointer->current_w;
         H=myPointer->current_h;
         GLW=W;
         GLH=H;
     }
     printf("Making Gl surface %dx%d\n",GLW,GLH);

     SDL_Surface *glScreen = NULL ;
        
     glScreen = SDL_SetVideoMode(GLW ,GLH , 32, SDL_HWSURFACE |  SDL_NOFRAME|SDL_GL_DOUBLEBUFFER |SDL_OPENGL );     
     SDL_ShowCursor(0);

     init_GL();
     glViewport(0,0, GLW, GLH);
    glMatrixMode(GL_PROJECTION);
//        glPushMatrix();
        glLoadIdentity();
        glOrtho(0.0f,  GLW, GLH, 0.0f, -1.0f, 1.0f);
       
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
//      
     glEnable ( GL_TEXTURE_2D );
     glEnable (GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void feet_tex_to_screen(double a,double splash){
        if( a >= 1 ){
            glBegin(GL_TRIANGLE_STRIP);
                glTexCoord2f (0.0,0.0); glVertex2f (0.0,GLH/2 - GLH / a /2);
                glTexCoord2f (0.0,1.0); glVertex2f (0.0,GLH/2 + GLH / a /2 );
                glTexCoord2f (1.0,0.0); glVertex2f (0+GLW,GLH/2 - GLH / a /2);
                glTexCoord2f (1.0,1.0); glVertex2f (0+GLW,GLH/2 + GLH / a /2 );
           glEnd();
           glDisable ( GL_TEXTURE_2D );
/*           glColor4f(splash,splash,splash,1);
           glBegin(GL_TRIANGLE_STRIP);
                glVertex2f (0.0,0.0);
                glVertex2f (0.0,GLH/2 - GLH / a /2);
                glVertex2f (GLW,0);
                glVertex2f (GLW,GLH/2 - GLH / a /2 );
           glEnd();
           glBegin(GL_TRIANGLE_STRIP);
                glVertex2f (0.0,GLH);
                glVertex2f (0.0,GLH/2 + GLH / a /2);
                glVertex2f (GLW,GLH);
                glVertex2f (GLW,GLH/2 + GLH / a /2 );
           glEnd();
*/
        }else{
            glBegin(GL_TRIANGLE_STRIP);
                glTexCoord2f (0.0,0.0); glVertex2f (GLW/2 - GLW * a / 2,0.0);
                glTexCoord2f (0.0,1.0); glVertex2f (GLW/2 - GLW * a / 2,0+GLH);
                glTexCoord2f (1.0,0.0); glVertex2f (GLW/2 + GLW * a / 2,0.0);
                glTexCoord2f (1.0,1.0); glVertex2f (GLW/2 + GLW * a / 2,GLH);
           glEnd();
           glDisable ( GL_TEXTURE_2D );
  /*         glColor4f(splash,splash,splash,1);

            glBegin(GL_TRIANGLE_STRIP);
                glVertex2f (0,0.0);
                glVertex2f (GLW/2 - GLW * a / 2,0);
                glVertex2f (0,GLH);
                glVertex2f (GLW/2 - GLW * a / 2,GLH);
           glEnd();
            glBegin(GL_TRIANGLE_STRIP);
                glVertex2f (GLW,0.0);
                glVertex2f (GLW/2 + GLW * a / 2,0);
                glVertex2f (GLW,GLH);
                glVertex2f (GLW/2 + GLW * a / 2,GLH);
           glEnd();*/

        }
        glColor4f(1,1,1,splash);
            glBegin(GL_TRIANGLE_STRIP);
                 glVertex2f (0,0.0);
                 glVertex2f (0,0+GLH);
                 glVertex2f (GLW,0.0);
                 glVertex2f (GLW,GLH);
           glEnd();

}
void feet2_tex_to_screen(double a,double splash){
        
//        a = picw/pich     *    sh/sw
        double w,h;
         
        if( a >= 1 ){
         
            w = 1/a;
            glBegin(GL_TRIANGLE_STRIP);
                glTexCoord2f ( 0.5-w/2  ,0); glVertex2f (0,0);
                glTexCoord2f ( 0.5-w/2   ,1); glVertex2f (0,GLH);
                glTexCoord2f (0.5+w/2,.0); glVertex2f (GLW,0);
                glTexCoord2f (0.5+w/2,1.0); glVertex2f (GLW,GLH);
           glEnd();
           glDisable ( GL_TEXTURE_2D );
        }else{
            h=a;
          glBegin(GL_TRIANGLE_STRIP);
                glTexCoord2f (0,0.5-h/2); glVertex2f (0,0);
                glTexCoord2f (0,0.5+h/2); glVertex2f (0,GLH);
                glTexCoord2f (1,0.5-h/2); glVertex2f (GLW,0);
                glTexCoord2f (1,0.5+h/2); glVertex2f (GLW,GLH);
          glEnd();
           glDisable ( GL_TEXTURE_2D );
        }

        glColor4f(1,1,1,splash);
            glBegin(GL_TRIANGLE_STRIP);
                 glVertex2f (0,0.0);
                 glVertex2f (0,0+GLH);
                 glVertex2f (GLW,0.0);
                 glVertex2f (GLW,GLH);
           glEnd();

}

void showImage(int n,double splash){
        n = img_renu [n];
        glEnable ( GL_TEXTURE_2D );
//        glClearColor(splash, splash, splash, 1.0f);
        glClearColor(0,0,0, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        if(!GLLOWMEM){
            glBindTexture(GL_TEXTURE_2D, imgs[n]);
        }else{
             glBindTexture(GL_TEXTURE_2D, imgs[0]);
             glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sdl_imgs[n]->w, sdl_imgs[n]->h,
                        0, getGlFormat(sdl_imgs[n]->format), GL_UNSIGNED_BYTE, sdl_imgs[n]->pixels);
             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
             glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
       
       // glColor4f(1,1,1,1-splash);
        glColor4f(1,1,1,1);
        double a;
        a = aspect [n] * GLH / GLW  * aspect_coef;
        
        
        if(pic_feet == 0){
            feet_tex_to_screen(a,splash);       
        }else{
            feet2_tex_to_screen(a,splash);       
        }
       SDL_GL_SwapBuffers();
      

}

void showMult(double a){
    double op;
    op=sin(fabs((a-trunc(a)-0.5))*2*M_PI);
    
    
    
    glDisable ( GL_TEXTURE_2D );
    glClearColor(0, 0, 0, 1.0f);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glColor4f(1,1,1,op);
    glBegin(GL_TRIANGLE_STRIP);
         glVertex2f (GLW/2 - GLW/(25),GLH/2 - GLH / (25));
         glVertex2f (GLW/2 - GLW/(25),GLH/2 + GLH / (25) );
         glVertex2f (GLW/2 + GLW/(25),GLH/2 - GLH / (25));
         glVertex2f (GLW/2 + GLW/(25),GLH/2 + GLH / (25) );
    glEnd();
    
    
    
    SDL_GL_SwapBuffers();
    glEnable ( GL_TEXTURE_2D );

}


int readKey(){
   SDL_Event event;
   if(SDL_PollEvent(&event)){
        if(event.type == SDL_KEYDOWN) {
            return event.key.keysym.scancode;
        
        }
   
   }     
   
   return -1;
}

#endif
