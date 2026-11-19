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
		else if (i_material == "AidKit")
			return MaterialType::AidKit;
        else if (i_material == "Spanner")
            return MaterialType::Spanner;
		else if (i_material == "DoorBunker")
		    return MaterialType::DoorBunker;
		else if (i_material == "Note")
		    return MaterialType::Note;

        return MaterialType::None;
    }

    std::string MaterialToString(const MaterialType& i_material) {
        if (i_material == MaterialType::Wood)
            return "Wood";
        else if (i_material == MaterialType::Stone)
            return "Stone";
        else if (i_material == MaterialType::Metall)
            return "Metall";
		else if (i_material == MaterialType::AidKit)
			return "AidKit";
        else if (i_material == MaterialType::DoorBunker)
            return "DoorBunker";
        else if ( i_material == MaterialType::Spanner)
            return "Spanner";
        else if (i_material == MaterialType::Note)
            return "Note";

        return "";
    }
}