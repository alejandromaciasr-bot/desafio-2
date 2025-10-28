#pragma once
#include <string>

struct Anuncio {
    int adId;
    char categoria; // 'c', 'b', 'a' (a = prioridad alta)
    std::string texto;
};
