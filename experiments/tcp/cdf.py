import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

data = np.loadtxt("single-server.txt")
data = data[-1000:]
sns.displot(data, kind='ecdf')
plt.show()
