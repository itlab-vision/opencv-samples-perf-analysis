import pandas as pd
import os
from argparse import ArgumentParser


parser = ArgumentParser()
parser.add_argument("-i", '--input_predictions_dir', type=str)
parser.add_argument("-cd", '--cifar_dir', type=str)
parser.add_argument("-ap", '--accuracy_svm_exe_path', type=str)
parser.add_argument("-t", '--type', choices=['x86', 'mangopi', 'licheepi'])
args = parser.parse_args()


input_predictions_dir = args.input_predictions_dir
cifar_dir = args.cifar_dir
accuracy_svm_exe_path = args.accuracy_svm_exe_path

if args.type == 'x86':
    versions_1=["scalar", "vector"]
    versions_2=["sequence", "openmp"]
elif args.type == 'mangopi':
    versions_1=["scalar", "vector128", "vector512"]
    versions_2=["sequence"]
elif args.type == 'licheepi':
    versions_1=["scalar", "vector128", "vector512"]
    versions_2=["sequence", "openmp"]

###################################################


svm_types=[100]
kernel_types = [0]
vocabulary_sizes= [250,500,1000]
detector_types=['SIFT']
descriptor_types=['SIFT']


def parse_accuracy(output):
    number_start_index = output.find(':')
    acc = output[number_start_index+1:]
    return float(acc.strip())

table = []

for detector in detector_types:
    for descriptor in descriptor_types:
        for svm_type in svm_types:
            for kernel_type in kernel_types:
                for vocabulary_size in vocabulary_sizes:
                    table_row = [svm_type, kernel_type, vocabulary_size]
                    for v1 in versions_1:
                        for v2 in versions_2:
                            
                            input_filename = os.path.join(
                                                          input_predictions_dir,
                                                          f'{svm_type}_{kernel_type}_{vocabulary_size}_{detector}_{descriptor}_{v1}_{v2}_predictions.yml'
                                                          )       
                            
                	
                            try:
                                output = os.popen(f'{accuracy_svm_exe_path} cifar10 {input_filename} {cifar_dir}').read()
                                acc = parse_accuracy(output)
                                table_row += [acc]
                            except:
                                table_row += [None]
                    
                    table.append(table_row)

print(table)
        
columns = ['Svm type','Kernel type', 'Vocabulary size'] + [f'{v1} {v2}' for v1 in versions_1 for v2  in versions_2]
        
df = pd.DataFrame(table, columns = columns)

df.to_excel(f'accuracy_svm_{args.type}.xlsx')

