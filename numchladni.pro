TOP_DIR    = $$PWD

HOME=$$system(echo $HOME)


#######  SOLVER  #######
# USE ONLY ONE OF THEM... !!!

#CONFIG += USE_GSL
#GSL_DIR    = $$HOME/local/gsl/1.15

CONFIG += USE_LAPACK
LAPACK_DIR  = $$PWD/lapack

#CONFIG += USE_MAGMA
#MAGMA_DIR   = $$HOME/inst/Sonst/magma-1.3.0
#LAPACK_DIR  = $$HOME/inst/Sonst/lapack-3.4.2
#CUDA_DIR    = /usr/local/cuda
#CUDA_LIBDIR = $$CUDA_DIR/lib

#########################

### You should not need to modify the following stuff...

include( numchladni.pri )

