import os

def convert_to_vw():
    with open('lccrf.original.dat', 'r') as f1, open('lccrf.vw.dat', 'w') as f2:
        for line in f1:
            line = line.strip()
            if len(line) == 0:
                print(line, file=f2)
                continue

            fields = line.split()
            label = fields[0]
            print(f'{label} 1.0 | {" ".join(i + ":1.0" for i in fields[1:])}', file=f2)

def convert_to_internal():
    with open('train.lccrf.converted.dat', 'r') as f1, open('lccrf.internal.dat', 'w') as f2:
        for line in f1:
            line = line.strip()
            if len(line) == 0:
                print(line, file=f2)
                continue

            fields = line.split()
            label = fields[0]
            print(f'{label} 1.0 unk {" ".join("|1 " + i for i in fields[1:])}', file=f2)


if __name__ == "__main__":
    convert_to_vw()