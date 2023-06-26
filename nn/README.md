# nn

## AlexNet

./nn alexnet_deploy.prototxt bvlc_alexnet.caffemodel ../../nn/imagenet_examples/ 256 1 224 224 104 117 123 out.yml

output: 

probability: [0.98853534, 0.0053764447, 0.0014227713, 0.0013081059, 0.00071462174]  
class: 948  
image: ../../nn/imagenet_examples/ILSVRC2012_val_00000247.JPEG  
probability: [0.99663144, 0.0033101817, 2.8077528e-05, 1.2696366e-05, 9.5023433e-06]  
class: 13  
image: ../../nn/imagenet_examples/ILSVRC2012_val_00018592.JPEG  
probability: [0.99796295, 0.0011566947, 0.00010571909, 0.00010568157, 0.00010495343]  
class: 625  

for model: 

https://raw.githubusercontent.com/BVLC/caffe/88c96189bcbf3853b93e2b65c7b5e4948f9d5f67/models/bvlc_alexnet/deploy.prototxt

https://storage.openvinotoolkit.org/repositories/open_model_zoo/public/2022.1/alexnet/bvlc_alexnet.caffemodel

# GoogLeNet

./nn googlenet.prototxt bvlc_googlenet.caffemodel ../../nn/imagenet_examples/ 256 1 224 224 104 117 123 out.yml

output: 

image: ../../nn/imagenet_examples/ILSVRC2012_val_00000023.JPEG  
probability: [0.99829763, 0.00056126592, 0.0003486975, 6.7908339e-05, 6.555381e-05]  
class: 948  
image: ../../nn/imagenet_examples/ILSVRC2012_val_00000247.JPEG  
probability: [0.99999547, 4.3205505e-06, 2.8146468e-07, 2.0098549e-08, 1.2273974e-08]  
class: 13  
image: ../../nn/imagenet_examples/ILSVRC2012_val_00018592.JPEG  
probability: [0.89906287, 0.027598409, 0.020923723, 0.0196473, 0.0062733875]  
class: 625  

https://raw.githubusercontent.com/BVLC/caffe/88c96189bcbf3853b93e2b65c7b5e4948f9d5f67/models/bvlc_googlenet/deploy.prototxt

https://storage.openvinotoolkit.org/repositories/open_model_zoo/public/2022.1/googlenet-v1/bvlc_googlenet.caffemodel

