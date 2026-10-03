#pragma once

#include <cstring>

// Ukrainian API names used as canonical keys for region alert state.
namespace Regions {
constexpr const char *VINNYTSIA = "Вінницька область";
constexpr const char *VOLYN = "Волинська область";
constexpr const char *DNIPROPETROVSK = "Дніпропетровська область";
constexpr const char *DONETSK = "Донецька область";
constexpr const char *ZHYTOMYR = "Житомирська область";
constexpr const char *ZAKARPATTIA = "Закарпатська область";
constexpr const char *ZAPORIZHZHIA = "Запорізька область";
constexpr const char *IVANO_FRANKIVSK = "Івано-Франківська область";
constexpr const char *KYIV_OBLAST = "Київська область";
constexpr const char *KIROVOHRAD = "Кіровоградська область";
constexpr const char *LUHANSK = "Луганська область";
constexpr const char *LVIV = "Львівська область";
constexpr const char *MYKOLAIV = "Миколаївська область";
constexpr const char *ODESA = "Одеська область";
constexpr const char *POLTAVA = "Полтавська область";
constexpr const char *RIVNE = "Рівненська область";
constexpr const char *SUMY = "Сумська область";
constexpr const char *TERNOPIL = "Тернопільська область";
constexpr const char *KHARKIV = "Харківська область";
constexpr const char *KHERSON = "Херсонська область";
constexpr const char *KHMELNYTSKYI = "Хмельницька область";
constexpr const char *CHERKASY = "Черкаська область";
constexpr const char *CHERNIVTSI = "Чернівецька область";
constexpr const char *CHERNIHIV = "Чернігівська область";
constexpr const char *KYIV_CITY = "м. Київ";
constexpr const char *SEVASTOPOL = "Автономна Республіка Крим";

struct RegionIndex {
	const char *name;
	const char *nameEn;
	int index;
	bool topHalf;
};

// Keep these indexes aligned with the display layout. Update this table when
// the physical region order changes.
constexpr RegionIndex INDEXES[] = {
	{SEVASTOPOL, "Autonomous Republic of Crimea", 0, false},
	{KHERSON, "Kherson Oblast", 1, false},
	{ZAPORIZHZHIA, "Zaporizhzhia Oblast", 2, false},
	{DONETSK, "Donetsk Oblast", 3, false},
	{LUHANSK, "Luhansk Oblast", 4, true},
	{KHARKIV, "Kharkiv Oblast", 5, true},
	{DNIPROPETROVSK, "Dnipropetrovsk Oblast", 6, false},
	{POLTAVA, "Poltava Oblast", 7, true},
	{SUMY, "Sumy Oblast", 8, true},
	{CHERNIHIV, "Chernihiv Oblast", 9, true},
	{KYIV_CITY, "Kyiv City", 10, true},
	{KYIV_OBLAST, "Kyiv Oblast", 11, true},
	{CHERKASY, "Cherkasy Oblast", 12, false},
	{KIROVOHRAD, "Kirovohrad Oblast", 13, false},
	{MYKOLAIV, "Mykolaiv Oblast", 14, false},
	{ODESA, "Odesa Oblast", 15, false},
	{VINNYTSIA, "Vinnytsia Oblast", 16, true},
	{KHMELNYTSKYI, "Khmelnytskyi Oblast", 17, true},
	{ZHYTOMYR, "Zhytomyr Oblast", 18, true},
	{RIVNE, "Rivne Oblast", 19, true},
	{VOLYN, "Volyn Oblast", 20, true},
	{LVIV, "Lviv Oblast", 21, true},
	{ZAKARPATTIA, "Zakarpattia Oblast", 22, false},
	{IVANO_FRANKIVSK, "Ivano-Frankivsk Oblast", 23, false},
	{TERNOPIL, "Ternopil Oblast", 24, true},
	{CHERNIVTSI, "Chernivtsi Oblast", 25, false},
};

constexpr size_t INDEX_COUNT = sizeof(INDEXES) / sizeof(INDEXES[0]);

inline const char *canonicalNameForName(const char *name) {
	if (name == nullptr) {
		return nullptr;
	}
	for (size_t i = 0; i < INDEX_COUNT; ++i) {
		if (std::strcmp(INDEXES[i].name, name) == 0 ||
			std::strcmp(INDEXES[i].nameEn, name) == 0) {
			return INDEXES[i].name;
		}
	}
	return nullptr;
}

inline int indexForName(const char *name) {
	const char *canonicalName = canonicalNameForName(name);
	if (canonicalName == nullptr) {
		return -1;
	}
	for (size_t i = 0; i < INDEX_COUNT; ++i) {
		if (INDEXES[i].name == canonicalName) {
			return INDEXES[i].index;
		}
	}
	return -1;
}
}  // namespace Regions
