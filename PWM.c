#include "stm32f10x_lib.h" 

// Message dans la Flash


// Message dans la SRAM
u32 tableau[256];
int i;
int resultat;
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
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA /*|RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1*/ , ENABLE);
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		}
	}
/*
void init_ADC(){
	ADC_InitTypeDef ADC_InitStructure;
ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
ADC_InitStructure.ADC_ScanConvMode = DISABLE;
ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None ;
ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Left;
ADC_InitStructure.ADC_NbrOfChannel = 1;
ADC_Init( ADC1, &ADC_InitStructure);
ADC_RegularChannelConfig( ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);
ADC_DMACmd(ADC1, ENABLE); // à chaque fin de conversion, demande de transfert DMA.
ADC_Cmd(ADC1, ENABLE );
ADC_ResetCalibration(ADC1); // Enable ADC1 reset calibration register 
while(ADC_GetResetCalibrationStatus(ADC1)); // Check the end of ADC1 reset calibration register 
ADC_StartCalibration(ADC1); // Start ADC1 calibration 
while(ADC_GetCalibrationStatus(ADC1)); // Check the end of ADC1 calibration 
ADC_SoftwareStartConvCmd( ADC1, ENABLE ); 
}
*/
/*
void init_DMA(){
		DMA_InitTypeDef DMA_InitStruct;
		DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
		DMA_InitStruct.DMA_MemoryBaseAddr = (u32)&tableau ; 
		DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_InitStruct.DMA_BufferSize = 512;
		DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Disable;
		DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
		DMA_InitStruct.DMA_Priority = DMA_Priority_Medium;
		DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
		DMA_Init( DMA1_Channel1, &DMA_InitStruct ); 
		DMA_ITConfig(DMA1_Channel1, DMA_IT_TC|DMA_IT_HT, ENABLE);
		DMA_Cmd(DMA1_Channel1, ENABLE );
}
*/
void init_NVIC(){
		NVIC_InitTypeDef NVIC_InitStruct;
		NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQChannel;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //Slide 22 groupe priorité ou sous-priorité
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01; // Priorité la plus élevée
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

void EXTI0_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line0) == SET) {
			if ( TIM2->CCR3 == 0){
        // Appui détecté sur le bouton-poussoir
        TIM_SetCompare3(TIM2, 100); 
        TIM_SetCompare4(TIM2, 0);
        EXTI_ClearITPendingBit(EXTI_Line0); // Effacer le drapeau d'interruption
				}
			else{
				TIM_SetCompare3(TIM2, 0); 
        TIM_SetCompare4(TIM2, 0);
				EXTI_ClearITPendingBit(EXTI_Line0);
    }
	}
}

void init_EXTI(){
	EXTI_InitTypeDef EXTI_InitStruct;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
  EXTI_InitStruct.EXTI_Line = EXTI_Line0;
  EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // Front descendant
  EXTI_InitStruct.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStruct);
} 
void init_GPIO(){
	GPIO_InitTypeDef GPIO_InitStruct;
	 // Configuration de (PA0) Bouton pressoir 2
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 ;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Float
    GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 ;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; // Activation de la résistance de pull-up interne
    GPIO_Init(GPIOA, &GPIO_InitStruct);

/*
    // Configuration des broches PB8 à PB15 comme sorties pour les LED
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // Sortie Push-Pull
    GPIO_Init(GPIOB, &GPIO_InitStruct); */
}
void Timer_init() { 
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
u16 TIM_period = 999;
u16 TIM_Prescaler = 72;
/* Time base configuration */
TIM_InternalClockConfig(TIM2); 
TIM_TimeBaseStructure.TIM_Period = TIM_period;
TIM_TimeBaseStructure.TIM_Prescaler = TIM_Prescaler; /* Fpwm = 1MHZ /*/
TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	/* PWM1 Mode configuration: Channel3 */
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
TIM_OCInitStructure.TIM_Pulse = 0; 
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
TIM_OC3Init(TIM2, &TIM_OCInitStructure);
TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
TIM_OC4Init(TIM2, &TIM_OCInitStructure);
TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
TIM_ARRPreloadConfig(TIM2, ENABLE);
TIM_UpdateDisableConfig(TIM2, DISABLE);
TIM_UpdateRequestConfig ( TIM2, TIM_UpdateSource_Regular);
TIM_Cmd ( TIM2, ENABLE ); 
}
int main(void) {
		// Initialisation du système
	//CLOCK
		FLASH_SetLatency(FLASH_Latency_2);
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		init_HSE();
		Timer_init();
	  init_NVIC();
		init_EXTI();
		init_GPIO();
		//init_DMA();
		//init_ADC();
		while(1){
			
		}
  
}








