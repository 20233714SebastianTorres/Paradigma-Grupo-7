module Main where
import Tipos
import LecturaCSV
import Consolidacion
import Text.Printf (printf)

main :: IO ()
main = do
  resultado <- leerDatos "datos"
  case resultado of
    Left e -> putStrLn ("Error leyendo CSV: " ++ e)
    Right datos -> do
      putStrLn "=== PROYECTO G7 - INTEGRANTE 1 (HASKELL) ==="
      printf "Puntos: %d | Conexiones: %d | Camiones: %d | Mudanzas: %d\n"
        (length (puntos datos)) (length (conexiones datos))
        (length (camiones datos)) (length (mudanzas datos))
      case ahorroConsolidacion ["m002", "m006"] "c02" datos of
        Left e -> putStrLn ("No se puede consolidar: " ++ e)
        Right r -> do
          printf "Mudanzas: %s | Camion: %s\n" (show (mudanzasAgrupadas r)) (camionUsado r)
          printf "Separado: S/ %.2f | Consolidado: S/ %.2f | Ahorro: S/ %.2f\n"
            (costoSeparado r) (costoConsolidado r) (ahorro r)
