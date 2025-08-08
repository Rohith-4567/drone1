import serial
import re
import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial.transform import Rotation as R
from mpl_toolkits.mplot3d import Axes3D
import time

# Setup Serial
ser = serial.Serial('/dev/tty.usbserial-0001', 115200)  # change port if needed
time.sleep(2)  # allow MPU to initialize

# Plot setup
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Define unit vectors for 3D axis
origin = np.array([[0, 0, 0]]).T
axis_len = 1

def draw_frame(R_mat):
    ax.clear()
    ax.set_xlim([-1.5, 1.5])
    ax.set_ylim([-1.5, 1.5])
    ax.set_zlim([-1.5, 1.5])
    ax.set_axis_off()
    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")
    ax.set_title("MPU Orientation (Square Frame)")

    # Define square corners in the XY plane at Z=0, centered at origin
    half_len = 0.5
    square_corners = np.array([
        [-half_len, -half_len, 0],
        [ half_len, -half_len, 0],
        [ half_len,  half_len, 0],
        [-half_len,  half_len, 0],
        [-half_len, -half_len, 0],  # close the square by repeating first point
    ]).T  # shape (3,5)

    # Rotate all corners by R_mat
    rotated_corners = R_mat @ square_corners  # shape (3,5)

    # Plot edges of the square
    ax.plot(rotated_corners[0, :], rotated_corners[1, :], rotated_corners[2, :], color='m', linewidth=3)

    plt.draw()
    plt.pause(0.001)

# Main loop
pattern = r'Roll\s*:\s*(-?\d+\.\d+)\s*Pitch\s*:\s*(-?\d+\.\d+)\s*Yaw\s*:\s*(-?\d+\.\d+)'

try:
    while True:
        line = ser.readline().decode('utf-8', errors='replace').strip()
        match = re.search(pattern, line)
        if match:
            roll, pitch, yaw = map(float, match.groups())
            # Convert from degrees to radians
            r = R.from_euler('xyz', [roll, pitch, yaw], degrees=True)
            R_matrix = r.as_matrix()
            draw_frame(R_matrix)
except KeyboardInterrupt:
    print("Simulation stopped.")
finally:
    ser.close()