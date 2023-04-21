#include <stdio.h>
#include <stdlib.h>

#include "dsp.h"

#define SAMPLE_FREQ 48000.0f

#define SIZE ((int)(2 * SAMPLE_FREQ / FREQ_C4))

h_dsp_t h_dsp;

int main(int argc, char **argv)
{
    h_dsp.sample_frequency = SAMPLE_FREQ;

    h_dsp.params[X_PARAM] = 0.f;
    h_dsp.params[Y_PARAM] = 0.f;
    h_dsp.inputs[X_INPUT] = 0.f;
    h_dsp.inputs[Y_INPUT] = 0.f;

    dsp_init(&h_dsp);

    h_dsp.params[X_PARAM] = 0.f;
    h_dsp.params[Y_PARAM] = 0.f;
    for (int i = 0; i < SIZE; i++)
    {
        dsp_process(&h_dsp);
        printf("%f ", h_dsp.outputs[AUDIO_OUTPUT]);
    }
    printf("\n");

    dsp_init(&h_dsp);

    h_dsp.params[X_PARAM] = 5.f;
    h_dsp.params[Y_PARAM] = 0.f;
    for (int i = 0; i < SIZE; i++)
    {
        dsp_process(&h_dsp);
        printf("%f ", h_dsp.outputs[AUDIO_OUTPUT]);
    }
    printf("\n");

    dsp_init(&h_dsp);

    h_dsp.params[X_PARAM] = 10.f;
    h_dsp.params[Y_PARAM] = 0.f;
    for (int i = 0; i < SIZE; i++)
    {
        dsp_process(&h_dsp);
        printf("%f ", h_dsp.outputs[AUDIO_OUTPUT]);
    }
    printf("\n");

    dsp_init(&h_dsp);

    h_dsp.params[X_PARAM] = 0.f;
    h_dsp.params[Y_PARAM] = 5.f;
    for (int i = 0; i < SIZE; i++)
    {
        dsp_process(&h_dsp);
        printf("%f ", h_dsp.outputs[AUDIO_OUTPUT]);
    }
    printf("\n");

    dsp_init(&h_dsp);

    h_dsp.params[X_PARAM] = 5.f;
    h_dsp.params[Y_PARAM] = 5.f;
    for (int i = 0; i < SIZE; i++)
    {
        dsp_process(&h_dsp);
        printf("%f ", h_dsp.outputs[AUDIO_OUTPUT]);
    }
    printf("\n");

    dsp_init(&h_dsp);

    h_dsp.params[X_PARAM] = 10.f;
    h_dsp.params[Y_PARAM] = 5.f;
    for (int i = 0; i < SIZE; i++)
    {
        dsp_process(&h_dsp);
        printf("%f ", h_dsp.outputs[AUDIO_OUTPUT]);
    }
    printf("\n");

    dsp_init(&h_dsp);

    h_dsp.params[X_PARAM] = 0.f;
    h_dsp.params[Y_PARAM] = 10.f;
    for (int i = 0; i < SIZE; i++)
    {
        dsp_process(&h_dsp);
        printf("%f ", h_dsp.outputs[AUDIO_OUTPUT]);
    }
    printf("\n");

    dsp_init(&h_dsp);

    h_dsp.params[X_PARAM] = 5.f;
    h_dsp.params[Y_PARAM] = 10.f;
    for (int i = 0; i < SIZE; i++)
    {
        dsp_process(&h_dsp);
        printf("%f ", h_dsp.outputs[AUDIO_OUTPUT]);
    }
    printf("\n");

    dsp_init(&h_dsp);

    h_dsp.params[X_PARAM] = 10.f;
    h_dsp.params[Y_PARAM] = 10.f;
    for (int i = 0; i < SIZE; i++)
    {
        dsp_process(&h_dsp);
        printf("%f ", h_dsp.outputs[AUDIO_OUTPUT]);
    }
    printf("\n");

    return 0;
}