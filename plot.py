import matplotlib.pyplot as plt

with open("perf/parallel.txt", "r") as f:
    parallel = f.readlines()
    parallel = [float(x.strip()) for x in parallel]

with open("perf/sequential.txt", "r") as f:
    sequential = f.readlines()
    sequential = [float(x.strip()) for x in sequential]

plt.hist(parallel, bins=25, alpha=0.45, range=(0,62), color='red')
plt.hist(sequential, bins=25, alpha=0.45, range=(0,62), color='blue')

plt.title("Computing time")

plt.legend(['Parallel',
            'Sequential'])

plt.show()
