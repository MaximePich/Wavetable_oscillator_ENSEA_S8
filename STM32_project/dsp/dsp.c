#include "dsp.h"
#include "wave.h"
#include "math.h"

float analog_to_minus1_to_1(uint16_t analog)
{  
    float val = (float)(analog - 2048) / 2048.f;
    if (val>1)
    {
        return 1.f;
    }else if (val<-1)
    {
        return -1.f;
    }else
    {
        return val;
    }
}

void normalize_to_line_tension(uint32_t *buffer, float multiplier)
{//Line tension is between -1.5 and 1.5V
for (int i = 0; i < 252; i++)
{
    buffer[i] = (uint32_t)(((buffer[i]-2048)/ 4096.f)*multiplier)+2048;
}

}

float clamp (float voltage, float min, float max)
{
    float ret = voltage;

    if (ret < min) 
    {
        ret = min;
    }
    if (ret > max)
    {
        ret = max;
    }

    return ret;
}

void dsp_init(h_dsp_t * h_dsp)
{
    h_dsp->phase = 0.0f;
}

// Process one sample
// This function is called by the main loop at the sampling frequency
void dsp_process(h_dsp_t * h_dsp, uint32_t * dac_buffer, int32_t dac_buffer_size)
{
    // Compute pitch voltage
    float pitch = (float)(h_dsp->params[PITCH_PARAM])/12.f;//encoder base pitch

    // C0 = 2V = 2376LSB ; C2 = 4V = 2814LSB
    float voct_sub = 2376.f;                    // Compensate the offset
    float voct_div = (2814.f - 2376.f) / 2.f;   // Compensate the gain
    float voct = ((float)(h_dsp->inputs[PITCH_INPUT]) - voct_sub) / voct_div;
    pitch += voct;

    // Compute oscillator frequency
    h_dsp->frequency = FREQ_C4 * pow(2.f, pitch); // Not sure about that either

    // Compute X and Y voltages
    int32_t X_param = h_dsp->params[X_PARAM];
    int32_t Y_param = h_dsp->params[Y_PARAM];
    float X_att = (float)(h_dsp->params[X_ATTV_PARAM]) / 4096.f;
    float Y_att = (float)(h_dsp->params[Y_ATTV_PARAM]) / 4096.f;

    // Transfered from 0 to DAC_BUFFER_SIZE/2 - 1
    for (int it = 0 ; it < dac_buffer_size ; it++)
    {
        //Mix X and Y params with the input modifiers
        float X = analog_to_minus1_to_1(X_param+(h_dsp->inputs[X_INPUT]*X_att));
        float Y = analog_to_minus1_to_1(Y_param+(h_dsp->inputs[Y_INPUT]*Y_att));

        // Accumulate the phase
        h_dsp->phase += h_dsp->frequency / h_dsp->sample_frequency;
        if (h_dsp->phase >= 1.f) h_dsp->phase -= 1.f;


        //phase varies between 0 and 1 for one buffer cycle
        float w1,w2,w3,w4;
        w1 = sine_wave(h_dsp->phase,0);
        w2 = square_wave(h_dsp->phase,0.25);
        w3 = saw_wave(h_dsp->phase);
        w4 = triangle_wave(h_dsp->phase);
        float sample_voltage = wave_mixer(w1,w2,w3,w4,X,Y);
        
        //make sample_voltage varies between -1 and 1
        float mult = 1.f;//0.15 to output 1.5V (Line tension)
        sample_voltage = ((sample_voltage/2.f)*mult)+0.5f;
        //make sample_voltage varies between 0 and 4095
        uint16_t index = (uint16_t)(sample_voltage * 4095);
        //put the sample in the buffer
        dac_buffer[it] = index;
    }
    //normalize_to_line_tension(dac_buffer, 1.5f);
}