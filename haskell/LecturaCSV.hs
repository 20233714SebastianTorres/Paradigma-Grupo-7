-- FA - Integrante 1: 4.2.1 Lectura de los archivos CSV.
module LecturaCSV
  ( leerDatos, leerPuntos, leerConexiones, leerCamiones, leerMudanzas
  ) where

import Tipos
import Control.Exception (IOException, evaluate, try)
import Data.Char (toLower)
import Text.Read (readMaybe)
import System.FilePath ((</>))

-- Admite comas y comillas escapadas dentro de campos CSV. Los registros
-- de este proyecto ocupan una linea por registro (sin saltos internos).
camposCSV :: String -> Either String [String]
camposCSV = avanzar False [] []
  where
    avanzar True _ _ [] = Left "comillas sin cerrar"
    avanzar False actual acumulados [] =
      Right (reverse (reverse actual : acumulados))
    avanzar enComillas actual acumulados ('"':'"':resto)
      | enComillas = avanzar True ('"':actual) acumulados resto
    avanzar enComillas actual acumulados ('"':resto) =
      avanzar (not enComillas) actual acumulados resto
    avanzar False actual acumulados (',':resto) =
      avanzar False [] (reverse actual : acumulados) resto
    avanzar enComillas actual acumulados (c:resto) =
      avanzar enComillas (c:actual) acumulados resto

leerArchivo :: FilePath -> ([String] -> Either String a) -> IO (Either String [a])
leerArchivo ruta construir = do
  contenido <- try (do
      texto <- readFile ruta
      _ <- evaluate (length texto)
      return texto) :: IO (Either IOException String)
  case contenido of
    Left e -> return (Left ("No se pudo leer " ++ ruta ++ ": " ++ show e))
    Right texto -> do
      let ls = filter (not . null) (map quitarCR (lines texto))
      let convertidas = mapM camposCSV ls
      return $ do
        todas <- convertidas
        let sinEncabezado = case todas of
              ((x:_):resto) | map toLower x `elem` ["codigo", "origen", "código"] -> resto
              _ -> todas
        mapM (uncurry revisarFila) (zip [1 :: Int ..] sinEncabezado)
  where
    quitarCR s = if not (null s) && last s == '\r' then init s else s
    revisarFila n fila = case construir fila of
      Left e -> Left (ruta ++ ", registro " ++ show n ++ ": " ++ e)
      Right x -> Right x

numero :: Read a => String -> Either String a
numero s = maybe (Left ("numero invalido: " ++ s)) Right (readMaybe s)

opcional :: String -> Maybe String
opcional "" = Nothing
opcional s  = Just s

interpretarPunto :: [String] -> Either String Punto
interpretarPunto [a,b,c,d] = Right (Punto a b c d)
interpretarPunto _ = Left "se esperaban 4 campos para un punto"

interpretarConexion :: [String] -> Either String Conexion
interpretarConexion [a,b,c,d,e] = do
  km <- numero c
  minutos <- numero d
  Right (Conexion a b km minutos e)
interpretarConexion _ = Left "se esperaban 5 campos para una conexion"

interpretarCamion :: [String] -> Either String Camion
interpretarCamion [a,b,c,d,e] = do
  capacidad <- numero c
  tarifa <- numero d
  Right (Camion a b capacidad tarifa e)
interpretarCamion _ = Left "se esperaban 5 campos para un camion"

interpretarMudanza :: [String] -> Either String Mudanza
interpretarMudanza [a,b,c,d,e,f,g] = do
  carga <- numero d
  Right (Mudanza a b c carga e f (opcional g) Nothing Nothing)
interpretarMudanza [a,b,c,d,e,f,g,h,i] = do
  carga <- numero d
  Right (Mudanza a b c carga e f (opcional g) (opcional h) (opcional i))
interpretarMudanza _ = Left "se esperaban 7 o 9 campos para una mudanza"

leerPuntos :: FilePath -> IO (Either String [Punto])
leerPuntos ruta = leerArchivo ruta interpretarPunto

leerConexiones :: FilePath -> IO (Either String [Conexion])
leerConexiones ruta = leerArchivo ruta interpretarConexion

leerCamiones :: FilePath -> IO (Either String [Camion])
leerCamiones ruta = leerArchivo ruta interpretarCamion

leerMudanzas :: FilePath -> IO (Either String [Mudanza])
leerMudanzas ruta = leerArchivo ruta interpretarMudanza

leerDatos :: FilePath -> IO (Either String Datos)
leerDatos carpeta = do
  ps <- leerPuntos (carpeta </> "puntos.csv")
  cs <- leerConexiones (carpeta </> "conexiones.csv")
  us <- leerCamiones (carpeta </> "unidades.csv")
  ms <- leerMudanzas (carpeta </> "pedidos.csv")
  return $ Datos <$> ps <*> cs <*> us <*> ms
