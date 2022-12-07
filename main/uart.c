/*$file${.::uart.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: fmw-torrador-v3.qm
* File:  ${.::uart.c}
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
/*$endhead${.::uart.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include "qpc.h"
#include <string.h>
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "common.h"
#include "esp_unicviewad.h"

#define TXD_PIN 17
#define RXD_PIN 16
#define UART_NUM UART_NUM_2

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

#define COLOR_GRAO 54468
#define COLOR_AR 30000

static const char * TAG = "UART";

Q_DEFINE_THIS_FILE

/*$declare${AOs::Uart} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Uart} .............................................................*/
typedef struct {
/* protected: */
    QActive super;

/* public: */
    QueueHandle_t uartQueue;
} Uart;

/* private: */
static void Uart_uartTask(void * pvParameters);
static void Uart_processEvent(
    uint8_t * data,
    int start,
    int end);

/* public: */
static void Uart_changePage(unsigned short int picid);
static void Uart_changeIcon(
    unsigned short int vp,
    unsigned short int icon);
static void Uart_changeText(
    unsigned short int vp,
    const char * text);
static void Uart_changeGraphicsCircle(
    unsigned short int vp,
    int * temperatures,
    unsigned short int count,
    SensorType sensor,
    int minX,
    int maxX,
    int minY,
    int maxY,
    bool delta,
    int deltaX,
    int maxTemp);
static void Uart_changeGraphicsLine(
    unsigned short int vp,
    int * values,
    unsigned short int count,
    SensorType sensor,
    int minX,
    int maxX,
    int minY,
    int maxY,
    bool deltaBound,
    int deltaBoundX,
    int maxTemp,
    int minTemp,
    int originY,
    bool isDelta);
static void Uart_changeChart(
    unsigned short int vp,
    SensorType sensor,
    int * values,
    unsigned short int count,
    int minX,
    int maxX,
    int minY,
    int maxY,
    bool deltaBound,
    int deltaBoundX,
    int minTemp,
    int maxTemp,
    int originY,
    bool isDelta);

/* protected: */
static QState Uart_initial(Uart * const me, void const * const par);
static QState Uart_state1(Uart * const me, QEvt const * const e);
/*$enddecl${AOs::Uart} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

static Uart l_uart; /* a única instância do AO IHM */

QActive * const AO_Uart = &l_uart.super;

/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 6.9.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${AOs::Uart} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Uart} .............................................................*/

/*${AOs::Uart::uartTask} ...................................................*/
static void Uart_uartTask(void * pvParameters) {
    Uart *me = &l_uart;
    uart_event_t uart_event;

        for (;;) {
            if (xQueueReceive(me->uartQueue, (void *)&uart_event, portMAX_DELAY)) {
                size_t buffer_size = uart_event.size;
                uint8_t data[128];

                /* Checa se o tipo de evento é do tipo 0 (DATA), se não for, analisar pq pode ser fonte de bugs */
                if (uart_event.type == 0) {
                    uart_read_bytes(UART_NUM, data, buffer_size, portMAX_DELAY);

                    Uart_processEvent(data, 0, buffer_size);
                } else {
                    ESP_LOGE(TAG, "[UNKNOWN UART EVENT]: %d", uart_event.type);
                }
            }
        }
}

/*${AOs::Uart::processEvent} ...............................................*/
static void Uart_processEvent(
    uint8_t * data,
    int start,
    int end)
{
    /*
    Se não conter um header, é um touch release.
    Se conter, é um input no slider
    */

    /*
    for(int i=start; i<end; i++)
        printf("%d |", data[i]);
    printf("\n");
    */

    if(data[start] != '\x5A') {
        if (end - start >= 24) {
            unsigned short pre = 0, cool = 0;
            memcpy(&pre, &data[start], sizeof(unsigned short));
            swapbytes(&pre, sizeof(unsigned short));

            memcpy(&cool, &data[start] + 4 + sizeof(unsigned short), sizeof(unsigned short));
            swapbytes(&cool, sizeof(unsigned short));

            UartInputConfigSaveEvt *uic = Q_NEW(UartInputConfigSaveEvt, UART_INPUT_CONFIG_SAVE_SIG);
            uic->pre = pre;
            uic->cool = cool;

            QACTIVE_POST(AO_Uart, &uic->super, me);
        } else {
            UartInputTouchEvt *ure = Q_NEW(UartInputTouchEvt, UART_INPUT_TOUCH_SIG);
            ure->length = end-start;

            ESP_LOGI(TAG, "[TOUCH], Length: %d", (end-start));
            QACTIVE_POST(AO_Uart, &ure->super, me);
        }
    } else {
        UartInputSliderEvt *ure = Q_NEW(UartInputSliderEvt, UART_INPUT_SLIDER_SIG);

        unsigned short int vp = 0;
        unsigned short int value = 0;
        ControlType controlType = CONTROL_NONE;

        memcpy(&vp, data+4, sizeof(unsigned short int));
        memcpy(&value, data+7, sizeof(unsigned short int));
        swapbytes(&vp, sizeof(unsigned short int));
        swapbytes(&value, sizeof(unsigned short int));

        if(vp == VP_CONTROL_POTENCIA) {
            controlType = POTENCIA;
        } else if(vp == VP_CONTROL_CILINDRO) {
            controlType = CILINDRO;
        } else if(vp == VP_CONTROL_TURBINA) {
            controlType = TURBINA;
        }

        ure->control = controlType;
        ure->value = value;

        ESP_LOGI(TAG, "[SLIDER]: Type: %d, value %d", controlType, value);
        QACTIVE_POST(AO_Uart, &ure->super, me);
    }
}

/*${AOs::Uart::changePage} .................................................*/
static void Uart_changePage(unsigned short int picid) {
    unsigned char container[MAX_CONTAINER_SIZE];

    ESP_LOGI(TAG, "[CHANGE PAGE]: %u", picid);
    swapbytes(&picid, sizeof(unsigned short int));

    int length = packet_picid(&picid, container);

    ESP_LOGD(TAG, "[UART CONTAINER]: [");
    for (int i = 0; i < length; i++) {
        ESP_LOGD(TAG, "%2.2X ", container[i]);
    }
    ESP_LOGD(TAG, "]\n");

    uart_write_bytes(UART_NUM, container, length);
}

/*${AOs::Uart::changeIcon} .................................................*/
static void Uart_changeIcon(
    unsigned short int vp,
    unsigned short int icon)
{
    unsigned char container[MAX_CONTAINER_SIZE];

    swapbytes(&vp, sizeof(vp));
    swapbytes(&icon, sizeof(icon));
    int length = packet_write_vp(vp, &icon, sizeof(unsigned short int), container);

    //print_container(container, length);
    uart_write_bytes(UART_NUM, container, length);
}

/*${AOs::Uart::changeText} .................................................*/
static void Uart_changeText(
    unsigned short int vp,
    const char * text)
{
    unsigned char container[MAX_CONTAINER_SIZE];

    swapbytes(&vp, sizeof(vp));
    int length = packet_write_vp(vp, text, strlen(text), container);

    uart_write_bytes(UART_NUM, container, length);
}

/*${AOs::Uart::changeGraphicsCircle} .......................................*/
static void Uart_changeGraphicsCircle(
    unsigned short int vp,
    int * temperatures,
    unsigned short int count,
    SensorType sensor,
    int minX,
    int maxX,
    int minY,
    int maxY,
    bool delta,
    int deltaX,
    int maxTemp)
{
    unsigned char container[MAX_CONTAINER_SIZE];
    unsigned char payload_container[MAX_PAYLOAD_SIZE];

    unsigned short int color = 0;
    if(sensor == SENSOR_GRAO) {
        color = COLOR_GRAO;
    } else {
        color = COLOR_AR;
    }

    swapbytes(&vp, sizeof(vp));
    int payload_length = packet_graphics_circles(temperatures, count, color, minX, maxX, minY, maxY, delta, deltaX, maxTemp, payload_container);
    int length = packet_write_vp(vp, payload_container, payload_length, container);

    uart_write_bytes(UART_NUM, container, length);
}

/*${AOs::Uart::changeGraphicsLine} .........................................*/
static void Uart_changeGraphicsLine(
    unsigned short int vp,
    int * values,
    unsigned short int count,
    SensorType sensor,
    int minX,
    int maxX,
    int minY,
    int maxY,
    bool deltaBound,
    int deltaBoundX,
    int maxTemp,
    int minTemp,
    int originY,
    bool isDelta)
{
    unsigned char container[MAX_CONTAINER_SIZE];
    unsigned char payload_container[MAX_PAYLOAD_SIZE];

    unsigned short int color = 0;
    if(sensor == SENSOR_GRAO) {
        color = COLOR_GRAO;
    } else {
        color = COLOR_AR;
    }

    swapbytes(&vp, sizeof(vp));
    int payload_length = packet_graphics_lines(values, count, color, minX, maxX, minY, maxY, deltaBound, deltaBoundX, minTemp, maxTemp, originY, isDelta, payload_container);
    int length = packet_write_vp(vp, payload_container, payload_length, container);

    uart_write_bytes(UART_NUM, container, length);
}

/*${AOs::Uart::changeChart} ................................................*/
static void Uart_changeChart(
    unsigned short int vp,
    SensorType sensor,
    int * values,
    unsigned short int count,
    int minX,
    int maxX,
    int minY,
    int maxY,
    bool deltaBound,
    int deltaBoundX,
    int minTemp,
    int maxTemp,
    int originY,
    bool isDelta)
{
    if(sensor == SENSOR_AR) {
        Uart_changeGraphicsLine(vp, values, count, sensor, minX, maxX, minY, maxY, deltaBound, deltaBoundX, maxTemp, minTemp, originY, isDelta);
    } else {
        Uart_changeGraphicsLine(vp, values, count, sensor, minX, maxX, minY, maxY, deltaBound, deltaBoundX, maxTemp, minTemp, originY, isDelta);
    }
}

/*${AOs::Uart::SM} .........................................................*/
static QState Uart_initial(Uart * const me, void const * const par) {
    /*${AOs::Uart::SM::initial} */
    return Q_TRAN(&Uart_state1);
}

/*${AOs::Uart::SM::state1} .................................................*/
static QState Uart_state1(Uart * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Uart::SM::state1::UART_INPUT_TOUCH} */
        case UART_INPUT_TOUCH_SIG: {
            UartInputTouchEvt *uev = Q_EVT_CAST(UartInputTouchEvt);

            IhmInputTouchEvt *iev = Q_NEW(IhmInputTouchEvt, IHM_INPUT_TOUCH_SIG);
            iev->length = uev->length;

            QACTIVE_POST(AO_Ihm, &iev->super, me);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Uart::SM::state1::UART_INPUT_SLIDER} */
        case UART_INPUT_SLIDER_SIG: {
            UartInputSliderEvt *uev = Q_EVT_CAST(UartInputSliderEvt);
            ControlType control = uev->control;
            int value = uev->value;

            IhmInputSliderEvt *iev = Q_NEW(IhmInputSliderEvt, IHM_INPUT_SLIDER_SIG);
            iev->control = control;
            iev->value = value;

            QACTIVE_POST(AO_Ihm, &iev->super, me);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Uart::SM::state1::UART_OUTPUT_PAGE} */
        case UART_OUTPUT_PAGE_SIG: {
            UartOutputPageEvt *uev = Q_EVT_CAST(UartOutputPageEvt);

            Uart_changePage(uev->picid);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Uart::SM::state1::UART_OUTPUT_ICON} */
        case UART_OUTPUT_ICON_SIG: {
            UartOutputIconEvt *uev = Q_EVT_CAST(UartOutputIconEvt);

            Uart_changeIcon(uev->vp, uev->icon);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Uart::SM::state1::UART_OUTPUT_TEXT} */
        case UART_OUTPUT_TEXT_SIG: {
            UartOutputTextEvt *uev = Q_EVT_CAST(UartOutputTextEvt);

            Uart_changeText(uev->vp, uev->text);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Uart::SM::state1::UART_OUTPUT_CHART} */
        case UART_OUTPUT_CHART_SIG: {
            UartOutputChartEvt *uev = Q_EVT_CAST(UartOutputChartEvt);

            Uart_changeChart(uev->vp, uev->sensor, uev->values, uev->count,
            uev->minX, uev->maxX, uev->minY, uev->maxY, uev->deltaBound, uev->deltaBoundX, uev->minTemp, uev->maxTemp, uev->originY, uev->isDelta);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Uart::SM::state1::UART_INPUT_CONFIG_SAVE} */
        case UART_INPUT_CONFIG_SAVE_SIG: {
            UartInputConfigSaveEvt *uic = Q_EVT_CAST(UartInputConfigSaveEvt);

            IhmInputConfigSaveEvt *hic = Q_NEW(IhmInputConfigSaveEvt, IHM_INPUT_CONFIG_SAVE_SIG);
            hic->pre = uic->pre;
            hic->cool = uic->cool;

            QACTIVE_POST(AO_Ihm, &hic->super, me);
            status_ = Q_HANDLED();
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}
/*$enddef${AOs::Uart} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$define${AOs::Uart_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Uart_ctor} ........................................................*/
void Uart_ctor(void) {
    Uart *me = &l_uart;

    uart_config_t uart_config = {
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        };

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 30, &me->uartQueue, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    xTaskCreate(Uart_uartTask, "UART_TASK", 8192, NULL, 2, NULL);

    ESP_LOGI(TAG, "Created uart task");



    QActive_ctor(&me->super, Q_STATE_CAST(&Uart_initial));
}
/*$enddef${AOs::Uart_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
