import subprocess
import sys

# Função para instalar pacotes automaticamente
def install_package(package):
    subprocess.check_call([sys.executable, "-m", "pip", "install", package])

# Tenta importar as bibliotecas necessárias, instalando-as se necessário
try:
    import pandas as pd
except ImportError:
    install_package('pandas')
    import pandas as pd

try:
    import pyarrow
except ImportError:
    install_package('pyarrow')

def csv_to_parquet(csv_file, parquet_file):
    df = pd.read_csv(csv_file)
    df.to_parquet(parquet_file, index=False)

csv_to_parquet('MotorDesempenho.csv', 'MotorDesempenho.parquet')
