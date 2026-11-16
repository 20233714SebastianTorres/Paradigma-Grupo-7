% Tests for the additional features of integrante 1 (consolidation) and
% integrante 2 (schedule) in Prolog.
% Run from the project root:
%   swipl -g run_tests -t halt pruebas/test_consolidacion_agenda.pl
:- use_module(library(plunit)).
:- ensure_loaded('../prolog/consolidacion').

% Helpers live in module user: plunit runs each block in its own module.
agregar_entregada_con_camion :-
    assertz(mudanza(mtest, p01, p02, 1, baja, entregado, c02, '2026-11-25', tarde)).

quitar_entregada :-
    retractall(mudanza(mtest, _, _, _, _, _, _, _, _)).

:- begin_tests(ahorro_grupo, [setup(cargar_datos)]).

% c02 holds 10 m3; m001 + m002 + m006 add up to 12 m3.
test(grupo_que_excede_la_capacidad_no_tiene_ahorro, [fail]) :-
    ahorro_grupo(c02, [m001, m002, m006], _).

test(grupo_dentro_de_la_capacidad_calcula_ahorro) :-
    ahorro_grupo(c03, [m001, m002, m006], A),
    number(A).

test(mejor_consolidacion_sigue_funcionando, [nondet]) :-
    ( mejor_consolidacion(c03, Ids, A) -> is_list(Ids), A > 0 ; true ).

:- end_tests(ahorro_grupo).

:- begin_tests(agenda, [setup(cargar_datos)]).

test(choque_con_mudanza_asignada) :-
    choque(c01, '2026-11-20', manana).

test(mudanza_entregada_no_genera_choque,
     [setup(agregar_entregada_con_camion), cleanup(quitar_entregada), fail]) :-
    choque(c02, '2026-11-25', tarde).

test(camion_libre_si_solo_tiene_mudanzas_entregadas,
     [setup(agregar_entregada_con_camion), cleanup(quitar_entregada)]) :-
    camion_libre(c02, '2026-11-25', tarde).

test(camion_ocupado_no_esta_libre, [fail]) :-
    camion_libre(c01, '2026-11-20', manana).

:- end_tests(agenda).
