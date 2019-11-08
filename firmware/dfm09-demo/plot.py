import matplotlib.pyplot as plt
import numpy as np
import math as m
import scipy
from  scipy import ndimage

x = np.loadtxt('12bit_symbols.csv')
xf = np.loadtxt('12bit_symbols_f.csv')

#x = x - 2**11
#plt.plot(x, 'o-')


#cs = np.cumsum(x)
#plt.plot(cs)

def moving_average(a, n=3) :
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n


def generate_kernel(taps, sigma=1.0, scale=1):
        if taps % 2 == 0:
            raise Exception('taps should be odd')
        s = 2.0 * sigma * sigma
        r = 2.0 * sigma * sigma
        summm = 0.0
        kernel = np.zeros(taps)
        # Generate gaussian kernel
        for y in range(0, taps, 1):
            r = m.sqrt((y-m.floor(taps/2))*(y-m.floor(taps/2)))
            kernel[y] = ( m.exp( -(r*r)/s ) ) / (m.pi * s)
            summm += kernel[y]
            print(kernel[y])

        print("summ = {}".format(summm))
        print("m.floor(taps/2) = {}".format(m.floor(taps/2)))

        # Normalize kernel
        for y in range(0, taps, 1):
            kernel[y] = float( kernel[y] / summm * float(scale) )
        print(kernel)
        print("new sum {}".format(sum(kernel)))

        return (kernel, summm)


#xm = moving_average(x, n=5)
#plt.plot(xm, 'x-')

#w = np.repeat(1.0, 19)/19
#xxm = np.convolve(x, w, 'valid')
#plt.plot(xxm, '.-')



sigma = 2
xg =ndimage.filters.gaussian_filter1d(x, sigma)
plt.plot(xg, label="gaussian_filter1d")


taps = 17
(kernel, summ) = generate_kernel(taps, sigma, 2**12)
print("kernel for c code = {}".format([int(i) for i in kernel]))

(kernel, summ) = generate_kernel(taps, sigma, 1)
print("kernel for python code = {}".format(kernel))
#Python loop for passing filter
xc = np.zeros(len(x))
for i in range(len(x)-taps):
    for j in range(0, taps, 1):
        xc[i] = xc[i] + kernel[j] * x[i+j]
    #print(i, xc[i])

plt.plot(xc, '+k-', label="python loop")
plt.plot(xf, '+r-', label="c loop")
plt.legend()
plt.show()
