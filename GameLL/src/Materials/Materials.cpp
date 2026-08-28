#include "Materials.h"

namespace Materials
{
    MaterialType StringToMaterialType(const std::string& i_material)
    {
        if (i_material == "Wood")
            return MaterialType::Wood;

        else if (i_material == "Stone")
            return MaterialType::Stone;

        else if (i_material == "Metall")
            return MaterialType::Metall;

        return MaterialType::None;
    }
}