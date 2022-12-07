#include "esp_unicviewad.h"

#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#define WRITE_CONTROL_REG 0x80
#define READ_CONTROL_REG 0x81
#define WRITE_VP 0x82
#define READ_VP 0x83

#define PICID_REGISTER 0x03

#define FNC_CIRCLE 0x0005
#define FNC_LINE 0x0002

#define CIRCLE_RADIUS 3

const unsigned char HEADER[2] = {0x5A, 0xa5};

const int CHART_ORIGIN_Y = 235;

static const char *TAG = "UNICVIEW";

void *swapbytes(void *inp, size_t len) {
    unsigned int i;
    unsigned char *in = (unsigned char *)inp, tmp;

    for (i = 0; i < len / 2; i++) {
        tmp = *(in + i);
        *(in + i) = *(in + len - i - 1);
        *(in + len - i - 1) = tmp;
    }

    return inp;
}

int packet_write_vp(unsigned short int vp, unsigned char *payload, unsigned char length, unsigned char *container) {
    unsigned char request = WRITE_VP;
    int newLength = sizeof(unsigned char) + sizeof(unsigned short int) + length;
    int offset = 0;

    memset(container, 0x00, MAX_CONTAINER_SIZE * sizeof(unsigned char));

    // copia o header para o container (HEADER H + HEADER L)
    memcpy(container, HEADER, 2 * sizeof(unsigned char));
    offset += 2 * sizeof(unsigned char);

    // copia o tamanho do payload para o container
    memcpy(container + offset, &newLength, sizeof(unsigned char));
    offset += sizeof(unsigned char);

    // copia request para o container
    memcpy(container + offset, &request, sizeof(unsigned char));
    offset += sizeof(unsigned char);

    // copia vp para container
    memcpy(container + offset, &vp, sizeof(unsigned short int));
    offset += sizeof(unsigned short int);

    // copia payload para container
    memcpy(container + offset, payload, length * sizeof(unsigned char));
    offset += length * sizeof(unsigned char);

    // retorna tamanho da carga útil do container
    return offset;
}

int packet_write_register(unsigned char reg, unsigned char *payload, unsigned char length, unsigned char *container) {
    unsigned char request = WRITE_CONTROL_REG;
    int newLength = sizeof(unsigned char) + sizeof(unsigned char) + length;
    int offset = 0;

    memset(container, 0x00, MAX_CONTAINER_SIZE * sizeof(unsigned char));

    // copia o header para o container (HEADER H + HEADER L)
    memcpy(container, HEADER, 2 * sizeof(unsigned char));
    offset += 2 * sizeof(unsigned char);

    // copia o tamanho do payload para o container
    memcpy(container + offset, &newLength, sizeof(unsigned char));
    offset += sizeof(unsigned char);

    // copia request para o container
    memcpy(container + offset, &request, sizeof(unsigned char));
    offset += sizeof(unsigned char);

    // copia register para container
    memcpy(container + offset, &reg, sizeof(unsigned char));
    offset += sizeof(unsigned char);

    // copia payload para container
    memcpy(container + offset, payload, length * sizeof(unsigned char));
    offset += length * sizeof(unsigned char);

    // retorna tamanho da carga útil do container
    return offset;
}

int packet_read_register(unsigned char reg, unsigned char *payload, unsigned char length, unsigned char *container) {
    unsigned char request = READ_CONTROL_REG;
    int newLength = sizeof(unsigned char) + sizeof(unsigned char) + length;
    
    int offset = 0;

    memset(container, 0x00, MAX_CONTAINER_SIZE * sizeof(unsigned char));

    // copia o header para o container (HEADER H + HEADER L)
    memcpy(container, HEADER, 2 * sizeof(unsigned char));
    offset += 2 * sizeof(unsigned char);

    // copia o tamanho do payload para o container
    memcpy(container + offset, &newLength, sizeof(unsigned char));
    offset += sizeof(unsigned char);

    // copia request para o container
    memcpy(container + offset, &request, sizeof(unsigned char));
    offset += sizeof(unsigned char);

    // copia register para container
    memcpy(container + offset, &reg, sizeof(unsigned char));
    offset += sizeof(unsigned char);

    // copia payload para container
    memcpy(container + offset, payload, length * sizeof(unsigned char));
    offset += length * sizeof(unsigned char);

    // retorna tamanho da carga útil do container
    return offset;
}

static int find_largest_temperature(int *temperatures, unsigned short int count) {
    int max = -999;
    for (unsigned short int i = 0; i < count; i++) {
        if (temperatures[i] > max) {
            max = temperatures[i];
        }
    }

    return max;
}

static int map_temp_to_max_temp(int temp, int minTemp, int maxTemp, int minY, int maxY) {
    if(minTemp == maxTemp)
        maxTemp++;
    const int in_min = minTemp, out_min = minY;
    const int in_max = maxTemp, out_max = maxY;

    if (in_max - in_min == 0)
        return out_min;

    return (temp - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int packet_picid(unsigned short int *picid, unsigned char *container) {
    return packet_write_register(PICID_REGISTER, picid, sizeof(unsigned short int), container);
}

int packet_graphics_circles(int *temperatures, unsigned short int func_count, unsigned short int color, int minX, int maxX, int minY, int maxY, bool delta, int deltaX, int maxTemp, unsigned char *container) {
    // unsigned short int func_code = FNC_CIRCLE;

    // unsigned short int func_count_copy = func_count;
    // unsigned short int func_code_copy = func_code;

    // const int MIN_X = minX + CIRCLE_RADIUS;
    // const int MAX_X = maxX - CIRCLE_RADIUS;
    // const int MIN_Y = minY - CIRCLE_RADIUS;
    // const int MAX_Y = maxY + CIRCLE_RADIUS;

    // swapbytes(&func_code_copy, sizeof(unsigned short int));
    // swapbytes(&func_count_copy, sizeof(unsigned short int));

    // // int newLength = sizeof(unsigned char) + sizeof(unsigned short int) + length;
    int offset = 0;

    // memset(container, 0x00, MAX_PAYLOAD_SIZE * sizeof(unsigned char));

    // // copia fnc code para o container
    // memcpy(container + offset, &func_code_copy, sizeof(unsigned short int));
    // offset += sizeof(unsigned short int);

    // // copia o fnc count para o container
    // memcpy(container + offset, &func_count_copy, sizeof(unsigned short int));
    // offset += sizeof(unsigned short int);

    // unsigned short int curr_x = MAX_X;
    // unsigned short int radius = CIRCLE_RADIUS;
    // for (unsigned short int i = func_count; i > 0; i--) {
    //     unsigned short int curr_y = map_temp_to_max_temp(temperatures[i], maxTemp, MIN_Y, MAX_Y);

    //     if (delta) {
    //         if (i > 0 && i < 6) {
    //             curr_x -= ((MAX_X - deltaX - 1 - (5 * radius)) / 5);
    //         } else {
    //             curr_x -= ((deltaX - 4 - radius - (25 * radius)) / 25);
    //         }
    //     } else {
    //         if (i > 0) {
    //             curr_x -= ((MAX_X - 4 - radius - (31 * radius)) / 31);
    //         }
    //     }

    //     unsigned short int curr_x_copy = curr_x;
    //     unsigned short int curr_y_copy = curr_y;

    //     unsigned short int radius_copy = radius;
    //     unsigned short int color_copy = color;

    //     swapbytes(&curr_x_copy, sizeof(unsigned short int));
    //     swapbytes(&curr_y_copy, sizeof(unsigned short int));
    //     swapbytes(&radius_copy, sizeof(unsigned short int));
    //     swapbytes(&color_copy, sizeof(unsigned short int));

    //     // copia x para container
    //     memcpy(container + offset, &curr_x_copy, sizeof(unsigned short int));
    //     offset += sizeof(unsigned short int);

    //     // copia y para container
    //     memcpy(container + offset, &curr_y_copy, sizeof(unsigned short int));
    //     offset += sizeof(unsigned short int);

    //     // copia raio para container
    //     memcpy(container + offset, &radius_copy, sizeof(unsigned short int));
    //     offset += sizeof(unsigned short int);

    //     // copia cor para container
    //     memcpy(container + offset, &color_copy, sizeof(unsigned short int));
    //     offset += sizeof(unsigned short int);
    // }

    // // retorna tamanho da carga útil do container
    return offset;
}

int packet_graphics_lines(int *temperatures, unsigned short int func_count, unsigned short int color, int minX, int maxX, int minY, int maxY, bool deltaBound, int deltaBoundX, int minTemp, int maxTemp, int originY, bool isDelta, unsigned char *container) {
    unsigned short int func_code = FNC_LINE;

    unsigned short int func_count_copy = func_count;
    unsigned short int func_code_copy = func_code;
    unsigned short int color_copy = color;

    const int MIN_X = minX + 1;
    const int MAX_X = maxX - 1;
    const int MIN_Y = minY - 1;
    const int MAX_Y = maxY + 1;

    swapbytes(&func_code_copy, sizeof(unsigned short int));
    swapbytes(&func_count_copy, sizeof(unsigned short int));
    swapbytes(&color_copy, sizeof(unsigned short int));

    // int newLength = sizeof(unsigned char) + sizeof(unsigned short int) + length;
    int offset = 0;

    memset(container, 0x00, MAX_PAYLOAD_SIZE * sizeof(unsigned char));

    // copia fnc code para o container
    memcpy(container + offset, &func_code_copy, sizeof(unsigned short int));
    offset += sizeof(unsigned short int);

    // copia o fnc count para o container
    memcpy(container + offset, &func_count_copy, sizeof(unsigned short int));
    offset += sizeof(unsigned short int);

    // copia o color para o container
    memcpy(container + offset, &color_copy, sizeof(unsigned short int));
    offset += sizeof(unsigned short int);

    unsigned short int curr_x = MAX_X;
    for (int i = func_count-1, j = 0; i >= 0; i--, j++) {
        unsigned short int curr_y = 93;

        if (isDelta) {
            if (i-1 > 0 && temperatures[i - 1] > 0) {
                curr_y = map_temp_to_max_temp(temperatures[i - 1] * 10, minTemp, maxTemp, MIN_Y, MAX_Y);
            }
        } else {
            // ESP_LOGE(TAG, "I: %d- value: %d", i, temperatures[i]);
            curr_y = map_temp_to_max_temp(temperatures[i], minTemp, maxTemp, MIN_Y, MAX_Y);
        }

        if (deltaBound) {
            if (j > 0 && j <= 6) {
                curr_x -= (MAX_X - deltaBoundX) / 5;
            } else if (j >= 6) {
                curr_x -= (deltaBoundX - MIN_X) / 24;
            }
        } else {
            if (j > 0)
                curr_x -= (MAX_X - MIN_X) / 30;
        }

        unsigned short int curr_x_copy = curr_x;
        unsigned short int curr_y_copy = curr_y;

        swapbytes(&curr_x_copy, sizeof(unsigned short int));
        swapbytes(&curr_y_copy, sizeof(unsigned short int));

        // copia x para container
        memcpy(container + offset, &curr_x_copy, sizeof(unsigned short int));
        offset += sizeof(unsigned short int);

        // copia y para container
        memcpy(container + offset, &curr_y_copy, sizeof(unsigned short int));
        offset += sizeof(unsigned short int);

        // ESP_LOGE(TAG, "Copied: x:%d, y:%d", curr_x, curr_y);
    }

    // retorna tamanho da carga útil do container
    return offset;
}