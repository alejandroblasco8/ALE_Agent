import sys
import pandas as pd
import matplotlib.pyplot as plt

# === CONFIGURACION ===

# Celdas de RAM a ignorar porque no aportan información util
ram_ignore = set([

])

# === PARAMETROS DE ENTRADA ===

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

ram_data = df.iloc[:, :-1]  # Quitar la ultima columna (acción)

# === CALCULAMOS LA VARIABILIDAD ===

diffs = ram_data.diff().fillna(0).abs()
change_counts = (diffs != 0).sum()

# Eliminar celdas que no aportan informacion util
for i in ram_ignore:
    if i in change_counts:
        change_counts[i] = 0

# === SELECCION Y GUARDADO ===

top_cells = change_counts.sort_values(ascending=False).head(num_cells)
print(f"\nTop {num_cells} celdas más variables (ignorando {sorted(ram_ignore)}):\n")
print(top_cells)

# Guardar resultados
top_cells.to_csv("top_ram_cells.csv", header=["change_count"])
print("\nArchivo 'top_ram_cells.csv' guardado.")

# === VISUALIZACION ===

plt.figure(figsize=(14, 5))
plt.bar(range(128), change_counts)
plt.title("Heatmap de frecuencia de cambio en celdas de RAM")
plt.xlabel("Dirección RAM (0–127)")
plt.ylabel("Cambios detectados")
plt.axhline(y=top_cells.iloc[-1], color='r', linestyle='--', label="Corte top")
plt.legend()
plt.tight_layout()
plt.savefig("ram_heatmap.png")
plt.show()
