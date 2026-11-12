-- FA - Integrante 1: 4.2.2 Definicion de tipos.
module Tipos where

data Punto = Punto
  { codigoPunto :: String
  , nombrePunto :: String
  , tipoPunto :: String
  , zonaPunto :: String
  } deriving (Show, Eq)

data Conexion = Conexion
  { origenConexion :: String
  , destinoConexion :: String
  , distanciaKm :: Double
  , tiempoMin :: Int
  , estadoConexion :: String
  } deriving (Show, Eq)

data Camion = Camion
  { codigoCamion :: String
  , tipoCamion :: String
  , capacidadM3 :: Double
  , tarifaKm :: Double
  , estadoCamion :: String
  } deriving (Show, Eq)

data Mudanza = Mudanza
  { codigoMudanza :: String
  , origenMudanza :: String
  , destinoMudanza :: String
  , cargaM3 :: Double
  , prioridadMudanza :: String
  , estadoMudanza :: String
  , camionAsignado :: Maybe String
  , fechaMudanza :: Maybe String
  , franjaMudanza :: Maybe String
  } deriving (Show, Eq)

data Datos = Datos
  { puntos :: [Punto]
  , conexiones :: [Conexion]
  , camiones :: [Camion]
  , mudanzas :: [Mudanza]
  } deriving (Show, Eq)
