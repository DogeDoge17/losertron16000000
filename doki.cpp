#include "doki.hpp"

void sort_expressions() {
	std::unordered_map<std::string, int> folderIndexMap;
	for (const auto &folder: girlDefaults.Folders) {
		folderIndexMap[folder.Name] = folder.ZIndex;
	}

	std::ranges::sort(selectedExpressions,
	                  [&folderIndexMap](const DokiExpression *a, const DokiExpression *b) {
		                  int zIndexA = INT_MAX;
		                  int zIndexB = INT_MAX;
		                  if (const auto itA = folderIndexMap.find(a->category); itA != folderIndexMap.end())
			                  zIndexA = itA->second;
		                  if (const auto itB = folderIndexMap.find(b->category); itB != folderIndexMap.end())
			                  zIndexB = itB->second;
		                  return zIndexA < zIndexB;
	                  });
}

void load_defaults() {
	try {
		std::ifstream file(std::filesystem::path("dokis") / girlsv[girl] / "defaults.json");
		if (!file.is_open()) {
			throw std::runtime_error("Could not open JSON file.");
		}

		nlohmann::json j;
		file >> j;

		girlDefaults = j.get<GirlsGirling>();
	} catch (std::exception &e) {
		std::cerr << "Error reading JSON file: " << e.what() << '\n';
	}

	selectedExpressions = std::vector<DokiExpression *>();

	//_selectedExpressions = new();
	//List<DokiExpression> megaList = new List<DokiExpression>();
	//for (int i = 0; i < expressions.Count; i++)
	//	megaList.AddRange(expressions.Values.ToList()[i]);
	std::unordered_set<std::string> defaults = std::unordered_set<std::string>();

	for (auto &Folder: girlDefaults.Folders)
		for (auto &j: Folder.Default)
			defaults.emplace(j);

	for (auto &expressionV: all_expressions | std::views::values)
		for (auto &expression: expressionV)
			if (defaults.contains(expression.uri.filename().string())) {
				expression.backgroundColor = GREEN;
				selectedExpressions.push_back(&expression);
			}


	//megaList.ForEach(image => { if (defaults.Contains(new Path(image.Uri).FileName.ToString())) { _selectedExpressions.Add(image); image.BackgroundColor = Colors.Green; } });
	sort_expressions();
	update_warnings(nullptr);
}

void load_image(DokiExpression *dokiExp, std::pair<DokiExpression *, uint8_t *> &loaded) {
	uint8_t *pixels = nullptr;
	load_texture(dokiExp->uri, pixels, dokiExp->texture.width, dokiExp->texture.height);
	dokiExp->bounds = crop_image(pixels, dokiExp->texture.width, dokiExp->texture.height);

	loaded.first = dokiExp;
	loaded.second = pixels;
}

void load_image_data() {
	if (expressionGroups) {
		for (size_t i = 0; i < tabCount; i++) {
			delete[] expressionGroups[i];
		}
		delete[] expressionGroups;
	}

	if (!all_expressions.empty()) {
		for (auto &expressionGroup: all_expressions)
			for (auto &expression: expressionGroup.second)
				glDeleteTextures(1, &expression.texture.id);
	}

	auto dirIter = std::filesystem::directory_iterator(std::filesystem::path("dokis") / girlsv[girl]);

	tabCount = std::count_if(
		begin(dirIter),
		end(dirIter),
		[](auto &entry) { return entry.is_directory(); }
	);

	expressionGroups = new char *[tabCount];

	std::vector<std::thread> alphaScanThreads = std::vector<std::thread>();

	size_t k = 0;

	all_expressions = std::unordered_map<std::string, std::vector<DokiExpression> >();
	all_expressions.reserve(tabCount);
	for (const auto &folder: std::filesystem::directory_iterator(std::filesystem::path("dokis") / girlsv[girl])) {
		if (!folder.is_directory())
			continue;
		std::string expressionName = folder.path().filename().string();
		expressionGroups[k] = new char[expressionName.length() + 1];
		strcpy(expressionGroups[k++], expressionName.c_str());

		all_expressions[expressionName] = std::vector<DokiExpression>();
		for (const auto &expression: std::filesystem::directory_iterator(folder.path()))
			all_expressions[expressionName].emplace_back(expression.path());
	}

	std::vector<std::unique_ptr<std::pair<DokiExpression *, uint8_t *> > > pixels = std::vector<std::unique_ptr<
		std::pair<DokiExpression *, uint8_t *> > >();

	for (auto &yessirskiii: all_expressions | std::views::values) {
		for (auto &expression: yessirskiii) {
			auto loaded = std::make_unique<std::pair<DokiExpression *, uint8_t *> >();
			alphaScanThreads.emplace_back(load_image, &expression, std::ref(*loaded));
			pixels.push_back(std::move(loaded));
		}
	}
	for (auto &alphaScanThread: alphaScanThreads)
		alphaScanThread.join();

	for (const auto &loaded: pixels) {
		if (loaded->second) {
			loaded->first->texture.id = texture_send(loaded->second, loaded->first->texture.width,
			                                         loaded->first->texture.height);
			stbi_image_free(loaded->second);
		}
	}
}

void init_doki() {
	load_image_data();
	load_defaults();
}

void init_toutes_dokis() {
	//Path[] girlies = FileSystem.GetDirectories();
	//for (int i = 0; i < girlies.Length; i++) {
	//    string filename = girlies[i].FileName;
	//    girlPicker.Items.Add(filename.First().ToString().ToUpper() + filename.Substring(1));
	//    // makes the name capitalised to be all fancy
	//}
	//girlPicker.SelectedIndexChanged += ReinitDoki;
	//girlPicker.SelectedIndex = girlPicker.Items.IndexOf(_girl.First().ToString().ToUpper() + _girl.Substring(1));


	auto dirIter = std::filesystem::directory_iterator("dokis");

	girlsc = static_cast<int>(std::count_if(
		begin(dirIter),
		end(dirIter),
		[](auto &entry) { return entry.is_directory(); }
	));

	girlsv = static_cast<char **>(malloc(sizeof(char *) * girlsc));
	girlsvDISP = static_cast<char **>(malloc(sizeof(char *) * girlsc));

	int i = 0;
	for (const auto &dir: std::filesystem::directory_iterator("dokis")) {
		if (i > girlsc)
			break;

		size_t len;
		girlsv[i] = static_cast<char *>(
			malloc(len = sizeof(char) * strlen(dir.path().filename().string().c_str()) + 1));
		girlsvDISP[i] = static_cast<char *>(malloc(len + 1));
		strcpy(girlsv[i], dir.path().filename().string().c_str());
		strcpy(girlsvDISP[i], girlsv[i]);

		if (girlsv[i][0] == 'n')
			girl = i;

		if (girlsvDISP[i][0] > 97 && girlsvDISP[i][0] < 122)
			girlsvDISP[i][0] -= 32;

		i++;
	}
	/*for(size_t i = 0; i < girlsc; i++) {
			printf("%s\n", girlsv[i]);
			printf("%s\n", girlsvDISP[i]);
	}*/
}


