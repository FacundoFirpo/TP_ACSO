def load_words(path):
    with open(path, "r") as f:
        return [line.strip() for line in f if line.strip()]

def simulate_cuenta(target, words, lo, hi, attempts):
    if attempts[0] >= 100:
        return -1
    if lo > hi:
        return -1
    mid = (lo + hi) // 2
    attempts[0] += 1
    word = words[mid]
    if target == word:
        return mid
    elif target < word:
        return simulate_cuenta(target, words, lo, mid - 1, attempts)
    else:
        return simulate_cuenta(target, words, mid + 1, hi, attempts)

if __name__ == "__main__":
    palabras = load_words("/Users/facu/Documents/UDESA/acso/TP_ACSO/TP2-x86_64/bomb47/palabras.txt")
    palabras.sort()

    for word in palabras:
        attempts = [0]
        idx = simulate_cuenta(word, palabras, 0, len(palabras) - 1, attempts)
        if idx != -1 and attempts[0] > 6:
            print(f"{attempts[0]} {word}")  # esta es la línea que tenés que pasar a la bomba
            break
