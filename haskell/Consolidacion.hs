-- FA - Integrante 1: funcionalidad adicional (Haskell).
-- Consolida mudanzas pendientes de una misma zona y calcula el ahorro.
module Consolidacion (ResumenAhorro(..), ahorroConsolidacion) where

import Tipos
import Data.List (find, nub, permutations)

data ResumenAhorro = ResumenAhorro
  { mudanzasAgrupadas :: [String]
  , camionUsado :: String
  , kilometrosSeparados :: Double
  , kilometrosConsolidados :: Double
  , costoSeparado :: Double
  , costoConsolidado :: Double
  , ahorro :: Double
  } deriving (Show, Eq)

minimo :: Ord a => [a] -> Maybe a
minimo [] = Nothing
minimo xs = Just (minimum xs)

-- Considera tramos habilitados en ambos sentidos y evita ciclos.
distanciaMinima :: [Conexion] -> String -> String -> Maybe Double
distanciaMinima tramos inicio fin = recorrer [inicio] inicio
  where
    recorrer visitados actual
      | actual == fin = Just 0
      | otherwise = minimo
          [ km + resto
          | (vecino, km) <- vecinos actual
          , vecino `notElem` visitados
          , Just resto <- [recorrer (vecino:visitados) vecino]
          ]
    vecinos punto =
      [ (if origenConexion c == punto then destinoConexion c else origenConexion c, distanciaKm c)
      | c <- tramos
      , estadoConexion c == "habilitada"
      , distanciaKm c > 0
      , origenConexion c == punto || destinoConexion c == punto
      ]

trayecto :: [Conexion] -> [String] -> Maybe Double
trayecto _ [] = Just 0
trayecto _ [_] = Just 0
trayecto cs (x:y:resto) = do
  d <- distanciaMinima cs x y
  mas <- trayecto cs (y:resto)
  return (d + mas)

-- Visita primero todos los lugares de recojo y luego todos los destinos.
-- Se calcula el recorrido mas corto entre los ordenes posibles.
kmConsolidados :: [Conexion] -> [Mudanza] -> Maybe Double
kmConsolidados cs ms =
  minimo [total | recogidas <- permutations origenes
                , entregas <- permutations destinos
                , Just total <- [trayecto cs (recogidas ++ entregas)]]
  where
    origenes = nub (map origenMudanza ms)
    destinos = nub (map destinoMudanza ms)

buscar :: String -> (a -> String) -> [a] -> Either String a
buscar codigo identificador xs =
  maybe (Left ("No existe: " ++ codigo)) Right (find ((== codigo) . identificador) xs)

-- Uso: ahorroConsolidacion ["m002", "m006"] "c02" datos
-- El calculo compara: suma de recorridos directos mas cortos vs.
-- recojo de todas las cargas y posterior entrega, SIN vuelta a deposito.
ahorroConsolidacion :: [String] -> String -> Datos -> Either String ResumenAhorro
ahorroConsolidacion codigos codigoCam datos = do
  if length codigos < 2 || length (nub codigos) /= length codigos
    then Left "Seleccione al menos dos codigos distintos"
    else Right ()
  camion <- buscar codigoCam codigoCamion (camiones datos)
  if estadoCamion camion == "disponible"
    then Right () else Left "El camion no esta disponible"
  solicitudes <- mapM (\codigo -> buscar codigo codigoMudanza (mudanzas datos)) codigos
  if all ((== "pendiente") . estadoMudanza) solicitudes
    then Right () else Left "Todos los servicios deben estar pendientes"
  if sum (map cargaM3 solicitudes) <= capacidadM3 camion
    then Right () else Left "La carga total excede la capacidad del camion"
  zonas <- mapM (\m -> zonaPunto <$> buscar (origenMudanza m) codigoPunto (puntos datos)) solicitudes
  if length (nub zonas) == 1
    then Right () else Left "Las mudanzas deben partir de la misma zona"
  separados <- mapM (\m ->
    maybe (Left ("No existe ruta para " ++ codigoMudanza m)) Right
      (distanciaMinima (conexiones datos) (origenMudanza m) (destinoMudanza m))) solicitudes
  juntos <- maybe (Left "No existe una ruta consolidada") Right
    (kmConsolidados (conexiones datos) solicitudes)
  let kmSep = sum separados
      tarifa = tarifaKm camion
      precioSep = kmSep * tarifa
      precioJunto = juntos * tarifa
  Right $ ResumenAhorro codigos codigoCam kmSep juntos
    precioSep precioJunto (precioSep - precioJunto)
