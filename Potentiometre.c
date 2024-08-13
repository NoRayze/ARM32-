#include "stm32f10x_lib.h" 

// Message dans la Flash


// Message dans la SRAM

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
			RCC_ADCCLKConfig(RCC_PCLK2_Div6);
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1 , ENABLE);
			
		}
	}
void init_ADC(){
	ADC_InitTypeDef ADC_InitStructure;
ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
ADC_InitStructure.ADC_ScanConvMode = DISABLE;
ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None ;
ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
ADC_InitStructure.ADC_NbrOfChannel = 1;
ADC_Init( ADC1, &ADC_InitStructure);
ADC_RegularChannelConfig( ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);
ADC_DMACmd(ADC1, ENABLE); // à chaque fin de conversion, demande de transfert DMA.
ADC_Cmd(ADC1, ENABLE );
ADC_ResetCalibration(ADC1); /* Enable ADC1 reset calibration register */
while(ADC_GetResetCalibrationStatus(ADC1)); /* Check the end of ADC1 reset calibration register */
ADC_StartCalibration(ADC1); /* Start ADC1 calibration */ 
while(ADC_GetCalibrationStatus(ADC1)); /* Check the end of ADC1 calibration */
ADC_SoftwareStartConvCmd( ADC1, ENABLE ); 
}

void init_DMA(){
		DMA_InitTypeDef DMA_InitStruct;
		DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
		DMA_InitStruct.DMA_MemoryBaseAddr = (u32)&GPIOB->ODR + 1 ; 
		DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStruct.DMA_BufferSize = 1;
		DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
		DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
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
void init_EXTI(){
	EXTI_InitTypeDef EXTI_InitStruct;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
  EXTI_InitStruct.EXTI_Line = EXTI_Line1;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // Front descendant
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStruct);
} 
void init_GPIO(){
	GPIO_InitTypeDef GPIO_InitStruct;
	 // Configuration de (PA1) l'ADC1 en entrée
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // Activation de la résistance de pull-up interne
    GPIO_Init(GPIOA, &GPIO_InitStruct);


    // Configuration des broches PB8 à PB15 comme sorties pour les LED
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // Sortie Push-Pull
    GPIO_Init(GPIOB, &GPIO_InitStruct);
}
int main(void) {
		// Initialisation du système
	//CLOCK
		FLASH_SetLatency(FLASH_Latency_2);
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		init_HSE();
	  init_NVIC();
		init_EXTI();
		init_GPIO();
		init_DMA();
		init_ADC();
		while(1){
		}
  
}



