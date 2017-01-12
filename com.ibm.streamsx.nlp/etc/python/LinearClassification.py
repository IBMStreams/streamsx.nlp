# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.
import sys
import csv
from sklearn.svm import *
from sklearn.feature_extraction.text import *
from sklearn.externals import joblib

if ((__name__ == "__main__") and (len(sys.argv) >= 3)):
    linear_svc_pkl_file = sys.argv[1]
    tfidf_vectorizer_pkl_file = sys.argv[2]
elif ((__name__ == "__main__") and (len(sys.argv) < 3)):
    sys.exit("Usage: python LinearClassification.py <LinearSVC_model_pkl_file> <TfidfVectorizer_model_pkl_file>")

# 1) restore the models with the pkl files created by the LinearClassificationModelBuilder.py script

# LinearSVC model
model = joblib.load(linear_svc_pkl_file)

# TfidfVectorizer model
vectorizer = joblib.load(tfidf_vectorizer_pkl_file)

# 2) data processing
while 1:
    s = sys.stdin.readline();
    if ( s == ''):
        break;
    txt = s.rstrip('\n');
    test_doc = [txt]
    test_matrix  = vectorizer.transform(test_doc)  # sparce matrix of test document
    pred = model.predict(test_matrix)
    decision = model.decision_function(test_matrix)
    classes = model.classes_
    outstring = str(pred) + '|*|' + str(decision) + '|*|' + str(classes)
    #print type(pred), type(decision), type(classes)
    print(outstring)
