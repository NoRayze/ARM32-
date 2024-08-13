#include "stm32f10x_lib.h" 

// Message dans la Flash
const uc8 message1[] = "Centre de microelectronique de provence - George Charpak";

// Message dans la SRAM
volatile u8 message2[10000];
int i;
int taille_message1;
void init_HSE(){
	ErrorStatus HSEStartUpStatus;
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
			//
		}
	}
void init_DMA(){
		DMA_InitTypeDef DMA_InitStruct;
		DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&message1;
		DMA_InitStruct.DMA_MemoryBaseAddr = (u32)&message2; 
		DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStruct.DMA_BufferSize = sizeof(message1);
		DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
		DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
		DMA_InitStruct.DMA_M2M = DMA_M2M_Enable;
		DMA_Init( DMA1_Channel1, &DMA_InitStruct ); 
		DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
		DMA_Cmd(DMA1_Channel1, ENABLE );
}
void init_NVIC(){
		NVIC_InitTypeDef NVIC_InitStruct;
		NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQChannel;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //Slide 22 groupe priorité ou sous-priorité
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00; // Priorité la plus élevée
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}
void DMAChannel1_IRQHandler(void) {
    if (DMA_GetFlagStatus(DMA1_FLAG_TC1) == SET) {
        
        DMA_ClearFlag(DMA1_FLAG_TE1); // Effacer le drapeau d'interruption
    }
	}
	
int main(void) {
		// Initialisation du système
	//CLOCK
			FLASH_SetLatency(FLASH_Latency_2);
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		init_HSE();
	  init_NVIC();
		init_DMA();
		while(1){
		}
    // Maintenant, uc8_message2 contient le même message que uc8_message1
}



