#pragma once
#include <string>

struct Usuario {
    std::string nickname;   // ej: "ana"
    char tipo;              // 'e' (estandar) o 'p' (premium)
    std::string ciudad;
    std::string pais;
    std::string fechaRegistro;
};
