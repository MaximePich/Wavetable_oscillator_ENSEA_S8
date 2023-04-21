/**
 * @file wave.h
 * @brief This is a header file for a wave library.
 */

#ifndef __DRV_WAVE_H
#define __DRV_WAVE_H

#include <math.h>

#define PI 3.1415

float saw_wave(float x);
float triangle_wave(float x);
float sine_wave(float x,float phase);
float square_wave(float x,float phase);
float wave_mixer(float wave1,float wave2, float wave3, float wave4,float x, float y);
#endif //__DRV_WAVE_H