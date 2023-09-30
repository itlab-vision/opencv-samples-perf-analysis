import pandas as pd
import os
from argparse import ArgumentParser


parser = ArgumentParser()
parser.add_argument("-rd", '--reference_dir', type=str)
parser.add_argument("-cd", '--comparable_dir', type=str)
parser.add_argument("-dp", '--diff_exe_path', type=str)
parser.add_argument("-t", '--type', choices=['x86', 'mangopi', 'licheepi'])
args = parser.parse_args()


reference_dir = args.reference_dir
comparable_dir = args.comparable_dir
diff_exe_path = args.diff_exe_path

# Uncomment experiments to yours

if args.type == 'x86':
    ## x86
    versions_1=["scalar", "vector"]
    versions_2=["sequence", "openmp"]
elif args.type == 'mangopi':
    ## MangoPi
    versions_1=["scalar", "vector128", "vector512"]
    versions_2=["sequence"]
elif args.type == 'licheepi':
    ## LicheePi
    versions_1=["scalar", "vector128", "vector512"]
    versions_2=["sequence", "openmp"]

###################################################

kernels=["kernel_3", "kernel_5", "kernel_7", "kernel_9", "kernel_11", "kernel_13"]
resolutions = (0,1,2,3)
resolution_values = ['1920x1080', '3840x2160', '7680x4320', '15260x8640']

def parse_number_of_diff_pixels(output):
    number_start_index = output.find(':')
    number_of_differences = output[number_start_index+2:]
    return int(number_of_differences)  


table = []
for kernel in kernels:
    for resolution in resolutions:
        table_row = [resolution_values[resolution], kernel]
        reference_filename = f'{kernel}_scalar_sequence_{resolution}.png'
        for v1 in versions_1:
            for v2 in versions_2:
                comparable_filename = f'{kernel}_{v1}_{v2}_{resolution}.png'                
                reference_path = os.path.join(reference_dir, reference_filename)
                comparable_path = os.path.join(comparable_dir, comparable_filename)
                try:
                    output = os.popen(f'{diff_exe_path} {reference_path} {comparable_path}').read()
                    num_diff_pixels = parse_number_of_diff_pixels(output)
                    table_row += [num_diff_pixels]
                except:
                    table_row += [None]
                    
        table.append(table_row)
        
columns = ['Resolution','kernel'] + [f'{v1} {v2}' for v1 in versions_1 for v2  in versions_2]
        
df = pd.DataFrame(table, columns = columns)

df.to_excel(f'diff_filter2d_{args.type}.xlsx')

