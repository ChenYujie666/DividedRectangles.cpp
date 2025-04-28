import os
import pandas as pd
import matplotlib.pyplot as plt

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

    for iter_num in iterations:
        iter_data = debug_data[debug_data[iteration_column] == iter_num]
        plt.scatter(iter_data[radius_column], iter_data[value_column], label=f"Iteration {iter_num}", alpha=0.6)

    plt.xlabel("Radius")
    plt.ylabel("Function Value")
    plt.title("Radius vs Function Value Across Iterations")
    plt.legend()
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    debug_file_path = "d:\\CodeLab\\DividedRectangles.cpp\\debugdata\\rects.txt"  # Update with the actual file path
    try:
        debug_info = read_debug_info(debug_file_path)
        visualize_debug_data(debug_info)
    except FileNotFoundError as e:
        print(e)
