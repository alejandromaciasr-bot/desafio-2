#pragma once
#include <string>

struct Album {
    int albumId;          // 2 digitos (bb)
    int artistaId;        // 5 digitos (aaaaa)
    std::string nombre;
    std::string sello;
    std::string fecha;
    int duracionSeg;
    std::string rutaBase;   // ej: "/users/.../claudia\\ lorelle/"
    std::string portadaPng; // ej: "image/lugar\\ secreto.png"
};
