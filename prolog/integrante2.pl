:- use_module(library(lists)).
:- dynamic conexion/5, camion/5, mudanza/9.

% 4.3.2 Conexion directa
% Dice si dos puntos tienen un tramo habilitado, en cualquier sentido.
% A es Origen, B es destino, D es la distancia en km
conectado(A, B, D) :-
    conexion(A, B, D, _, habilitada).
conectado(A, B, D) :-
    conexion(B, A, D, _, habilitada).

% 4.3.3 Alcanzabilidad
% Dice si se puede llegar de un punto a otro, directa o indirectamente.
alcanzable(A, B) :-
    alcanzable_aux(A, B, [A]).

alcanzable_aux(A, B, _) :-
    conectado(A, B, _).

alcanzable_aux(A, B, Visitados) :-
    conectado(A, X, _),
    not(member(X, Visitados)),
    alcanzable_aux(X, B, [X | Visitados]).

% FA Autor: Rado Delgado Jean Carlo de Jesus
% Agenda de servicios: detecta choques de horario y verifica disponibilidad.
choque(Camion, Fecha, Franja) :-
    mudanza(_, _, _, _, _, _, Camion, Fecha, Franja),
    Fecha \= '',
    Franja \= ''.

camion_libre(Camion, Fecha, Franja) :-
    camion(Camion, _, _, _, _),
    not(choque(Camion, Fecha, Franja)).