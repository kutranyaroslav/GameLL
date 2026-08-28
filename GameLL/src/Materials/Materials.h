#pragma once 

#include <string>
#include <vector>
// here we add the material type for the tile
namespace Materials {


	enum class MaterialType
	{
		None = 0,
		Wood,
		Stone,
		Metall,
	};
	MaterialType StringToMaterialType(const std::string& i_material);
	std::string MaterialToString(const MaterialType& i_material);
	struct Material {
		Material(const std::string& i_name = "", MaterialType i_type = MaterialType::None) : m_name(i_name), m_type(i_type) {
			if (i_name != "") {
				m_type = Materials::StringToMaterialType(i_name);
			}
		}
		//if materials are added we have to add them manually here 
		std::string m_name;
		MaterialType m_type;
		std::vector<std::string> m_soundNames;

	};


}