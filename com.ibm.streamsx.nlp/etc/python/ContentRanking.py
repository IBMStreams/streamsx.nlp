# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.
import sys

if ((__name__ == "__main__") and (len(sys.argv) >= 2)):
    model_pklz_file = sys.argv[1]
    kb_ind_file = sys.argv[2]
    d_lemms_file = sys.argv[3]
elif ((__name__ == "__main__") and (len(sys.argv) < 2)):
    sys.exit("Usage: python ContentRanking.py <model_pklz_file> <kb_ind_file> <d_lemms_file>")

# 1. init
# load d_Lemms
import json
with open(d_lemms_file, 'r') as fp:
    d_lemms_ = json.load(fp)
d_lemms = {}
for k,v in d_lemms_.items():
    d_lemms[str(k)] = v

# load model of keras_LSTM
from sklearn.externals.joblib import load, dump
from keras.models import model_from_json
from sklearn.metrics import classification_report

def LoadModel_numpy(file_name, compile=False):
   
    model = load(file_name)
    res_model = model_from_json(model["model_topology"])
    res_model.set_weights(model["model_weights"])
    return res_model

final_model = LoadModel_numpy(model_pklz_file)
final_model.compile(optimizer='rmsprop', loss='mse', metrics=['accuracy'])

# Load articles from KB and save in dict structe "KB_dic[id_art] = (name_article - KB007, text)"
with open(kb_ind_file, 'r') as fp:
    KB_dic_ = json.load(fp)  #
KB_dic = {}
for k,v in KB_dic_.items():
    KB_dic[int(k)] = v

import numpy as np
kb_ind_vector  = [int(key) for key in KB_dic.keys()] # for selecting TOP 5
doc_kb_ind = np.array([[int(key)] for key in KB_dic.keys()]) # for all doc the train_kb_ind is same.

#2.
# parsing input 

def predict_top_5_IDs(clear_text):

    doc_vect_KB = []
    doc_vect_SG = []
    doc_lemms = []
    word_list = []#add
    word_ind = [] 
 
    l_test_x = []
    l_doc_lemms = []
    for lemm in clear_text.split(" "):
        word_list.append(lemm) #add
        l_doc_lemms.append(d_lemms.get(lemm, 1))
    l_test_x.append(l_doc_lemms)
    l_test_x_ = len(KB_dic.keys())  * l_test_x

    from keras.preprocessing.sequence import pad_sequences
    test_x  = np.array(pad_sequences(l_test_x_, maxlen=100))
    pred = final_model.predict_proba([test_x, doc_kb_ind], verbose=0, batch_size=1366) # not that PREDICT_Proba , not predict_classes

    ind_pred = {}
    for ind,val in zip(kb_ind_vector, pred):
        ind_pred[ind]= val[0]

    top_5_ind = sorted(ind_pred.items(), key=lambda x: (x[1],x[0]), reverse=True)[:5]
    log = "R"
    
    for ind in top_5_ind:
        log += ";"
        log += str(ind[0])

    print(log)
    return

# 2) data processing
while 1:
    s = sys.stdin.readline();
    if ( s == ''):
        break;
    doc = s.rstrip('\n');
    predict_top_5_IDs(doc)


