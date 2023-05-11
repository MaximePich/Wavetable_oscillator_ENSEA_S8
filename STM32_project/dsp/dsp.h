/**
 * @file dsp.h
 * @brief This is a header file for a DSP library.
 */

#ifndef __DSP_H
#define __DSP_H

#include <stdint.h>

/**
 * @brief The frequency of middle C (C4).
 */
#define FREQ_C4 261.625f

/**
 * @brief Enumeration for identifying different parameters in the DSP structure. 
 * X and Y parameters are modified by the potentiometers and the pitch is modified by the encoder
 */
typedef enum dsp_param_id_enum
{
    PITCH_PARAM, /**< Identifier for the pitch parameter. */
    X_PARAM, /**< Identifier for the X parameter. */
    Y_PARAM, /**< Identifier for the Y parameter. */
    X_ATTV_PARAM, /**< Identifier for the X attenuation parameter. */
    Y_ATTV_PARAM, /**< Identifier for the Y attenuation parameter. */
    PARAMS_LEN /**< Length of the parameter array. */
} dsp_param_id_t;

/**
 * @brief Enumeration for identifying different inputs.
 * Pitch, X and Y come from jack inputs
 */
typedef enum dsp_input_id_enum
{
    PITCH_INPUT, /**< Identifier for the pitch input. */
    X_INPUT, /**< Identifier for the X input. */
    Y_INPUT, /**< Identifier for the Y input. */
    INPUTS_LEN /**< Length of the input array. */
} dsp_input_id_t;

/**
 * @brief Structure for storing digital signal processing information.
 */
typedef struct h_dsp_struct
{
    int32_t params[PARAMS_LEN]; /**< Array for storing parameters. */
    int32_t inputs[INPUTS_LEN]; /**< Array for storing inputs. */

    float sample_frequency; /**< The sample frequency of the digital signal. */
    float frequency;

    float phase; /**< The phase of the digital signal. */
} h_dsp_t;

/**
 * @brief Convert an unint16_t to a float between -1 and 1.
 * @param analog The unint16_t to convert.
 */
float analog_to_minus1_to_1(uint16_t analog);

/**
 * @brief Change the buffer array range to the line tension range.
 * @param tension final output tension max value
 */
void normalize_to_line_tension(uint32_t *buffer, float tension);

/**
 * @brief Initializes the DSP structure.
 * @param h_dsp Pointer to the DSP structure to be initialized.
 */
void dsp_init(h_dsp_t * h_dsp);

/**
 * @brief Processes the digital signal.
 * @param h_dsp Pointer to the DSP structure for processing the digital signal.
 */
void dsp_process(h_dsp_t * h_dsp, uint32_t * dac_buffer, int32_t dac_buffer_size);

#endif // __DSP_H
