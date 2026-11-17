module Main where

import Tipos
import LecturaCSV
import Integrante3
import Text.Printf (printf)

main :: IO ()
main = do
  putStrLn "=========================================================="
  putStrLn "     PRUEBAS DEL MODULO HASKELL - INTEGRANTE 3"
  putStrLn "=========================================================="

  resDatos <- leerDatos "datos"
  case resDatos of
    Left err -> putStrLn $ "Error al cargar CSVs: " ++ err
    Right dt -> do
      let cs = conexiones dt
          cams = camiones dt

      -- 1. Prueba 4.2.3: Vecinos
      putStrLn "\n--- [4.2.3] Prueba de Vecinos de 'p01' ---"
      let vP01 = vecinos "p01" cs
      putStrLn "Vecinos conectados directamente por tramos habilitados:"
      mapM_ (\(vec, km, min) -> printf "  -> %s (%.1f km, %d min)\n" vec km min) vP01

      -- 2. Prueba 4.2.4: Rutas posibles
      putStrLn "\n--- [4.2.4] Prueba de Rutas Posibles entre 'p01' y 'p05' ---"
      let rutas = rutasPosibles "p01" "p05" cs
      printf "Total de rutas encontradas (sin ciclos): %d\n" (length rutas)
      mapM_ (\(i, r) -> printf "  Ruta %d: %s\n" (i :: Int) (unwords r)) (zip [1..] rutas)

      -- 3. Prueba 4.2.5: Costo de ruta
      putStrLn "\n--- [4.2.5] Prueba de Costo de Ruta ---"
      case filter (\c -> codigoCamion c == "c02") cams of
        [] -> putStrLn "No se encontro el camion c02"
        (cam02:_) -> do
          let rutaEjemplo = ["p01", "p02", "p04", "p05"]
          putStrLn $ "Ruta evaluada: " ++ unwords rutaEjemplo ++ " con camion " ++ codigoCamion cam02 ++ " (tarifa S/ " ++ show (tarifaKm cam02) ++ "/km)"
          case costoRuta rutaEjemplo cam02 cs of
            Left err -> putStrLn $ "Error calculando costo: " ++ err
            Right cst -> printf "  -> Costo total calculado: S/ %.2f\n" cst

      -- 4. Prueba FA: Cotización comparativa y recomendación automática
      putStrLn "\n--- [FA] Cotizacion Comparativa y Recomendacion de Camion ---"
      let codMudanzaTest = "m001"
      putStrLn $ "Consultando cotizacion automatica para el pedido: " ++ codMudanzaTest
      case cotizarServicio codMudanzaTest dt of
        Left err -> putStrLn $ "No se pudo cotizar: " ++ err
        Right cotizaciones -> do
          case cotizaciones of
            (prim:_) -> do
              putStrLn $ "Ruta optima encontrada: " ++ unwords (rutaSugerida prim)
              printf "Distancia: %.2f km\n" (distanciaKmRuta prim)
            _ -> return ()
          putStrLn "Camiones evaluados (ordenados por menor costo):"
          mapM_ (\c -> printf "  Camion %s (%s): Costo S/ %.2f | Capacidad usada: %.1f%% | S/ %.2f por m3\n"
                        (camionCotizado c) (tipoCamionCotizado c) (costoEstimado c)
                        (porcentajeUtilizado c) (precioPorM3 c)) cotizaciones
          case cotizaciones of
            (mejor:_) -> printf ">> Recomendacion: Utilizar el camion %s por un total de S/ %.2f\n"
                          (camionCotizado mejor) (costoEstimado mejor)
            [] -> putStrLn "Sin opciones disponibles."

      putStrLn "\n=========================================================="
      putStrLn "              TODAS LAS PRUEBAS COMPLETADAS"
      putStrLn "=========================================================="
