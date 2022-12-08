#include "common.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "nvs_flash.h"

#define NAMESPACE "config"
#define RECIPE_NUMBER_KEY "recipe_number"
#define ROAST_NUMBER_KEY "roast_number"
#define GLOBAL_MAX_PRE_HEAT_KEY "max_pre_heat"
#define GLOBAL_MAX_ROAST_KEY "max_roast"

static const char *TAG = "STORAGE";

static nvs_handle_t my_nvs_handle;
static uint32_t roast_number = 1;
static uint32_t recipe_number = 1;
static uint16_t global_max_pre_heat = 1;
static uint16_t global_max_roast = 1;

static uint8_t get_u32(const char *key, uint32_t base_value) {
    uint32_t temp_out;

    esp_err_t err = nvs_get_u32(my_nvs_handle, key, &temp_out);
    if (err != ESP_OK) {
        nvs_set_u32(my_nvs_handle, key, base_value);
        nvs_commit(my_nvs_handle);
        return base_value;
    }

    return temp_out;
}

static void set_u32(const char *key, uint32_t new_value) {
    ESP_ERROR_CHECK(nvs_set_u32(my_nvs_handle, key, new_value));

    nvs_commit(my_nvs_handle);
}

static uint8_t get_u16(const char *key, uint16_t base_value) {
    uint16_t temp_out;

    esp_err_t err = nvs_get_u16(my_nvs_handle, key, &temp_out);
    if (err != ESP_OK) {
        nvs_set_u16(my_nvs_handle, key, base_value);
        nvs_commit(my_nvs_handle);
        return base_value;
    }

    return temp_out;
}

static void set_u16(const char *key, uint16_t new_value) {
    ESP_ERROR_CHECK(nvs_set_u16(my_nvs_handle, key, new_value));

    nvs_commit(my_nvs_handle);
}

static void set_roast_number(uint32_t new_value) {
    if (roast_number != new_value) {
        set_u32(ROAST_NUMBER_KEY, new_value);
        roast_number = new_value;
    }
}

static void set_recipe_number(uint32_t new_value) {
    if (recipe_number != new_value) {
        set_u32(RECIPE_NUMBER_KEY, new_value);
        recipe_number = new_value;
    }
}

static void set_global_max_pre_heat(uint16_t new_value) {
    if (global_max_pre_heat != new_value) {
        set_u16(GLOBAL_MAX_PRE_HEAT_KEY, new_value);
        global_max_pre_heat = new_value;
    }
}

static void set_global_max_roast(uint16_t new_value) {
    if (global_max_roast != new_value) {
        set_u16(GLOBAL_MAX_ROAST_KEY, new_value);
        global_max_roast = new_value;
    }
}

static void init_cache() {
    roast_number = get_u32(ROAST_NUMBER_KEY, 0);
    recipe_number = get_u32(RECIPE_NUMBER_KEY, 0);
    global_max_pre_heat = get_u16(GLOBAL_MAX_PRE_HEAT_KEY, 0);
    global_max_roast = get_u16(GLOBAL_MAX_ROAST_KEY, 0);
}

static void init_fs() {
    esp_vfs_spiffs_conf_t web_conf = {
        .base_path = "/website",
        .partition_label = "website",
        .max_files = 5,
        .format_if_mount_failed = true};

    esp_vfs_spiffs_conf_t storage_conf = {
        .base_path = "/storage",
        .partition_label = "storage",
        .max_files = 5,
        .format_if_mount_failed = true};

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&web_conf));
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&storage_conf));

    size_t total = 0, used = 0;
    esp_err_t ret = esp_spiffs_info("website", &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    ret = esp_spiffs_info("storage", &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

void storage_set_global_config(uint16_t pre_heat, uint16_t roast) {
    set_global_max_pre_heat(pre_heat);
    set_global_max_roast(roast);
}

void storage_get_global_config(uint16_t *pre_heat, uint16_t *roast) {
    *pre_heat = global_max_pre_heat;
    *roast = global_max_roast;
}

void storage_create_new_roast(char *name) {
    uint32_t new_roast_num = roast_number + 1;
    set_roast_number(new_roast_num);

    char filename[9] = "\0";
    sprintf(filename, "%u", new_roast_num);
    strcpy(name, filename);

    char path[100] = {0};
    sprintf(path, "/storage/roasts/%u", new_roast_num);

    FILE *fp;
    fp = fopen(path, "w");
    fclose(fp);
}

void storage_add_roast_control_record(time_t total_time, ControlType type, int value) {
    char path[100] = {0};
    sprintf(path, "/storage/roasts/%u", roast_number);

    FILE *f = fopen(path, "a");
    if (type == CILINDRO) {
        fprintf(f, "%ld,CONTROL_CILINDRO,%d\n", total_time, value);
    } else if (type == POTENCIA) {
        fprintf(f, "%ld,CONTROL_POTENCIA,%d\n", total_time, value);
    } else if (type == TURBINA) {
        fprintf(f, "%ld,CONTROL_TURBINA,%d\n", total_time, value);
    }

    fclose(f);
}

void storage_add_roast_sensor_record(time_t total_time, SensorType type, int value) {
    char path[100] = {0};
    sprintf(path, "/storage/roasts/%u", roast_number);

    FILE *f = fopen(path, "a");
    if (type == SENSOR_GRAO) {
        fprintf(f, "%ld,SENSOR_GRAO,%d\n", total_time, value);
    } else if (type == SENSOR_AR) {
        fprintf(f, "%ld,SENSOR_AR,%d\n", total_time, value);
    } else if (type == SENSOR_GAS) {
        fprintf(f, "%ld,GAS,%d\n", total_time, value);
    }

    fclose(f);
}

void storage_add_roast_pre_heat_record(time_t total_time, int temp_grao) {
    char path[100] = {0};
    sprintf(path, "/storage/roasts/%u", roast_number);

    FILE *f = fopen(path, "a");
    fprintf(f, "%ld,ROAST,%d\n", total_time, temp_grao);
    fclose(f);
}

void storage_add_roast_cooler_record(time_t total_time, int temp_grao) {
    char path[100] = {0};
    sprintf(path, "/storage/roasts/%u", roast_number);

    FILE *f = fopen(path, "a");
    fprintf(f, "%ld,COOLER,%d\n", total_time, temp_grao);
    fclose(f);
}

void storage_add_roast_q1_record(time_t total_time) {
    char path[100] = {0};
    sprintf(path, "/storage/roasts/%u", roast_number);

    FILE *f = fopen(path, "a");
    fprintf(f, "%ld,F-Q1,0\n", total_time);
    fclose(f);
}

void storage_add_roast_q2_record(time_t total_time) {
    char path[100] = {0};
    sprintf(path, "/storage/roasts/%u", roast_number);

    FILE *f = fopen(path, "a");
    fprintf(f, "%ld,Q1-Q2,0\n", total_time);
    fclose(f);
}

esp_err_t storage_get_current_roast_summary(int *temps_ar, int *temps_grao, int *temps_count_ar, int *temps_count_grao) {
    char path[600] = {0};

    sprintf(path, "/storage/roasts/%d", roast_number);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        return ESP_FAIL;
    }

    char lineRead[256];
    while (fgets(lineRead, sizeof(lineRead), file)) {  // httpd_resp_sendstr_chunk(req, lineRead);
    }

    // httpd_resp_sendstr_chunk(req, NULL);
    fclose(file);

    return ESP_OK;
}

static int cmp_by_date(const void *a, const void *b) {
    char ptr[25];
    long ret1 = strtol(*(char **)a, &ptr, 10);
    long ret2 = strtol(*(char **)b, &ptr, 10);

    return ret1 - ret2;
}

static int get_last_entries_from_dir(char *base_path, uint8_t pageNum, char *roast1, char *roast2, char *roast3) {
    DIR *dir = opendir(base_path);
    struct dirent *entry = readdir(dir);
    char roasts[25][25];

    int count = 0;
    int ret = -1;
    char full_path[257];
    for (int i = 0; i < 25; i++) {
        if (entry == NULL) {
            break;
        }

        snprintf(full_path, sizeof(full_path) / sizeof(full_path[0]), "%s/%s", base_path, entry->d_name);
        strcpy(roasts[i], full_path);

        entry = readdir(dir);
        count++;
        ret++;
    }

    ESP_LOGE(TAG, "count: %d", count);

    if (count == 0) {
        ESP_LOGE(TAG, "STORAGE_GET_LAST_ENTRIES: Nenhum arquivo encontrado");
        return -1;
    }

    qsort(&roasts, count, sizeof(roasts[0]), cmp_by_date);

    if (count > 0) {
        strcpy(roast1, roasts[pageNum]);
    }
    if (count > 1) {
        strcpy(roast2, roasts[pageNum + 1]);
    }
    if (count > 2) {
        strcpy(roast3, roasts[pageNum+2]);
    }

    closedir(dir);
    return ret;
}

int storage_get_roasts_page(uint8_t pageNum, RoastsResponse *res) {
    ESP_LOGE(TAG, "Getting roasts page");
    int ret = get_last_entries_from_dir("/storage/roasts", pageNum, res->roast1, res->roast2, res->roast3);

    if (ret == -1)
        return -1;

    res->pageNum = pageNum;
    ESP_LOGE(TAG, "%d", pageNum);
    ESP_LOGE(TAG, "%d", res->pageNum);

    if (pageNum > 0)
        res->prevPage = true;

    if (ret > 3)
        res->nextPage = true;

    return ret;
}

int storage_get_recipes_page(uint8_t pageNum, RecipesResponse *res) {
    int ret = get_last_entries_from_dir("/storage/recipes", pageNum, res->roast1, res->roast2, res->roast3);

    if (ret == -1)
        return -1;

    res->pageNum = pageNum;

    if (pageNum > 0)
        res->prevPage = true;

    if (ret > 3)
        res->nextPage = true;

    return ret;
}

typedef enum EntryTypeTag {
    ENTRY_NONE,
    ENTRY_SENSOR_GRAO,
    ENTRY_SENSOR_AR,
    ENTRY_STAGE_ROAST,
    ENTRY_STAGE_COOLER,
    ENTRY_CONTROL_CILINDRO,
    ENTRY_CONTROL_TURBINA,
    ENTRY_CONTROL_POTENCIA,
} EntryType;

static EntryType parse_data_from_entry(char *line, int *out_value, time_t *interval) {
    char line_cpy[257];  // Cria uma cópia da string para não afetar o buffer original
    char tokens[3][25];
    char *token;
    strcpy(line_cpy, line);

    token = strtok(line_cpy, ",");
    int count = 0;
    while (token != NULL) {
        strcpy(tokens[count], token);

        token = strtok(NULL, ",");
        count++;
    }

    *interval = atoi(tokens[0]);
    *out_value = atoi(tokens[2]);
    if (strcmp(tokens[1], "ROAST") == 0)
        return ENTRY_STAGE_ROAST;
    else if (strcmp(tokens[1], "COOLER") == 0)
        return ENTRY_STAGE_COOLER;
    else if (strcmp(tokens[1], "SENSOR_GRAO") == 0)
        return ENTRY_SENSOR_GRAO;
    else if (strcmp(tokens[1], "SENSOR_AR") == 0)
        return ENTRY_SENSOR_AR;
    else if (strcmp(tokens[1], "CONTROL_CILINDRO") == 0)
        return ENTRY_CONTROL_CILINDRO;
    else if (strcmp(tokens[1], "CONTROL_POTENCIA") == 0)
        return ENTRY_CONTROL_POTENCIA;
    else if (strcmp(tokens[1], "CONTROL_TURBINA") == 0)
        return ENTRY_CONTROL_TURBINA;
    else {
        ESP_LOGE(TAG, "Encontrado entrada errada nesta torra: %s", line_cpy);
        return ENTRY_NONE;
    }

    return ENTRY_NONE;
}

void storage_transform_roast(char *roast) {
    ESP_LOGE(TAG, "TRANSFORMINGG");
    uint32_t new_recipe_num = recipe_number + 1;
    set_recipe_number(new_recipe_num);

    char roast_path[100] = {0};
    sprintf(roast_path, "%s", roast);

    char recipe_path[100] = {0};
    sprintf(recipe_path, "/storage/recipes/%u", new_recipe_num);

    FILE *roastfp;
    FILE *recipefp;

    roastfp = fopen(roast_path, "r");
    recipefp = fopen(recipe_path, "w");

    char lineBefore[256];
    char lineAfter[256];
    EntryType last_type;
    while (fgets(lineBefore, sizeof(lineBefore), roastfp)) {
        int value = 0;
        time_t interval = 0;
        EntryType type = parse_data_from_entry(lineBefore, &value, &interval);
        last_type = type;

        /*
            ENTRY_NONE,
            ENTRY_SENSOR_GRAO,
            ENTRY_SENSOR_AR,
            ENTRY_STAGE_ROAST,
            ENTRY_STAGE_COOLER
            */
        if (type == ENTRY_CONTROL_POTENCIA ||
            type == ENTRY_CONTROL_CILINDRO ||
            type == ENTRY_CONTROL_TURBINA) {
            /* Atravessa o arquivo procurando a ultima mudança de controle repetida */
            while (fgets(lineAfter, sizeof(lineAfter), roastfp)) {
                type = parse_data_from_entry(lineAfter, &value, &interval);
                if (type != last_type) {
                    /* Ao encontrar, adiciona somente ele na receita */
                    fputs(lineBefore, recipefp);

                    /* Retornar a posição do cursor para antes, como um ungetc para strings */
                    size_t len = strlen(lineAfter);
                    fseek(roastfp, 0 - len, SEEK_CUR);
                    break;
                }
                strcpy(lineBefore, lineAfter);
                last_type = type;
            }
        } else {
            fputs(lineBefore, recipefp);
        }
    }

    fclose(roastfp);
    fclose(recipefp);
}

static void update_data_from_sensor_entry(SensorType type, int value, SensorData *sensorData, int *aux_grao, int *aux_ar) {
    /*
    if (value > sensorData->max_temp) {
        sensorData->max_temp = value;
    } else if (value < sensorData->min_temp) {
        sensorData->min_temp = value;
    }

    if (type == SENSOR_GRAO && sensorData->temps_grao_count > 0) {
        int value_cmp = (value - sensorData->deltas_grao[sensorData->temps_grao_count - 1]) * 10;

        if (value_cmp > sensorData->max_temp_delta) {
            sensorData->max_temp_delta = value_cmp;
        } else if (value_cmp < sensorData->min_temp_delta) {
            sensorData->min_temp_delta = value_cmp;
        }
    } else if (type == SENSOR_AR && sensorData->temps_ar_count > 0) {
        int value_cmp = (value - sensorData->deltas_ar[sensorData->temps_ar_count - 1]) * 10;

        if (value_cmp > sensorData->max_temp_delta) {
            sensorData->max_temp_delta = value_cmp;
        } else if (value_cmp < sensorData->min_temp_delta) {
            sensorData->min_temp_delta = value_cmp;
        }
    }

    if (type == SENSOR_GRAO) {
        if (sensorData->temps_grao_count < 31) {
            int new_count = sensorData->temps_grao_count + 1;
            sensorData->temps_grao_count = new_count;
            sensorData->temps_grao[new_count - 1] = value;

            if (sensorData->temps_grao_count > 1) {
                sensorData->deltas_grao[new_count - 1] = value - sensorData->temps_grao[new_count - 2];
            } else if (sensorData->temps_grao_count == 1) {
                sensorData->deltas_grao[0] = 0;
            }
        } else {
            sensorData->temps_grao[*aux_grao] = (sensorData->temps_grao[*aux_grao] + sensorData->temps_grao[*aux_grao + 1]) / 2;
            sensorData->deltas_grao[*aux_grao] = (sensorData->deltas_grao[*aux_grao] + sensorData->deltas_grao[*aux_grao + 1]) / 2;

            for (int i = *aux_grao + 1; i < 30; i++) {
                sensorData->temps_grao[i] = sensorData->temps_grao[i + 1];
                sensorData->deltas_grao[i] = sensorData->deltas_grao[i + 1];
            }
            sensorData->temps_grao[30] = value;
            sensorData->deltas_grao[30] = value - sensorData->temps_grao[29];

            (*aux_grao)++;
            *aux_grao = *aux_grao % 25;
        }
    } else if (type == SENSOR_AR) {
        if (sensorData->temps_ar_count < 31) {
            int new_count = sensorData->temps_ar_count + 1;
            sensorData->temps_ar_count = new_count;
            sensorData->temps_ar[new_count - 1] = value;

            if (sensorData->temps_ar_count > 1) {
                sensorData->deltas_ar[new_count - 1] = value - sensorData->temps_ar[new_count - 2];
            } else if (sensorData->temps_ar_count == 1) {
                sensorData->deltas_ar[0] = 0;
            }
        } else {
            sensorData->temps_ar[*aux_ar] = (sensorData->temps_ar[*aux_ar] + sensorData->temps_ar[*aux_ar + 1]) / 2;
            sensorData->deltas_ar[*aux_ar] = (sensorData->deltas_ar[*aux_ar] + sensorData->deltas_ar[*aux_ar + 1]) / 2;
            for (int i = *aux_ar + 1; i < 30; i++) {
                sensorData->temps_ar[i] = sensorData->temps_ar[i + 1];
                sensorData->deltas_ar[i] = sensorData->deltas_ar[i + 1];
            }
            sensorData->temps_ar[30] = value;
            sensorData->deltas_ar[30] = value - sensorData->temps_ar[29];

            (*aux_ar)++;
            *aux_ar = *aux_ar % 25;
        }
    }
    */
}

int storage_get_roast(char *roast, RoastResponse *res) {
    /*
    FILE *file = fopen(roast, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Não foi possivel encontrar arquivo: %s", roast);
        return -1;
    }

    int aux_grao = 0;
    int aux_ar = 0;
    char lineRead[256];
    while (fgets(lineRead, sizeof(lineRead), file)) {
        int value = 0;
        int interval = 0;
        EntryType type = parse_data_from_entry(lineRead, &value, &interval);

        if (type == ENTRY_SENSOR_GRAO)
            update_data_from_sensor_entry(SENSOR_GRAO, value, res->sensorData, &aux_grao, &aux_ar);
        else if (type == ENTRY_SENSOR_AR)
            update_data_from_sensor_entry(SENSOR_AR, value, res->sensorData, &aux_grao, &aux_ar);
    }

    for (int i = 0; i < res->sensorData->temps_grao_count; i++) {
        // ESP_LOGE(TAG, "Grao Sensor Values[%d]: %d", i, res->sensorData->temps_grao[i]);
        // ESP_LOGE(TAG, "Grao Sensor Deltas[%d]: %d", i, res->sensorData->deltas_grao[i]);
    }
    strcpy(res->roast, roast);

    // httpd_resp_sendstr_chunk(req, NULL);
    fclose(file);

*/
    return 1;
}

int storage_get_recipe(char *roast, RecipeResponse *res) {
    /*
    FILE *file = fopen(roast, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Não foi possivel encontrar arquivo: %s", roast);
        return -1;
    }

    int aux_grao = 0;
    int aux_ar = 0;
    char lineRead[256];
    while (fgets(lineRead, sizeof(lineRead), file)) {
        int value = 0;
        time_t interval = 0;
        EntryType type = parse_data_from_entry(lineRead, &value, &interval);

        if (type == ENTRY_SENSOR_GRAO)
            update_data_from_sensor_entry(SENSOR_GRAO, value, res->sensorData, &aux_grao, &aux_ar);
        else if (type == ENTRY_SENSOR_AR)
            update_data_from_sensor_entry(SENSOR_AR, value, res->sensorData, &aux_grao, &aux_ar);
    }

    for (int i = 0; i < res->sensorData->temps_grao_count; i++) {
        // ESP_LOGE(TAG, "Grao Sensor Values[%d]: %d", i, res->sensorData->temps_grao[i]);
        // ESP_LOGE(TAG, "Grao Sensor Deltas[%d]: %d", i, res->sensorData->deltas_grao[i]);
    }
    strcpy(res->roast, roast);

    // httpd_resp_sendstr_chunk(req, NULL);
    fclose(file);

    */
    return 1;
}

int storage_get_recipe_data(char *recipe, RecipeData *commands) {
    FILE *file = fopen(recipe, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Não foi possivel encontrar arquivo: %s", recipe);
        return -1;
    }

    int count = 0;
    char lineRead[256];
    while (fgets(lineRead, sizeof(lineRead), file) && count < 100) {
        int value = 0;
        time_t interval = 0;
        EntryType type = parse_data_from_entry(lineRead, &value, &interval);

        if (type == ENTRY_CONTROL_CILINDRO) {
            commands->intervals[count] = interval;
            commands->controls[count] = CILINDRO;
            commands->values[count] = value;

            count++;
        } else if (type == ENTRY_CONTROL_POTENCIA) {
            commands->intervals[count] = interval;
            commands->controls[count] = POTENCIA;
            commands->values[count] = value;

            count++;
        } else if (type == ENTRY_CONTROL_TURBINA) {
            commands->intervals[count] = interval;
            commands->controls[count] = TURBINA;
            commands->values[count] = value;

            count++;
        }
    }

    commands->count = count;

    // httpd_resp_sendstr_chunk(req, NULL);
    fclose(file);

    return 1;
}

void storage_init() {
    ESP_LOGI(TAG, "Inicializando Armazenamento...");
    esp_err_t err;

    // NVS para Configurações persistentes e persistencia de alguns outros valores
    err = nvs_open(NAMESPACE, NVS_READWRITE, &my_nvs_handle);
    ESP_ERROR_CHECK(err);

    init_cache();
    init_fs();
}