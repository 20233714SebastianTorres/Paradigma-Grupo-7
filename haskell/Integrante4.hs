-- Integrante 4: Walter Melendez
-- Funcionalidades base 4.2.6 (reporte de pedidos), 4.2.7 (resultado esperado)
-- y funcionalidad adicional "Simulacion de cierres viales".
module Integrante4
  ( -- 4.2.6
    contarPorEstado
  , cargaPorCamion
  , pendientesPorPrioridad
  , reportePedidos
    -- 4.2.7
  , resumenPedido
    -- Funcionalidad adicional
  , ComparacionRutas(..)
  , incrementoKm, incrementoMin, incrementoCosto
  , compararCostoRutas
  , simularCierre
  , ImpactoCierre(..)
  , impactoCierre
  , reporteImpacto
  ) where

import Tipos
import Integrante3 (rutasPosibles, costoRuta, distanciaYTiempoRuta, rutaMasCorta)
import Data.List (find, intercalate, sortBy)
import Data.Ord (comparing)
import Text.Printf (printf)

-- ---------------------------------------------------------------------------
-- 4.2.6 Reporte de pedidos
-- ---------------------------------------------------------------------------

-- Cantidad de pedidos en cada estado (foldr recorre la lista una vez por estado).
contarPorEstado :: [Mudanza] -> [(String, Int)]
contarPorEstado ms = map contar ["pendiente", "asignado", "entregado"]
  where
    contar estado = (estado, foldr (\m n -> if estadoMudanza m == estado then n + 1 else n) 0 ms)

-- Carga total (m3) de los pedidos asignados a cada camion; los camiones sin
-- pedidos asignados aparecen con 0.
cargaPorCamion :: [Camion] -> [Mudanza] -> [(String, Double)]
cargaPorCamion cs ms = map cargaDe cs
  where
    asignadas = filter ((== "asignado") . estadoMudanza) ms
    cargaDe c = ( codigoCamion c
                , foldr ((+) . cargaM3) 0
                    (filter ((== Just (codigoCamion c)) . camionAsignado) asignadas) )

-- Orden de las prioridades: alta primero.
rangoPrioridad :: String -> Int
rangoPrioridad "alta"  = 0
rangoPrioridad "media" = 1
rangoPrioridad "baja"  = 2
rangoPrioridad _       = 3

-- Pedidos pendientes de mayor a menor prioridad. sortBy es estable: dos pedidos
-- con la misma prioridad conservan el orden del CSV.
pendientesPorPrioridad :: [Mudanza] -> [Mudanza]
pendientesPorPrioridad =
  sortBy (comparing (rangoPrioridad . prioridadMudanza)) . filter ((== "pendiente") . estadoMudanza)

-- "m001 (alta), m006 (alta), m002 (media)"
listaPrioridades :: [Mudanza] -> String
listaPrioridades ms =
  intercalate ", " [codigoMudanza m ++ " (" ++ prioridadMudanza m ++ ")" | m <- pendientesPorPrioridad ms]

-- Texto del reporte 4.2.6, listo para imprimir.
reportePedidos :: Datos -> [String]
reportePedidos datos =
  [ "Pedidos por estado: " ++ intercalate " | " [e ++ ": " ++ show n | (e, n) <- contarPorEstado ms] ]
  ++ [ printf "Carga asignada a %s: %.1f m3" c q | (c, q) <- cargaPorCamion (camiones datos) ms ]
  ++ [ "Pendientes por prioridad: " ++ listaPrioridades ms ]
  where
    ms = mudanzas datos

-- ---------------------------------------------------------------------------
-- 4.2.7 Resultado esperado
-- ---------------------------------------------------------------------------

buscar :: String -> String -> (a -> String) -> [a] -> Either String a
buscar tipo cod campo lista =
  maybe (Left ("No existe " ++ tipo ++ " " ++ cod)) Right (find ((== cod) . campo) lista)

flecha :: [String] -> String
flecha = intercalate " -> "

-- Resumen de un pedido: cuantas rutas tiene, el costo de la mas corta con el
-- camion indicado y los pendientes ordenados por prioridad.
resumenPedido :: String -> String -> Datos -> Either String [String]
resumenPedido codPedido codCamion datos = do
  m   <- buscar "el pedido" codPedido codigoMudanza (mudanzas datos)
  cam <- buscar "el camion" codCamion codigoCamion (camiones datos)
  let cs = conexiones datos
      (o, d) = (origenMudanza m, destinoMudanza m)
  (ruta, _) <- maybe (Left ("No hay ruta habilitada entre " ++ o ++ " y " ++ d)) Right
                     (rutaMasCorta o d cs)
  costo <- costoRuta ruta cam cs
  return
    [ "Pedido: " ++ codPedido ++ " (" ++ o ++ " -> " ++ d ++ ")"
    , "Rutas posibles: " ++ show (length (rutasPosibles o d cs))
    , printf "Costo de ruta %s (%s): S/ %.2f" (flecha ruta) (tipoCamion cam) costo
    , "Pedidos pendientes por prioridad: " ++ listaPrioridades (mudanzas datos)
    ]

-- ---------------------------------------------------------------------------
-- FA - Autor: Walter Melendez
-- Simulacion de cierres viales y evaluacion del impacto en las mudanzas.
-- Como los datos en Haskell son inmutables, simular un cierre es crear una
-- copia modificada: los datos originales nunca cambian.
-- ---------------------------------------------------------------------------

data ComparacionRutas = ComparacionRutas
  { rutaOriginal     :: [String]
  , rutaAlternativa  :: [String]
  , kmOriginal       :: Double
  , kmAlternativa    :: Double
  , minOriginal      :: Int
  , minAlternativa   :: Int
  , costoOriginal    :: Double
  , costoAlternativa :: Double
  } deriving (Show, Eq)

incrementoKm :: ComparacionRutas -> Double
incrementoKm c = kmAlternativa c - kmOriginal c

incrementoMin :: ComparacionRutas -> Int
incrementoMin c = minAlternativa c - minOriginal c

incrementoCosto :: ComparacionRutas -> Double
incrementoCosto c = costoAlternativa c - costoOriginal c

-- Diferencias de distancia, tiempo y costo entre dos rutas con un camion.
-- Uso: compararCostoRutas "c02" rutaOriginal rutaAlternativa datos
compararCostoRutas :: String -> [String] -> [String] -> Datos -> Either String ComparacionRutas
compararCostoRutas codCamion original alterna datos = do
  cam <- buscar "el camion" codCamion codigoCamion (camiones datos)
  let cs = conexiones datos
  (kmO, minO) <- distanciaYTiempoRuta original cs
  (kmA, minA) <- distanciaYTiempoRuta alterna cs
  return ComparacionRutas
    { rutaOriginal = original, rutaAlternativa = alterna
    , kmOriginal = kmO, kmAlternativa = kmA
    , minOriginal = minO, minAlternativa = minA
    , costoOriginal = kmO * tarifaKm cam, costoAlternativa = kmA * tarifaKm cam
    }

-- El tramo a-b, en cualquier sentido.
esTramo :: String -> String -> Conexion -> Bool
esTramo a b c = (origenConexion c, destinoConexion c) `elem` [(a, b), (b, a)]

-- Copia de los datos con el tramo a-b cerrado.
simularCierre :: String -> String -> Datos -> Datos
simularCierre a b datos = datos { conexiones = map cerrar (conexiones datos) }
  where
    cerrar c = if esTramo a b c then c { estadoConexion = "cerrada" } else c

-- La ruta pasa por el tramo a-b (se revisan los pares de puntos consecutivos).
usaTramo :: String -> String -> [String] -> Bool
usaTramo a b ruta = any (\(x, y) -> (x, y) `elem` [(a, b), (b, a)]) (zip ruta (drop 1 ruta))

-- Pedido afectado por el cierre: su ruta actual y, si existe, la comparacion
-- con la ruta alternativa (Nothing = el pedido queda sin ruta).
data ImpactoCierre = ImpactoCierre
  { pedidoAfectado :: String
  , rutaActual     :: [String]
  , alternativa    :: Maybe ComparacionRutas
  } deriving (Show, Eq)

-- Pedidos pendientes cuya ruta mas corta pasa por el tramo a-b.
impactoCierre :: String -> String -> String -> Datos -> Either String [ImpactoCierre]
impactoCierre a b codCamion datos
  | not (any (\c -> esTramo a b c && estadoConexion c == "habilitada") (conexiones datos)) =
      Left ("El tramo " ++ a ++ " - " ++ b ++ " no existe o ya esta cerrado.")
  | otherwise = mapM evaluar afectados
  where
    cs = conexiones datos
    csCerrado = conexiones (simularCierre a b datos)
    afectados =
      [ (m, ruta)
      | m <- filter ((== "pendiente") . estadoMudanza) (mudanzas datos)
      , Just (ruta, _) <- [rutaMasCorta (origenMudanza m) (destinoMudanza m) cs]
      , usaTramo a b ruta
      ]
    evaluar (m, ruta) =
      case rutaMasCorta (origenMudanza m) (destinoMudanza m) csCerrado of
        Nothing       -> Right (ImpactoCierre (codigoMudanza m) ruta Nothing)
        Just (alt, _) -> ImpactoCierre (codigoMudanza m) ruta . Just
                           <$> compararCostoRutas codCamion ruta alt datos

-- Texto del impacto, listo para imprimir.
reporteImpacto :: [ImpactoCierre] -> [String]
reporteImpacto [] = ["Ningun pedido pendiente usa ese tramo."]
reporteImpacto is = concatMap lineas is
  where
    lineas i = case alternativa i of
      Nothing ->
        [ "Pedido " ++ pedidoAfectado i ++ " afectado: " ++ flecha (rutaActual i)
        , "  Sin ruta alternativa: el pedido queda inaccesible." ]
      Just c ->
        [ "Pedido " ++ pedidoAfectado i ++ " afectado: " ++ flecha (rutaActual i)
        , "  Alternativa: " ++ flecha (rutaAlternativa c)
        , printf "  Distancia: %.1f km -> %.1f km (+%.1f km)" (kmOriginal c) (kmAlternativa c) (incrementoKm c)
        , printf "  Tiempo: %d min -> %d min (+%d min)" (minOriginal c) (minAlternativa c) (incrementoMin c)
        , printf "  Costo: S/ %.2f -> S/ %.2f (incremento S/ %.2f)" (costoOriginal c) (costoAlternativa c) (incrementoCosto c)
        ]
