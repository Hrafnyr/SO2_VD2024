import pandas as pd
import json
from tabulate import tabulate
from termcolor import colored
import subprocess

print("Ingrese un PID (0 para mostrar todos)")
valorPid = input()
if int(valorPid) == 0:

    # Ruta del archivo de procesos (ajústalo según corresponda)
    file_path = '/proc/statsAllProcs'

    # Función para parsear el archivo JSON
    def parse_stats_file(file_path):
        try:
            # Leer el archivo y cargar el contenido JSON
            with open(file_path, 'r') as file:
                data = json.load(file)

            # Extraer la lista de procesos
            procs = data.get('procs', [])

            # Crear un DataFrame con los datos de los procesos
            df = pd.DataFrame(procs)

            return df, data.get('dataGlobal', {})
        except FileNotFoundError:
            print(f"Error: El archivo {file_path} no existe.")
            return None, None
        except json.JSONDecodeError:
            print(f"Error: No se pudo parsear el archivo {file_path} como JSON.")
            return None, None

    # Llamar a la función para obtener los datos
    df, global_data = parse_stats_file(file_path)

    # Estilización adicional: usar tabulate para la tabla
    if df is not None:
        # Calcular valores en MB y porcentaje de uso para los procesos
        df['vm_size (MB)'] = df['vm_size (KB)'] / 1024
        df['vm_rss (MB)'] = df['vm_rss (KB)'] / 1024
        df['usage (%)'] = (df['vm_rss (KB)'] / df['vm_size (KB)']) * 100

       # Estilizar datos de procesos
        df['oom_score'] = df['oom_score'].apply(lambda x: colored(x, 'red') if x <= 0 else str(x))
        df['pid'] = df['pid'].apply(lambda x: colored(x, 'green'))
        df['name'] = df['name'].apply(lambda x: colored(x, 'cyan'))
        df['vm_size (KB)'] = df['vm_size (KB)'].apply(lambda x: colored(f"{x} KB", 'yellow'))
        df['vm_rss (KB)'] = df['vm_rss (KB)'].apply(lambda x: colored(f"{x} KB", 'magenta'))
        df['vm_size (MB)'] = df['vm_size (MB)'].apply(lambda x: colored(f"{x:.2f} MB", 'yellow'))
        df['vm_rss (MB)'] = df['vm_rss (MB)'].apply(lambda x: colored(f"{x:.2f} MB", 'magenta'))
        df['usage (%)'] = df['usage (%)'].apply(lambda x: colored(f"{x:.2f}%", 'blue'))

        # Usar tabulate para una presentación más bonita
        table = tabulate(df, headers='keys', tablefmt='fancy_grid', showindex=False)

        # Mostrar la tabla estilizada
        print("Datos de Procesos:")
        print(table)

        #Mostrar el resumen global con colores
        print("\nResumen Global:")
        if global_data:
            # Calcular valores en MB para los datos globales
            global_data['total_vm_size (MB)'] = global_data['total_vm_size(KB)'] / 1024
            global_data['total_vm_rss (MB)'] = global_data['total_vm_rss(KB)'] / 1024

            # Convertir el resumen global a un DataFrame para mostrarlo como tabla
            global_df = pd.DataFrame(list(global_data.items()), columns=['Campo', 'Valor'])
            
            # Formatear valores para evitar notación científica
            global_df['Valor'] = global_df['Valor'].apply(
                lambda x: f"{x:,.2f}" if isinstance(x, (int, float)) else x
            )

            # Aplicar color a las filas y columnas
            global_df['Campo'] = global_df['Campo'].apply(lambda x: colored(x, 'cyan', attrs=['bold']))  # Azul y negrita
            global_df['Valor'] = global_df['Valor'].apply(lambda x: colored(x, 'green'))  # Verde
            
            print(tabulate(global_df, headers='keys', tablefmt='fancy_grid', showindex=False))

else:
    # Comando para escribir en el archivo dentro de /proc
    command = f'echo "{valorPid}" | sudo tee /proc/watch_202010833'

    # Ejecutar el comando
    subprocess.run(command, shell=True, check=True)
     # Ruta del archivo de procesos (ajústalo según corresponda)
    file_path = '/proc/watch_202010833'

    # Función para parsear el archivo JSON
    data=None
    with open(file_path, 'r') as file:
        data = json.load(file)

    if data!=None:
        # Convertir el diccionario a un DataFrame
        df = pd.DataFrame(list(data.items()), columns=['Campo', 'Valor'])

        vm_size_kb = int(data['VmSize'].split()[0])  # Extraer solo el número de VmSize
        vm_rss_kb = int(data['VmRSS'].split()[0])   # Extraer solo el número de VmRSS

        # Calcular valores en MB
        vm_size_mb = vm_size_kb / 1024
        vm_rss_mb = vm_rss_kb / 1024

        # Calcular porcentaje de uso
        porcentaje_uso = (vm_rss_kb / vm_size_kb) * 100 if vm_size_kb != 0 else 0

        # Crear nuevo DataFrame con columnas adicionales
        df_expanded = pd.DataFrame({
            'Campo': ['VmSize', 'VmRSS', 'OOM_Score'],
            'Valor (Original)': [f'{vm_size_kb} kB', f'{vm_rss_kb} kB', data['OOM_Score']],
            'Valor (MB)': [f'{vm_size_mb:.2f} MB', f'{vm_rss_mb:.2f} MB', 'N/A'],
            'Porcentaje de Uso (%)': ['N/A', f'{porcentaje_uso:.2f}%', 'N/A']
        })

        df_expanded['Campo'] = df_expanded['Campo'].apply(lambda x: colored(x, 'cyan', attrs=['bold']))
        df_expanded['Valor (Original)'] = df_expanded['Valor (Original)'].apply(lambda x: colored(x, 'yellow'))
        df_expanded['Valor (MB)'] = df_expanded['Valor (MB)'].apply(lambda x: colored(x, 'green'))
        df_expanded['Porcentaje de Uso (%)'] = df_expanded['Porcentaje de Uso (%)'].apply(
            lambda x: colored(x, 'red') if '%' in x and float(x.strip('%')) > 80 else colored(x, 'blue')
        )

        # Mostrar tabla estilizada
        print(tabulate(df_expanded, headers='keys', tablefmt='fancy_grid', showindex=False))
    else:
        print("No se tiene informacion")
