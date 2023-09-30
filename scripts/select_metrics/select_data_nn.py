import pandas as pd

from argparse import ArgumentParser
import re

parser = ArgumentParser()
parser.add_argument("-ifn", '--in_file_name', type=str, default="nn.out")
parser.add_argument("-ofn", '--out_file_name', type=str, default="nn.xlsx")
args = parser.parse_args()

data_result = pd.DataFrame()

with open(args.in_file_name, 'r') as file:
    file_string = file.read()

file_array = re.split("=+\n=+", file_string)


models_df = {}
for exp in file_array:
    model_name = re.findall("Model name: (.+)\n", exp).pop()
    batch = re.findall("Batch size: (.+)\n", exp).pop()
    type_1 = re.findall("Version type 1: (.+)\n", exp).pop()
    type_2 = re.findall("Version type 2: (.+)\n", exp).pop()
    type = type_1 + " " + type_2
    convert_time = re.findall("convert time: (.+) s", exp).pop()
    inference_time = re.findall("inference time: (.+) s", exp).pop()
    latency = re.findall("latency: (.+) s", exp).pop()
    average_time_per_batch = re.findall("average time per batch: (.+) s", exp).pop()
    fps = re.findall("FPS: (.+)\n", exp).pop()


    if model_name in models_df:
        (ctime_df, itime_df, latency_df, avg_df, fps_df) = models_df[model_name]
    else:
        ctime_df = pd.DataFrame()
        itime_df = pd.DataFrame()
        latency_df = pd.DataFrame()
        avg_df = pd.DataFrame()
        fps_df = pd.DataFrame()
        models_df[model_name] = (ctime_df, itime_df, latency_df, avg_df, fps_df)

    ctime_df.loc[batch, "model_name"] = model_name
    ctime_df.loc[batch, "metric"] = "convert time"
    ctime_df.loc[batch, "Batch size"] = batch
    ctime_df.loc[batch, type] = convert_time

    itime_df.loc[batch, "model_name"] = model_name
    itime_df.loc[batch, "metric"] = "inference time"
    itime_df.loc[batch, "Batch size"] = batch
    itime_df.loc[batch, type] = inference_time

    latency_df.loc[batch, "model_name"] = model_name
    latency_df.loc[batch, "metric"] = "latency"
    latency_df.loc[batch, "Batch size"] = batch
    latency_df.loc[batch, type] = latency

    avg_df.loc[batch, "model_name"] = model_name
    avg_df.loc[batch, "metric"] = "average time per batch"
    avg_df.loc[batch, "Batch size"] = batch
    avg_df.loc[batch, type] = average_time_per_batch

    fps_df.loc[batch, "model_name"] = model_name
    fps_df.loc[batch, "metric"] = "fps"
    fps_df.loc[batch, "Batch size"] = batch
    fps_df.loc[batch, type] = fps




result_df = pd.DataFrame()
for key_im, model_df in models_df.items():
    for curr_df in model_df:
        result_df = pd.concat([result_df, curr_df])

print(result_df)
result_df.to_excel(args.out_file_name, index=False)

