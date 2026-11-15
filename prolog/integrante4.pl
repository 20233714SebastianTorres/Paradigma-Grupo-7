% Integrante 4: Walter Melendez
% Funcionalidades base 4.3.4, 4.3.5, 4.3.6 y funcionalidad adicional
% "Simulacion de cierres viales y evaluacion del impacto en las mudanzas".
%
% Uso (desde la carpeta raiz):
%   swipl prolog/integrante4.pl
%   ?- cargar_datos.
%   ?- ruta_evitando(p01, p05, p03, Ruta).
%   ?- puede_atender(c02, m001).
%   ?- pedidos_inaccesibles(Pedidos).
%   ?- analizar_cierre(p04, p05, Resultado).

:- use_module(library(lists)).
:- ensure_loaded(consolidacion).  % cargar_datos/0: lee los 4 CSV (4.3.1)
:- ensure_loaded(integrante2).    % conectado/3 (4.3.2) y alcanzable/2 (4.3.3)

% Recorrido simple entre dos puntos por tramos habilitados, sin repetir puntos.
% recorrido(+Actual, +Destino, +Visitados, -Ruta)
recorrido(Destino, Destino, _, [Destino]).
recorrido(Actual, Destino, Visitados, [Actual|Resto]) :-
    Actual \== Destino,
    conectado(Actual, Siguiente, _),
    \+ memberchk(Siguiente, Visitados),
    recorrido(Siguiente, Destino, [Siguiente|Visitados], Resto).

% 4.3.4 Ruta evitando un punto.
% El punto a evitar se pone en la lista de visitados desde el inicio,
% asi el backtracking nunca puede pasar por el.
ruta_evitando(Origen, Destino, Evitar, Ruta) :-
    Origen \== Evitar,
    Destino \== Evitar,
    recorrido(Origen, Destino, [Evitar, Origen], Ruta).

% 4.3.5 Atencion de pedidos: el camion esta disponible y su capacidad
% alcanza para la carga del pedido.
puede_atender(Camion, Pedido) :-
    camion(Camion, _, Capacidad, _, disponible),
    mudanza(Pedido, _, _, Carga, _, _, _, _, _),
    Carga =< Capacidad.

% 4.3.6 Pedidos inaccesibles: pendientes cuyo destino no es alcanzable
% desde su origen usando solo tramos habilitados.
pedidos_inaccesibles(Pedidos) :-
    findall(Pedido,
            ( mudanza(Pedido, Origen, Destino, _, _, pendiente, _, _, _),
              \+ alcanzable(Origen, Destino) ),
            Pedidos).

% FA - Autor: Walter Melendez
% Simulacion de cierres viales y evaluacion del impacto en las mudanzas.
% El cierre se simula pasando el tramo como parametro (sin_tramo/1): la base
% de conocimiento no se modifica (no hay assert ni retract).

% Conexion habilitada que ignora el tramo cerrado en la simulacion.
% Cierre = ninguno | sin_tramo(A, B)
via(ninguno, X, Y, Km) :-
    conectado(X, Y, Km).
via(sin_tramo(A, B), X, Y, Km) :-
    conectado(X, Y, Km),
    \+ mismo_tramo(A, B, X, Y).

mismo_tramo(A, B, A, B).
mismo_tramo(A, B, B, A).

% Igual que recorrido/4, pero acumulando los km y respetando el cierre.
recorrido_km(_, Destino, Destino, _, [Destino], 0).
recorrido_km(Cierre, Actual, Destino, Visitados, [Actual|Resto], Km) :-
    Actual \== Destino,
    via(Cierre, Actual, Siguiente, Tramo),
    \+ memberchk(Siguiente, Visitados),
    recorrido_km(Cierre, Siguiente, Destino, [Siguiente|Visitados], Resto, KmResto),
    Km is Tramo + KmResto.

% Ruta mas corta: se juntan todas las rutas (backtracking) y se toma la menor.
ruta_mas_corta(Cierre, Origen, Destino, Ruta, Km) :-
    findall(K-R, recorrido_km(Cierre, Origen, Destino, [Origen], R, K), Rutas),
    keysort(Rutas, [Km-Ruta|_]).

% La ruta pasa por el tramo A-B en cualquier sentido.
usa_tramo([X, Y|_], A, B) :-
    mismo_tramo(A, B, X, Y), !.
usa_tramo([_|Resto], A, B) :-
    usa_tramo(Resto, A, B).

% Ruta alternativa con el tramo cerrado, o sin_alternativa si no existe.
alternativa(A, B, Origen, Destino, alternativa(Ruta, Km)) :-
    ruta_mas_corta(sin_tramo(A, B), Origen, Destino, Ruta, Km), !.
alternativa(_, _, _, _, sin_alternativa).

% analizar_cierre(+A, +B, -Resultado)
% Resultado: lista de afectado(Pedido, RutaOriginal, KmOriginal, Alternativa)
% para cada pedido pendiente cuya ruta mas corta pasa por el tramo A-B y que,
% con el cierre, queda sin ruta o con una ruta mas larga (si hay otra ruta
% igual de corta, el pedido no se considera afectado).
% Falla si el tramo no existe o ya esta cerrado.
analizar_cierre(A, B, Resultado) :-
    once(conectado(A, B, _)),
    findall(afectado(Pedido, Ruta, Km, Alternativa),
            ( mudanza(Pedido, Origen, Destino, _, _, pendiente, _, _, _),
              ruta_mas_corta(ninguno, Origen, Destino, Ruta, Km),
              usa_tramo(Ruta, A, B),
              alternativa(A, B, Origen, Destino, Alternativa),
              empeora(Km, Alternativa) ),
            Resultado).

% El cierre empeora el traslado: no hay alternativa o es mas larga.
empeora(_, sin_alternativa).
empeora(Km, alternativa(_, KmAlt)) :-
    KmAlt > Km + 1.0e-9.
