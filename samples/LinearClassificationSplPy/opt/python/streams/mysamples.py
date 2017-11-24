# Import the SPL decorators
from streamsx.spl import spl
#sklearn
from sklearn.svm import *
from sklearn.feature_extraction.text import *


# Defines the SPL namespace for any functions in this module
# Multiple modules can map to the same namespace
def splNamespace():
    return "mysamples"

@spl.map()
class LinearClassification:
    "Class that represents the SPL classificarion operator"
    def __init__(self, verbose=""):
        self.verbose_ = verbose
        self.corpus = []
        self.classes = []
        if (self.verbose_):
            print("LinearClassification.__init__ verbose="+self.verbose_)

    def __call__(self, *tuple):
        if (tuple[0] == "True"):
            if (self.verbose_):
                print ("LinearClassification.__call__ Training corpus="+tuple[1]+" classes="+tuple[2])
            self.corpus.append(tuple[1])
            self.classes.append(tuple[2])
        else:
            if (tuple[4] == "True"):
                if (self.verbose_):
                    print ("LinearClassification.__call__ Training End")
                self.vectorizer = TfidfVectorizer(lowercase=False, ngram_range=(1, 3)) # From Sklearn sklearn.feature_extraction.text.TfidfVectorizer
                self.vectorizer.fit(self.corpus) # train model
                self.corpus_matrix = self.vectorizer.transform(self.corpus) # sparce matrix of training documents
                self.model = LinearSVC()
                self.model.fit(self.corpus_matrix, self.classes) # train model
                # end of training
                self.corpus = []
                self.classes = []
            else:
                s = tuple[3];
                if (self.verbose_):
                    print ("LinearClassification.__call__ Processing ln="+s)
                if ( s != ''):
                    txt = s.rstrip('\n');
                    test_doc = [txt]
                    test_matrix  = self.vectorizer.transform(test_doc)  # sparce matrix of test document
                    pred = self.model.predict(test_matrix)
                    decision = self.model.decision_function(test_matrix)
                    classes = self.model.classes_
                    #outstring = str(pred) + '|*|' + str(decision) + '|*|' + str(classes)
                    #print type(pred), type(decision), type(classes)
                    #print(outstring)
                    return (pred, decision, classes)
