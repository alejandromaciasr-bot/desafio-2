#pragma once
#include <string>
#include "usuario.h"
#include "artista.h"
#include "album.h"
#include "cancion.h"
#include "anuncio.h"
#include "favrow.h"
#include "followrow.h"
#include "metrics.h"

class Sistema {
public:
    // arreglos dinamicos crudos (sin stl ni templates)
    Usuario*   usuarios;   int nUsuarios,   capUsuarios;
    Artista*   artistas;   int nArtistas,   capArtistas;
    Album*     albums;     int nAlbums,     capAlbums;
    Cancion*   canciones;  int nCanciones,  capCanciones;
    Anuncio*   anuncios;   int nAnuncios,   capAnuncios;
    FavRow*    favoritos;  int nFavs,       capFavs;
    FollowRow* follows;    int nFollows,    capFollows;

    // estado
    Usuario*   usuarioActual;
    int        ultimoAdId;
    Metrics    metrics;

    Sistema();
    ~Sistema();

    // crecer capacidad por tipo (new[]/delete[])
    void ensureCapUsuarios (int needed);
    void ensureCapArtistas (int needed);
    void ensureCapAlbums   (int needed);
    void ensureCapCanciones(int needed);
    void ensureCapAnuncios (int needed);
    void ensureCapFavs     (int needed);
    void ensureCapFollows  (int needed);

    // I) carga/actualizacion
    bool cargarUsuarios   (const char* path);
    bool cargarArtistas   (const char* path);
    bool cargarAlbums     (const char* path);
    bool cargarCanciones  (const char* path);
    bool cargarFavoritos  (const char* path);
    bool cargarFollows    (const char* path);
    bool cargarAnuncios   (const char* path);

    bool guardarFavoritos (const char* path);
    bool guardarFollows   (const char* path);

    // busquedas
    Usuario* findUsuarioByNick(const std::string& nick);
    Album*   findAlbum(int artistaId, int albumId);
    Cancion* findSongById(int songId9);

    // ii) login
    bool login(const std::string& nick);

    // publicidad
    int  pesoCategoria(char c);
    int  pickAnuncio();

    // rutas
    void buildCoverPath (const Cancion& c, const Album& a, std::string& out);
    void buildAudioPath (const Cancion& c, const Album& a, bool premium, std::string& out);

    // iii) reproduccion aleatoria
    void reproducirAleatoriaAuto(int K=5, int segundos=3);   // auto 3s, k=5 + anuncios en estandar
    void reproducirAleatoriaPasoAPaso();                     // controles segun tipo, sin temporizador

    // iv) favoritos (premium)
    bool favExists (const std::string& nick, int songId9);
    bool favAdd    (const std::string& nick, int songId9);
    bool favRemove (const std::string& nick, int songId9);

    bool followAdd   (const std::string& follower, const std::string& followed);
    bool followRemove(const std::string& follower, const std::string& followed);

    int  buildFavoritosEfectivos(const std::string& nick, int* outIds, int outCap);

    void reproducirFavoritosOrden   (const std::string& nick, int M=6, int segundos=3);
    void reproducirFavoritosAleatoria(const std::string& nick, int M=6, int segundos=3);

    // v) metricas
    std::size_t estimateMemoryBytes() const;
    void        printMetrics() const;

    // menu
    void menuPrincipal();
};
