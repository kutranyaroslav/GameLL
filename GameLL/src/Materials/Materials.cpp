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

    std::string MaterialToString(const MaterialType& i_material) {
        if (i_material == MaterialType::Wood)
            return "Wood";
        else if (i_material == MaterialType::Stone)
            return "Stone";
        else if (i_material == MaterialType::Metall)
            return "Metall";

        return "";
    }
}