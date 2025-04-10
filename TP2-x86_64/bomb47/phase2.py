def misterio(edi):
    if edi < 0:
        return
    # Logic to avoid bomb explosion
    return True

def solve_phase_2():
    for num1 in range(-1000,1000):  # Adjust range as needed
        if num1 == 0:  # Skip zero
            continue
        for num2 in range(-1000,1000):  # Adjust range as needed
            if num2 == 0:  # Skip zero
                continue
            for num3 in range(-1000,1000):  # Adjust range as needed
                if num3 == 0:  # Skip zero
                    continue
                # Convert numbers to integers
                rbp = int(num1)
                rbx = int(num2)
                r12 = int(num3)

                # Perform operations as seen in phase_2
                ebx = rbx ^ rbp
                ebx >>= 1

                # Make r12 negative
                r12 = -abs(r12)

                if ebx == r12:
                    if misterio(r12):
                        print(f"Combination found: {num1}, {num2}, {num3}")
                        return

    print("No combination found.")

solve_phase_2()