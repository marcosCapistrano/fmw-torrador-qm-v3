/*$file${..::components::common::include::common.h} vvvvvvvvvvvvvvvvvvvvvvvv*/
/*
* Model: fmw-torrador-v3.qm
* File:  ${..::components::common::include::common.h}
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
/*$endhead${..::components::common::include::common.h} ^^^^^^^^^^^^^^^^^^^^^*/
#ifndef COMMON_H
#define COMMON_H

#include "qpc.h"
#include "esp_log.h"

enum Signals {
    UART_INPUT_TOUCH_SIG = Q_USER_SIG, /* the new request signal */
    UART_INPUT_SLIDER_SIG,
    UART_OUTPUT_PAGE_SIG,
    UART_OUTPUT_ICON_SIG,
    UART_OUTPUT_TEXT_SIG,
    UART_OUTPUT_NUMBER_SIG,
    UART_OUTPUT_CHART_SIG,
    UART_INPUT_CONFIG_SAVE_SIG,

    IHM_INPUT_TOUCH_SIG,
    IHM_INPUT_SLIDER_SIG,
    IHM_STAGE_TIMER_TIMEOUT_SIG,
    IHM_INPUT_CONFIG_SAVE_SIG,

    REQUEST_NEXT_STAGE_SIG,
    NOTIFY_NEXT_STAGE_SIG,
    REQUEST_NEXT_SUBSTAGE_SIG,
    NOTIFY_NEXT_SUBSTAGE_SIG,
    NOTIFY_SUBSTAGE_EXIT_SIG,

    PWM_TIMEOUT_SIG,
    SENSOR_TIMEOUT_SIG,

    REQUEST_RECIPES_SIG,
    RESPONSE_RECIPES_SIG,
    REQUEST_RECIPE_SIG,
    RESPONSE_RECIPE_SIG,

    REQUEST_ROASTS_SIG,
    REQUEST_ROAST_SIG,
    RESPONSE_ROASTS_SIG,
    RESPONSE_ROAST_SIG,

    REQUEST_MODE_SIG,
    NOTIFY_MODE_SIG,

    SENSOR_UPDATE_SIG,
    SENSOR_GAS_UPDATE_SIG,
    SENSOR_DATA_SIG,
    SENSOR_GAS_DATA_SIG,
    CHART_DATA_SIG,

    CONTROL_UPDATE_SIG,
    CONTROL_DATA_SIG,

    CONFIG_UPDATE_SIG,

    REQUEST_TRANSFORM_ROAST_SIG,
    DATA_RECIPE_TIMEOUT_SIG,

    REQUEST_SUMMARY_SIG,
    RESPONSE_SUMMARY_SIG,

    REQUEST_CONFIG_SIG,
    RESPONSE_CONFIG_SIG,
};

typedef enum ControlType {
    CONTROL_NONE,
    POTENCIA,
    CILINDRO,
    TURBINA,
    RESFRIADOR,
} ControlType;

typedef enum SensorTypeTag {
    SENSOR_AR,
    SENSOR_GRAO,
    SENSOR_GAS
} SensorType;

typedef enum PageTypeTag {
    PAGE_MAIN_MENU,
    PAGE_MANUAL_MODE,
    PAGE_RECIPES,
    PAGE_RECIPE,
    PAGE_ROASTS,
    PAGE_ROAST,
    PAGE_CONTROLS,
    PAGE_CONFIG
} PageType;

typedef enum ModeTypeTag {
    MODE_NONE,
    MODE_MANUAL,
    MODE_AUTO
} ModeType;

typedef enum SubstageTypeTag {
    SUB_IDLE,
    F,
    Q1,
    Q2,
} SubstageType;

typedef enum StageTypeTag {
    IDLE,
    PRE_HEAT,
    ROAST,
    COOL,
    SUMMARY,
} StageType;

typedef struct ModeDataTag {
    ModeType mode;
    char roast[25];
} ModeData;

typedef struct TempDataTag {
    int count;
    int temps[31];
    int deltas[31];
    int temp;
    int delta;
} TempData;

typedef struct CommandDataTag {
    char command[15];
    time_t time;
} CommandData;

typedef struct SensorDataTag {
    TempData ar;
    TempData grao;
    int max;
    int min;
    int max_delta;
    int min_delta;
    int grao_aux;
    int ar_aux;
} SensorData;

typedef struct RoastsResponseTag {
    uint8_t pageNum;
    char roast1[25];
    char roast2[25];
    char roast3[25];
    bool prevPage;
    bool nextPage;
} RoastsResponse;

typedef struct RoastResponseTag {
    char roast[25];
    SensorData *sensorData;
} RoastResponse;

typedef struct RecipesResponseTag {
    uint8_t pageNum;
    char roast1[25];
    char roast2[25];
    char roast3[25];
    bool prevPage;
    bool nextPage;
} RecipesResponse;

typedef struct RecipeResponseTag {
    char roast[25];
    SensorData *sensorData;
} RecipeResponse;

typedef struct ControlDataTag {
    uint16_t potencia;
    uint16_t cilindro;
    uint16_t turbina;
    uint16_t resfriador;
} ControlData;

typedef struct CommandDataTag {
    ControlData control_data;
    time_t timestamp;
} CommandData;

typedef struct RoastDataTag {
    time_t time_start;
    time_t time_end;
    SensorData sensor_data;
    ControlData control_data;
    char name[9];
} RoastData;

typedef struct RecipeDataTag {
    char name[25];
    time_t intervals[100];
    ControlType controls[100];
    int values[100];
    int count;
    int curr_command;
} RecipeData;

typedef struct ConfigTag {
    uint16_t max_pre_heat;
    uint16_t max_roast;
} Config;

typedef struct ControlStateTag {
    int potencia;
    int cilindro;
    ControlToggle turbina;
    ControlToggle resfriador;
} ControlState;

typedef struct RoastsPageDataTag {
    int page;
    char roast1[25];
    char roast2[25];
    char roast3[25];
    bool prev;
    bool next;
} RoastsPageData;

typedef struct RoastPageDataTag {
    char roast[25];
} RoastPageData;

typedef struct ControlPageDataTag {
    int sensor_grao;
    int sensor_ar;
    int potencia;
    int cilindro;
    int turbina;
    int resfriador;
}

typedef struct ConfigPageDataTag {
    uint16_t max_pre_heat;
    uint16_t max_roast;
} ConfigPageData;

typedef enum DataTypeTag {
    DATA_PAGE,
} DataType;

typedef struct PageDataTag {
    PageType page;
    union {
        ControlPageData control_data;
        ConfigPageData config_data;
    } data;
} PageData;

typedef union DataTag {
    PageData page_data;
} Data;

typedef struct IhmStateTag {
    ControlState control;
} IhmState;

/*$declare${Events} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${Events::UartInputTouchEvt} .............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    uint8_t length;
} UartInputTouchEvt;

/*${Events::IhmInputTouchEvt} ..............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    uint8_t length;
} IhmInputTouchEvt;

/*${Events::IhmInputSliderEvt} .............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    ControlType control;
    uint8_t value;
} IhmInputSliderEvt;

/*${Events::NotifyNextStageEvt} ............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    bool auto_mode;
} NotifyNextStageEvt;

/*${Events::RequestNextStageEvt} ...........................................*/
typedef struct {
/* protected: */
    QEvt super;
} RequestNextStageEvt;

/*${Events::UartInputSliderEvt} ............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    ControlType control;
    uint8_t value;
} UartInputSliderEvt;

/*${Events::UartOutputPageEvt} .............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    unsigned short int picid;
} UartOutputPageEvt;

/*${Events::UartOutputIconEvt} .............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    unsigned short int vp;
    unsigned short int icon;
} UartOutputIconEvt;

/*${Events::UartOutputTextEvt} .............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    unsigned short int vp;
    const char * text;
} UartOutputTextEvt;

/*${Events::UartOutputChartEvt} ............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    unsigned short int vp;
    int * values;
    SensorType sensor;
    unsigned short int count;
    int minX;
    int maxX;
    int minY;
    int maxY;
    bool deltaBound;
    int deltaBoundX;
    int maxTemp;
    int minTemp;
    int originY;
    bool isDelta;
} UartOutputChartEvt;

/*${Events::RequestModeEvt} ................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    ModeType mode;
    char roast[25];
} RequestModeEvt;

/*${Events::SensorUpdateEvt} ...............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    SensorType type;
    uint8_t value;
} SensorUpdateEvt;

/*${Events::PerifStartEvt} .................................................*/
typedef struct {
/* protected: */
    QEvt super;
} PerifStartEvt;

/*${Events::PerifStopEvt} ..................................................*/
typedef struct {
/* protected: */
    QEvt super;
} PerifStopEvt;

/*${Events::ControlUpdateEvt} ..............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    ControlType control;
    int value;
} ControlUpdateEvt;

/*${Events::ControlDataEvt} ................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    ControlType control;
    int value;
} ControlDataEvt;

/*${Events::SensorDataEvt} .................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    SensorType type;
    int value;
    int delta;
} SensorDataEvt;

/*${Events::RequestTransformRoastEvt} ......................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    char roast[25];
} RequestTransformRoastEvt;

/*${Events::NotifyModeEvt} .................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    ModeType mode;
    char roast[25];
} NotifyModeEvt;

/*${Events::NotifyNextSubstageEvt} .........................................*/
typedef struct {
/* protected: */
    QEvt super;
} NotifyNextSubstageEvt;

/*${Events::RequestNextSubstageEvt} ........................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    SubstageType substage;
} RequestNextSubstageEvt;

/*${Events::UartInputConfigSaveEvt} ........................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    unsigned short pre;
    unsigned short cool;
} UartInputConfigSaveEvt;

/*${Events::IhmInputConfigSaveEvt} .........................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    unsigned short pre;
    unsigned short roast;
} IhmInputConfigSaveEvt;

/*${Events::ConfigUpdateEvt} ...............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    uint16_t pre_heat;
    uint16_t roast;
} ConfigUpdateEvt;

/*${Events::UartOutputNumberEvt} ...........................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    unsigned short int vp;
    unsigned short value;
} UartOutputNumberEvt;

/*${Events::NotifySubstageExitEvt} .........................................*/
typedef struct {
/* protected: */
    QEvt super;
} NotifySubstageExitEvt;

/*${Events::ChartDataEvt} ..................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    SensorType type;
    TempData * temp_data;
    int max;
    int min;
} ChartDataEvt;

/*${Events::DataRequestEvt} ................................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    PageType page;
} DataRequestEvt;

/*${Events::DataResponseEvt} ...............................................*/
typedef struct {
/* protected: */
    QEvt super;

/* public: */
    DataType type;
    Data data;
} DataResponseEvt;
/*$enddecl${Events} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$declare${AOs::Ihm_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Ihm_ctor} .........................................................*/
void Ihm_ctor(void);
/*$enddecl${AOs::Ihm_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${AOs::Perif_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Perif_ctor} .......................................................*/
void Perif_ctor(void);
/*$enddecl${AOs::Perif_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${AOs::DataBroker_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::DataBroker_ctor} ..................................................*/
void DataBroker_ctor(void);
/*$enddecl${AOs::DataBroker_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${AOs::Uart_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::Uart_ctor} ........................................................*/
void Uart_ctor(void);
/*$enddecl${AOs::Uart_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$declare${AOs::AO_Ihm} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::AO_Ihm} ...........................................................*/
extern QActive * const AO_Ihm;
/*$enddecl${AOs::AO_Ihm} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${AOs::AO_Perif} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::AO_Perif} .........................................................*/
extern QActive * const AO_Perif;
/*$enddecl${AOs::AO_Perif} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${AOs::AO_DataBroker} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::AO_DataBroker} ....................................................*/
extern QActive * const AO_DataBroker;
/*$enddecl${AOs::AO_DataBroker} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
/*$declare${AOs::AO_Uart} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${AOs::AO_Uart} ..........................................................*/
extern QActive * const AO_Uart;
/*$enddecl${AOs::AO_Uart} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

/*$declare${Common} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv*/

/*${Common::postUart_setPage} ..............................................*/
void postUart_setPage(unsigned short int picid);

/*${Common::postUart_setString} ............................................*/
void postUart_setString(
    unsigned short int vp,
    char * text,
    bool reset,
    size_t reset_len);

/*${Common::postUart_setIcon} ..............................................*/
void postUart_setIcon(
    unsigned short int vp,
    unsigned short int icon);

/*${Common::postUart_setNumber} ............................................*/
void postUart_setNumber(
    unsigned short int vp,
    unsigned short value);

/*${Common::postData_requestData} ..........................................*/
void postData_requestData(
    DataType type,
    Data data);

/*${Common::postIhm_respondData} ...........................................*/
void postIhm_respondData(
    DataType type,
    Data data);

/*${Common::postUart_setNumberAsString} ....................................*/
void postUart_setNumberAsString(
    unsigned short int vp,
    char * text,
    char * ext,
    bool reset,
    size_t reset_len);
/*$enddecl${Common} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

#endif
