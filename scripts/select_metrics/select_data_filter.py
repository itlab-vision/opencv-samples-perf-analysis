import pandas as pd

from argparse import ArgumentParser
import re

parser = ArgumentParser()
parser.add_argument("-ifn", '--in_file_name', type=str, default="filter.out")
parser.add_argument("-ofn", '--out_file_name', type=str, default="filter.xlsx")
args = parser.parse_args()

data_result = pd.DataFrame()

with open(args.in_file_name, 'r') as file:
    file_string = file.read()

file_array = re.split("=+\n=+", file_string)

df_from_images = {}
for exp in file_array:
    input_resolution = re.findall("Input image: (.+)\.", exp).pop()
    input_resolution = input_resolution.replace("_", "x")
    kernel = re.findall("Kernel: kernel_(.+)\n", exp).pop()
    kernel = kernel +"x" + kernel
    type_1 = re.findall("Version type 1: (.+)\n", exp).pop()
    type_2 = re.findall("Version type 2: (.+)\n", exp).pop()
    type = type_1 + " " + type_2
    time = re.findall("elapsed time: (.+)s\n", exp).pop()

    if input_resolution in df_from_images:
        curr_df = df_from_images[input_resolution]
    else:
        curr_df = pd.DataFrame()
        df_from_images[input_resolution] = curr_df

    curr_df.loc[kernel, "input resolution"] = input_resolution
    curr_df.loc[kernel, "kernel"] = kernel
    curr_df.loc[kernel, type] = time

result_df = pd.DataFrame()
for key_im, curr_df in df_from_images.items():
    result_df = pd.concat([result_df, curr_df])

print(result_df)
result_df.to_excel(args.out_file_name, index=False)

