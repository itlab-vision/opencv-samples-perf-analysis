import pandas as pd

from argparse import ArgumentParser
import re

parser = ArgumentParser()
parser.add_argument("-ifn", '--in_file_name', type=str, default="resize_x86.out")
parser.add_argument("-ofn", '--out_file_name', type=str, default="resize_x86.xlsx")
args = parser.parse_args()

data_result = pd.DataFrame()

with open(args.in_file_name, 'r') as file:
    file_string = file.read()

file_array = re.split("=+\n=+", file_string)

all_df_for_method = {}
for exp in file_array:
    input_resolution = re.findall("Input image: (.+)\.bmp", exp).pop()
    input_resolution = input_resolution.replace("_", "x")
    output_resolution = re.findall("Output image resolution: (.+)\n", exp).pop()
    method = re.findall("Interpolation method: (.+)\n", exp).pop()
    type_1 = re.findall("Version type 1: (.+)\n", exp).pop()
    type_2 = re.findall("Version type 2: (.+)\n", exp).pop()
    type = type_1 + " " + type_2
    time = re.findall("Elapsed time in seconds: (.+)\n", exp).pop()

    if method in all_df_for_method:
        df_from_images = all_df_for_method[method]
    else:
        df_from_images = {}
        all_df_for_method[method] = df_from_images

    if input_resolution in df_from_images:
        curr_df = df_from_images[input_resolution]
    else:
        curr_df = pd.DataFrame()
        df_from_images[input_resolution] = curr_df

    curr_df.loc[output_resolution, "method"] = method
    curr_df.loc[output_resolution, "input resolution"] = input_resolution
    curr_df.loc[output_resolution, "output resolution"] = output_resolution
    curr_df.loc[output_resolution, type] = time

result_df = pd.DataFrame()
for key_m, df_method in all_df_for_method.items():
    for key_im, curr_df in df_method.items():
        result_df = pd.concat([result_df, curr_df])

print(result_df)
result_df.to_excel(args.out_file_name, index=False)

