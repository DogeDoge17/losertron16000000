#pragma once
#include <filesystem>
#include <imgui.h>
#include <iostream>
#include <json.hpp>
#include <thread>
#include <fstream>
#include <fstream>
#include <unordered_set>

#include "bitmap.hpp"
#include "ui.hpp"

struct DokiExpression {  
   Texture texture{}; // Initialize texture  
   Bounds bounds{};   // Initialize bounds  
   std::filesystem::path uri;  
   ImColor backgroundColor{}; // Initialize backgroundColor  
   std::string category;  

   DokiExpression(std::filesystem::path path) : uri(path), texture{}, bounds{}, backgroundColor{} {} 
	 DokiExpression() : uri{}, texture{}, bounds{  }, backgroundColor{} {}
};

struct GirlieFolder {
		int Max{};
		int ZIndex{};
		std::string Name;
		std::vector<std::string> Bypass;
		std::vector<std::string> Default;
};


struct GirlsGirling {
		std::vector<GirlieFolder> Folders;
		std::array<std::vector<std::string>, 2>Groups;
};

inline void from_json(const nlohmann::json& j, GirlieFolder& f) {
	j.at("max").get_to(f.Max);
	j.at("z-index").get_to(f.ZIndex);
	j.at("name").get_to(f.Name);
	j.at("bypass").get_to(f.Bypass);
	j.at("default").get_to(f.Default);
}

inline void from_json(const nlohmann::json& j, GirlsGirling& g) {
	j.at("folders").get_to(g.Folders);
	j.at("groups").get_to(g.Groups);
}

inline int girl;
inline char** girlsv;
inline char** girlsvDISP;
inline int girlsc;

inline GirlsGirling girlDefaults;
inline std::vector<DokiExpression*> selectedExpressions;

inline std::unordered_map<std::string, std::vector<DokiExpression>> all_expressions;

void init_toutes_dokis();
void init_doki();