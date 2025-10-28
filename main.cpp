#include "sistema.h"
#include <iostream>

int main(){
    Sistema sys;

    // los csv deben estar en el directorio de ejecucion (tu build).
    sys.cargarUsuarios  ("usuarios.csv");
    sys.cargarArtistas  ("artistas.csv");
    sys.cargarAlbums    ("albums.csv");
    sys.cargarCanciones ("canciones.csv");
    sys.cargarFavoritos ("favoritos.csv");
    sys.cargarFollows   ("follows.csv");
    sys.cargarAnuncios  ("publicidad.csv");

    sys.menuPrincipal();
    return 0;
}
