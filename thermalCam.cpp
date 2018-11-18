#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <string> 
#include <fstream>
#include <chrono>
#include <thread>
#include "headers/MLX90640_API.h"
#include "headers/MLX90640_RPI_I2C_Driver.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_NONE    "\x1b[30m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define FMT_STRING "\u2588\u2588"

#define MLX_I2C_ADDR 0x33
#define REFRESH_RATE MLX90640_CR1_REFRESH_16_HZ // Subpages rate is half

// WAIT_TIME_MILLISECONDS = 2*0.68*1000/fps
// 2: there are 2 subframes
// 0.68: wait 68% (a bit less then 80% from the datasheet)
// 1000: s -> ms
#define WAIT_TIME_MILLISECONDS 85

int main(int argc, char *argv[]){
    static uint16_t eeMLX90640[832];
    float emissivity = 1;
    uint16_t frame[834];
    static float image[768];
    float eTa;
    static uint16_t data[768*sizeof(float)];

    int error;
    int64_t max_elapsed=0;
    int64_t max_calcs=0;
    int16_t CtrlReg1 = 0;
    static int64_t t_start=0,\
    t_end=0,\
    elapsed = 0;
    static float mlx90640To[768];

    std::ofstream fReadErrors;
    
    // printf("Starting I2C...\n");
    error = MLX90640_I2CInit();
    // printf("Error: %d\n", error);
    // printf("Setting I2C freq...\n");
    MLX90640_I2CFreqSet(1000000); // Hz
    
    // Configure Control Register 1
    CtrlReg1 =  MLX90640_CR1_SUBPAGE_MODE_ON &\
    MLX90640_CR1_TRANSFER_TO_RAM_ON &\
    MLX90640_CR1_SUBPAGE_REPEAT_OFF &\
    REFRESH_RATE &\
    MLX90640_CR1_ADC_RESOLUTION_16BIT &\
    MLX90640_CR1_CHESS_MODE;
    // printf("Setting Control Register 1...\n");
    error = MLX90640_ConfReg1(MLX_I2C_ADDR, CtrlReg1);
    // printf("Error: %d\n", error);

    paramsMLX90640 mlx90640;
    // printf("Damping EE...\n");
    error = MLX90640_DumpEE(MLX_I2C_ADDR, eeMLX90640);
    // printf("Error: %d\n", error);
    // printf("Extracting parameters...\n");
    error = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    // printf("Error: %d\n", error);

    std::cout << std::endl;
    while(1){
        do{
            t_start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            error = MLX90640_GetFullFrameData(MLX_I2C_ADDR, frame);
            if(error) std::this_thread::sleep_for(std::chrono::milliseconds(5));
            if (error != 0 && error != -100){
                fReadErrors.open("thermalCamLog.txt", std::ios_base::app);
                fReadErrors << "[" << t_start <<"]: " <<  error << "\n";
                fReadErrors.close();
            }
        }while(error);

        MLX90640_InterpolateOutliers(frame, eeMLX90640);

        frame[833] = 0;
        eTa = MLX90640_GetTa(frame, &mlx90640);
        MLX90640_CalculateTo(frame, &mlx90640, emissivity, eTa, mlx90640To);

        frame[833] = 1;
        eTa = MLX90640_GetTa(frame, &mlx90640);
        MLX90640_CalculateTo(frame, &mlx90640, emissivity, eTa, mlx90640To);

        for(int y = 23; y >= 0; y--){
            for(int x = 31; x >= 0; x--){
                // frame[32 * (23-y) + x] = 0; // Clear the pixel value
                float val = mlx90640To[32 * (23-y) + x];
                if(val > 99.99) val = 99.99;
                if(val > 32.0){
                    std::cout << ANSI_COLOR_MAGENTA << FMT_STRING << ANSI_COLOR_RESET;
                }
                else if(val > 29.0){
                    std::cout << ANSI_COLOR_RED << FMT_STRING << ANSI_COLOR_RESET;
                }
                else if (val > 26.0){
                    std::cout << ANSI_COLOR_YELLOW << FMT_STRING << ANSI_COLOR_RESET;
                }
                else if ( val > 20.0 ){
                    std::cout << ANSI_COLOR_NONE << FMT_STRING << ANSI_COLOR_RESET;
                }
                else if (val > 17.0) {
                    std::cout << ANSI_COLOR_GREEN << FMT_STRING << ANSI_COLOR_RESET;
                }
                else if (val > 10.0) {
                    std::cout << ANSI_COLOR_CYAN << FMT_STRING << ANSI_COLOR_RESET;
                }
                else {
                    std::cout << ANSI_COLOR_BLUE << FMT_STRING << ANSI_COLOR_RESET;
                }
            }
            std::cout << std::endl;
        }
        std::cout << "\x1b[33A";

        t_end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        elapsed = t_end - t_start;
        if( elapsed < WAIT_TIME_MILLISECONDS){
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME_MILLISECONDS-elapsed));
        };
    };
    MLX90640_I2CEnd();
    return 0;
}
