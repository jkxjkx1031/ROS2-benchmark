import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

data = np.loadtxt("single-server.txt")
sns.displot(data)
plt.show()
