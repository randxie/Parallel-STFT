import numpy as np
import matplotlib.pyplot as plt

'''
# pure openMP
n = np.array([1,2,4,6,8,16])
t_cal = np.array([68.180, 34.840, 17.652, 12.113, 9.298, 9.204])
t_plot = np.array([42.492, 27.264 , 20.204, 17.800, 16.688, 16.823])

print t_cal[0]/t_cal

print t_plot[0]/t_plot

# red dashes, blue squares and green triangles
plt.plot(n, t_cal, 'r--',label='STFT calculation time')
plt.plot(n, t_plot, 'bs--', label='Plotting time')
plt.plot(n, t_cal+t_plot, 'g^--', label='Total time')
legend = plt.legend(loc='upper center')
plt.xlabel('Number of threads (1 for sequential)')
plt.ylabel('Time (s)')
plt.show()
'''

# Mixed MPI and openMP
n = np.array([1,2,3,4,5,6])
t_cal = np.array([68.180, 17.955, 12.115, 10.547, 10.470, 10.826])

print t_cal[0]/t_cal


# red dashes, blue squares and green triangles
plt.plot(n, t_cal, 'r--',label='STFT calculation time')
legend = plt.legend(loc='upper center')
plt.xlabel('Number of nodes in MPI, with number of thead being 2 in openMP')
plt.ylabel('Time (s)')
plt.show()