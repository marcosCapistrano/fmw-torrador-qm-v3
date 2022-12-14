/*$file${..::components::common::common.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: fmw-torrador-v3.qm
* File:  ${..::components::common::common.c}
*
* This code has been generated by QM 5.2.2 <www.state-machine.com/qm>.
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* SPDX-License-Identifier: GPL-3.0-or-later
*
* This generated code is open source software: you can redistribute it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation.
*
* This code is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* NOTE:
* Alternatively, this generated code may be distributed under the terms
* of Quantum Leaps commercial licenses, which expressly supersede the GNU
* General Public License and are specifically designed for licensees
* interested in retaining the proprietary status of their code.
*
* Contact information:
* <www.state-machine.com/licensing>
* <info@state-machine.com>
*/
/*$endhead${..::components::common::common.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include "common.h"
#include <string.h>

Q_DEFINE_THIS_FILE

static const char *TAG = "COMMON";

/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 6.9.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${Common} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${Common::postUart_setPage} ..............................................*/
void postUart_setPage(unsigned short int picid) {
    ESP_LOGV(TAG, "[IHM_UART_SET_PAGE]");

    UartOutputPageEvt *pageEv = Q_NEW(UartOutputPageEvt, UART_OUTPUT_PAGE_SIG);
    pageEv->picid = picid;
    QACTIVE_POST(AO_Uart, &pageEv->super, me);
}

/*${Common::postUart_setString} ............................................*/
void postUart_setString(
    unsigned short int vp,
    char * text,
    bool reset,
    size_t reset_len)
{
    ESP_LOGV(TAG, "[IHM_UART_SET_STRING]");

    if(reset) {
        char resetStr[100] = {0};
        int i;

        for(i=0; i<reset_len; i++)
            strcpy(&resetStr[i], " ");

        i++;
        strcpy(&resetStr[i], "\0");
        //ESP_LOGE(TAG, "Reseting to: %s - len: %d", resetStr, strlen(resetStr));

        UartOutputTextEvt *resetEv = Q_NEW(UartOutputTextEvt, UART_OUTPUT_TEXT_SIG);
        resetEv->vp = vp;
        resetEv->text = resetStr;
        QACTIVE_POST(AO_Uart, &resetEv->super, me);
    }

    if(strlen(text) > reset_len) {
        strcpy(&text[reset_len-1], "\0");
    }

    UartOutputTextEvt *textEv = Q_NEW(UartOutputTextEvt, UART_OUTPUT_TEXT_SIG);
    textEv->vp = vp;
    textEv->text = text;
    QACTIVE_POST(AO_Uart, &textEv->super, me);


}

/*${Common::postUart_setIcon} ..............................................*/
void postUart_setIcon(
    unsigned short int vp,
    unsigned short int icon)
{
    ESP_LOGV(TAG, "[IHM_UART_SET_ICON]");

    UartOutputIconEvt *iconEv = Q_NEW(UartOutputIconEvt, UART_OUTPUT_ICON_SIG);
    iconEv->vp = vp;
    iconEv->icon = icon;
    QACTIVE_POST(AO_Uart, &iconEv->super, me);
}

/*${Common::postUart_setNumber} ............................................*/
void postUart_setNumber(
    unsigned short int vp,
    unsigned short value)
{
    ESP_LOGV(TAG, "[IHM_UART_SET_NUMBER]");

    UartOutputNumberEvt *numberEv = Q_NEW(UartOutputNumberEvt, UART_OUTPUT_NUMBER_SIG);
    numberEv->vp = vp;
    numberEv->value = value;
    QACTIVE_POST(AO_Uart, &numberEv->super, me);


}

/*${Common::postData_requestData} ..........................................*/
void postData_requestData(
    DataType type,
    Data data)
{
    ESP_LOGV(TAG, "[IHM_DATA_REQUEST_DATA]");

    DataRequestEvt *reqEv = Q_NEW(DataRequestEvt, DATA_REQUEST_SIG);
    reqEv->type = type;
    reqEv->data = data;
    QACTIVE_POST(AO_DataBroker, reqEv, me);
}

/*${Common::postIhm_respondData} ...........................................*/
void postIhm_respondData(
    DataType type,
    Data data)
{
    ESP_LOGV(TAG, "[DATA_IHM_RESPOND_DATA]");

    DataResponseEvt *resEv = Q_NEW(DataResponseEvt, DATA_RESPONSE_SIG);
    resEv->type = type;
    resEv->data = data;
    QACTIVE_POST(AO_Ihm, resEv, me);
}

/*${Common::postUart_setNumberAsString} ....................................*/
void postUart_setNumberAsString(
    unsigned short int vp,
    char * text,
    char * ext,
    bool reset,
    size_t reset_len)
{
    ESP_LOGV(TAG, "[IHM_UART_SET_NUMBER_AS_STRING]");

    if(reset) {
        char resetStr[100] = {0};
        int i;

        for(i=0; i<reset_len; i++)
            strcpy(&resetStr[i], " ");

        i++;
        strcpy(&resetStr[i], "\0");
        //ESP_LOGE(TAG, "Reseting to: %s - len: %d", resetStr, strlen(resetStr));

        UartOutputTextEvt *resetEv = Q_NEW(UartOutputTextEvt, UART_OUTPUT_TEXT_SIG);
        resetEv->vp = vp;
        resetEv->text = resetStr;
        QACTIVE_POST(AO_Uart, &resetEv->super, me);
    }

    if(strlen(text) > reset_len) {
        strcpy(&text[reset_len-1], "\0");
    }

    char str[6] = {0};
    sprintf(str, "%d%s", value, ext);

    UartOutputTextEvt *textEv = Q_NEW(UartOutputTextEvt, UART_OUTPUT_TEXT_SIG);
    textEv->vp = vp;
    textEv->text = str;
    QACTIVE_POST(AO_Uart, &textEv->super, me);


}
/*$enddef${Common} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
