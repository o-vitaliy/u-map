#pragma once

#include <cstring>

// Region name strings as returned by the "states" object of
// https://ubilling.net.ua/aerialalerts/ — use these constants instead of
// retyping the Ukrainian names when matching against RegionAlert::name.
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
constexpr const char *SEVASTOPOL = "м. Севастополь";

struct RegionIndex {
	const char *name;
	int index;
};

// Keep these indexes aligned with the display layout. Update this table when
// the physical region order changes.
constexpr RegionIndex INDEXES[] = {
	{SEVASTOPOL, 0},
	{KHERSON, 1},
	{ZAPORIZHZHIA, 2},
	{LUHANSK, 3},
	{DONETSK, 4},
	{KHARKIV, 5},
	{DNIPROPETROVSK, 6},
	{POLTAVA, 7},
	{SUMY, 8},
	{CHERNIHIV, 9},
    {KIROVOHRAD, 10},
	{MYKOLAIV, 11},
	{ODESA, 12},
	{CHERKASY, 13},
	{KYIV_OBLAST, 14},
	{ZHYTOMYR, 15},
	{VINNYTSIA, 16},
	{KHMELNYTSKYI, 17},
	{RIVNE, 18},
	{VOLYN, 19},
	{LVIV, 20},
	{ZAKARPATTIA, 21},
	{IVANO_FRANKIVSK, 22},
	{CHERNIVTSI, 23},
	{TERNOPIL, 24},
	{KYIV_CITY, 25},
};

constexpr size_t INDEX_COUNT = sizeof(INDEXES) / sizeof(INDEXES[0]);

inline int indexForName(const char *name) {
	for (size_t i = 0; i < INDEX_COUNT; ++i) {
		if (std::strcmp(INDEXES[i].name, name) == 0) {
			return INDEXES[i].index;
		}
	}
	return -1;
}
}  // namespace Regions
