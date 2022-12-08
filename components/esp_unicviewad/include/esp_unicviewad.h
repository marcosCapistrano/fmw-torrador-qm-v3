#ifndef ESP_UNICVIEWAD_H
#define ESP_UNICVIEWAD_h

#include <stddef.h>
#include <stdbool.h>

#define MAX_CONTAINER_SIZE 258
#define MAX_PAYLOAD_SIZE 253



/* STANDARD UNICVIEW COMMANDS */
void *swapbytes(void *inp, size_t len);
int packet_write_vp(unsigned short int vp, unsigned char *payload, unsigned char length, unsigned char *container);
int packet_write_register(unsigned char reg, unsigned char *payload, unsigned char length, unsigned char *container);
int packet_read_register(unsigned char reg, unsigned char *payload, unsigned char length, unsigned char *container);

/* FACILITIES THAT USE THE STANDARD COMMANDS */
int packet_picid(unsigned short int *vp, unsigned char *container);
int packet_graphics_circles(int *temperatures, unsigned short int func_count, unsigned short int color, int minX, int maxX, int minY, int maxY, bool delta, int deltaX, int maxTemp, unsigned char *container);
int packet_graphics_lines(int *temperatures, unsigned short int func_count, unsigned short int color, int minX, int maxX, int minY, int maxY, bool deltaBound, int deltaBoundX, int minTemp, int maxTemp, int originY, bool isDelta, unsigned char *container);

#endif
