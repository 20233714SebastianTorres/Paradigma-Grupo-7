module Integrante3
  ( vecinos
  , rutasPosibles
  , costoRuta
  , distanciaYTiempoRuta
  , rutaMasCorta
  , CotizacionCamion(..)
  , cotizarServicio
  ) where

import Tipos
import Data.List (find, sortBy)
import Data.Ord (comparing)

-- 4.2.3 Funcion de vecinos
-- Obtiene los puntos conectados directamente a un punto,
-- considerando el doble sentido y solo conexiones habilitadas.
-- Devuelve una tupla: (codigoPuntoVecino, distanciaKm, tiempoMin)
vecinos :: String -> [Conexion] -> [(String, Double, Int)]
vecinos punto cs =
  [ (if origenConexion c == punto then destinoConexion c else origenConexion c, distanciaKm c, tiempoMin c)
  | c <- cs
  , estadoConexion c == "habilitada"
  , distanciaKm c > 0
  , origenConexion c == punto || destinoConexion c == punto
  ]

-- 4.2.4 Rutas posibles
-- Funcion recursiva que genera todas las rutas entre origen y destino sin ciclos.
rutasPosibles :: String -> String -> [Conexion] -> [[String]]
rutasPosibles origen destino cs = buscarRutas [origen] origen
  where
    buscarRutas visitados actual
      | actual == destino = [[actual]]
      | otherwise =
          [ actual : resto
          | (vec, _, _) <- vecinos actual cs
          , vec `notElem` visitados
          , resto <- buscarRutas (vec : visitados) vec
          ]

-- 4.2.5 Costo de una ruta
-- Dada una ruta (lista de puntos), calcula la distancia y tiempo total.
-- Si algun tramo no existe o esta cerrado, devuelve Left con el detalle.
distanciaYTiempoRuta :: [String] -> [Conexion] -> Either String (Double, Int)
distanciaYTiempoRuta [] _ = Right (0.0, 0)
distanciaYTiempoRuta [_] _ = Right (0.0, 0)
distanciaYTiempoRuta (a:b:resto) cs = do
  (d, t) <- buscarTramo a b cs
  (dResto, tResto) <- distanciaYTiempoRuta (b:resto) cs
  return (d + dResto, t + tResto)
  where
    buscarTramo x y conexionesLista =
      case find (\c -> estadoConexion c == "habilitada" &&
                       ((origenConexion c == x && destinoConexion c == y) ||
                        (origenConexion c == y && destinoConexion c == x))) conexionesLista of
        Just c  -> Right (distanciaKm c, tiempoMin c)
        Nothing -> Left ("El tramo " ++ x ++ " -> " ++ y ++ " no existe o esta cerrado.")

-- Costo de la ruta para un camion dado: distancia * tarifaKm
costoRuta :: [String] -> Camion -> [Conexion] -> Either String Double
costoRuta ruta camion cs = do
  (dist, _) <- distanciaYTiempoRuta ruta cs
  return (dist * tarifaKm camion)

-- FA: Cotizacion comparativa y recomendacion de camion
data CotizacionCamion = CotizacionCamion
  { camionCotizado      :: String -- codigo del camion
  , tipoCamionCotizado  :: String
  , rutaSugerida        :: [String]
  , distanciaKmRuta     :: Double
  , costoEstimado       :: Double
  , porcentajeUtilizado :: Double
  , precioPorM3         :: Double
  } deriving (Show, Eq)

buscarElemento :: String -> (a -> String) -> [a] -> Either String a
buscarElemento cod selector lista =
  case find ((== cod) . selector) lista of
    Just elemEncontrado -> Right elemEncontrado
    Nothing             -> Left ("No se encontro el elemento con codigo: " ++ cod)

-- Calcula la distancia de una lista de puntos
distanciaRutaSimple :: [String] -> [Conexion] -> Double
distanciaRutaSimple [] _ = 0.0
distanciaRutaSimple [_] _ = 0.0
distanciaRutaSimple (a:b:resto) cs =
  let d = case find (\c -> estadoConexion c == "habilitada" &&
                           ((origenConexion c == a && destinoConexion c == b) ||
                            (origenConexion c == b && destinoConexion c == a))) cs of
            Just c  -> distanciaKm c
            Nothing -> 999999.0
  in d + distanciaRutaSimple (b:resto) cs

-- Encuentra la ruta mas corta entre todas las posibles
rutaMasCorta :: String -> String -> [Conexion] -> Maybe ([String], Double)
rutaMasCorta orig dest cs =
  let todas = rutasPosibles orig dest cs
  in if null todas
     then Nothing
     else
       let conDist = map (\r -> (r, distanciaRutaSimple r cs)) todas
       in Just (sortBy (comparing snd) conDist !! 0)

-- Solo recibe el codigo de la mudanza y el objeto Datos:
-- Obtiene el origen y destino del pedido, determina la ruta optima automaticamente,
-- y compara los camiones disponibles.
cotizarServicio :: String -> Datos -> Either String [CotizacionCamion]
cotizarServicio codMudanza datos = do
  mud <- buscarElemento codMudanza codigoMudanza (mudanzas datos)
  if estadoMudanza mud /= "pendiente"
    then Left "El servicio de mudanza debe estar en estado 'pendiente'."
    else Right ()
  let cs = conexiones datos
      orig = origenMudanza mud
      dest = destinoMudanza mud
  (mejorRuta, dist) <- case rutaMasCorta orig dest cs of
                         Just (r, d) -> Right (r, d)
                         Nothing     -> Left ("No hay ruta habilitada entre " ++ orig ++ " y " ++ dest)
  let carga = cargaM3 mud
      camsAptos = filter (\c -> estadoCamion c == "disponible" && capacidadM3 c >= carga) (camiones datos)
  if null camsAptos
    then Left "No existen camiones disponibles con capacidad suficiente para este servicio."
    else do
      let cotizaciones = map (calcularCotizacion mejorRuta dist carga) camsAptos
          ordenadas = sortBy (comparing costoEstimado) cotizaciones
      Right ordenadas
  where
    calcularCotizacion r dist carga cam =
      let costo = dist * tarifaKm cam
          porc  = (carga / capacidadM3 cam) * 100.0
          pM3   = if carga > 0 then costo / carga else 0.0
      in CotizacionCamion
          { camionCotizado      = codigoCamion cam
          , tipoCamionCotizado  = tipoCamion cam
          , rutaSugerida        = r
          , distanciaKmRuta     = dist
          , costoEstimado       = costo
          , porcentajeUtilizado = porc
          , precioPorM3         = pM3
          }
