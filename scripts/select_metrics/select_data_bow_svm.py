import pandas as pd

from argparse import ArgumentParser
import re

parser = ArgumentParser()
parser.add_argument("-ifn", '--in_file_name', type=str, default="bow_svm.out")
parser.add_argument("-ofn", '--out_file_name', type=str, default="bow_svm.xlsx")
args = parser.parse_args()

data_result = pd.DataFrame()

with open(args.in_file_name, 'r') as file:
    file_string = file.read()

file_array = re.split("=+\n=+", file_string)


svm_df = {}
for exp in file_array:
    svm_name = re.findall("SVM type: (.+)\n", exp).pop()
    kernel_type = re.findall("Kernel type: (.+)\n", exp).pop()
    type_1 = re.findall("Version type 1: (.+)\n", exp).pop()
    type_2 = re.findall("Version type 2: (.+)\n", exp).pop()
    type = type_1 + " " + type_2
    keypoint_time = re.findall("keypoint detection time: (.+)s", exp).pop()
    feature_time = re.findall("feature generation time: (.+)s", exp).pop()
    predict_time = re.findall("predict time: (.+)s", exp).pop()


    if svm_name in svm_df:
        (ktime_df, ftime_df, ptime_df) = svm_df[svm_name]
    else:
        ktime_df = pd.DataFrame()
        ftime_df = pd.DataFrame()
        ptime_df = pd.DataFrame()
        svm_df[svm_name] = (ktime_df, ftime_df, ptime_df)

    ktime_df.loc[kernel_type, "SVM type"] = svm_name
    ktime_df.loc[kernel_type, "time"] = "keypoint detection"
    ktime_df.loc[kernel_type, "Kernel type"] = kernel_type
    ktime_df.loc[kernel_type, type] = keypoint_time

    ftime_df.loc[kernel_type, "SVM type"] = svm_name
    ftime_df.loc[kernel_type, "time"] = "feature generation"
    ftime_df.loc[kernel_type, "Kernel type"] = kernel_type
    ftime_df.loc[kernel_type, type] = feature_time

    ptime_df.loc[kernel_type, "SVM type"] = svm_name
    ptime_df.loc[kernel_type, "time"] = "predict"
    ptime_df.loc[kernel_type, "Kernel type"] = kernel_type
    ptime_df.loc[kernel_type, type] = predict_time


result_df = pd.DataFrame()
for key_im, model_df in svm_df.items():
    for curr_df in model_df:
        result_df = pd.concat([result_df, curr_df])

print(result_df)
result_df.to_excel(args.out_file_name, index=False)

