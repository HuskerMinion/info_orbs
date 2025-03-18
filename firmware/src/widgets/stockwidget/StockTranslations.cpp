#include "StockTranslations.h"

// Languages are defined in I18n.h:
// EN, DE, FR

// All translation variables for StockWidget should start with "t_stock"

constexpr Translation t_stockList = {
    "List of securities to track.  If there are more securities than the number of screens, your list will paginate. You can track forex, crypto (symbol/USD) or stocks from any exchange (if one ticker is part of multiple exchanges you can add on '&country = Canada' to narrow down to your ticker)", // EN
    nullptr, // DE
    nullptr, // FR
};

constexpr Translation t_stockChangeFormat = {
    "Show percent or price change", // EN
    nullptr, // DE
    nullptr, // FR
};

constexpr TranslationMulti<2> t_stockChangeFormats = {
    {
        "Percent", // EN
        nullptr, // DE
        nullptr, // FR
    },
    {
        "Price", // EN
        nullptr, // DE
        nullptr, // FR
    }};

constexpr Translation t_stock52week = {
    "52 Week:", // EN
    nullptr, // DE
    nullptr, // FR
};

constexpr Translation t_stockSwitchInterval = {
    "Time to disply before paginating, set to 0 to disable auto pagination", // EN
    nullptr, // DE
    nullptr, // FR
};
