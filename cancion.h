#pragma once
#include <string>

struct Cancion {
    int songId9;              // 9 digitos = aaaaa bb cc
    std::string nombre;
    int duracionSeg;
    std::string subrutaAudio; // ej: "audio/a\\ tu\\ lado"

    static int artistaFrom(int sid){ return sid/10000; }
    static int albumFrom(int sid){ return (sid/100)%100; }
    static int trackFrom(int sid){ return sid%100; }
};
