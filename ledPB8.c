/*----------------------------------------------------------------------------
 *      E x a m p l e
 *----------------------------------------------------------------------------
 *      Name:    EN_TETE.C
 *      Purpose: Fichier de démarrage
 *      Rev.:    V3.20
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
 *-----------------------------------------------------------------------------
 * FONCTION		:  
 *
 *------------------------------------------------------------------------------
 * AUTEUR				:  A. MARQUES
 * Développé le 		:  13 - 03 - 09
 * Dernière modif le 	:
 *------------------------------------------------------------------------------ */


#include "stm32f10x_lib.h"            /* STM32F10x Library Definitions       */


/*----------------------------------------------------------------------------
 *        Main: Initialize 
 *---------------------------------------------------------------------------*/
void EXTI9_5_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line6) == SET) {
        // Appui détecté sur le bouton-poussoir
        GPIO_WriteBit(GPIOB, GPIO_Pin_8, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_8)));
        EXTI_ClearITPendingBit(EXTI_Line6); // Effacer le drapeau d'interruption
    }
	}


int main(void) {
    // Initialisation du système
		GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
		ErrorStatus HSEStartUpStatus;
	//Clock
		FLASH_SetLatency(FLASH_Latency_2);
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		
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
			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); //Clock sur GPIO
    // Configuration du bouton-poussoir (BP4 = PB6) comme source d'interruption
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // Activation de la résistance de pull-up interne
	  //GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);

    EXTI_InitStruct.EXTI_Line = EXTI_Line6;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // Front descendant
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    // Configuration de la LED (PB8)

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // Sortie Push-Pull
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configuration de la priorité de l'interruption EXTI4
   
		NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQChannel;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //Slide 22 groupe priorité ou sous-priorité
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00; // Priorité la plus élevée
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    while (1) {
    }
	}
	else{
		//printf("HSE don't start");
		while(1);
}





/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
