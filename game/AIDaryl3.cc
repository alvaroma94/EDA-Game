
#include "Player.hh"

using namespace std;


/**
 * Escriu el nom * del teu jugador i guarda 
 * aquest fitxer amb el nom AI*.cc
 */
#define PLAYER_NAME Daryl3


/**
 * Podeu declarar constants aquí
 */

const int X[8] = {1, 1, 0, -1, -1, -1, 0, 1};
const int Y[8] = {0, 1, 1, 1, 0, -1, -1, -1};

struct PLAYER_NAME : public Player {


  /**
   * Factory: retorna una nova instància d'aquesta classe.
   * No toqueu aquesta funció.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }
    

  /**
   * Els atributs dels vostres jugadors es poden definir aquí.
   */

   struct Orientacion {
   	int x;
   	int y;
   	int z;
   };

   //SOLDADOS

    void mover_soldado(vector<vector<Posicio>> &m, Posicio objetivo, int id) {
		Info in = dades(id);
		Posicio actual = in.pos;
		//Buscamos actual a partir de objetivo
		Posicio anteriorObjetivo = m[objetivo.x][objetivo.y];
		while(anteriorObjetivo != actual) {
		    objetivo = anteriorObjetivo;
		    anteriorObjetivo = m[objetivo.x][objetivo.y];
		}
		//En objetivo tienes la posicion a la que quieres ir;
		ordena_soldat(id, objetivo.x, objetivo.y);
    }

    void bfs_soldat(int id) {
	vector<vector<Posicio>> m(MAX, vector<Posicio> (MAX));
	Info in = dades(id);
	int x = in.pos.x;
	int y = in.pos.y;
	int equip = in.equip;
	for (int i = 0; i < 8; ++i) {
	    int xx = x + X[i];
	    int yy = y + Y[i];
	    if (xx >= 0 and xx < MAX and yy >= 0 and yy < MAX and que(xx,yy) != 3 and que(xx,yy) != 4) {
		//Si tenemos un post al lado, lo cojemos.
		if(valor_post(xx,yy) > 0) {
		    if(de_qui_post(xx,yy) != equip) {
	    		ordena_soldat(id, xx, yy);
			return;
		    }
		}
		int id2 = quin_soldat(xx, yy);
		//Si tenemos un enemigo al lado, le atacamos.
		if (id2 and dades(id2).equip != equip) {
		    ordena_soldat(id, xx, yy);
		    return;
		}
	    }
	}
	vector<vector<bool>> visitados(MAX, vector<bool> (MAX, false));
	//Info in = dades(id);
	Posicio p = in.pos;
	int mi_equipo = in.equip;
	int vida = in.vida;
	queue<Posicio> q;
	q.push(p);
	visitados[p.x][p.y] = true;
	while(not q.empty()) {
	    Posicio objetivo = q.front();
	    q.pop();
	    //Buscamos los post
	    for(int i = 0; i < 8; ++i) {
		int xx = objetivo.x + X[i];
		int yy = objetivo.y + Y[i];
		if(valid(xx,yy) and not visitados[xx][yy] and que(xx,yy) < 3 and temps_foc(xx,yy) == 0){
		    if (quin_soldat(xx,yy) == 0){
			//Marcamos la casilla como visitada
			visitados[xx][yy] = true;
			m[xx][yy] = objetivo;
			if(valor_post(xx, yy) != 0) {
			    if(de_qui_post(xx, yy) != mi_equipo) {
				objetivo.x = xx;
				objetivo.y = yy;
				mover_soldado(m, objetivo, id);
				return;
			    }
			}
		    }
		    Posicio nueva;
		    nueva.x = xx;
		    nueva.y = yy;
		    q.push(nueva);
		}
	    }
	}
   }

   //HELICOPTEROS

    int contarEnemigos(int id) {
	Info in = dades(id);
	Posicio pos = in.pos;
	int yo = in.equip;
	int nSolAmigo = 0;
	int nSolEnemigo = 0;
	for(int i = pos.x-2; i < pos.x+2; ++i) {
	    for(int j = pos.y-2; j < pos.y+2; ++j) {
		int id2 = quin_soldat(i, j);
		if(id2 > 0) {
		    Info in2 = dades(id2);
		    if(in2.equip == yo) ++nSolAmigo;
		    else ++nSolEnemigo;
		}
	    }
	}
	return nSolEnemigo-nSolAmigo;
    }
  
    void mover_heli(vector<vector<vector<Orientacion>>> &m, Orientacion objetivo, int id){
    	Info in = dades(id);
	Posicio pactual = in.pos;
	Orientacion actual;
	actual.x = pactual.x;
	actual.y = pactual.y;
	actual.z = in.orientacio;
	//Buscamos actual a partir de objetivo
	Orientacion aObj = m[objetivo.x][objetivo.y][objetivo.z];
	while(aObj.x != actual.x or aObj.y != actual.y or aObj.z != actual.z) {
	    objetivo = aObj;
	    aObj = m[objetivo.x][objetivo.y][objetivo.z];
	}
	int accion;
	if(aObj.z == objetivo.z) accion = AVANCA1;
	else if(objetivo.z == actual.z+1 or (objetivo.z == 0 and actual.z == 3)) accion = CONTRA_RELLOTGE;
	else if(objetivo.z+1 == actual.z or (objetivo.z == 3 and actual.z == 0)) accion = RELLOTGE;
	else accion = RELLOTGE;
	//En objetivo tienes la posicion a la que quieres ir;
	ordena_helicopter(id, accion);
    }

    void buscarHeli(vector<vector<vector<bool>>> &visitados, int equip) {
    	for(int i = 0; i < MAX; ++i) {
	    for(int j = 0; j < MAX ; ++j) {
		if(not visitados[i][j][0] and quin_heli(i,j) > 0 and dades(quin_heli(i,j)).equip != equip) {
		    for(int i2 = i -2; i2 < i + 2; ++i2) {
    			for(int j2 = j -2; j2 < j + 2; ++j2) {
			    for(int z = 0; z < 4; ++z) {
    				visitados[i2][j2][z] = true;
			    }
			}
		    }
		}
	    }
    	}
    }
    
    void bfs_heli(int id) {
	vector<vector<vector<Orientacion>>> m(MAX, vector<vector<Orientacion>> (MAX, vector<Orientacion> (4)));
	Info in = dades(id);
	int x = in.pos.x;
	int y = in.pos.y;
	int equip = in.equip;
	if (contarEnemigos(id) > 3 and in.napalm == 0) {
		ordena_helicopter(id, NAPALM);
		return;
	}
	vector<vector<vector<bool>>> visitados(MAX, vector<vector<bool>> (MAX, vector<bool> (4, false)));
	buscarHeli(visitados, equip);
	Posicio posic = in.pos;
	Orientacion p;
	p.x = posic.x;
	p.y = posic.y;
	p.z = in.orientacio;
	if(valor_post(p.x, p.y) != 0) {
		if(de_qui_post(p.x, p.y) != equip) {
		    return;
		}
	}
	int id2 = quin_soldat(p.x, p.y);
	// Si tenim un enemic al costat, l'ataquem.
	if (id2 and dades(id2).equip != equip) {
		return;
	}
	queue<Orientacion> q;
	q.push(p);
	visitados[p.x][p.y][p.z] = true;
	while(not q.empty()) {
	    Orientacion objetivo = q.front();
	    q.pop();
	    //Busqueda Post
	    if(valor_post(objetivo.x, objetivo.y) != 0) {
		if(de_qui_post(objetivo.x, objetivo.y) != equip) {
		    mover_heli(m, objetivo, id);
		    return;
		}
	    }
	    int id2 = quin_soldat(objetivo.x, objetivo.y);
	    // Si tenim un enemic al costat, l'ataquem.
	    if (id2 and dades(id2).equip != equip) {
		mover_heli(m, objetivo, id);
		return;
	    }
	    if(valid(objetivo.x, objetivo.y)){
		bool delante = false;
		int xx;
		int yy;
		if(objetivo.z == 0) {
		    xx = objetivo.x + 3;
		    yy = objetivo.y - 2;
		    for(int i = yy; i < objetivo.y + 3; ++i) {
	  		if(que(xx,i) == 4) delante = true;
		    }
		    xx = objetivo.x +1;
		    yy = objetivo.y;
		}
		if(objetivo.z == 1) {
		    xx = objetivo.x -2;
		    yy = objetivo.y +3;
		    for(int i = xx; i < objetivo.x + 3; ++i) {
	    		if(que(i,yy) == 4) delante = true;
		    }
		    xx = objetivo.x;
		    yy = objetivo.y+1;
		}
		if(objetivo.z == 2) {
		    xx = objetivo.x - 3;
		    yy = objetivo.y - 2;
		    for(int i = yy; i < objetivo.y + 3; ++i) {
	  		if(que(xx,i) == 4) delante = true;
		    }
		    xx = objetivo.x -1;
		    yy = objetivo.y;
		}
		if(objetivo.z == 3) {
		    xx = objetivo.x - 2;
		    yy = objetivo.y - 3;
		    for(int i = xx; i < objetivo.x + 3; ++i) {
			if(que(i,yy) == 4) delante = true;
		    }
		    xx = objetivo.x;
		    yy = objetivo.y-1;
		}
		if(not delante) {
		    Orientacion ori;
		    ori.x = xx;
		    ori.y = yy;
		    ori.z = objetivo.z;
		    if(not visitados[ori.x][ori.y][ori.z]) {
			q.push(ori);
			visitados[ori.x][ori.y][ori.z] = true;
			m[ori.x][ori.y][ori.z] = objetivo;
		    }
		}
		Orientacion neg;
		neg.x = objetivo.x;
		neg.y = objetivo.y;
		neg.z = objetivo.z-1;
		if(neg.z < 0) neg.z = 3;
		if(not visitados[neg.x][neg.y][neg.z]) {
		    q.push(neg);
		    visitados[neg.x][neg.y][neg.z] = true;
		    m[neg.x][neg.y][neg.z] = objetivo;;
		}
		Orientacion posit;
		posit.x = objetivo.x;
		posit.y = objetivo.y;
		posit.z = objetivo.z+1;
		if(posit.z > 3) posit.z = 0;
		if(not visitados[posit.x][posit.y][posit.z]) {
		    q.push(posit);
		    visitados[posit.x][posit.y][posit.z] = true;
		    m[posit.x][posit.y][posit.z] = objetivo;
		}
	    }
	}
    }
    
    void caure_paracas(int id) {
	Info in = dades(id);
	Posicio pos = in.pos;
	int auxx = pos.x;
	int auxy = pos.y;
	int boom = 3;
	for(int itx = auxx-2; itx < auxx+2 and boom > 0; ++itx) {
	    for(int ity = auxy-2; ity < auxy+2 and boom > 0; ++ity) {
		if (valid(itx,ity) and que(itx,ity) != 4 and que(itx,ity) != 3 and temps_foc(itx,ity) == 0 and quin_soldat(itx,ity) == 0){
		    ordena_paracaigudista(itx, ity);
		    --boom;
		}
	    }
	}
    }
    
    void caure_paracas2(int id) {
	Info in = dades(id);
	Posicio pos = in.pos;
	int auxx = pos.x;
	int auxy = pos.y;
	int boom = 1;
	for(int itx = auxx-2; itx < auxx+2 and boom > 0; ++itx) {
	    for(int ity = auxy-2; ity < auxy+2 and boom > 0; ++ity) {
		if (valid(itx,ity) and que(itx,ity) != 4 and que(itx,ity) != 3 and temps_foc(itx,ity) == 0 and quin_soldat(itx,ity) == 0){
		    ordena_paracaigudista(itx, ity);
		    --boom;
		}
	    }
	}
    }

  /**
   * Mètode play.
   * 
   * Aquest mètode serà invocat una vegada cada torn.
   */     
  virtual void play () {
	int mi_equipo = qui_soc();
	VE S = soldats(mi_equipo);
	VE H = helis(mi_equipo);
	if (not dades(H[0]).paraca.empty()) caure_paracas(H[0]);
	if (not dades(H[1]).paraca.empty()) caure_paracas2(H[1]);
	for (int i = 0; i < (int)H.size(); ++i) bfs_heli(H[i]);
	for (int i = 0; i < (int)S.size(); ++i) bfs_soldat(S[i]);
  }
};


/**
 * No toqueu aquesta línia.
 */
RegisterPlayer(PLAYER_NAME);

