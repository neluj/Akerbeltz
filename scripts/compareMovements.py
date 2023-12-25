#!/usr/bin/env python3

import sys

if len(sys.argv) != 2:
    print("Error: debes proporcionar un argumento.")
    sys.exit(1)

fileName = sys.argv[1]

def printLines(lines):
    count = 0
    # Strips the newline character
    for line in lines:
        count += 1
        print("Line{}: {}".format(count, line.strip()))

def moveListNand(moveList1, moveList2):
    first = map(lambda s: s.strip(), moveList1)
    second = map(lambda t: t.strip(), moveList2)

    nand =  list(set(second) - set(first))
    return "\n".join(nand)

def compareMovements():
    #Load movements from file
    file = open(fileName, 'r')
    lines = file.readlines()

    #Split movements 
    split_index = lines.index("NEW\n")

    first_nl = lines[:split_index]
    second_nl = lines[split_index+1:]

    #Compare them and get differences
    first_nand =  moveListNand(first_nl, second_nl)
    second_nand = moveListNand(second_nl, first_nl)

    #Write differences on a new file
    file = open('Nand.txt', 'w')

    file.write("First move list size: {}\n".format(len(first_nl)))
    file.write("First moves not included in second: \n\n")
    file.writelines(first_nand)

    file.write("\n\n\nSecond move list size: {}\n".format(len(second_nl)))
    file.write("Second moves not included in first: \n\n")
    file.writelines(second_nand)

    file.close()

compareMovements()


    

#OUTPUT: sizes per each list
        #List with moves that are not contained in the list but yes in the other list