import numpy as np
import matplotlib.pyplot as plt

def bezier_curve(p0, p1, p2, p3, t):
    """Compute the position on a cubic Bezier curve at parameter value t."""
    b = (1-t)**3 * p0 + 3*t*(1-t)**2 * p1 + 3*t**2*(1-t) * p2 + t**3 * p3
    return b

# Define the control points
P0 = np.array([0, 0])
P1 = np.array([0.25, 0.5])
P2 = np.array([0.75, 0.5])
P3 = np.array([1, 1])

# Evaluate the curve at different parameter values
t = np.linspace(0, 1, 1000)
curve = np.array([bezier_curve(P0, P1, P2, P3, i) for i in t])

# Add noise to the control points
P1 += np.random.normal(scale=0.05, size=2)
P2 += np.random.normal(scale=0.05, size=2)

# Evaluate the noisy curve
noisy_curve = np.array([bezier_curve(P0, P1, P2, P3, i) for i in t])

# Plot the curves
fig, ax = plt.subplots()
ax.plot(curve[:,0], curve[:,1], label='Smooth')
ax.plot(noisy_curve[:,0], noisy_curve[:,1], label='Noisy')
ax.legend()
plt.show()
