-- Pruebas del integrante 4 (Haskell): 4.2.6, 4.2.7 y funcionalidad adicional.
-- Ejecutar desde la carpeta raiz:
--   runghc -ihaskell pruebas/TestIntegrante4.hs
module Main where

import Tipos
import LecturaCSV (leerDatos)
import Integrante4
import Integrante3 (distanciaYTiempoRuta, rutasPosibles)
import Data.List (find)
import System.Exit (exitFailure)

-- Compara numeros con tolerancia para evitar errores de redondeo.
casi :: Double -> Double -> Bool
casi a b = abs (a - b) < 1e-9

prueba :: String -> Bool -> IO Bool
prueba nombre ok = do
  putStrLn ((if ok then "[OK]    " else "[FALLA] ") ++ nombre)
  return ok

esLeft :: Either a b -> Bool
esLeft (Left _) = True
esLeft _        = False

main :: IO ()
main = do
  lectura <- leerDatos "datos"
  datos <- either (\e -> putStrLn e >> exitFailure) return lectura
  let ms = mudanzas datos
      comparacion = compararCostoRutas "c02" ["p01","p03","p04","p05"]
                                             ["p01","p03","p04","p08","p06","p05"] datos
      impacto a b = impactoCierre a b "c02" datos
      afectados = either (const []) (map pedidoAfectado)
      -- p01-p04 (14 km) empata con p01-p03-p04 (6 + 8 km)
      datosEmpate = datos { conexiones = conexiones datos ++ [Conexion "p01" "p04" 14 40 "habilitada"] }
      datosDuplicado = datos { conexiones = conexiones datos ++ [Conexion "p03" "p02" 1 3 "habilitada"] }
      conexionDe a b d = find (\c -> origenConexion c == a && destinoConexion c == b) (conexiones d)
  resultados <- sequence
    -- 4.2.6 Reporte de pedidos
    [ prueba "4.2.6 cuenta pedidos por estado"
        (contarPorEstado ms == [("pendiente",3),("asignado",1),("entregado",2)])
    , prueba "4.2.6 carga asignada por camion (incluye camiones sin carga)"
        (cargaPorCamion (camiones datos) ms == [("c01",10),("c02",0),("c03",0)])
    , prueba "4.2.6 pendientes ordenados por prioridad (empate: orden del CSV)"
        (map codigoMudanza (pendientesPorPrioridad ms) == ["m001","m006","m002"])
    -- 4.2.7 Resultado esperado
    , prueba "4.2.7 resumen del pedido m002 con c02"
        (resumenPedido "m002" "c02" datos == Right
          [ "Pedido: m002 (p02 -> p06)"
          , "Rutas posibles: 9"
          , "Costo de ruta p02 -> p03 -> p04 -> p08 -> p06 (camion_mediano): S/ 78.00"
          , "Pedidos pendientes por prioridad: m001 (alta), m006 (alta), m002 (media)"
          ])
    , prueba "4.2.7 pedido inexistente devuelve error"
        (esLeft (resumenPedido "m999" "c02" datos))
    , prueba "4.2.7 camion inexistente devuelve error"
        (esLeft (resumenPedido "m002" "c99" datos))
    -- FA: comparacion de rutas
    , prueba "FA compara distancia, tiempo y costo de dos rutas"
        (case comparacion of
           Right c -> casi (kmOriginal c) 21 && casi (kmAlternativa c) 26.5
                   && minOriginal c == 62 && minAlternativa c == 79
                   && casi (costoOriginal c) 84 && casi (costoAlternativa c) 106
                   && casi (incrementoCosto c) 22 && casi (incrementoKm c) 5.5
                   && incrementoMin c == 17
           Left _  -> False)
    , prueba "FA ruta con tramo inexistente devuelve error"
        (esLeft (compararCostoRutas "c02" ["p01","p05"] ["p01","p03","p04","p05"] datos))
    , prueba "FA camion inexistente devuelve error"
        (esLeft (compararCostoRutas "c99" ["p01","p03"] ["p01","p02","p03"] datos))
    -- FA: simulacion de cierre
    , prueba "FA simularCierre cierra el tramo en la copia (en cualquier sentido)"
        (fmap estadoConexion (conexionDe "p04" "p05" (simularCierre "p05" "p04" datos)) == Just "cerrada")
    , prueba "FA simularCierre no modifica los datos originales"
        (fmap estadoConexion (conexionDe "p04" "p05" datos) == Just "habilitada")
    , prueba "FA cierre p04-p05 afecta solo a m001 y tiene alternativa"
        (case impacto "p04" "p05" of
           Right [i] -> pedidoAfectado i == "m001"
                     && rutaActual i == ["p01","p03","p04","p05"]
                     && fmap rutaAlternativa (alternativa i) == Just ["p01","p03","p04","p08","p06","p05"]
           _         -> False)
    , prueba "FA cierre p03-p04 afecta a m001, m002 y m006"
        (afectados (impacto "p03" "p04") == ["m001","m002","m006"])
    , prueba "FA sin alternativa cuando el destino queda aislado"
        (case impactoCierre "p04" "p05" "c02" (simularCierre "p05" "p06" datos) of
           Right [i] -> pedidoAfectado i == "m001" && alternativa i == Nothing
           _         -> False)
    , prueba "FA tramo inexistente devuelve error"
        (esLeft (impacto "p01" "p07"))
    , prueba "FA tramo ya cerrado devuelve error"
        (esLeft (impacto "p03" "p06"))
    -- FA: con rutas empatadas, solo se afecta si el cierre alarga la ruta
    , prueba "FA cierre sin aumento de distancia (ruta empatada) no afecta a nadie"
        (afectados (impactoCierre "p01" "p03" "c02" datosEmpate) == [])
    , prueba "FA cierre p04-p05 sigue afectando a m001 con rutas empatadas"
        (afectados (impactoCierre "p04" "p05" "c02" datosEmpate) == ["m001"])
    -- Integrante 3: tramo registrado dos veces (en sentido inverso)
    , prueba "4.2.5 con un tramo duplicado se usa la distancia menor"
        (fmap fst (distanciaYTiempoRuta ["p02","p03"] (conexiones datosDuplicado)) == Right 1)
    , prueba "4.2.4 con un tramo duplicado no se repiten rutas"
        (length (rutasPosibles "p02" "p06" (conexiones datosDuplicado)) == 9)
    ]
  let fallas = length (filter not resultados)
  if fallas == 0
    then putStrLn ("Todas las pruebas pasaron (" ++ show (length resultados) ++ ").")
    else putStrLn (show fallas ++ " prueba(s) fallaron.") >> exitFailure
