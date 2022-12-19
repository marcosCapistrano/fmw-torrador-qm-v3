/*$file${.::perif.c} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: fmw-torrador-v3.qm
* File:  ${.::perif.c}
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
/*$endhead${.::perif.c} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
#include "qpc.h"
#include "common.h"
#include "esp_log.h"

Q_DEFINE_THIS_FILE

static const char * TAG = "PERIF";



void cilindro_set_on(bool on);
bool get_resfriador_state();
void resfriador_set_on(bool on);
void potencia_set_duty(int value);
void cilindro_set_duty(int value);
void turbina_set_duty(int value);
int sample_sensor_ar();
int sample_sensor_grao();
int sample_sensor_gas();

/*$declare${AOs::Perif} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Perif} ............................................................*/
typedef struct {
/* protected: */
    QActive super;

/* public: */
    QTimeEvt sensorTimeEvt;
    int realCil;
    int wantedCil;
    QTimeEvt pwmTimeEvt;
} Perif;

/* protected: */
static QState Perif_initial(Perif * const me, void const * const par);
static QState Perif_sensoring(Perif * const me, QEvt const * const e);
static QState Perif_pre_heating(Perif * const me, QEvt const * const e);
static QState Perif_roasting(Perif * const me, QEvt const * const e);
static QState Perif_idle(Perif * const me, QEvt const * const e);
static QState Perif_off(Perif * const me, QEvt const * const e);
static QState Perif_cooling(Perif * const me, QEvt const * const e);
static QState Perif_summary(Perif * const me, QEvt const * const e);
/*$enddecl${AOs::Perif} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

static Perif l_perif; /* a única instância do AO IHM */

#define SENSOR_INTERVAL  \
    (QTimeEvtCtr)((1 * CONFIG_FREERTOS_HZ))

#define PWM_CHECK_INTERVAL  \
    (QTimeEvtCtr)((1 * CONFIG_FREERTOS_HZ))

QActive * const AO_Perif = &l_perif.super;

/*$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/
/* Check for the minimum required QP version */
#if (QP_VERSION < 690U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpc version 6.9.0 or higher required
#endif
/*$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$define${AOs::Perif} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Perif} ............................................................*/

/*${AOs::Perif::SM} ........................................................*/
static QState Perif_initial(Perif * const me, void const * const par) {
    /*${AOs::Perif::SM::initial} */
    me->realCil = 0;
    cilindro_set_duty(0);
    return Q_TRAN(&Perif_off);
}

/*${AOs::Perif::SM::sensoring} .............................................*/
static QState Perif_sensoring(Perif * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Perif::SM::sensoring} */
        case Q_ENTRY_SIG: {
            ESP_LOGI(TAG, "[SENSORING][ENTRY]");

            QTimeEvt_armX(&me->sensorTimeEvt, SENSOR_INTERVAL, 0U);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::sensoring} */
        case Q_EXIT_SIG: {
            ESP_LOGI(TAG, "[SENSORING][EXIT]");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::sensoring::SENSOR_TIMEOUT} */
        case SENSOR_TIMEOUT_SIG: {
            int temp_ar = sample_sensor_ar();
            vTaskDelay(pdMS_TO_TICKS(100));
            int gas = sample_sensor_gas();
            vTaskDelay(pdMS_TO_TICKS(100));
            int temp_grao = sample_sensor_grao();

            //ESP_LOGE(TAG, "gas here: %d", gas);

            SensorUpdateEvt *sde_gr;
            sde_gr = Q_NEW(SensorUpdateEvt, SENSOR_UPDATE_SIG);
            sde_gr->type = SENSOR_GRAO;
            sde_gr->value = temp_grao;
            QACTIVE_POST(AO_DataBroker, &sde_gr->super, me);

            SensorUpdateEvt *sde_ar;
            sde_ar = Q_NEW(SensorUpdateEvt, SENSOR_UPDATE_SIG);
            sde_ar->type = SENSOR_AR;
            sde_ar->value = temp_ar;
            QACTIVE_POST(AO_DataBroker, &sde_ar->super, me);

            SensorUpdateEvt *sde_gas;
            sde_gas = Q_NEW(SensorUpdateEvt, SENSOR_UPDATE_SIG);
            sde_gas->type = SENSOR_GAS;
            sde_gas->value = gas;
            QACTIVE_POST(AO_DataBroker, &sde_gas->super, me);

            QTimeEvt_rearm(&me->sensorTimeEvt, SENSOR_INTERVAL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::sensoring::PWM_TIMEOUT} */
        case PWM_TIMEOUT_SIG: {
            if(abs(me->wantedCil - me->realCil) > 5) {
                if(me->wantedCil > me->realCil) {
                    int newCil = me->realCil + 5;
                    cilindro_set_duty(newCil);
                    me->realCil = newCil;
                    QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
                } else {
                    int newCil = me->realCil - 5;
                    cilindro_set_duty(newCil);
                    me->realCil = newCil;
                    QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
                }
            } else if(me->realCil != me->wantedCil) {
                cilindro_set_duty(me->wantedCil);
                me->realCil = me->wantedCil;
            }
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::sensoring::CONTROL_UPDATE} */
        case CONTROL_UPDATE_SIG: {
            ControlUpdateEvt *evt = Q_EVT_CAST(ControlUpdateEvt);
            ControlType type = evt->control;
            int value = evt->value;

            ESP_LOGD(TAG, "Received command: %d - value: %d", type, value);

            if(type == POTENCIA) {
                potencia_set_duty(value);
            } else if(type == CILINDRO) {
                if(value > 100) value = 100;
                else if(value < 0) value = 0;

                me->wantedCil = value;
                QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
            } else if(type == TURBINA) {
                turbina_set_duty(value);
            } else if(type == RESFRIADOR) {
                resfriador_set_on(value);
            }

            ControlDataEvt *contEv;
            contEv = Q_NEW(ControlDataEvt, CONTROL_DATA_SIG);
            contEv->control = type;
            contEv->value = value;
            QACTIVE_POST(AO_DataBroker, &contEv->super, me);

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

/*${AOs::Perif::SM::sensoring::pre_heating} ................................*/
static QState Perif_pre_heating(Perif * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Perif::SM::sensoring::pre_heating} */
        case Q_ENTRY_SIG: {
            ESP_LOGD(TAG, "[SENSORING][PRE_HEAT][ENTRY]");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::sensoring::pre_heating} */
        case Q_EXIT_SIG: {
            ESP_LOGI(TAG, "[SENSORING][PRE_HEAT][EXIT]");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::sensoring::pre_heating::NOTIFY_NEXT_STAGE} */
        case NOTIFY_NEXT_STAGE_SIG: {
            status_ = Q_TRAN(&Perif_roasting);
            break;
        }
        default: {
            status_ = Q_SUPER(&Perif_sensoring);
            break;
        }
    }
    return status_;
}

/*${AOs::Perif::SM::sensoring::roasting} ...................................*/
static QState Perif_roasting(Perif * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Perif::SM::sensoring::roasting} */
        case Q_ENTRY_SIG: {
            ESP_LOGD(TAG, "[SENSORING][ROASTING][ENTRY]");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::sensoring::roasting} */
        case Q_EXIT_SIG: {
            ESP_LOGD(TAG, "[SENSORING][ROASTING][EXIT]");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::sensoring::roasting::NOTIFY_NEXT_STAGE} */
        case NOTIFY_NEXT_STAGE_SIG: {
            status_ = Q_TRAN(&Perif_cooling);
            break;
        }
        default: {
            status_ = Q_SUPER(&Perif_sensoring);
            break;
        }
    }
    return status_;
}

/*${AOs::Perif::SM::sensoring::idle} .......................................*/
static QState Perif_idle(Perif * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Perif::SM::sensoring::idle} */
        case Q_ENTRY_SIG: {
            ESP_LOGD(TAG, "[SENSORING][IDLE][ENTRY]");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::sensoring::idle} */
        case Q_EXIT_SIG: {
            ESP_LOGD(TAG, "[SENSORING][IDLE][EXIT]");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::sensoring::idle::NOTIFY_NEXT_STAGE} */
        case NOTIFY_NEXT_STAGE_SIG: {
            status_ = Q_TRAN(&Perif_pre_heating);
            break;
        }
        default: {
            status_ = Q_SUPER(&Perif_sensoring);
            break;
        }
    }
    return status_;
}

/*${AOs::Perif::SM::off} ...................................................*/
static QState Perif_off(Perif * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Perif::SM::off} */
        case Q_ENTRY_SIG: {
            ESP_LOGI(TAG, "[OFF][ENTRY]");

            const int newCil = 0;
            const int newPot = 0;
            const int newTurb = 0;

            if(get_resfriador_state()) {
                resfriador_set_on(false);
            }

            potencia_set_duty(newPot);

            ControlDataEvt *contEv = Q_NEW(ControlDataEvt, CONTROL_DATA_SIG);
            contEv->control = POTENCIA;
            contEv->value = newPot;
            QACTIVE_POST(AO_DataBroker, &contEv->super, me);

            me->wantedCil = newCil;
            ControlDataEvt *contEv2;
            contEv2 = Q_NEW(ControlDataEvt, CONTROL_DATA_SIG);
            contEv2->control = CILINDRO;
            contEv2->value = newCil;
            QACTIVE_POST(AO_DataBroker, &contEv2->super, me);

            turbina_set_duty(newTurb);
            ControlDataEvt *contEv3;
            contEv3 = Q_NEW(ControlDataEvt, CONTROL_DATA_SIG);
            contEv3->control = TURBINA;
            contEv3->value = newTurb;
            QACTIVE_POST(AO_DataBroker, &contEv3->super, me);

            QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::off} */
        case Q_EXIT_SIG: {
            ESP_LOGI(TAG, "[OFF][EXIT]");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::off::PWM_TIMEOUT} */
        case PWM_TIMEOUT_SIG: {
            if(abs(me->wantedCil - me->realCil) > 5) {
                if(me->wantedCil > me->realCil) {
                    int newCil = me->realCil + 5;
                    cilindro_set_duty(newCil);
                    me->realCil = newCil;
                    QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
                } else {
                    int newCil = me->realCil - 5;
                    cilindro_set_duty(newCil);
                    me->realCil = newCil;
                    QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
                }
            } else if(me->realCil != me->wantedCil) {
                cilindro_set_duty(me->wantedCil);
                me->realCil = me->wantedCil;
            }
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::off::NOTIFY_MODE} */
        case NOTIFY_MODE_SIG: {
            NotifyModeEvt *nme = Q_EVT_CAST(NotifyModeEvt);
            /*${AOs::Perif::SM::off::NOTIFY_MODE::[MODE_ENTER]} */
            if (nme->mode == MODE_AUTO) {
                status_ = Q_TRAN(&Perif_pre_heating);
            }
            /*${AOs::Perif::SM::off::NOTIFY_MODE::[nme->mode==MODE_MANUAL]} */
            else if (nme->mode == MODE_MANUAL) {
                status_ = Q_TRAN(&Perif_idle);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        default: {
            status_ = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status_;
}

/*${AOs::Perif::SM::cooling} ...............................................*/
static QState Perif_cooling(Perif * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Perif::SM::cooling} */
        case Q_ENTRY_SIG: {
            ESP_LOGD(TAG, "[COOLING][ENTRY]");

            if(!get_resfriador_state()) {
                resfriador_set_on(true);
            }
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::cooling} */
        case Q_EXIT_SIG: {
            ESP_LOGD(TAG, "[COOLING][EXIT]");

            QTimeEvt_disarm(&me->sensorTimeEvt);
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::cooling::NOTIFY_NEXT_STAGE} */
        case NOTIFY_NEXT_STAGE_SIG: {
            status_ = Q_TRAN(&Perif_summary);
            break;
        }
        /*${AOs::Perif::SM::cooling::CONTROL_UPDATE} */
        case CONTROL_UPDATE_SIG: {
            ControlUpdateEvt *evt = Q_EVT_CAST(ControlUpdateEvt);
            ControlType type = evt->control;
            int value = evt->value;

            ESP_LOGD(TAG, "Received command: %d - value: %d", type, value);

            if(type == POTENCIA) {
                potencia_set_duty(value);
            } else if(type == CILINDRO) {
                if(value > 100) value = 100;
                else if(value < 0) value = 0;

                me->wantedCil = value;
                QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
            } else if(type == TURBINA) {
                turbina_set_duty(value);
            } else if(type == RESFRIADOR) {
                resfriador_set_on(value);
            }

            ControlDataEvt *contPot;
            contPot = Q_NEW(ControlDataEvt, CONTROL_DATA_SIG);
            contPot->control = type;
            contPot->value = value;
            QACTIVE_POST(AO_DataBroker, &contPot->super, me);

            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::cooling::PWM_TIMEOUT} */
        case PWM_TIMEOUT_SIG: {
            if(abs(me->wantedCil - me->realCil) > 5) {
                if(me->wantedCil > me->realCil) {
                    int newCil = me->realCil + 5;
                    cilindro_set_duty(newCil);
                    me->realCil = newCil;
                    QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
                } else {
                    int newCil = me->realCil - 5;
                    cilindro_set_duty(newCil);
                    me->realCil = newCil;
                    QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
                }
            } else if(me->realCil != me->wantedCil) {
                cilindro_set_duty(me->wantedCil);
                me->realCil = me->wantedCil;
            }
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::cooling::SENSOR_TIMEOUT} */
        case SENSOR_TIMEOUT_SIG: {
            int temp_ar = sample_sensor_ar();
            vTaskDelay(pdMS_TO_TICKS(100));
            int gas = sample_sensor_gas();
            vTaskDelay(pdMS_TO_TICKS(100));
            int temp_grao = sample_sensor_grao();

            //ESP_LOGE(TAG, "gas here: %d", gas);

            SensorUpdateEvt *sde_gr;
            sde_gr = Q_NEW(SensorUpdateEvt, SENSOR_UPDATE_SIG);
            sde_gr->type = SENSOR_GRAO;
            sde_gr->value = temp_grao;
            QACTIVE_POST(AO_DataBroker, &sde_gr->super, me);

            SensorUpdateEvt *sde_ar;
            sde_ar = Q_NEW(SensorUpdateEvt, SENSOR_UPDATE_SIG);
            sde_ar->type = SENSOR_AR;
            sde_ar->value = temp_ar;
            QACTIVE_POST(AO_DataBroker, &sde_ar->super, me);

            SensorUpdateEvt *sde_gas;
            sde_gas = Q_NEW(SensorUpdateEvt, SENSOR_UPDATE_SIG);
            sde_gas->type = SENSOR_GAS;
            sde_gas->value = gas;
            QACTIVE_POST(AO_DataBroker, &sde_gas->super, me);

            QTimeEvt_rearm(&me->sensorTimeEvt, SENSOR_INTERVAL);
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

/*${AOs::Perif::SM::summary} ...............................................*/
static QState Perif_summary(Perif * const me, QEvt const * const e) {
    QState status_;
    switch (e->sig) {
        /*${AOs::Perif::SM::summary} */
        case Q_ENTRY_SIG: {
            ESP_LOGI(TAG, "[SUMMARY][ENTRY]");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::summary} */
        case Q_EXIT_SIG: {
            ESP_LOGI(TAG, "[SUMMARY][EXIT]");
            status_ = Q_HANDLED();
            break;
        }
        /*${AOs::Perif::SM::summary::NOTIFY_NEXT_STAGE} */
        case NOTIFY_NEXT_STAGE_SIG: {
            status_ = Q_TRAN(&Perif_pre_heating);
            break;
        }
        /*${AOs::Perif::SM::summary::NOTIFY_MODE} */
        case NOTIFY_MODE_SIG: {
            NotifyModeEvt *nme = Q_EVT_CAST(NotifyModeEvt);
            /*${AOs::Perif::SM::summary::NOTIFY_MODE::[MODE_EXIT]} */
            if (nme->mode == MODE_NONE) {
                status_ = Q_TRAN(&Perif_off);
            }
            else {
                status_ = Q_UNHANDLED();
            }
            break;
        }
        /*${AOs::Perif::SM::summary::PWM_TIMEOUT} */
        case PWM_TIMEOUT_SIG: {
            if(abs(me->wantedCil - me->realCil) > 5) {
                if(me->wantedCil > me->realCil) {
                    int newCil = me->realCil + 5;
                    cilindro_set_duty(newCil);
                    me->realCil = newCil;
                    QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
                } else {
                    int newCil = me->realCil - 5;
                    cilindro_set_duty(newCil);
                    me->realCil = newCil;
                    QTimeEvt_rearm(&me->pwmTimeEvt, PWM_CHECK_INTERVAL);
                }
            } else if(me->realCil != me->wantedCil) {
                cilindro_set_duty(me->wantedCil);
                me->realCil = me->wantedCil;
            }

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
/*$enddef${AOs::Perif} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$define${AOs::Perif_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Perif_ctor} .......................................................*/
void Perif_ctor(void) {
    Perif *me = &l_perif;

    QActive_ctor(&me->super, Q_STATE_CAST(&Perif_initial));
    QTimeEvt_ctorX(&me->sensorTimeEvt, &me->super, SENSOR_TIMEOUT_SIG, 0U);
    QTimeEvt_ctorX(&me->pwmTimeEvt, &me->super, PWM_TIMEOUT_SIG, 0U);
}
/*$enddef${AOs::Perif_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
