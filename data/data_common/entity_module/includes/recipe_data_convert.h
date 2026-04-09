#ifndef RECIPE_DATA_CONVERT_H
#define RECIPE_DATA_CONVERT_H

#include "entity_module_global.h"
struct RecipeDataModel;
struct RecipeEnvironmentModel;

void ENTITY_MODULE_EXPORT recipeDataToJson(QString& json, const RecipeDataModel&);
int ENTITY_MODULE_EXPORT recipeDataFromJson(const QString& json, RecipeDataModel&);
void ENTITY_MODULE_EXPORT recipeEnvironmentToJson(QString& json, const RecipeEnvironmentModel& env);
int ENTITY_MODULE_EXPORT recipeEnvironmentFromJson(const QString& json, RecipeEnvironmentModel& env);

#endif // RECIPE_DATA_CONVERT_H
