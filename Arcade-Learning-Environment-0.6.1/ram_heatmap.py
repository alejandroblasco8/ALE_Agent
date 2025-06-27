import sys
import pandas as pd
import matplotlib.pyplot as plt

# === CONFIGURACIÓN ===

# Celdas de RAM a ignorar porque no aportan información útil
ram_ignore = set([
    0,1,2,3,4,5,                    #puntuacion
    29,30,31,33,34,35,64,66,    #animaciones
    28,                         #bala jugador
    50,
    63,                         #cuando estamos muertos
    69,70,71,                    #altura enemigos
    90,
    126
])

# Celdas de RAM que sí o sí quieres mantener aunque no estén en el top
ram_keep = set([
    16,17,18,19,                 #posiciones jugador y enemigos
    21,                             #altura bala jugador
    37,38,39,40,41,42,43,44,45,46 #altura y animacion bala enemigos
])

# === PARÁMETROS DE ENTRADA ===

if len(sys.argv) < 2:
    print("Usage: python ram_heatmap.py <csv_file> [num_top_cells]")
    sys.exit(1)

csv_file = sys.argv[1]
num_cells = int(sys.argv[2]) if len(sys.argv) > 2 else 16

# === CARGA DE DATOS ===

try:
    df = pd.read_csv(csv_file, header=None)
except Exception as e:
    print(f"Error al abrir el archivo '{csv_file}': {e}")
    sys.exit(1)

ram_data = df.iloc[:, :-1]  # quitar columna de acción

# === CÁLCULO DE VARIABILIDAD ===

diffs = ram_data.diff().fillna(0).abs()
change_counts = (diffs != 0).sum()

# === FILTRADO DE CELDAS IGNORADAS ===

# Eliminar las celdas que están en ram_ignore
for i in ram_ignore:
    if i in change_counts:
        change_counts[i] = 0

# === SELECCIÓN FINAL ===

# Excluir ram_ignore y ram_keep del ranking inicial
excluded = ram_ignore.union(ram_keep)
filtered_counts = change_counts.drop(labels=excluded, errors='ignore')

# Seleccionar top N más variables
top_from_variability = filtered_counts.sort_values(ascending=False).head(num_cells)

# Añadir manualmente las celdas útiles (ram_keep)
keep_cells = change_counts.loc[list(ram_keep)]
final_cells = pd.concat([top_from_variability, keep_cells])

# Eliminar duplicados y ordenar por índice
final_cells = final_cells[~final_cells.index.duplicated(keep='first')].sort_index()

# === SALIDA ===

print(f"\nTop {num_cells} celdas más variables + {len(ram_keep)} forzadas (ignorando {sorted(ram_ignore)}):\n")
print(final_cells)

# Guardar en CSV
final_cells.to_csv("top_ram_cells.csv", header=["change_count"])
print("\nArchivo 'top_ram_cells.csv' guardado.")

# === VISUALIZACIÓN ===

plt.figure(figsize=(14, 5))
plt.bar(range(128), change_counts)
plt.title("Heatmap de frecuencia de cambio en celdas de RAM")
plt.xlabel("Dirección RAM (0–127)")
plt.ylabel("Cambios detectados")
plt.axhline(y=top_from_variability.iloc[-1], color='r', linestyle='--', label="Corte top")
plt.legend()
plt.tight_layout()
plt.savefig("ram_heatmap.png")
plt.show()
