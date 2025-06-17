/**
 ******************************************************************************
 * \file    		main.c
 * \author  		https://github.com/Grom-
 ******************************************************************************
 *           			COPYRIGHT 2022 STMicroelectronics
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/

#include "Drivers/rng/rng.h"
#include "Drivers/uart/uart.h"
#include "stselib.h"
#include <stdio.h>
#include <string.h>

/* Defines -------------------------------------------------------------------*/
#define PRINT_RESET "\x1B[0m"
#define PRINT_CLEAR_SCREEN "\x1B[1;1H\x1B[2J"
#define PRINT_RED "\x1B[31m"   /* Red */
#define PRINT_GREEN "\x1B[32m" /* Green */
#define RANDOM_SIZE 32

/* Root CA key used for STSE-L SPL01 */
#define STM_STSAFE_L_CA0001                                                                             \
    0x30, 0x82, 0x01, 0x67, 0x30, 0x82, 0x01, 0x19, 0xa0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x01, 0x01,     \
        0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x30, 0x4b, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, \
        0x04, 0x06, 0x13, 0x02, 0x4e, 0x4c, 0x31, 0x1e, 0x30, 0x1c, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, \
        0x15, 0x53, 0x54, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x65, 0x6c, 0x65, 0x63, 0x74, 0x72, 0x6f, 0x6e, \
        0x69, 0x63, 0x73, 0x20, 0x6e, 0x76, 0x31, 0x1c, 0x30, 0x1a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, \
        0x13, 0x53, 0x54, 0x4d, 0x5f, 0x53, 0x54, 0x53, 0x41, 0x46, 0x45, 0x2d, 0x4c, 0x5f, 0x43, 0x41, \
        0x30, 0x30, 0x30, 0x31, 0x30, 0x20, 0x17, 0x0d, 0x32, 0x34, 0x30, 0x36, 0x30, 0x37, 0x30, 0x30, \
        0x30, 0x30, 0x30, 0x30, 0x5a, 0x18, 0x0f, 0x32, 0x30, 0x35, 0x34, 0x30, 0x36, 0x30, 0x37, 0x30, \
        0x30, 0x30, 0x30, 0x30, 0x30, 0x5a, 0x30, 0x4b, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, \
        0x06, 0x13, 0x02, 0x4e, 0x4c, 0x31, 0x1e, 0x30, 0x1c, 0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x15, \
        0x53, 0x54, 0x4d, 0x69, 0x63, 0x72, 0x6f, 0x65, 0x6c, 0x65, 0x63, 0x74, 0x72, 0x6f, 0x6e, 0x69, \
        0x63, 0x73, 0x20, 0x6e, 0x76, 0x31, 0x1c, 0x30, 0x1a, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0c, 0x13, \
        0x53, 0x54, 0x4d, 0x5f, 0x53, 0x54, 0x53, 0x41, 0x46, 0x45, 0x2d, 0x4c, 0x5f, 0x43, 0x41, 0x30, \
        0x30, 0x30, 0x31, 0x30, 0x2a, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x03, 0x21, 0x00, 0x84, \
        0x47, 0xf2, 0xc0, 0x98, 0xbe, 0x15, 0xf6, 0x05, 0xc6, 0x98, 0xd5, 0xfa, 0xc5, 0x7b, 0x56, 0x0f, \
        0x1c, 0xcf, 0x1f, 0x37, 0x9f, 0xe9, 0x88, 0xaa, 0x2f, 0xea, 0xa2, 0x93, 0xb5, 0xde, 0xd1, 0xa3, \
        0x20, 0x30, 0x1e, 0x30, 0x0f, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x04, 0x08, 0x30, 0x06, 0x01, 0x01, \
        0xff, 0x02, 0x01, 0x00, 0x30, 0x0b, 0x06, 0x03, 0x55, 0x1d, 0x0f, 0x04, 0x04, 0x03, 0x02, 0x02, \
        0x04, 0x30, 0x05, 0x06, 0x03, 0x2b, 0x65, 0x70, 0x03, 0x41, 0x00, 0x29, 0x31, 0xa8, 0x6f, 0xed, \
        0x6f, 0xd7, 0x54, 0x8a, 0x03, 0x20, 0x72, 0x84, 0x5d, 0x77, 0x87, 0x97, 0xe2, 0x36, 0x4b, 0x52, \
        0x65, 0xea, 0xc1, 0xbf, 0x7b, 0x39, 0x36, 0x05, 0x75, 0x5b, 0x5d, 0x76, 0x19, 0xfd, 0x32, 0x9c, \
        0xfd, 0x90, 0x93, 0x17, 0xf9, 0x53, 0xb4, 0xb8, 0x21, 0xdf, 0x9c, 0xe2, 0x0d, 0xa7, 0xcd, 0x74, \
        0x5b, 0x9e, 0xf1, 0xab, 0xf7, 0x94, 0x9f, 0x4a, 0xb0, 0xfb, 0x05

#define STSAFE_CERTIFICATE_ZONE_0 0U
#define STSE_STATIC_PRIVATE_KEY_SLOT_0 0U

/* STDIO redirect */
#if defined(__GNUC__) && !defined(__ARMCC_VERSION)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#define GETCHAR_PROTOTYPE int __io_getchar()
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#define GETCHAR_PROTOTYPE int fgetc(FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE {
    uart_putc(ch);
    return ch;
}
GETCHAR_PROTOTYPE {
    return uart_getc();
}

void apps_terminal_init(uint32_t baudrate) {
    (void)baudrate;
    /* - Initialize UART for example output log (baud 115200)  */
    uart_init(115200);
    /* Disable I/O buffering for STDOUT stream*/
    setvbuf(stdout, NULL, _IONBF, 0);
    /* - Clear terminal */
    printf(PRINT_RESET PRINT_CLEAR_SCREEN);
}

void apps_print_hex_buffer(uint8_t *buffer, uint16_t buffer_size) {
    uint16_t i;
    for (i = 0; i < buffer_size; i++) {
        if (i % 16 == 0) {
            printf(" \n\r ");
        }
        printf(" 0x%02X", buffer[i]);
    }
}

void apps_randomize_buffer(uint8_t *pBuffer, uint16_t buffer_length) {
    for (uint16_t i = 0; i < buffer_length; i++) {
        *(pBuffer + i) = (rng_generate_random_number() & 0xFF);
    }
}

int main(void) {
    stse_ReturnCode_t stse_ret = STSE_API_INVALID_PARAMETER;
    stse_Handler_t stse_handler;
    PLAT_UI16 certificate_size;
    stse_certificate_t parsed_stse_certificate;
    stse_certificate_t parsed_ca_selfsigned_cert;
    static const uint8_t ca_selfsigned_cert[] = {STM_STSAFE_L_CA0001};

    /* - Initialize Terminal */
    apps_terminal_init(115200);

    /* - Print Example instruction on terminal */
    printf(PRINT_CLEAR_SCREEN);
    printf("----------------------------------------------------------------------------------------------------------------");
    printf("\n\r-                          STSAFE-L010 Multi-Steps Device Authentication Example                               -");
    printf("\n\r----------------------------------------------------------------------------------------------------------------");
    printf("\n\r- This example illustrates STSAFE-L010 device authentication process using Multi-Step approach.                -");
    printf("\n\r- it can be taken as reference for building distant server authentication use cases.                           -");
    printf("\n\r----------------------------------------------------------------------------------------------------------------");

    /* ## Initialize STSAFE-L010 device handler */

    stse_ret = stse_set_default_handler_value(&stse_handler);
    if (stse_ret != STSE_OK) {
        printf(PRINT_RED "\n\r ## stse_set_default_handler_value ERROR : 0x%04X\n\r", stse_ret);
        while (1)
            ; // infinite loop
    }

    stse_handler.device_type = STSAFE_L010;
    stse_handler.io.Devaddr = 0x0C;
    stse_handler.io.busID = 1; // Needed to use expansion board I2C

    printf("\n\r - Initialize target STSAFE-L010");
    stse_ret = stse_init(&stse_handler);
    if (stse_ret != STSE_OK) {
        printf(PRINT_RED "\n\r ## stse_init ERROR : 0x%04X\n\r", stse_ret);
        while (1)
            ; // infinite loop
    }

    /* ## Parse CA self-signed certificate */

    stse_ret = stse_certificate_parse(ca_selfsigned_cert, &parsed_ca_selfsigned_cert, NULL);
    if (stse_ret != STSE_OK) {
        printf(PRINT_RED "\n\n\r## ST CA stse_certificate_parse ERROR : 0x%04X", stse_ret);
        while (1)
            ; // infinite loop
    }
    printf("\n\n\r## CA self-signed certificate : \n\r");
    stse_certificate_print_parsed_cert(&parsed_ca_selfsigned_cert);

    /* ## Get target STSE-L010 Certificate  */

    stse_ret = stse_get_device_certificate_size(&stse_handler, STSAFE_CERTIFICATE_ZONE_0, &certificate_size);
    if (stse_ret != STSE_OK) {
        printf(PRINT_RED "\n\n\r## stse_get_device_certificate_size ERROR : 0x%04X", stse_ret);
        while (1)
            ; // infinite loop
    }

    PLAT_UI8 stse_certificate[certificate_size];

    stse_ret = stse_get_device_certificate(&stse_handler, STSAFE_CERTIFICATE_ZONE_0, certificate_size, stse_certificate);
    if (stse_ret != STSE_OK) {
        printf(PRINT_RED "\n\n\r## stse_get_device_certificate ERROR : 0x%04X", stse_ret);
        while (1)
            ; // infinite loop
    }

    /* ## Parse target STSAFE-L010 certificate */

    stse_ret = stse_certificate_parse(stse_certificate, &parsed_stse_certificate, NULL);
    if (stse_ret != STSE_OK) {
        printf(PRINT_RED "\n\n\r## STSE stse_certificate_parse ERROR : 0x%04X", stse_ret);
        while (1)
            ; // infinite loop
    }
    printf("\n\n\r## Target STSAFE-L010 certificate : \n\r");
    stse_certificate_print_parsed_cert(&parsed_stse_certificate);

    /* - Verify STSE Certificate with CA self-signed Certificate */
    stse_ret = stse_certificate_is_parent(&parsed_ca_selfsigned_cert, &parsed_stse_certificate, NULL);
    if (stse_ret != STSE_OK) {
        printf(PRINT_RED "\n\n\r## stse_certificate_is_parent ERROR : 0x%04X", stse_ret);
        while (1)
            ; // infinite loop
    } else {
        printf("\n\n\r## Device Certificate Verified\n\r");
    }

    /* ## Extract certificate key type */
    stse_ecc_key_type_t key_type = stse_certificate_get_key_type(&parsed_stse_certificate);
    PLAT_UI16 signature_size = stse_ecc_info_table[key_type].signature_size;
    PLAT_UI8 signature[signature_size];

    /* ## Generate a challenge ( aligned with key type size requirement) */

    printf("\n\n\r## Host random challenge : \n\r");
    PLAT_UI16 challenge_size = STSAFEL_ECC_SIGNATURE_CHALLENGE_LENGTH;
    PLAT_UI8 challenge[challenge_size];
    apps_randomize_buffer(challenge, challenge_size);
    apps_print_hex_buffer(challenge, challenge_size);

    /* ##  Signature over challenge */

    stse_ret = stse_ecc_generate_signature(
        &stse_handler,                  /* STSE handler */
        STSE_STATIC_PRIVATE_KEY_SLOT_0, /* Slot number */
        key_type,                       /* Certificate key type */
        challenge,                      /* Challenge */
        challenge_size,                 /* Challenge size */
        signature);                     /* Challenge signature */

    if (stse_ret != STSE_OK) {
        printf(PRINT_RED "\n\r## stse_ecc_generate_signature ERROR : 0x%04X", stse_ret);
        while (1)
            ; // infinite loop
    }
    printf("\n\n\r## Device signature over Host challenge: ");
    apps_print_hex_buffer(signature, signature_size);

    /*## Verify The Signature of the Random Number*/

    stse_ret = stse_certificate_verify_signature(
        &parsed_stse_certificate,
        challenge,
        challenge_size,
        signature, (signature_size >> 1),
        &signature[signature_size >> 1], (signature_size >> 1));
    if (stse_ret != STSE_OK) {
        printf(PRINT_RED "\n\n\r## stse_certificate_verify_signature ERROR : 0x%04X", stse_ret);
        while (1)
            ; // infinite loop
    } else {
        printf(PRINT_GREEN "\n\n\r# ## Device Authenticated (Challenge signature verified successfully)");
    }

    while (1)
        ; // infinite loop

    return 0;
}
