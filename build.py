import os
import platform
import subprocess
import sys

def print_colored(text, color):
    """Stampa testo colorato, supportando sia Windows che Unix."""
    colors = {
        'green': '\033[92m',
        'red': '\033[91m',
        'reset': '\033[0m'
    }
    
    # Su Windows, prima abilita il supporto ANSI
    if platform.system() == 'Windows':
        os.system('color')
    
    print(f"{colors[color]}{text}{colors['reset']}")

def main():
    """Funzione principale che gestisce il processo di build."""
    system = platform.system()
    print_colored(f"Rilevato sistema operativo: {system}", 'green')
    
    # Crea la directory build se non esiste
    if not os.path.exists('build'):
        print_colored("Creando la directory build...", 'green')
        os.makedirs('build')
    
    # Entra nella directory build
    os.chdir('build')
    
    try:
        # Configura CMake in base al sistema operativo
        print_colored("Configurando CMake...", 'green')
        if system == 'Windows':
            subprocess.run(['cmake', '..', '-G', 'MinGW Makefiles'], check=True)
        else:
            subprocess.run(['cmake', '..'], check=True)
        
        # Compila il progetto
        print_colored("Compilando il progetto...", 'green')
        subprocess.run(['cmake', '--build', '.'], check=True)
        
        print_colored("Build completata con successo!", 'green')
        exe_path = 'chat_client.exe' if system == 'Windows' else 'chat_client'
        print_colored(f"Puoi trovare l'eseguibile in: build/{exe_path}", 'green')
        
    except subprocess.CalledProcessError as e:
        print_colored(f"Errore durante la build: {str(e)}", 'red')
        sys.exit(1)
    except Exception as e:
        print_colored(f"Errore inaspettato: {str(e)}", 'red')
        sys.exit(1)
    
    # Torna alla directory originale
    os.chdir('..')

if __name__ == '__main__':
    main()