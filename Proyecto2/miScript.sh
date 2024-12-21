#!/bin/bash

read -p "Ingresa el PID del proceso a monitorear: " PID

if ! [ -d /proc/$PID ]; then
    echo "PID inválido. El proceso no existe."
    exit 1
fi

# Bucle para monitorear
while true; do
    clear
    echo "Presiona 'x' para salir del monitoreo."
    echo "Monitoreando proceso con PID: $PID"
    echo "Mostrando VmSize y VmRSS cada 3 segundos."

    echo "---------------------------------"
    echo "Timestamp: $(date)"
    echo "$PID" > /proc/watch_202010833
    
    cat /proc/watch_202010833

    # Espera 3 segundos mientras verifica si se presiona 'x'
    read -t 2 -n 1 key
    if [[ $key == "x" ]]; then
        echo "Monitoreo terminado."
        break
    fi
done
