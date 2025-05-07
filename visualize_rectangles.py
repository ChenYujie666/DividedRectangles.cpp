import matplotlib.pyplot as plt
import matplotlib.patches as patches
import pandas as pd

def draw_rectangle(ax, center, size, value, edge_color='blue', face_color='none', text_color='red'):
    """
    Draws a rectangle on the given axis.

    Args:
        ax (matplotlib.axes.Axes): The axis to draw the rectangle on.
        center (list): The center of the rectangle [x, y].
        size (list): The size of the rectangle [width, height].
        value (float): The value to annotate inside the rectangle.
        edge_color (str): The color of the rectangle's edge.
        face_color (str): The color to fill the rectangle.
        text_color (str): The color of the annotation text.
    """
    # Calculate bottom-left corner
    bottom_left = [center[i] - size[i] / 2 for i in range(len(center))]
    width, height = size

    # Add rectangle to the plot
    rect = patches.Rectangle(bottom_left, width, height, linewidth=1, edgecolor=edge_color, facecolor=face_color)
    ax.add_patch(rect)

    # Annotate rectangle with the value
    ax.text(center[0], center[1], f"{value:.2f}", color=text_color, ha="center", va="center", fontsize=8)


def read_debug_data(file_path):
    """
    Reads the debug data from a .txt file into a pandas DataFrame.

    Args:
        file_path (str): Path to the debug data file.

    Returns:
        pd.DataFrame: A DataFrame containing the parsed debug data.
    """
    try:
        # Assuming the debug data is tab-separated
        data = pd.read_csv(file_path, sep="\t", header=None)
        data.columns = ["iteration", "radius", "value", "center", "division"]
        return data
    except FileNotFoundError:
        print(f"File not found: {file_path}")
        return None


if __name__ == "__main__":



    cur_iter = 0

    df_data = read_debug_data(".\\debugdata\\rects.txt")
    while (cur_iter < 10):
        cur_iter+=1
        df = df_data[df_data["iteration"]<=cur_iter]  # Limit to first 10 rows for demonstration

        print(len(df))


        fig, ax = plt.subplots(figsize=(8, 8))
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.set_title("Rectangle Visualization Demo")
        ax.set_xlabel("X-axis")
        ax.set_ylabel("Y-axis")
        ax.set_aspect('equal', adjustable='box')  # Set equal aspect ratio for x and y axes
        # Extract centers and values for contour plot
        centers = [eval(row["center"]) for _, row in df.iterrows()]
        values = [row["value"] for _, row in df.iterrows()]

        # Convert centers and values to grid format
        x = [c[0] for c in centers]
        y = [c[1] for c in centers]
        z = values

        # Create a contour plot
        contour = ax.tricontourf(x, y, z, levels=20, cmap="viridis")
        plt.colorbar(contour, ax=ax, label="Value")
        # plt.show()  # Show the plot only once



        rectangles = []
        for _, row in df.iterrows():
            center = eval(row["center"])  # Convert string to list
            division = eval(row["division"])  # Convert string to list
            size = [2*0.5 * (3 ** -d) for d in division]  # Calculate size based on division
            value = row["value"]
            rectangles.append({"center": center, "size": size, "value": value})

        for rect in rectangles:
            draw_rectangle(ax, rect["center"], rect["size"], rect["value"])

        plt.grid(False)  # Disable grid




        plt.show()  # Show the plot only once

