import numpy as np
from pydub import AudioSegment
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import os


def get_waveform(file_path, N):
    """
    Obtient le signal audio du fichier wave et le divise en sous-tableaux de taille N.
    """
    waveform = AudioSegment.from_wav(file_path)
    
    global num_samples
    num_samples = N if N <= len(waveform) else len(waveform)
    waveform = waveform[:num_samples]
    waveform_samples = np.array(waveform.get_array_of_samples()).astype(float)
    waveform_samples=normalize_array(waveform_samples)
    num_periods = len(waveform_samples) // N
    waveform_samples = waveform_samples[:num_periods * N]
    waveform_samples = waveform_samples.reshape((num_periods, N))
    return waveform_samples

def normalize_array(waveform):
    """
    Normalise les valeurs d'une forme d'onde audio.
    """
    waveform_samples = np.array(waveform.get_array_of_samples())
    arr = (waveform_samples - np.mean(waveform_samples)) / np.std(waveform_samples)
    max_val = np.max(np.abs(arr.flatten()))
    if max_val > 0:
        arr = arr / max_val
    return arr


def normalize_array(arr):
    max_val = np.max(np.abs(arr.flatten()))
    norm_arr = arr.astype(np.float32) / max_val
    return norm_arr
      

def autocorr_fft(y):
    """Calcul de l'autocorrélation de y en utilisant la FFT"""
    
    # Normalisation de y
    y = y.astype(float) - np.mean(y.astype(float))    # Calcul de la taille de la FFT
    n = len(y)
    # Calcul de la FFT de y
    y_fft = np.fft.fft(y, n=n*2)
    # Calcul de la densité spectrale de puissance de y
    psd = y_fft * np.conj(y_fft) / n
    # Calcul de la corrélation croisée de y avec elle-même
    autocorr = np.real(np.fft.ifft(psd))
    # Normalisation de l'autocorrélation
    autocorr = autocorr[:n] / (np.arange(n, 0, -1))
    return autocorr

def get_pseudo_period(y, fs):
  # Obtenir l'autocorrélation du signal audio

  r = autocorr_fft(y)

  # Ignorer les valeurs négatives de lags
  lags = np.arange(-len(y) + 1, len(y))
  pos_lags = lags[lags >= 0]
  pos_r = r[pos_lags]

  # Trouver le premier maximum après le pic principal
  max_index = np.argmax(pos_r[1:]) + 1
  T_index = pos_lags[max_index]

  # Convertir l'index en temps

  T = T_index / fs
  return T

def get_waveform_array(n):
  # Remplir le tableau des waveform avec les waveforms des signaux audio  
  # Initialiser la liste waveform_list
  waveform_list = []

  file_count = 0  # compteur de fichiers
  for filename in os.listdir(folder_path):
    if file_count >= n:
      break  # arrêter la boucle si le nombre de fichiers atteint n

    if filename.endswith('.wav'):
      file_path = os.path.join(folder_path, filename)
        # Affichage du nom de l'élément (fichier ou dossier)
      print(f"traitement du fichier {file_count}")
        # traitement du fichier
      file=AudioSegment.from_wav(file_path)
      y = np.array(file.get_array_of_samples())
      pseudo_period = get_pseudo_period(y, 44100)
      if pseudo_period<limit_period:
        print(f"la valeur de pseudo période est trop faible, traitement du fichier suivant")
        file_count += 1
        continue
      else:
          print(f"la valeur de pseudo période est de {pseudo_period}")
          waveform = get_waveform(file_path, int(1/pseudo_period))
          waveform_list.append(waveform)  
      file_count += 1  # incrémenter le compteur de fichiers traités
        
      
  return waveform_list


def figure_diplay():

  for i in range(len(waveform_array)):
    fig = plt.figure(i+1)
    ax = fig.add_subplot(111)
    ax.plot(waveform_array[i])
    ax.set_title(f"fichier {i+1}")
    ax.set_xlabel("Time (samples)")
    ax.set_ylabel("Amplitude")
    plt.show()

#ecrit le tableau 3D waveform_array en fichier c 
def write_waveform_c_array(waveform_array, file_path):
    
  num_files = len(waveform_array)
  num_periods = len(waveform_array[0])
  num_samples = len(waveform_array[0][0])    

  with open(file_path, mode='w') as file:
        file.write('#include <stdlib.h>\n\n')
        file.write(f'float waveform_array[{num_files}][{num_periods}][{num_samples}] = {{\n')
        
        for file_idx in range(num_files):
            file.write('{\n')
            for period_idx in range(num_periods):
                file.write('{')
                for sample_idx in range(num_samples):
                    file.write(f'{waveform_array[file_idx][period_idx][sample_idx]:.6f}')
                    if sample_idx < num_samples - 1:
                        file.write(', ')
                file.write('}')
                if period_idx < num_periods - 1:
                    file.write(',')
                file.write('\n')
            file.write('}')
            if file_idx < num_files - 1:
                file.write(',')
            file.write('\n')
            
        file.write('};')

"input à rentrer sur le programme"
folder_path = '/home/evrard/Documents/ENSEA/2A/Option_Elec&Son/Projet_TableOnde/Ressource fichier'
#n =0   # nombre de fichiers à traiter
limit_period=0.001
data_path = "waveform_array.csv"

"input à rentrer sur la console"
n = int(input("Entrez le nombre de fichiers : "))
#folder_path = int(input("Entrez le chemin de dossier : "))
#limit_period = int(input("Entrez la pseudo periode limite: "))
#data_path = int(input("Entrez le chemin de dossier : "))

"Obtenir le tableau des wave formes"
waveform_array = get_waveform_array(n)
figure_diplay()


"creation du fichier.c"
write_waveform_c_array(waveform_array, 'waveform_array.c')