# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.
#=======================================
#   "Smart search" in Knowledge Base   #
#=======================================
 
# import libraries for implementation

import numpy as np
import pandas as pd

from sklearn.preprocessing import *
from sklearn.cross_validation import *
from sklearn.metrics import *
from sklearn.externals.joblib import load, dump

from os import path, environ, remove

from keras.models import *
from keras.layers import *
from keras.regularizers import *
from keras.layers.embeddings import Embedding
from keras.callbacks import *
from keras.optimizers import *
from keras.preprocessing.sequence import pad_sequences

import sys

if ((__name__ == "__main__") and (len(sys.argv) >= 4)):
    x_reg_file = sys.argv[1]
    y_reg_file = sys.argv[2]
    output_dir = sys.argv[3]
    n_epoch = int(sys.argv[4])
elif ((__name__ == "__main__") and (len(sys.argv) < 2)):
    sys.exit("Usage: python ContentRanking.py <x_reg_file> <y_reg_file> <output_dir> <n_epoch>")

environ["THEANO_FLAGS"] = "device=gpu, floatX=float32" # select GPU


# Stores weights and model toppology in file <file_name> as a dictionary with fields:
#        model_weights - weights,
#        model_topology - topology
#
# Input parameters:
#    file_name_params - dictinary of parameters to be added to file file_name
#    compress - pklz compression,
#    model_desc - free-form text field with model description
#    
# Returns:
# File name(s), sklearn.externals.joblib.dump output actually

def SaveModel_numpy(model, file_name, file_name_params={}, compress=3, model_desc=""):
     
    model_weights = model.get_weights()
    model_topology = model.to_json()
    res = {"model_weights":model_weights,
           "model_topology":model_topology,
           "model_description":model_desc}
    return dump(res, file_name.format(**file_name_params), compress=compress)

############################
###### DATA PREPARATION ####
############################

### Input Data
import os

# Read Y_0 - 'ground truth'
# format Y_0  = [y1,y2,...,y_N] , y_n = [a1,a2,...,a_K] , 
# N - number of docs (# of rows in matrix)
# K = 1366 - number of article in KB (# of columns in matrix),
# a1 - rank of article from 'Ground truth generator'
# ID_name.txt - file with list of IDs of articles in accordance to the order in vector y_n (K entries)
# among them we have got "list_ids.csv" - it`s error, don`t matter, their rank is always 0

Y = pd.read_csv(y_reg_file, sep="\t", header=None, dtype=np.float).values
# Normalize Y to [0, 1] for each row 
Y = minmax_scale(Y, axis=1) #sklearn.preprocessing

Y = Y[:100] # to make calculations faster

# Calculate TOP 5: In the matrix TOP 5 are marked with 1, all others with 0
Y_top_5 = np.zeros(Y.shape)
# Sort each row by rank in ascending order, then take five last values 
# is is a row number, val is a column number (argsort returns indexes)
for i, val in enumerate(np.argsort(Y)[:, -5:]):
    Y_top_5[i, val] = 1

# Open file with input docs, all doc is X = [x1, x2,...,x_N], 
# x_n = [LEMMA_1 space LEMMA2 ...space LEMMA_M]
with open(x_reg_file) as inp_file:
    X_texts = inp_file.read().split("\n")[:-1] # [:-1] remove last element beacuse it's empty

X_texts = X_texts[:100] # to make caluclations faster

# Split all docs on train and test sets and their Articles rank
# Have to use one split call to keep indexes in synch
X_train, X_test, Y_train, Y_test = train_test_split(X_texts, Y_top_5, train_size=0.8) # from sklearn.cross_validation

#X_train = X_texts[:80]
#X_test = X_texts[80:100]
#Y_train = Y_top_5[:80]
#Y_test = Y_top_5[80:100]

# Form triplets of TRAIN samples:
# 1) Doc = [LEMMA1 ... LEMMA M], 
# 2) Index of KB artcle from TOP 5 ( class 1) and only 1 of 100 elements of class 0
# because there are too many examples of 0 and it skews training data
# All kb indexes are = list of 1 elements - ID from 0 to 1365, like this - [951]
# 3) Class 1 or 0

train_x_texts = [] # List of docs, each doc is a list of LEMMAs
train_kb_ind = [] # List of KM articles indexes in Y matrix. Each element is a list of 1 element. always only 1 element
train_y = [] # Intersection of text and article - a rank. One-dimentional array with size N

#This function returns True for every 100th call
#This avoids the random component in the training
#counter1 = 0
#def get100():
#    global counter1
#    result = False
#    if (counter1 % 100) == 0:
#        result = True
#    counter1 += 1
#    return result

# Make a selection of all "1" and every 100th 0 to avoid skewed distibution which is because of zeroes being
# much more common
for x, y in zip(X_train, Y_train): # zip is standard python function that creates array of tuples out of two enumerables
    for ind in range(len(y)):       
        if (y[ind] == 1) or (np.random.randint(0, 100) == 0):
        #if (y[ind] == 1) or (get100()):
            train_x_texts.append(x)
            train_kb_ind.append([ind]) # List of one element. do not change to scalar!          
            train_y.append(y[ind])
    
# At the moment we have Lemmas. Our classification model works with numbers so we need to convert
# lemmas to ID_Lemmas
            
# ### Coding TEXTs of DOCS 
# create dictionary  d_lemms: We start from 2 because 0 and 1 are reserved. 0 means that lemma is absent, 1 means that lemma is not in the dictionary (this is not used at this point)  
d_lemms = {}
i = 2
for doc in train_x_texts:
    for lemm in doc.split(" "):
        if lemm not in d_lemms:
            d_lemms[lemm] = i
            i += 1

# ==========================SAVE d_lemma in JSON ========================#
import json
with open(output_dir+'/d_lemms.json', 'w') as fp:
    json.dump(d_lemms, fp)
# ==========================SAVE d_lemma in JSON ========================#


# CREATE Train (l_train_x) and test (l_test_x) sets for our LSTM model 
# Format is similar to X and x_n but LEMMAs are replaced with their indexes
l_train_x = []

for doc in train_x_texts:
    l_doc_lemms = []
    for lemm in doc.split(" "):
        l_doc_lemms.append(d_lemms.get(lemm, 1))
    l_train_x.append(l_doc_lemms)

#print ("Training triplet example")
#print ("train_x_texts[0] = " + str(train_x_texts[0]))
#print ("l_train_x_[0] = " + str(l_train_x[0]))
#print ("train_kb_ind [0] = " + str(train_kb_ind [0]))
#print ("train_y[0] = " + str(train_y[0]))


# we set maximum length of doc M = 100 and if doc have less words, all other words have index 0 (padding symbol)
# trasfer to numpy array format
train_x = np.array(pad_sequences(l_train_x, maxlen=100)) 

train_kb_ind = np.array(train_kb_ind) # numpy array is more effective and supports more operations

###############################
# CREATE NEURAL NETWORK MODEL #
###############################
# Step 1. Two embedding layers for doc (word_model) and for ID of article from KB (ind_model)
# These layers need to transfer LEMMAs ID to vector with dimension = 150, and transfer
# Articles ID to vectors with dimension = 30
# Values of 150 and 30 are based on experimentation and data scientists experience

# Embedding layer #1
word_model = Sequential()
word_model.add(Embedding(train_x.max() + 2, 150, input_length=train_x.shape[1]))

# Embeding layer #2
ind_model = Sequential() # Sequence of layers
ind_model.add(Embedding(train_kb_ind.max()+2, 30, input_length=train_kb_ind.shape[1]))

# Step 2. Repeat value from ind_model train_x.shape[1] times to gives sequences:
# [index of lemma, Article ID] [index of lemma, Article ID] [index of lemm, Article ID]  etc...
# Article ID is the same for the whole sequence
ind_model.add(Reshape((30,))) # comma is important, don't remove!
ind_model.add(RepeatVector(train_x.shape[1]))

# Step 3. Create merged vector from word_model and ind_model
# Resulting vector has dimension = 180
# We do it for every word
branches = [word_model, ind_model]
merged = Merge(branches, mode = 'concat')

# Step 4. Create the final model (final_model) that contains:
# a) merged on previous step models
# b) LSTM layer with 15 neurons
# c) hidden layer with tangential (tanh) activation function and 5 neurons
# d) output elements (our predicted value is rank of relevance doc and ID articles, so it`s a single value)
# e) initialize optimizer and method of early stopping fit function
final_model = Sequential()

final_model.add(merged)

final_model.add(LSTM(15, return_sequences=False))

final_model.add(Dense(5, activation="tanh"))
final_model.add(Dense(1))


#====== Model training and operation ======

# Configure model training process
final_model.compile(optimizer=Nadam(), loss='mse')
early_stopper = EarlyStopping(monitor='val_loss', patience=3)

# Train model
h = final_model.fit([train_x, train_kb_ind] , np.array(train_y), validation_split=0.1, nb_epoch = n_epoch, callbacks=[early_stopper], batch_size=3000)

final_model.compile(optimizer='rmsprop', loss='mse', metrics=['accuracy'])
res_files = SaveModel_numpy(final_model, output_dir+'/kb_lstm_model.pklz')

print("End of training")

