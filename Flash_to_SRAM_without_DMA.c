#include "stm32f10x_lib.h" 

// Message dans la Flash
const uc8 message1[] = "Centre de microelectronique de provence - George Charpak";

// Message dans la SRAM
volatile u8 message2[10000];
int i;
int taille_message1;
int main(void) {
		// Initialisation du système
		
		ErrorStatus HSEStartUpStatus;
		
		//Clock
		FLASH_SetLatency(FLASH_Latency_2);
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    // Copier le message de la Flash vers la SRAM
		RCC_HSEConfig(RCC_HSE_ON); //Init HSE
		HSEStartUpStatus = RCC_WaitForHSEStartUp();
		if(HSEStartUpStatus == SUCCESS){
			RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);
			RCC_PLLCmd(ENABLE);
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
			RCC_HCLKConfig(RCC_SYSCLK_Div1);
			RCC_PCLK1Config(RCC_HCLK_Div2);
			RCC_PCLK2Config(RCC_HCLK_Div1);
			RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SRAM|RCC_AHBPeriph_FLITF,ENABLE);
    for (i = 0; i < sizeof(message1); i++) {
        message2[i] = message1[i];
    }

    // Maintenant, uc8_message2 contient le même message que uc8_message1
		
    
        
    }
}
