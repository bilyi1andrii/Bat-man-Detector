#include "main.h"
#include "periph_init.h"
#include "stm32f4xx_hal.h"
#include "arm_math.h"
#include <stdio.h>

// FIR FILTER
#define BLOCK_SIZE      32
#define NUM_TAPS        33

volatile bool data_rdy_f = false;

// ALSO FIR FILTER
// arm_fir_instance_f32 fir_instance;
// float32_t fir_state_buffer[NUM_TAPS + SAMPLES - 1];
// high-pass with cutoff at 15 kHz, fs = 100000
// float32_t fir_coefficients[NUM_TAPS] = {
//     -0.000238, -0.000964, -0.001078, 0.000960, 0.004648, 0.005909, -0.000000, -0.011892, -0.019151, -0.008409, 0.021487, 
//     0.049286, 0.039761, -0.029941, -0.145404, -0.254951, 0.700000, -0.254951, -0.145404, -0.029941, 0.039761, 0.049286, 
//     0.021487, -0.008409, -0.019151, -0.011892, -0.000000, 0.005909, 0.004648, 0.000960, -0.001078, -0.000964, -0.000238
// };


// ADC buffer to store conversion results
__attribute__((aligned(2))) uint16_t adc_values[CHANNELS * SAMPLES] = { 0 };

void Set_LED_State(uint8_t index)
{
    uint16_t pins[] = { LD3_Pin, LD4_Pin, LD5_Pin, LD6_Pin };
    for (int i = 0; i < 4; i++)
        HAL_GPIO_WritePin(GPIOD, pins[i], (i == index) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
void Reset_LED() {
    uint16_t pins[] = { LD3_Pin, LD4_Pin, LD5_Pin, LD6_Pin };
    for (int i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(GPIOD, pins[i], GPIO_PIN_RESET);
    }
}
// changed to float32_t here when implementing filter
uint16_t Calculate_Max_Amplitude(uint16_t *buffer, uint8_t channel, uint32_t num_samples,
                                 uint8_t channels)
{
    uint16_t max_val = 0, min_val = UINT16_MAX;

    for (uint16_t i = channel; i < num_samples * channels; i += channels)
    {
        uint16_t val = buffer[i];
        max_val = (val > max_val) ? val : max_val;
        min_val = (val < min_val) ? val : min_val;
    }

    return max_val - min_val; // Amplitude
}

// Main application entry
int main(void)
{
    SystemClock_Config();
    PeriphCommonClock_Config();
    HAL_Init();

    // Initialize peripherals
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_USART1_UART_Init();

    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_values, CHANNELS * SAMPLES) != HAL_OK)
        Error_Handler();

    // Initial test
    for (int i = 0; i < 4; i++)
    {
        Set_LED_State(i);
        HAL_Delay(500);
    }

    // initialize th FIR filter
    // arm_fir_init_f32(&fir_instance, NUM_TAPS, (float32_t *)fir_coefficients,
    //             fir_state_buffer, SAMPLES);


    // Main loop
    while (1)
    {
        // Wait for data
        if (data_rdy_f)
        {
            uint16_t amplitude[CHANNELS] = { 0 };

            // apply filter and calculate amplitude for each channel
            for (int i = 0; i < CHANNELS; ++i) {
            //     float32_t filtered_data[SAMPLES];
            //     for (int j = 0; j < SAMPLES; j++) 
            //         filtered_data[j] = (float32_t)adc_values[i + j * CHANNELS];
                
            //     arm_fir_f32(&fir_instance, filtered_data, filtered_data, SAMPLES);

            //     amplitude[i] = Calculate_Max_Amplitude(filtered_data, i, SAMPLES, CHANNELS);

                amplitude[i] = Calculate_Max_Amplitude(adc_values, i, SAMPLES, CHANNELS);
            }

                

            // Find the channel with the highest amplitude
            uint16_t max_amplitude = amplitude[0];
            int max_channel = 0;

            for (int channel = 0; channel < CHANNELS; ++channel)
            {
                if (amplitude[channel] > max_amplitude)
                {
                    max_amplitude = amplitude[channel];
                    max_channel = channel;
                }
            }

#ifdef DEBUG
            // Output results via UART
            printf("Channel Amplitudes: ");
            for (int channel = 0; channel < CHANNELS; ++channel)
                printf("%lu ", amplitude[channel]);
            printf("\n");

            // Stream ADC data via ITM for plotting
            // for (int i = 0; i < SAMPLES * CHANNELS; ++i)
            //     ITM_SendChar((uint8_t)(adc_values[i] & 0xFF)); // Example: LSB of ADC value
            for (int i = 0; i < SAMPLES; ++i)
            {
                // if (i % 10 == 0) // Send every 10th sample to reduce ITM traffic
                {
                    ITM->PORT[0].u8 = (uint8_t)(adc_values[i * CHANNELS + 0] & 0xFF); // Channel 0
                    ITM->PORT[1].u8 = (uint8_t)(adc_values[i * CHANNELS + 1] & 0xFF); // Channel 1
                    ITM->PORT[2].u8 = (uint8_t)(adc_values[i * CHANNELS + 2] & 0xFF); // Channel 2
                    ITM->PORT[3].u8 = (uint8_t)(adc_values[i * CHANNELS + 3] & 0xFF); // Channel 3
                }
            }
#endif
            // Control LEDs based on ADC result
            if (max_amplitude > 50)
                Set_LED_State(max_channel);
            else {
                Reset_LED();
            }
            data_rdy_f = false; // Processed
        }
        // Perform other tasks here (e.g., debugging or communication)
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        data_rdy_f = true; // Handle the data in the adc_values buffer

        // Restart the ADC conversion in manual mode
        // This might not be necessary as circular mode automatically restarts
        // if (HAL_ADC_Start_DMA(&hadc1, adc_values, CHANNELS * SAMPLES) != HAL_OK)
        //     Error_Handler();
    }
}

// Placeholder for future use for true realtime
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        // Handle first half of the buffer (adc_values[0] to adc_values[2047])
    }
}

// Error handler function
void Error_Handler(void)
{
    while (1)
    {
        HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin); // Toggle an LED
        HAL_Delay(500);                             // 500 ms delay
    }
}

// For all interrupt handlers for proper linking
extern "C"
{
    int _write(int file, char *ptr, int len)
    {
        // for (int i = 0; i < len; i++)
        //     ITM_SendChar((*ptr++));

        HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
        return len;
    }

    void SysTick_Handler(void)
    {
        HAL_IncTick(); // Increment HAL time base
        HAL_SYSTICK_IRQHandler();
    }

    void DMA2_Stream0_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_adc1); }
} //extern "C"