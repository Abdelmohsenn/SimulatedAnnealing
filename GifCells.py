import os
import glob
import numpy as np
import matplotlib.pyplot as plt
import imageio

def read_grid_from_csv(file_path):
    return np.loadtxt(file_path, delimiter=',')

def plot_grid(grid, step, output_folder):
    plt.figure(figsize=(10, 6))  
    from matplotlib.colors import LinearSegmentedColormap
    cmap = LinearSegmentedColormap.from_list('custom_gray', ['gray', 'white'], N=2)
    norm = plt.Normalize(vmin=0, vmax=1)
    plot_grid = np.where(grid == -1, 1, 0)
    plt.imshow(plot_grid, cmap=cmap, norm=norm)
    plt.title("T1")  # title hena
    plt.axis('off')  
    plt.savefig(os.path.join(output_folder, f"grid_step_{step}.png"), bbox_inches='tight', pad_inches=0)
    plt.close()

def create_gif(image_folder, gif_name):
    images = []
    for file_name in sorted(glob.glob(f"{image_folder}/*.png")):
        images.append(imageio.imread(file_name))
    imageio.mimsave(gif_name, images, duration=0.5)

csv_folder = '/Users/muhammadabdelmohsen/Desktop/Spring 24/DD2/Project/Gifs/T1'
image_folder = '/Users/muhammadabdelmohsen/Desktop/Spring 24/DD2/Project/Gifs/Images_T1'
os.makedirs(image_folder, exist_ok=True)

for step, csv_file in enumerate(sorted(glob.glob(f"{csv_folder}/*.csv"))):
    grid = read_grid_from_csv(csv_file)
    plot_grid(grid, step, image_folder)

gif_path = os.path.join(csv_folder, 'cell_placement.gif')
create_gif(image_folder, gif_path)

print(f"GIF created at {gif_path}")
