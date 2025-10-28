#include "sistema.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using std::string;
using std::cout;
using std::cin;
using std::endl;

//  helpers csv
static bool getlineCSV(std::ifstream& in, string& line){
    if(!std::getline(in, line)) return false;
    if (!line.empty() && line.back()=='\r') line.pop_back();
    return true;
}
static void splitSemicolon(const string& line, string* out, int expected){
    int idx=0; size_t start=0;
    while (idx<expected-1){
        size_t pos = line.find(';', start);
        if (pos==string::npos) break;
        out[idx++] = line.substr(start, pos-start);
        start = pos+1;
    }
    if (idx<expected) out[idx++] = (start<line.size()? line.substr(start) : "");
    while (idx<expected){ out[idx++] = ""; }
}

//  ctor/dtor
Sistema::Sistema():
    usuarios(nullptr), nUsuarios(0), capUsuarios(0),
    artistas(nullptr), nArtistas(0), capArtistas(0),
    albums(nullptr), nAlbums(0), capAlbums(0),
    canciones(nullptr), nCanciones(0), capCanciones(0),
    anuncios(nullptr), nAnuncios(0), capAnuncios(0),
    favoritos(nullptr), nFavs(0), capFavs(0),
    follows(nullptr), nFollows(0), capFollows(0),
    usuarioActual(nullptr), ultimoAdId(-1)
{
    std::srand((unsigned)std::time(nullptr));
}
Sistema::~Sistema(){
    delete[] usuarios;
    delete[] artistas;
    delete[] albums;
    delete[] canciones;
    delete[] anuncios;
    delete[] favoritos;
    delete[] follows;
}

//  crecer capacidad por tipo
void Sistema::ensureCapUsuarios (int needed){
    if (needed <= capUsuarios) return;
    int newCap = (capUsuarios==0) ? 4 : capUsuarios*2;
    while (newCap < needed) newCap *= 2;
    Usuario* nuevo = new Usuario[newCap];
    for (int i=0;i<capUsuarios;i++) nuevo[i] = usuarios[i];
    delete[] usuarios; usuarios = nuevo; capUsuarios = newCap;
}
void Sistema::ensureCapArtistas (int needed){
    if (needed <= capArtistas) return;
    int newCap = (capArtistas==0) ? 4 : capArtistas*2;
    while (newCap < needed) newCap *= 2;
    Artista* nuevo = new Artista[newCap];
    for (int i=0;i<capArtistas;i++) nuevo[i] = artistas[i];
    delete[] artistas; artistas = nuevo; capArtistas = newCap;
}
void Sistema::ensureCapAlbums   (int needed){
    if (needed <= capAlbums) return;
    int newCap = (capAlbums==0) ? 4 : capAlbums*2;
    while (newCap < needed) newCap *= 2;
    Album* nuevo = new Album[newCap];
    for (int i=0;i<capAlbums;i++) nuevo[i] = albums[i];
    delete[] albums; albums = nuevo; capAlbums = newCap;
}
void Sistema::ensureCapCanciones(int needed){
    if (needed <= capCanciones) return;
    int newCap = (capCanciones==0) ? 4 : capCanciones*2;
    while (newCap < needed) newCap *= 2;
    Cancion* nuevo = new Cancion[newCap];
    for (int i=0;i<capCanciones;i++) nuevo[i] = canciones[i];
    delete[] canciones; canciones = nuevo; capCanciones = newCap;
}
void Sistema::ensureCapAnuncios (int needed){
    if (needed <= capAnuncios) return;
    int newCap = (capAnuncios==0) ? 4 : capAnuncios*2;
    while (newCap < needed) newCap *= 2;
    Anuncio* nuevo = new Anuncio[newCap];
    for (int i=0;i<capAnuncios;i++) nuevo[i] = anuncios[i];
    delete[] anuncios; anuncios = nuevo; capAnuncios = newCap;
}
void Sistema::ensureCapFavs     (int needed){
    if (needed <= capFavs) return;
    int newCap = (capFavs==0) ? 4 : capFavs*2;
    while (newCap < needed) newCap *= 2;
    FavRow* nuevo = new FavRow[newCap];
    for (int i=0;i<capFavs;i++) nuevo[i] = favoritos[i];
    delete[] favoritos; favoritos = nuevo; capFavs = newCap;
}
void Sistema::ensureCapFollows  (int needed){
    if (needed <= capFollows) return;
    int newCap = (capFollows==0) ? 4 : capFollows*2;
    while (newCap < needed) newCap *= 2;
    FollowRow* nuevo = new FollowRow[newCap];
    for (int i=0;i<capFollows;i++) nuevo[i] = follows[i];
    delete[] follows; follows = nuevo; capFollows = newCap;
}

// -------- I) carga/actualizacion --------
bool Sistema::cargarUsuarios(const char* path){
    std::ifstream in(path); if(!in) return false;
    string line; getlineCSV(in, line);
    while (getlineCSV(in, line)){
        metrics.iterations++;
        string f[5]; splitSemicolon(line, f, 5);
        ensureCapUsuarios(nUsuarios+1);
        Usuario& u = usuarios[nUsuarios++];
        u.nickname = f[0];
        u.tipo = f[1].empty()? 'e' : f[1][0]; // 'p' o 'e'
        u.ciudad = f[2]; u.pais=f[3]; u.fechaRegistro=f[4];
    }
    return true;
}
bool Sistema::cargarArtistas(const char* path){
    std::ifstream in(path); if(!in) return false;
    string line; getlineCSV(in, line);
    while (getlineCSV(in, line)){
        metrics.iterations++;
        string f[3]; splitSemicolon(line, f, 3);
        ensureCapArtistas(nArtistas+1);
        Artista& a = artistas[nArtistas++];
        a.artistaId = std::atoi(f[0].c_str());
        a.nombre = f[1]; a.pais=f[2];
    }
    return true;
}
bool Sistema::cargarAlbums(const char* path){
    std::ifstream in(path); if(!in) return false;
    string line; getlineCSV(in, line);
    while (getlineCSV(in, line)){
        metrics.iterations++;
        string f[8]; splitSemicolon(line, f, 8);
        ensureCapAlbums(nAlbums+1);
        Album& a = albums[nAlbums++];
        a.albumId=std::atoi(f[0].c_str());
        a.artistaId=std::atoi(f[1].c_str());
        a.nombre=f[2]; a.sello=f[3]; a.fecha=f[4];
        a.duracionSeg=std::atoi(f[5].c_str());
        a.rutaBase=f[6]; a.portadaPng=f[7];
    }
    return true;
}
bool Sistema::cargarCanciones(const char* path){
    std::ifstream in(path); if(!in) return false;
    string line; getlineCSV(in, line);
    while (getlineCSV(in, line)){
        metrics.iterations++;
        string f[4]; splitSemicolon(line, f, 4);
        ensureCapCanciones(nCanciones+1);
        Cancion& c = canciones[nCanciones++];
        c.songId9=std::atoi(f[0].c_str());
        c.nombre=f[1];
        c.duracionSeg=std::atoi(f[2].c_str());
        c.subrutaAudio=f[3];
    }
    return true;
}
bool Sistema::cargarFavoritos(const char* path){
    std::ifstream in(path); if(!in) return false;
    string line; getlineCSV(in, line);
    while (getlineCSV(in, line)){
        metrics.iterations++;
        string f[2]; splitSemicolon(line, f, 2);
        ensureCapFavs(nFavs+1);
        favoritos[nFavs].nickname=f[0];
        favoritos[nFavs].songId9=std::atoi(f[1].c_str());
        nFavs++;
    }
    return true;
}
bool Sistema::cargarFollows(const char* path){
    std::ifstream in(path); if(!in) return false;
    string line; getlineCSV(in, line);
    while (getlineCSV(in, line)){
        metrics.iterations++;
        string f[2]; splitSemicolon(line, f, 2);
        ensureCapFollows(nFollows+1);
        follows[nFollows].follower=f[0];
        follows[nFollows].followed=f[1];
        nFollows++;
    }
    return true;
}
bool Sistema::cargarAnuncios(const char* path){
    std::ifstream in(path); if(!in) return false;
    string line; getlineCSV(in, line);
    while (getlineCSV(in, line)){
        metrics.iterations++;
        string f[3]; splitSemicolon(line, f, 3);
        ensureCapAnuncios(nAnuncios+1);
        anuncios[nAnuncios].adId=std::atoi(f[0].c_str());
        anuncios[nAnuncios].categoria = f[1].empty()? 'c' : f[1][0]; // 'a','b','c'
        anuncios[nAnuncios].texto=f[2];
        nAnuncios++;
    }
    return true;
}
bool Sistema::guardarFavoritos(const char* path){
    std::ofstream out(path); if(!out) return false;
    out<<"nickname;songId9\n";
    for (int i=0;i<nFavs;i++){
        metrics.iterations++;
        out<<favoritos[i].nickname<<";"<<favoritos[i].songId9<<"\n";
    }
    return true;
}
bool Sistema::guardarFollows(const char* path){
    std::ofstream out(path); if(!out) return false;
    out<<"followerNickname;followedNickname\n";
    for (int i=0;i<nFollows;i++){
        metrics.iterations++;
        out<<follows[i].follower<<";"<<follows[i].followed<<"\n";
    }
    return true;
}

// -------- busquedas --------
Usuario* Sistema::findUsuarioByNick(const string& nick){
    for (int i=0;i<nUsuarios;i++){
        metrics.iterations++;
        if (usuarios[i].nickname==nick) return &usuarios[i];
    }
    return nullptr;
}
Album* Sistema::findAlbum(int artistaId, int albumId){
    for (int i=0;i<nAlbums;i++){
        metrics.iterations++;
        if (albums[i].artistaId==artistaId && albums[i].albumId==albumId) return &albums[i];
    }
    return nullptr;
}
Cancion* Sistema::findSongById(int songId9){
    for (int i=0;i<nCanciones;i++){
        metrics.iterations++;
        if (canciones[i].songId9==songId9) return &canciones[i];
    }
    return nullptr;
}

// -------- ii) login --------
bool Sistema::login(const string& nick){
    usuarioActual = findUsuarioByNick(nick);
    return usuarioActual != nullptr;
}

// -------- publicidad --------
int Sistema::pesoCategoria(char c){
    if (c=='c') return 1;
    if (c=='b') return 2;
    return 3; // 'a'
}
int Sistema::pickAnuncio(){
    if (nAnuncios<=0) return -1;
    int total=0;
    for (int i=0;i<nAnuncios;i++){
        metrics.iterations++;
        if (nAnuncios>1 && anuncios[i].adId==ultimoAdId) continue;
        total += pesoCategoria(anuncios[i].categoria);
    }
    if (total<=0) return 0;
    int r = std::rand()%total, acc=0;
    for (int i=0;i<nAnuncios;i++){
        metrics.iterations++;
        if (nAnuncios>1 && anuncios[i].adId==ultimoAdId) continue;
        acc += pesoCategoria(anuncios[i].categoria);
        if (r<acc) return i;
    }
    return 0;
}

// -------- rutas --------
void Sistema::buildCoverPath(const Cancion&, const Album& a, string& out){
    out = a.rutaBase + a.portadaPng;
}
void Sistema::buildAudioPath(const Cancion& c, const Album& a, bool premium, string& out){
    out = a.rutaBase + c.subrutaAudio + (premium? "_320.ogg" : "_128.ogg");
}

// -------- iii-a) reproduccion aleatoria (auto 3s, k=5) --------
void Sistema::reproducirAleatoriaAuto(int K, int segundos){
    if (nCanciones<=0){ cout<<"no hay canciones.\n"; return; }
    metrics.reset();
    bool esPremium = (usuarioActual && usuarioActual->tipo=='p');

    int played = 0;
    while (played < K){
        int current = std::rand()%nCanciones;
        Cancion& c = canciones[current];
        Album* a = findAlbum(Cancion::artistaFrom(c.songId9), Cancion::albumFrom(c.songId9));
        if (!a){ cout<<"album no encontrado.\n"; break; }

        std::string cover,audio; buildCoverPath(c,*a,cover); buildAudioPath(c,*a,esPremium,audio);
        cout<<"\nreproduciendo: "<<c.nombre<<" (id "<<c.songId9<<")\n";
        cout<<"portada: "<<cover<<"\n";
        cout<<"audio:   "<<audio<<"\n";

        // anuncio cada 2 canciones solo para estandar
        if (!esPremium && (played%2==0)){
            int iAd = pickAnuncio();
            if (iAd>=0){
                cout<<"=== anuncio ===\n";
                cout<<anuncios[iAd].texto<<"\n";
                cout<<"cat: "<<anuncios[iAd].categoria<<"\n";
                cout<<"==============\n";
                ultimoAdId = anuncios[iAd].adId;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(segundos));
        played++;
    }

    cout<<"\n[fin reproduccion aleatoria auto]\n";
    metrics.memoryBytes = estimateMemoryBytes();
    printMetrics();
}

// -------- iii-b) reproduccion aleatoria (paso a paso) --------
void Sistema::reproducirAleatoriaPasoAPaso(){
    if (nCanciones<=0){ cout<<"no hay canciones.\n"; return; }
    metrics.reset();
    bool esPremium = (usuarioActual && usuarioActual->tipo=='p');

    // historial para premium (previa hasta 4)
    const int HMAX = 4;
    int history[HMAX]; int hsize=0;

    int current = std::rand()%nCanciones;
    bool repetir = false;

    while (true){
        Cancion& c = canciones[current];
        Album* a = findAlbum(Cancion::artistaFrom(c.songId9), Cancion::albumFrom(c.songId9));
        if (!a){ cout<<"album no encontrado.\n"; break; }

        std::string cover,audio; buildCoverPath(c,*a,cover); buildAudioPath(c,*a,esPremium,audio);
        cout<<"\nreproduciendo: "<<c.nombre<<" (id "<<c.songId9<<")\n";
        cout<<"portada: "<<cover<<"\n";
        cout<<"audio:   "<<audio<<"\n";

        if (!esPremium){
            cout<<"\ncomando: (1) detener  (2) iniciar  (0) salir\n> ";
            int op; if(!(cin>>op)){ cin.clear(); cin.ignore(10000,'\n'); continue; }
            if (op==0) { cout<<"saliendo de paso a paso.\n"; break; }
            if (op==1) { cout<<"[detenido]\n"; }
            if (op==2) { cout<<"[iniciado]\n"; }
            // tras la orden, avanzamos a otra aleatoria
            current = std::rand()%nCanciones;
        } else {
            cout<<"\ncomando: (2) siguiente  (3) previa  (4) repetir on/off  (0) salir\n> ";
            int op; if(!(cin>>op)){ cin.clear(); cin.ignore(10000,'\n'); continue; }
            if (op==0) { cout<<"saliendo de paso a paso.\n"; break; }
            if (op==2){ // siguiente
                if (!repetir){
                    if (hsize < HMAX) history[hsize++] = current;
                    else { for(int i=1;i<HMAX;i++) history[i-1]=history[i]; history[HMAX-1]=current; }
                }
                current = std::rand()%nCanciones;
            } else if (op==3){ // previa
                if (hsize>0) current = history[--hsize];
                else cout<<"no hay previa.\n";
            } else if (op==4){ // repetir
                repetir = !repetir;
                cout<<(repetir? "[repetir on]\n" : "[repetir off]\n");
            } else {
                cout<<"opcion invalida.\n";
            }
        }
    }

    cout<<"\n[fin reproduccion paso a paso]\n";
    metrics.memoryBytes = estimateMemoryBytes();
    printMetrics();
}

// -------- iv) favoritos --------
bool Sistema::favExists(const string& nick, int songId9){
    for (int i=0;i<nFavs;i++){
        metrics.iterations++;
        if (favoritos[i].nickname==nick && favoritos[i].songId9==songId9) return true;
    }
    return false;
}
bool Sistema::favAdd(const string& nick, int songId9){
    Usuario* u = findUsuarioByNick(nick); if(!u || u->tipo!='p') return false;
    if (favExists(nick, songId9)) return false;
    ensureCapFavs(nFavs+1);
    favoritos[nFavs].nickname=nick; favoritos[nFavs].songId9=songId9; nFavs++;
    return true;
}
bool Sistema::favRemove(const string& nick, int songId9){
    for (int i=0;i<nFavs;i++){
        metrics.iterations++;
        if (favoritos[i].nickname==nick && favoritos[i].songId9==songId9){
            for (int j=i+1;j<nFavs;j++){ favoritos[j-1]=favoritos[j]; metrics.iterations++; }
            nFavs--;
            return true;
        }
    }
    return false;
}
bool Sistema::followAdd(const string& follower, const string& followed){
    Usuario* u = findUsuarioByNick(follower); if(!u || u->tipo!='p') return false;
    for (int i=0;i<nFollows;i++){
        metrics.iterations++;
        if (follows[i].follower==follower && follows[i].followed==followed) return false;
    }
    ensureCapFollows(nFollows+1);
    follows[nFollows].follower=follower; follows[nFollows].followed=followed; nFollows++;
    return true;
}
bool Sistema::followRemove(const string& follower, const string& followed){
    for (int i=0;i<nFollows;i++){
        metrics.iterations++;
        if (follows[i].follower==follower && follows[i].followed==followed){
            for (int j=i+1;j<nFollows;j++){ follows[j-1]=follows[j]; metrics.iterations++; }
            nFollows--;
            return true;
        }
    }
    return false;
}
int Sistema::buildFavoritosEfectivos(const string& nick, int* outIds, int outCap){
    int k=0;
    // propios
    for (int i=0;i<nFavs;i++){
        metrics.iterations++;
        if (favoritos[i].nickname==nick){
            int id=favoritos[i].songId9; bool dup=false;
            for (int t=0;t<k;t++){ if(outIds[t]==id){ dup=true; break; } }
            if (!dup && k<outCap) outIds[k++]=id;
        }
    }
    // seguidos
    for (int i=0;i<nFollows;i++){
        metrics.iterations++;
        if (follows[i].follower==nick){
            const string& seg = follows[i].followed;
            for (int j=0;j<nFavs;j++){
                metrics.iterations++;
                if (favoritos[j].nickname==seg){
                    int id=favoritos[j].songId9; bool dup=false;
                    for (int t=0;t<k;t++){ if(outIds[t]==id){ dup=true; break; } }
                    if (!dup && k<outCap) outIds[k++]=id;
                }
            }
        }
    }
    return k;
}

// ejecucion de favoritos
void Sistema::reproducirFavoritosOrden(const string& nick, int /*M*/, int segundos){
    Usuario* u = findUsuarioByNick(nick); if(!u || u->tipo!='p'){ cout<<"solo premium.\n"; return; }
    metrics.reset();
    int ids[10050]; int k = buildFavoritosEfectivos(nick, ids, 10050);
    if (k<=0){ cout<<"no hay favoritos.\n"; return; }

    for (int i=0;i<k;i++){
        metrics.iterations++;
        Cancion* c = findSongById(ids[i]); if(!c) continue;
        Album* a = findAlbum(Cancion::artistaFrom(c->songId9), Cancion::albumFrom(c->songId9)); if(!a) continue;

        std::string cover,audio; buildCoverPath(*c,*a,cover); buildAudioPath(*c,*a,true,audio);
        cout<<"\n[favorito "<<(i+1)<<"/"<<k<<"] "<<c->nombre<<" (id "<<c->songId9<<")\n";
        cout<<"portada: "<<cover<<"\n";
        cout<<"audio:   "<<audio<<"\n";

        std::this_thread::sleep_for(std::chrono::seconds(segundos));
    }
    cout<<"\n[fin favoritos en orden]\n";
    metrics.memoryBytes = estimateMemoryBytes();
    printMetrics();
}
void Sistema::reproducirFavoritosAleatoria(const string& nick, int /*M*/, int segundos){
    Usuario* u = findUsuarioByNick(nick); if(!u || u->tipo!='p'){ cout<<"solo premium.\n"; return; }
    metrics.reset();
    int ids[10050]; int k = buildFavoritosEfectivos(nick, ids, 10050);
    if (k<=0){ cout<<"no hay favoritos.\n"; return; }

    int plays = (k<10? k:10);
    for (int i=0;i<plays;i++){
        metrics.iterations++;
        int pick = std::rand()%k;
        Cancion* c = findSongById(ids[pick]); if(!c) continue;
        Album* a = findAlbum(Cancion::artistaFrom(c->songId9), Cancion::albumFrom(c->songId9)); if(!a) continue;

        std::string cover,audio; buildCoverPath(*c,*a,cover); buildAudioPath(*c,*a,true,audio);
        cout<<"\n[favoritos aleatorio "<<(i+1)<<"] "<<c->nombre<<" (id "<<c->songId9<<")\n";
        cout<<"portada: "<<cover<<"\n";
        cout<<"audio:   "<<audio<<"\n";

        std::this_thread::sleep_for(std::chrono::seconds(segundos));
    }
    cout<<"\n[fin favoritos aleatorio]\n";
    metrics.memoryBytes = estimateMemoryBytes();
    printMetrics();
}

// -------- v) metricas --------
std::size_t Sistema::estimateMemoryBytes() const{
    // base: objeto + arreglos por capacidad (incluye los objetos std::string)
    std::size_t total = sizeof(*this);
    total += sizeof(Usuario)  * capUsuarios;
    total += sizeof(Artista)  * capArtistas;
    total += sizeof(Album)    * capAlbums;
    total += sizeof(Cancion)  * capCanciones;
    total += sizeof(Anuncio)  * capAnuncios;
    total += sizeof(FavRow)   * capFavs;
    total += sizeof(FollowRow)* capFollows;

    // estimar heap extra de strings por encima del SSO (heuristica ~15)
    const std::size_t SSO = 15;
    auto add_heap_of = [&](const std::string& s){
        std::size_t cap = s.capacity();
        if (cap > SSO) {
            total += (cap + 1); // aprox: buffer en heap + '\0'
        }
    };

    // usuarios
    for (int i=0;i<nUsuarios;i++){
        add_heap_of(usuarios[i].nickname);
        add_heap_of(usuarios[i].ciudad);
        add_heap_of(usuarios[i].pais);
        add_heap_of(usuarios[i].fechaRegistro);
    }
    // artistas
    for (int i=0;i<nArtistas;i++){
        add_heap_of(artistas[i].nombre);
        add_heap_of(artistas[i].pais);
    }
    // albums
    for (int i=0;i<nAlbums;i++){
        add_heap_of(albums[i].nombre);
        add_heap_of(albums[i].sello);
        add_heap_of(albums[i].fecha);
        add_heap_of(albums[i].rutaBase);
        add_heap_of(albums[i].portadaPng);
    }
    // canciones
    for (int i=0;i<nCanciones;i++){
        add_heap_of(canciones[i].nombre);
        add_heap_of(canciones[i].subrutaAudio);
    }
    // anuncios
    for (int i=0;i<nAnuncios;i++){
        add_heap_of(anuncios[i].texto);
    }
    // favoritos
    for (int i=0;i<nFavs;i++){
        add_heap_of(favoritos[i].nickname);
    }
    // follows
    for (int i=0;i<nFollows;i++){
        add_heap_of(follows[i].follower);
        add_heap_of(follows[i].followed);
    }

    return total;
}

void Sistema::printMetrics() const{
    std::cout<<"iteraciones (aprox): "<<metrics.iterations<<"\n";
    std::cout<<"memoria estimada (aprox): "<<estimateMemoryBytes()<<" bytes\n";
}

// -------- menu --------
void Sistema::menuPrincipal(){
    while (true){
        cout << "\n";
        cout << "=== udeatunes ===\n";
        cout << "1) login\n";
        cout << "2) reproduccion aleatoria (auto 3s, k=5)\n";
        cout << "3) reproduccion aleatoria (paso a paso)\n";
        cout << "4) favoritos (editar)\n";
        cout << "5) favoritos (seguir)\n";
        cout << "6) favoritos (ejecutar en orden)\n";
        cout << "7) favoritos (ejecutar aleatorio)\n";
        cout << "0) salir\n> ";
        int op; if(!(cin>>op)){ cin.clear(); cin.ignore(10000,'\n'); break; }
        if (op==0) break;

        if (op==1){
            cout << "usuario: "; string nick; cin >> nick;
            if (login(nick)) cout << "bienvenido, " << (usuarioActual?usuarioActual->nickname:"") << " ("<<(usuarioActual && usuarioActual->tipo=='p'?"premium":"estandar")<<")\n";
            else cout << "usuario no encontrado.\n";
        } else if (op==2){
            reproducirAleatoriaAuto(5,3);
        } else if (op==3){
            reproducirAleatoriaPasoAPaso();
        } else if (op==4){
            if (!usuarioActual || usuarioActual->tipo!='p'){ cout<<"solo premium.\n"; continue; }
            metrics.reset();
            cout << "(1) agregar  (2) eliminar\n> "; int a; cin>>a;
            cout << "songid9: "; int sid; cin>>sid;
            bool ok=false;
            if (a==1) ok = favAdd(usuarioActual->nickname, sid);
            else if (a==2) ok = favRemove(usuarioActual->nickname, sid);
            cout << (ok? "ok.\n":"operacion no realizada.\n");
            guardarFavoritos("favoritos.csv");
            metrics.memoryBytes = estimateMemoryBytes();
            printMetrics();
        } else if (op==5){
            if (!usuarioActual || usuarioActual->tipo!='p'){ cout<<"solo premium.\n"; continue; }
            metrics.reset();
            cout << "(1) seguir  (2) dejar de seguir\n> "; int a; cin>>a;
            cout << "usuario objetivo: "; string other; cin>>other;
            bool ok=false;
            if (a==1) ok = followAdd(usuarioActual->nickname, other);
            else if (a==2) ok = followRemove(usuarioActual->nickname, other);
            cout << (ok? "ok.\n":"operacion no realizada.\n");
            guardarFollows("follows.csv");
            metrics.memoryBytes = estimateMemoryBytes();
            printMetrics();
        } else if (op==6){
            if (!usuarioActual || usuarioActual->tipo!='p'){ cout<<"solo premium.\n"; continue; }
            reproducirFavoritosOrden(usuarioActual->nickname, 6, 3);
        } else if (op==7){
            if (!usuarioActual || usuarioActual->tipo!='p'){ cout<<"solo premium.\n"; continue; }
            reproducirFavoritosAleatoria(usuarioActual->nickname, 6, 3);
        } else {
            cout<<"opcion invalida.\n";
        }
    }
}
