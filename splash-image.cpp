int pic_feet=0;
double aspect_coef=1;
int GLLOWMEM=0;
int _halt =0;
#include"graphutil.cpp"
#include"stdio.h"
#include"math.h"
#include <sys/time.h> 

Uint32 GetTickCount(void){
    struct timeval t;
    gettimeofday( &t, NULL );
    return ((t.tv_sec) * 1000) + (t.tv_usec / 1000);
}

void printImageParam(SDL_Surface * im){
    printf("BitsPerPixel %d\nBytsPerPixel %d\nRmask %X\nGmask %X\nBmask %X\nAmask %X\n",(int) im->format->BitsPerPixel,(int) im->format->BytesPerPixel, im->format-> Rmask,im->format-> Gmask,im->format-> Bmask, im->format-> Amask);;

}

void trim(char *s){
    int i;
    for(i=0;*s;i++,s++);
    for(;(*s==10||*s==13||*s==0)&&i>0;i--,s--)*s=0;  
}
void loadImages(){
    FILE*f;
    char name[1024];
    SDL_Surface * img;
    f=fopen("images.txt","r");
    if(f!=NULL){
        while(!feof(f)){
            fgets(name,1024,f);
            trim(name);
            img = IMG_Load(name);
            if( img != NULL && getGlFormat(img->format) !=-1 ){
                printf("%s successfully loaded\n",name); 
                printImageParam(img);
                if(!GLLOWMEM){       
                    image2gl(img);
                    SDL_FreeSurface(img);
                }else{
                    if(n_imgs == 0)
                            glGenTextures(1, imgs);
                    image2cache(img);
                }
            }else{
                if(img == NULL){
                    printf("filed to load %s: %s\n",name,IMG_GetError());
                }else{
                    printf("filed to load %s: Can't transform it to openGl color format\n",name);
                    printImageParam(img);
                }
            }        
        }
        fclose(f);
    }
}
void oldloadImages(){
    FILE*f;
    char name[1024];
    SDL_Surface * img;
    f=fopen("images.txt","r");
    if(f!=NULL){
        while(!feof(f)){
            fscanf(f,"%1023s",name);
            img = IMG_Load(name);
            if( img != NULL){
                printf("%s successfully loaded\n",name); 
                printImageParam(img);       
                image2gl(img);
            }else{
                printf("filed to load %s: %s\n",name,IMG_GetError());
            }        
        }
        fclose(f);
    }
}


double zoom_value(double l,double r,double v){
    if(l>=r)
        return 1;
    if(v<l)
        return 0;
    if(v>r)
        return 1;
    
    return (v-l)/(r-l);


}
struct tpoint{
    double sec;
    double delay;
    double splash;
    double pow;
}tline[1024];
int n_tline=0;
int lastloop=-1;
int saveDelayi=0;
int savePowi=0;
int saveSplashi=0;

double getDelay(double sec){
    int i;
    if(tline[0].sec >= sec){
        return tline[0].delay; 
    }
    for(i=saveDelayi;i<n_tline && tline[i].sec < sec;i++);
    i--;
    if(i>=0 && i < n_tline - 1){
        double a;
        a=(sec - tline[i].sec ) /(tline[i+1].sec - tline[i].sec);
        // a = 0..1
        saveDelayi=i;
        return tline[i].delay*(1-a) + tline[i+1].delay*(a);
    }else if(n_tline>=1){
        return tline[n_tline-1].delay;
    }else{
        return 1;
    }
}
double getSplash(double sec){
    int i;
    if(tline[0].sec >= sec){
        return tline[0].splash; 
    }
    for(i=saveSplashi;i<n_tline && tline[i].sec < sec;i++);
    i--;
    if(i>=0 && i < n_tline - 1){
        double a;
        a=(sec - tline[i].sec ) /(tline[i+1].sec - tline[i].sec);
        // a = 0..1
        saveSplashi=i;
        return tline[i].splash*(1-a) + tline[i+1].splash*(a);
    }else if(n_tline>=1){
        return tline[n_tline-1].splash;
    }else{
        return 0.2;
    }
}
double getPow(double sec){
    int i;
    if(tline[0].sec >= sec){
        return tline[0].pow; 
    }
    for(i=savePowi;i<n_tline && tline[i].sec < sec;i++);
    i--;
    if(i>=0 && i < n_tline - 1){
        double a;
        a=(sec - tline[i].sec ) /(tline[i+1].sec - tline[i].sec);
        // a = 0..1
        savePowi=i;
        return tline[i].pow*(1-a) + tline[i+1].pow*(a);
    }else if(n_tline>=1){
        return tline[n_tline-1].pow;
    }else{
        return 1;
    }
}

double savesec = 0;
int savetp=0;
int saven=0;
void calcView(Uint32 t2,int &nr,double &ar){
    double tsec=t2/1000.;
    double sec=savesec;
    double sp2,delay;
    double a;
    int tp=savetp;
    int n=saven;
    int loop=0;
    while(tsec>sec){
        if(sec>tline[tp].sec){
            if(tp+1<n_tline){
                tp++;
            }
        }
       delay=getDelay(sec);
       n++;
       sec+=delay;
       if(n>=n_imgs){
            n-=n_imgs;
            loop++;
       }
    }
    n--;
    if(lastloop!=loop){
        shuffleImages();
        lastloop=loop;
    }
    sec-=delay;
    sp2 = getSplash(sec)/2;
    double p = getPow(sec);
    nr=n;
    a=(tsec-sec)/delay;
    if(a<sp2){
        a =1-a/sp2;
        ar=pow(a,p);
    }else
    if(a>1-sp2){
        a  = (a - (1 - sp2))/sp2;
        ar = pow(a,p);
    }else{
        ar=0;
    }
    savesec=sec;
    savetp=tp;
    saven=n;
}
double last=0;
void loadTimeLine(){
    FILE*f;
    double sec,delay,lsec,splash,p;
    lsec=-0.00000001;
    f=fopen("timeline.txt","r");
    if(f!=NULL){
        while(!feof(f)){
            fscanf(f,"%lf %lf %lf %lf",&sec,&delay,&splash,&p);
            if(sec>lsec && delay>0 && n_tline < 1024){
                if(delay <0.001 && splash < 0.0001 && p <0.0001){       

                    last=sec;

                }else{

                tline[n_tline].sec = sec;
                tline[n_tline].delay = delay;
                tline[n_tline].splash = splash;
                tline[n_tline].pow = p;
                n_tline++;
                lsec=sec;
                last = sec;
                }
            }
            
        }
        fclose(f);
    }

}
#include<string.h>
void parseParam( int argc, char*  args[]){
    int i;
    int w,h;
    int x0,y0;
    const char *help="usage:\nsplash-image       -window XRESxYRES      -position XPOSxYPOS        -aspect-coef ASPECT                                -no-load-to-video    -pic-feet\nprogram name        windows size           window position           aspect ratio for pixel                           in lack of videomemory use this to store images in RAM           0 - view all picture\n\nfiles:\n\ntimeline.txt\ntimepos1    delay1   splash_part_of_pic1  splash_gamma1\ntimepos2    delay2   splash_part_of_pic2  splash_gamma2\ntimepos3    delay3   splash_part_of_pic3  splash_gamma3\ntimepos4    delay4   splash_part_of_pic4  splash_gamma4\n...\n\nimages.txt\npath/to/image1\npath/to/image2\npath/to/image3\npath/to/image4\npath/to/image5\npath/to/image6\npath/to/image7\npath/to/image8\n...\n\nlog.txt\nprogram will put some to this file\n    \n\nExample:\nsplash-image -window 1024x768 -position 0x0 -aspect-coef 1 -no-load-to-video  -pic-feet 0\n    ";
    
    x0=0;
    y0=0;
    w=-1;
    h=-1;
    for(i=1;i<argc;i++){
        if(i+1<argc){
            if(!strcmp(args[i],"-window")){
                i++;
                sscanf(args[i],"%dx%d",&w,&h);
                printf("Catched '-window' resolution %d %d\n",w,h);
            }else
            if(!strcmp(args[i],"-position")){
                i++;
                sscanf(args[i],"%dx%d",&x0,&y0);
                printf("Catched '-position' values %d %d\n",x0,y0);
            
            }else if(!strcmp(args[i],"-aspect-coef")){
                i++;
                sscanf(args[i],"%lf",&aspect_coef);
                printf("Catched '-aspect-coef' value %lf\n",aspect_coef);
            
            }else if(!strcmp(args[i],"-pic-feet")){
                i++;
                sscanf(args[i],"%d",&pic_feet);
                printf("Catched '-pic-feet' value %d\n",pic_feet);
            
            }
            
            
        }
        if(!strcmp(args[i],"-no-load-to-video")){
            GLLOWMEM=1;
            printf("Catched '-no-load-to-video' (%d)\n",GLLOWMEM);
                
        }else if(!strcmp(args[i],"-h") || !strcmp(args[i],"--help") || !strcmp(args[i],"-help")){
            printf("Catched '-help'\n%s\n",help);
            fprintf(stderr,"Catched '-help'\n%s\n",help);
            _halt = 1;
        }
    
    }
    GLH=h;
    GLW=w;
    GLX0=x0;
    GLY0=y0;
}


int main( int argc, char* args[] ){
    int i;
    int frames=0;
    stdout = fopen("log.txt","w");
    Uint32 t1,t2;
    parseParam(argc, args);
    if(_halt){
        return 1;
    }
    initGl();    
    loadImages();   
    loadTimeLine();
    t1=GetTickCount();
    while(readKey()<0)
        showMult((GetTickCount()-t1)/1000.);
    while(readKey()>=0)
        showMult((GetTickCount()-t1)/1000.);
    t1=GetTickCount();
    while(readKey()<0){
        t2 = GetTickCount() - t1;
        if(n_imgs){
            double a;
            int n;
            calcView(t2,n,a);
            showImage( n, a );
            frames++;
        }
        if(t2 / 1000. > last){
            break;
        }
    }

    t2 = GetTickCount() - t1;
    printf("%d frames showed in %d seconds; last = %lf\n",frames,t2/1000,last);
    if(t2!=0)
        printf("%3.2lf fps\n",frames*1000./t2);
        
    fflush(stdout);

    fclose(stdout);
     while(readKey()<0)
        showMult((GetTickCount()-t1)/1000.);

    return 0;
}
