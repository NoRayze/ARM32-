/*----------------------------------------------------------------------------
 *      E x a m p l e
 *----------------------------------------------------------------------------
 *      Name:    EN_TETE.C
 *      Purpose: Fichier de d�marrage
 *      Rev.:    V3.20
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
 *-----------------------------------------------------------------------------
 * FONCTION		:  
 *
 *------------------------------------------------------------------------------
 * AUTEUR				:  A. MARQUES
 * D�velopp� le 		:  13 - 03 - 09
 * Derni�re modif le 	:
 *------------------------------------------------------------------------------ */


#include "stm32f10x_lib.h"            /* STM32F10x Library Definitions       */


/*----------------------------------------------------------------------------
 *        Main: Initialize 
 *---------------------------------------------------------------------------*/

volatile unsigned char operand1 = 0;
volatile unsigned char operand2 = 0;
volatile unsigned char result = 0;
volatile int buttonPressCount = 0;

void EXTI15_10_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line13) != RESET) {
        // Interruption d�clench�e par l'appui sur le bouton-poussoir (PC13)

        if (buttonPressCount == 0) {
            // Premi�re pression : Acqu�rir le premier op�rande � partir des commutateurs (SW1 � SW4)
            operand1 = GPIOC->IDR & 0x00F0;
            operand1 >>= 4; // D�calage pour obtenir la valeur des commutateurs
            buttonPressCount = 1; // Passer � la deuxi�me pression
        } else if (buttonPressCount == 1) {
            // Deuxi�me pression : Acqu�rir le deuxi�me op�rande � partir des commutateurs (SW1 � SW4)
            operand2 = GPIOC->IDR & 0x00F0;
            operand2 >>= 4; // D�calage pour obtenir la valeur des commutateurs

            // Effectuer la multiplication
            result = operand1 * operand2;

            // Afficher le r�sultat sur les LED (PB8 � PB15)
            GPIOB->ODR &= 0xFF00; // Effacer les 4 bits inf�rieurs
            GPIOB->ODR |= (result & 0xFF); // Mettre � jour les 4 bits inf�rieurs

            // R�initialiser les op�randes et le compteur de pression
            operand1 = 0;
            operand2 = 0;
            buttonPressCount = 0;

            EXTI_ClearITPendingBit(EXTI_Line13); // Effacer le drapeau d'interruption
        }
    }
}

int main(void) {
    // Initialisation du syst�me
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    // Activer les horloges pour les ports GPIOC et GPIOB
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);

    // Configuration des broches PC4 � PC7 comme entr�es pour les commutateurs (SW1 � SW4)
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU; // Activation de la r�sistance de pull-up interne
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configuration de la broche PC13 comme entr�e pour le bouton-poussoir
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configuration des broches PB8 � PB15 comme sorties pour les LED
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // Sortie Push-Pull
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Configuration de l'interruption EXTI13 pour le bouton-poussoir
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); // Activer l'horloge AFIO

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource13);

    EXTI_InitStruct.EXTI_Line = EXTI_Line13;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // Front descendant
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    // Configuration de la priorit� de l'interruption EXTI13
    NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQChannel; // EXTI15_10_IRQn g�re EXTI13 (PC13)
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00; // Priorit� la plus �lev�e
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    while (1) {
        // Votre code principal peut continuer � s'ex�cuter ici
    }
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
