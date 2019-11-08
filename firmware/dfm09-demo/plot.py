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

xm = moving_average(x, n=5)
#plt.plot(xm, 'x-')

#w = np.repeat(1.0, 19)/19
#xxm = np.convolve(x, w, 'valid')
#plt.plot(xxm, '.-')


sigma = 1.0
s = 2.0 * sigma * sigma
r = 2.0 * sigma * sigma
summm = 0.0
kernel = [0.0, 0.0, 0.0, 0.0, 0.0]
# Generate gaussian kernel
for y in range(-2, 3, 1):
    r = m.sqrt(y*y)
    kernel[y+2] = ( m.exp( -(r*r)/s ) ) / (m.pi * s)
    summm += kernel[y+2]
    print(kernel[y+2])

print("summ = {}".format(summm))

# Normalize kernel
#for y in range(-2, 3, 1):
#    kernel[y+2] = int( kernel[y+2] / summm  * 2**12 )
for y in range(-2, 3, 1):
    kernel[y+2] = float( kernel[y+2] / summm )
print(kernel)
print("new sum {}".format(sum(kernel)))


xg =ndimage.filters.gaussian_filter1d(x, 1)
plt.plot(xg, label="gaussian_filter1d")

xc = np.zeros(len(x))
for i in range(len(x)-5):
    for j in range(-2, 3, 1):
        xc[i] = xc[i] + kernel[j+2] * x[i+j+2]
    #print(i, xc[i])

plt.plot(xc, '+k-', label="python loop")
plt.plot(xf, '+r-', label="c loop")
plt.legend()
plt.show()
