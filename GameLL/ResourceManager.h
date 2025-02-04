#pragma once
#include<string>
#include<sstream>
#include<fstream>
#include<unordered_map>
#include "Utilitites.h"

template<typename Derived, typename T>
class ResourceManager
{
public:
	ResourceManager(const std::string& i_pathsFile) {
		LoadPaths(i_pathsFile);
	}
	virtual ~ResourceManager() { PurgeResources(); }
	T* GetResource(const std::string& i_id) {
		auto res = Find(i_id);
		return res ? res->first : nullptr;
	}
	std::string GetPath(const std::string& i_id) {
		auto path = m_paths.find(i_id);
		return (path != m_paths.end() ? path->second : "");
	}
	bool RequireResource(const std::string& i_id) {
		auto res = Find(i_id);
		if (res) {
			++res->second;
			return true;
		}
		auto path = m_paths.find(i_id);
		if (path == m_paths.end()) { return false; }
		T* resource = Load(path->second);
		if (!resource) { return false; }
		m_resources.emplace(i_id, std::make_pair(resource, 1));
		return true;
	}
	bool ReleaseResource(const std::string& i_id) {
		auto res = Find(i_id);
		if (!res) { return false; }
		--res->second;
		if (!res->second) { Unload(i_id); }
		return true;
	}
	void PurgeResources() {
		while (m_resources.begin() != m_resources.end()) {
			delete m_resources.begin()->second.first;
			m_resources.erase(m_resources.begin());
		}
	}
	T* Load(const std::string& i_path) {
		return static_cast<Derived*>(this)->Load(i_path);
	}
	bool Unload(const std::string& i_id) {
		auto itr = m_resources.find(i_id);
		if (itr == m_resources.end()) { return false; }
		delete itr->second.first;
		m_resources.erase(itr);
		return true;
	}
	void LoadPaths(const std::string& i_pathFile) {
		std::ifstream paths;
		std::string directory = Utils::GetWorkingDirectory();
		std::string fullway = directory + i_pathFile;
		paths.open(fullway);
		if (paths.is_open()) {
			std::string line;
			while (std::getline(paths, line)) {
				std::stringstream keystream(line);
				std::string pathName;
				std::string path;
				keystream >> pathName;
				keystream >> path; 
				m_paths.emplace(pathName, path);
			}
			paths.close();
			return;
		}
		std::cerr << "Error can not open the file from this path " << i_pathFile << "\n";
	}
private:
	std::unordered_map<std::string, std::pair<T*, unsigned int>> m_resources;
	std::unordered_map<std::string, std::string> m_paths;

	std::pair<T*, unsigned int>* Find(const std::string& i_id) {
		auto itr = m_resources.find(i_id);
		return itr != m_resources.end() ? &itr->second : nullptr;
	}
};

