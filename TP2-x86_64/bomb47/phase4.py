from itertools import product

# Array leído desde GDB (array.0)
array = [
    2, 13, 7, 14,
    5, 10, 6, 15,
    1, 12, 3, 4,
    11, 8, 16, 9
]

target = 60  # La suma que espera la bomba
valid_combinations = []

# Probamos todas las combinaciones de 6 números entre 0 y 15 (inclusive)
for combo in product(range(16), repeat=6):
    total = sum(array[i] for i in combo)
    if total == target:
        valid_combinations.append(combo)
        break  # con una sola basta para desactivar la bomba

# Función para convertir índices a caracteres ASCII visibles
def indices_to_string(indices):
    result = ''
    for i in indices:
        for c in range(32, 127):  # caracteres imprimibles ASCII
            if (c & 0xf) == i:
                result += chr(c)
                break
    return result

# Mostrar resultado
if valid_combinations:
    combo = valid_combinations[0]
    result = indices_to_string(combo)
    print(f"String válido: '{result}'  (índices: {combo})")
else:
    print("No se encontró ninguna combinación.")