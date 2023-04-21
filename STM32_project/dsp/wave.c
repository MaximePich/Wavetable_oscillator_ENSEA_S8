#include "wave.h"
float saw_wave(float x){
    //TODO handle phase
    int period=1;
    double y;
    double phase = 0.0;
    x = fmodf(x - phase, period); // Normalise la période sur 0-1
    if(x>0.5){
        y=x-1;
    }else{
        y=x;
    }
    return y*2;
}
float triangle_wave(float x)
{
    float y = saw_wave(x);
    y = (fabsf(y));
    return (y-0.5)*2;
}
float sine_wave(float x,float phase)
{
    int period=1;
    float y;
//    y = fmodf(x - phase, period); // Normalise period on 0-1 and handle phase
    return sinf(x*2*PI);
}

float square_wave(float x,float phase)
{
    int period=1;
    float y;
    y=fmodf(x,1);//Keeping only the floating part (0,1)
    y=y-phase;
    if(y<0){y+=1;}
    if(y>1){y-=1;}

    if(y<0.5){
        return 1;
    }else{
        return -1;
    }
}

float wave_mixer(float wave1,float wave2, float wave3, float wave4,float x, float y)
//x et y {-1:1}
//wave 1 et 2 axe X
//wave 3 et 4 axe Y
{
    float x_point = wave1 * (0.5-x/2) +  wave2 * (0.5+x/2);
    float y_point = wave3 * (0.5-y/2) +  wave4 * (0.5+y/2);

    return (x_point + y_point)/2;
}