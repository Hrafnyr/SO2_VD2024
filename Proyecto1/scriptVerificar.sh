#!/bin/bash

# Cambiar al directorio donde está el script
cd "$(dirname "$0")"

archivos=(
    "/proc/cpu_usage"
    "/proc/memory_stats"
    "/proc/myDiskInfo_202010833"
)

# Ruta al archivo .exe que deseas ejecutar
archivo_exe="./getSyscalls"

# Ejecutar el archivo .exe
if [ -f "$archivo_exe" ]; then
    echo "Ejecutando $archivo_exe..."
    ./$archivo_exe  # Ejecutar directamente si es un binario de Linux
else
    echo "Error: El archivo .exe no se encuentra en la ruta especificada."
fi

# Mostrar el contenido de los archivos con 'cat'
for archivo in "${archivos[@]}"
do
    echo "Mostrando contenido de $archivo..."
    cat "$archivo"
    echo "--------------------------"
done
