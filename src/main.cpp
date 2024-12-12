#include "main.h"
#include "periph_init.h"
#include "stm32f4xx_hal.h"
#include "arm_math.h"
#include "leds.h"
#include "argb.h"
#include <stdio.h>
#include "fir.h"
#include "fir.h"


volatile bool data_rdy_f = false;


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
    MX_TIM2_Init();
    ARGB_Init();

    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_values, CHANNELS * SAMPLES) != HAL_OK)
        Error_Handler();

    // Initial test
    
    // led ring
    
    // led ring
    test_leds();

    // built in leds
    for (int i = 0; i < 4; i++)
    {
        Set_LED_State(i);
        HAL_Delay(500);
    }

    // Initialize the FIR filter
    init_filter();

    // built in leds
    for (int i = 0; i < 4; i++)
    {
        Set_LED_State(i);
        HAL_Delay(500);
    }

    // Initialize the FIR filter
    init_filter();

    // Main loop
    while (1)
    {
        // Wait for data
        if (data_rdy_f)
        {
            uint16_t amplitude[CHANNELS] = { 0 };

            // Apply filter and calculate amplitude for each channel
            // Apply filter and calculate amplitude for each channel
            for (int i = 0; i < CHANNELS; ++i) {
                uint16_t channel_buffer[SAMPLES];

                for (int j = 0; j < SAMPLES; j++) {
                    channel_buffer[j] = adc_values[i + j * CHANNELS];
                }

                fir_filter(channel_buffer);

                for (int j = 0; j < SAMPLES; j++) {
                    adc_values[i + j * CHANNELS] = channel_buffer[j];
                }

                uint16_t channel_buffer[SAMPLES];

                for (int j = 0; j < SAMPLES; j++) {
                    channel_buffer[j] = adc_values[i + j * CHANNELS];
                }

                fir_filter(channel_buffer);

                for (int j = 0; j < SAMPLES; j++) {
                    adc_values[i + j * CHANNELS] = channel_buffer[j];
                }

                amplitude[i] = Calculate_Max_Amplitude(adc_values, i, SAMPLES, CHANNELS);
            }

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

            // estimate the angle of the sound
            uint8_t led_index = 0;
            uint8_t max_neighbour = amplitude[(max_channel+1)%CHANNELS] > amplitude[(max_channel-1)%CHANNELS]?1:-1;
            float tangent = (float)amplitude[(max_channel+max_neighbour)%CHANNELS];
            if (tangent > 0.8)
             {
                led_index = max_channel*4;
             }
             else if (tangent > 0.2)
             {
                led_index = (max_channel*4 + max_neighbour)%NUM_PIXELS;
             }
             else
             {
                led_index = (max_channel*4 + max_neighbour*2)%NUM_PIXELS;
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
            
            // built in leds
            if (max_amplitude > NO_SIGNAL_THRESHOLD) {
                light_led(led_index, red);
                Set_LED_State(max_channel);
            }
            else {
                reset_all();
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

    void DMA1_Stream6_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_tim2_ch2_ch4); }
} //extern "C"