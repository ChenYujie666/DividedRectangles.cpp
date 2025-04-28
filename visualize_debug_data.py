import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.animation as animation

def read_debug_info(file_path):
    """
    Reads the debug information saved by the C++ program.

    Args:
        file_path (str): Path to the debug info file.

    Returns:
        pd.DataFrame: A DataFrame containing the parsed debug information.
    """
    if not os.path.exists(file_path):
        raise FileNotFoundError(f"File not found: {file_path}")

    # Assuming the debug info is tab-separated
    debug_data = pd.read_csv(file_path, sep="\t", header=None)
    return debug_data

def visualize_debug_data(debug_data, iteration_column=0, radius_column=1, value_column=2):
    """
    Visualizes the debug data by plotting radius vs value for each iteration.

    Args:
        debug_data (pd.DataFrame): The debug data to visualize.
        iteration_column (int): The column index for iteration numbers.
        radius_column (int): The column index for radius values.
        value_column (int): The column index for function values.
    """
    iterations = debug_data[iteration_column].unique()
    plt.figure(figsize=(10, 6))


    plt.scatter(debug_data[radius_column], debug_data[value_column], alpha=0.5)

    plt.xlabel("Radius")
    plt.ylabel("Function Value")
    plt.title("Radius vs Function Value Across Iterations")
    plt.legend()
    plt.grid(True)
    plt.show()

def animate_debug_data(debug_info1, debug_info2, debug_info3, iteration_column=0, radius_column=1, value_column=2):
    """
    Creates an animated visualization of the debug data.

    Args:
        debug_info1, debug_info2, debug_info3 (pd.DataFrame): Debug data from different files.
        iteration_column (int): The column index for iteration numbers.
        radius_column (int): The column index for radius values.
        value_column (int): The column index for function values.
    """
    fig, ax = plt.subplots(figsize=(10, 6))
    ax.set_xlim(0, debug_info1[radius_column].max())
    ax.set_ylim(debug_info1[value_column].min(), debug_info1[value_column].max())
    ax.set_xlabel("Radius")
    ax.set_ylabel("Function Value")
    ax.set_title("Radius vs Function Value Across Iterations (Animated)")

    scatter1, = ax.plot([], [], 'bo', alpha=0.5, label="Rects")
    scatter2, = ax.plot([], [], 'go', alpha=0.5, label="Candidates")
    scatter3, = ax.plot([], [], 'ro', alpha=0.1, label="New Rects")
    ax.legend()

    def update(frame):
        iter_data1 = debug_info1[debug_info1[iteration_column] <= frame]
        iter_data2 = debug_info2[debug_info2[iteration_column] <= frame]
        iter_data3 = debug_info3[debug_info3[iteration_column] <= frame]

        scatter1.set_data(iter_data1[radius_column], iter_data1[value_column])
        scatter2.set_data(iter_data2[radius_column], iter_data2[value_column])
        scatter3.set_data(iter_data3[radius_column], iter_data3[value_column])

        return scatter1, scatter2, scatter3

    ani = animation.FuncAnimation(fig, update, frames=range(int(debug_info1[iteration_column].max()) + 1), interval=500, blit=True)
    plt.show()

if __name__ == "__main__":
    debug_file_path1 = "d:\\CodeLab\\DividedRectangles.cpp\\debugdata\\rects.txt"
    debug_file_path2 = "d:\\CodeLab\\DividedRectangles.cpp\\debugdata\\candidates.txt"
    debug_file_path3 = "d:\\CodeLab\\DividedRectangles.cpp\\debugdata\\new_rects.txt"

    try:
        debug_info1 = read_debug_info(debug_file_path1)
        debug_info2 = read_debug_info(debug_file_path2)
        debug_info3 = read_debug_info(debug_file_path3)

        animate_debug_data(debug_info1, debug_info2, debug_info3)

    except FileNotFoundError as e:
        print(e)
