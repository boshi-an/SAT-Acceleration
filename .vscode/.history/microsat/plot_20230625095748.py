import matplotlib.pyplot as plt

with open("perf/parallel.txt", "r") as f:
    parallel = f.readlines()
    parallel = [float(x.strip()) for x in parallel]

with open("perf/serial.txt", "r") as f:
    serial = f.readlines()
    serial = [float(x.strip()) for x in serial]

plt.hist(parallel, bins=25, alpha=0.45, color='red')
plt.hist(serial, bins=25, alpha=0.45, color='blue')

plt.title("Computing time")

plt.legend(['Parallel',
            'Serial'])

plt.show()
