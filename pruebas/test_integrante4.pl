% Pruebas del integrante 4 (Prolog): 4.3.4, 4.3.5, 4.3.6 y funcionalidad adicional.
% Ejecutar desde la carpeta raiz:
%   swipl -g run_tests -t halt pruebas/test_integrante4.pl
:- use_module(library(plunit)).
:- ensure_loaded('../prolog/integrante4').

% Helpers en el modulo user: plunit ejecuta cada bloque en su propio modulo,
% por eso los cambios a la base de conocimiento se hacen desde aqui.
cerrar_tramo(A, B) :-
    retract(conexion(A, B, D, T, habilitada)),
    assertz(conexion(A, B, D, T, cerrada)).

agregar_pedido_grande :-
    assertz(mudanza(mtest, p01, p02, 15, alta, pendiente, '', '', '')).

quitar_pedido_grande :-
    retractall(mudanza(mtest, _, _, _, _, _, _, _, _)).

% --- 4.3.4 Ruta evitando un punto ---------------------------------------
:- begin_tests(ruta_evitando, [setup(cargar_datos)]).

test(ninguna_ruta_pasa_por_el_punto_evitado, [nondet]) :-
    findall(R, ruta_evitando(p01, p05, p03, R), Rutas),
    Rutas \= [],
    forall(member(R, Rutas),
           ( R = [p01|_], last(R, p05), \+ member(p03, R) )).

test(primera_ruta_valida) :-
    once(ruta_evitando(p01, p05, p03, R)),
    R == [p01, p02, p04, p05].

test(no_se_puede_evitar_el_origen, [fail]) :-
    ruta_evitando(p01, p05, p01, _).

test(no_se_puede_evitar_el_destino, [fail]) :-
    ruta_evitando(p01, p05, p05, _).

test(evitar_p04_obliga_a_salir_de_p07_por_p08, [nondet]) :-
    % p07 solo se conecta con p04 y p08.
    forall(ruta_evitando(p07, p01, p04, R), R = [p07, p08|_]).

:- end_tests(ruta_evitando).

% --- 4.3.5 Atencion de pedidos -----------------------------------------
:- begin_tests(puede_atender, [setup(cargar_datos)]).

test(camion_disponible_con_capacidad) :-
    puede_atender(c02, m001).

test(capacidad_justa_alcanza) :-
    puede_atender(c02, m003).          % carga 10 y capacidad 10

test(camion_ocupado_no_atiende, [fail]) :-
    puede_atender(c01, m001).

test(capacidad_insuficiente_no_atiende,
     [ setup(agregar_pedido_grande), cleanup(quitar_pedido_grande) ]) :-
    mudanza(mtest, _, _, 15, _, _, _, _, _),   % el pedido de 15 m3 existe
    \+ puede_atender(c02, mtest),              % c02 tiene 10 m3
    puede_atender(c03, mtest).                 % c03 tiene 20 m3

test(backtracking_lista_camiones, [true(Camiones == [c02, c03])]) :-
    findall(C, puede_atender(C, m001), Camiones).

:- end_tests(puede_atender).

% --- 4.3.6 Pedidos inaccesibles ----------------------------------------
:- begin_tests(pedidos_inaccesibles, [setup(cargar_datos), cleanup(cargar_datos)]).

test(con_los_datos_actuales_no_hay, [true(P == [])]) :-
    pedidos_inaccesibles(P).

test(destino_aislado_por_cierres,
     [ setup(( cerrar_tramo(p04, p05), cerrar_tramo(p05, p06) )),
       cleanup(cargar_datos),
       true(P == [m001]) ]) :-
    % p05 (destino de m001) queda aislado al cerrar sus dos tramos.
    pedidos_inaccesibles(P).

:- end_tests(pedidos_inaccesibles).

% --- FA: Simulacion de cierres viales -------------------------------------
:- begin_tests(analizar_cierre, [setup(cargar_datos)]).

test(cierre_p04_p05_afecta_solo_a_m001) :-
    analizar_cierre(p04, p05, R),
    R = [afectado(m001, [p01, p03, p04, p05], Km, alternativa(Alt, KmAlt))],
    Km =:= 21,
    Alt == [p01, p03, p04, p08, p06, p05],
    KmAlt =:= 26.5.

test(cierre_p03_p04_afecta_a_tres_pedidos, [true(Ids == [m001, m002, m006])]) :-
    analizar_cierre(p03, p04, R),
    findall(Id, member(afectado(Id, _, _, _), R), Ids).

test(el_orden_del_tramo_no_importa) :-
    analizar_cierre(p04, p05, R1),
    analizar_cierre(p05, p04, R2),
    R1 == R2.

test(sin_alternativa_si_el_destino_queda_aislado,
     [ setup(cerrar_tramo(p05, p06)), cleanup(cargar_datos) ]) :-
    analizar_cierre(p04, p05, R),
    R = [afectado(m001, _, _, sin_alternativa)].

test(no_modifica_los_datos) :-
    analizar_cierre(p04, p05, _),
    conexion(p04, p05, _, _, habilitada).

test(tramo_inexistente_falla, [fail]) :-
    analizar_cierre(p01, p07, _).

test(tramo_ya_cerrado_falla, [fail]) :-
    analizar_cierre(p03, p06, _).

:- end_tests(analizar_cierre).
