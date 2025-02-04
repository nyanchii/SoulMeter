#include "pch.h"
#include "Language.h"

auto Language::GetLangFile(CHAR* langFile)
{
	// parse path
	CHAR path[MAX_PATH] = { 0 };
	sprintf_s(path, "%s%s", _langFolder, langFile);

	json j;

	do
	{
		// get raw data
		std::string langRaw;
		if (!file_contents(std::filesystem::path(path), &langRaw)) {
			Log::WriteLogA("[Language::SetCurrentLang] Lang file %s not found.", langFile);
			break;
		}

		// parse raw to json
		auto langData = json::parse(langRaw);
		if (langData.empty()) {
			Log::WriteLogA("[Language::SetCurrentLang] Lang file %s is empty.", langFile);
			break;
		}

		j = langData;

	} while (false);

	return j;
}

unordered_map<string, string> Language::MapLangData(CHAR* langFile)
{
	unordered_map<string, string> list;

	// get json data
	auto langData = GetLangFile(langFile);
	if (!langData.empty()) {
		for (json::iterator itr = langData.begin(); itr != langData.end(); itr++) 
			list[itr.key()] = itr.value().get<std::string>();
	}

	return list;
}

DWORD Language::SetCurrentLang(CHAR* langFile)
{
	DWORD error = ERROR_SUCCESS;

	do {

		// get json data
		auto newLang = MapLangData(langFile);
		if (newLang.empty()) {
			error = ERROR_NOT_FOUND;
			break;
		}

		// set current lang
		strcpy_s(_currentLang, langFile);

		_textList = newLang;

	} while (false);

	return error;
}

CHAR* Language::GetText(CHAR* text, unordered_map<string, string>* vector)
{
	if (vector == nullptr)
		vector = &_textList;
	
	if (vector->find(text) == vector->end()) {
		Log::WriteLogA("[Language::GetText] Lang text %s not found.", text);
		return text;
	}

	return (CHAR*)vector->at(text).c_str();
}

unordered_map<string, string> Language::GetAllLangFile()
{
	unordered_map<string, string> list;

	for (auto& p : std::filesystem::directory_iterator(_langFolder)) {
		if (p.is_regular_file()) {
			if (p.path().extension().string() == ".json") {

				string fileName = p.path().filename().string();

				auto langData = MapLangData((CHAR*)fileName.c_str());

				if (!langData.empty()) {

					CHAR* langName = GetText("STR_LANG_NAME", &langData);

					list.emplace(fileName, langName);
				}
			}
		}
	}

	return list;
}