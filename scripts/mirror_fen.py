def mirror_fen(fen):
    parts = fen.split(' ')
    rows = parts[0].split('/')
    mirrored = '/'.join(rows[::-1])
    mirrored = mirrored.swapcase()

    # Swap the moving color
    parts[1] = 'w' if parts[1] == 'b' else 'b'

    # Swap the castling rights
    parts[2] = parts[2].swapcase()

    return ' '.join([mirrored] + parts[1:])


fen = input("FEN: ")
print()
print("Output:")
print(mirror_fen(fen))
print()
