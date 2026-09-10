#pragma once

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
}  // namespace Regions
