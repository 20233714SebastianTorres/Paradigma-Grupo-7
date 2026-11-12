% FA - Integrante 1: funcionalidad adicional "Consolidacion de mudanzas pequenas".
% SWI-Prolog: cargar_datos. consolidable(c02, L).
% Se usan los mismos 4 archivos CSV que el resto del grupo.
:- use_module(library(csv)).
:- use_module(library(lists)).
:- dynamic punto/4, conexion/5, camion/5, mudanza/9.

cargar_datos :-
    retractall(punto(_,_,_,_)),
    retractall(conexion(_,_,_,_,_)),
    retractall(camion(_,_,_,_,_)),
    retractall(mudanza(_,_,_,_,_,_,_,_,_)),
    csv_read_file('datos/puntos.csv', P, [functor(row), convert(false)]),
    maplist(cargar_punto, P),
    csv_read_file('datos/conexiones.csv', C, [functor(row), convert(false)]),
    maplist(cargar_conexion, C),
    csv_read_file('datos/unidades.csv', U, [functor(row), convert(false)]),
    maplist(cargar_camion, U),
    csv_read_file('datos/pedidos.csv', M, [functor(row), convert(false)]),
    maplist(cargar_mudanza, M).

% Acepta CSV con/sin encabezados y pedidos de 7 o 9 columnas.
cargar_punto(row(codigo,_,_,_)) :- !.
cargar_punto(row(C,N,T,Z)) :- assertz(punto(C,N,T,Z)).

cargar_conexion(row(origen,_,_,_,_)) :- !.
cargar_conexion(row(A,B,D,T,E)) :-
    valor_numero(D, Dist), valor_numero(T, Tiempo),
    assertz(conexion(A,B,Dist,Tiempo,E)).

cargar_camion(row(codigo,_,_,_,_)) :- !.
cargar_camion(row(C,T,Cap,Precio,Estado)) :-
    valor_numero(Cap, CapN), valor_numero(Precio, PrecioN),
    assertz(camion(C,T,CapN,PrecioN,Estado)).

cargar_mudanza(row(codigo,_,_,_,_,_,_)) :- !.
cargar_mudanza(row(codigo,_,_,_,_,_,_,_,_)) :- !.
cargar_mudanza(row(Id,O,D,V,P,E,C)) :-
    valor_numero(V, Vol),
    assertz(mudanza(Id,O,D,Vol,P,E,C,'','')).
cargar_mudanza(row(Id,O,D,V,P,E,C,Fecha,Franja)) :-
    valor_numero(V, Vol),
    assertz(mudanza(Id,O,D,Vol,P,E,C,Fecha,Franja)).

valor_numero(N,N) :- number(N), !.
valor_numero(A,N) :- atom(A), atom_number(A,N).

conectado(A,B,Dist) :-
    conexion(X,Y,Dist,_,habilitada),
    (A=X,B=Y ; A=Y,B=X).

camino_simple(A,A,_,0).
camino_simple(A,B,Visitados,D) :-
    A \= B,
    conectado(A,Siguiente,D1),
    \+ memberchk(Siguiente, Visitados),
    camino_simple(Siguiente,B,[Siguiente|Visitados],D2),
    D is D1+D2.

distancia_minima(A,B,0) :- A == B, !.
distancia_minima(A,B,D) :-
    findall(K, camino_simple(A,B,[A],K), Rutas),
    Rutas \= [], min_list(Rutas,D).

% Subconjuntos en orden estable; min. 2 pedidos.
subgrupo([], []).
subgrupo([X|Xs], [X|Ys]) :- subgrupo(Xs,Ys).
subgrupo([_|Xs], Ys) :- subgrupo(Xs,Ys).

misma_zona([Id|Ids]) :-
    mudanza(Id,Origen,_,_,_,pendiente,_,_,_),
    punto(Origen,_,_,Zona),
    forall(member(Otro,Ids),
        (mudanza(Otro,Origen2,_,_,_,pendiente,_,_,_),
         punto(Origen2,_,_,Zona))).

volumen_total([],0).
volumen_total([Id|R],Volumen) :-
    mudanza(Id,_,_,V,_,_,_,_,_),
    volumen_total(R, Resto), Volumen is V+Resto.

alcanzables([]).
alcanzables([Id|Ids]) :-
    mudanza(Id,Origen,Destino,_,_,_,_,_,_),
    distancia_minima(Origen,Destino,_),
    alcanzables(Ids).

recojos_y_entregas([],[],[]).
recojos_y_entregas([Id|Ids],[O|Os],[D|Ds]) :-
    mudanza(Id,O,D,_,_,_,_,_,_),
    recojos_y_entregas(Ids,Os,Ds).

% Los puntos de recojo se visitan antes de iniciar las entregas.
distancia_itinerario([_],0).
distancia_itinerario([A,B|Xs],Total) :-
    distancia_minima(A,B,D),
    distancia_itinerario([B|Xs],Resto),
    Total is D+Resto.

km_consolidados(Ids,Km) :-
    recojos_y_entregas(Ids,Recogidas0,Entregas0),
    sort(Recogidas0,Recogidas), sort(Entregas0,Entregas),
    findall(D,
        (permutation(Recogidas,OrdenR),
         permutation(Entregas,OrdenE),
         append(OrdenR,OrdenE,Itinerario),
         distancia_itinerario(Itinerario,D)),
        Distancias),
    Distancias \= [], min_list(Distancias,Km).

% consolidable(+Camion, -ListaDePedidos): backtracking para alternativas.
consolidable(Camion,Ids) :-
    camion(Camion,_,Capacidad,_,disponible),
    findall(Id,mudanza(Id,_,_,_,_,pendiente,_,_,_),Pendientes),
    subgrupo(Pendientes,Ids),
    length(Ids,N), N>=2,
    misma_zona(Ids),
    volumen_total(Ids,Volumen), Volumen =< Capacidad,
    alcanzables(Ids),
    km_consolidados(Ids,_).

km_separados([],0).
km_separados([Id|Ids],Total) :-
    mudanza(Id,Origen,Destino,_,_,_,_,_,_),
    distancia_minima(Origen,Destino,Dist),
    km_separados(Ids,Resto), Total is Dist+Resto.

ahorro_grupo(Camion,Ids,Ahorro) :-
    camion(Camion,_,_,Tarifa,disponible),
    km_separados(Ids,PorSeparado),
    km_consolidados(Ids,Juntos),
    Ahorro is (PorSeparado-Juntos)*Tarifa.

% mejor_consolidacion(+Camion, -Lista, -Ahorro)
% Criterio: mayor ahorro monetario positivo, no solo mayor volumen.
mejor_consolidacion(Camion,Ids,Ahorro) :-
    setof(Negativo-Grupo,
          A^(consolidable(Camion,Grupo),
              ahorro_grupo(Camion,Grupo,A),
              A > 0, Negativo is -A),
          [Negativo-Ids|_]),
    Ahorro is -Negativo.
