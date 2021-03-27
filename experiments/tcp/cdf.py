import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

data = np.loadtxt("10-servers.txt")
data = data[-1000:]
sns.displot(data, kind='ecdf')
plt.show()
