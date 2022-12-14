#ifndef STORAGE_H
#define STORAGE_H

void storage_create_new_roast();
void storage_add_roast_sensor_record(time_t total_time, uint8_t type, int temp_ar);
void storage_add_roast_pre_heat_record(time_t total_time, int temp_grao);
void storage_add_roast_cooler_record(time_t total_time, int temp_grao);
int storage_get_roasts_page(uint8_t pageNum, RoastsResponse *res);
int storage_get_roast(char *roast, RoastResponse *res);
int storage_get_recipes_page(uint8_t pageNum, RecipesResponse *res);
int storage_get_recipe(char *roast, RecipeResponse *res);
void storage_add_roast_control_record(time_t total_time, ControlType type, int value);
void storage_transform_roast(char *roast);
void storage_set_global_config(uint16_t pre_heat, uint16_t roast);
void storage_get_global_config(uint16_t *pre_heat, uint16_t *roast);
int storage_get_recipe_data(char *recipe, RecipeData *commands);
void storage_add_roast_q1_record(time_t total_time);
void storage_add_roast_q2_record(time_t total_time);
void storage_init();

#endif