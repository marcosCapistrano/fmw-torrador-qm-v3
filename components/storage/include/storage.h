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
void storage_add_roast_control_record(time_t total_time, ControlType type, void *payload);
void storage_add_roast_gas_record(time_t total_time, int value);
void storage_transform_roast(char *roast);
int storage_get_recipe_commands(char *recipe, RecipeCommands *commands);
void storage_init();

#endif