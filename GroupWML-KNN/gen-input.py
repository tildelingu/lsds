from random import randint
import sys

loc_output = "knn-random-"+sys.argv[1]+"-"+sys.argv[2]+".input"
NUM_POINT = int(sys.argv[1]) + int(sys.argv[2])
NUM_CLASS = 3
NUM_DIM = 4

with open(loc_output, "w") as f:
    for cntline in xrange(NUM_POINT):
        for i in xrange(NUM_DIM):
            f.write(str(randint(0,99))+" ")
        f.write(str(randint(0,NUM_CLASS-1))+"\n")
