# Copyright (C)2016, International Business Machines Corporation
# All rights reserved.
import sys
import csv
from sklearn.svm import *
from sklearn.feature_extraction.text import *
from sklearn.externals import joblib

if ((__name__ == "__main__") and (len(sys.argv) >= 2)):
    training_file = sys.argv[1]
    output_dir = sys.argv[2]
    #print("training_file=%s" % (training_file, ))
elif ((__name__ == "__main__") and (len(sys.argv) < 2)):
    sys.exit("Usage: python LinearClassificationSample.py <training_file> <output_dir>")

# 1) training

corpus = []
classes = []

# training file is expected in csv format with comma as delimiter 
# <document string>,<class>
f = open(training_file, 'r') # opens the training file in csv format
try:
    reader = csv.reader(f)
    for row in reader:
        column = 0
        for col in row:
            column +=1
        if (column > 1):
            corpus.append(row[0])
            classes.append(row[1])
finally:
    f.close()  # closing

# Transfer text into vector
vectorizer = TfidfVectorizer(lowercase=False, ngram_range=(1, 3)) # From Sklearn sklearn.feature_extraction.text.TfidfVectorizer
vectorizer.fit(corpus) # train model

corpus_matrix = vectorizer.transform(corpus) # sparce matrix of training documents

model = LinearSVC()
model.fit(corpus_matrix, classes) # train model

# end of training

# persist the models
joblib.dump(model, output_dir+'/LinearSVC_model.pkl')
joblib.dump(vectorizer, output_dir+'/TfidfVectorizer_model.pkl')

