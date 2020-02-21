import numpy 
import os
import matplotlib
import root_numpy
import pandas
import glob
from hep_ml import reweight
from root_numpy import root2array
from matplotlib import pyplot as plt
from numpy.lib.recfunctions import stack_arrays
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split
from hep_ml.metrics_utils import ks_2samp_weighted
from sklearn.ensemble import GradientBoostingClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import roc_auc_score
from matplotlib.colors import LogNorm
from root_numpy import root2array
from numpy.lib.recfunctions import stack_arrays
import matplotlib.cm as cm
from matplotlib.colors import LogNorm
from sklearn.preprocessing import StandardScaler
from keras.utils.vis_utils import plot_model
from keras.models import Model
from keras.layers import Dense, Dropout, Input
from keras.callbacks import EarlyStopping, ModelCheckpoint
from collections import Counter
import itertools
from sklearn.metrics import confusion_matrix
from sklearn.utils import class_weight
import modules.plotter as plotter
from sklearn.externals.joblib import dump, load

def preparedatasetfortraining(signal,background,features,tag):
	df =  pandas.concat((signal[features], background[features]), ignore_index=True)
	X = df.as_matrix() # [Rows,Columns]: [Events, Features]
	#The weight array w can also easily be extracted using a similar procedure:
	w =  pandas.concat((signal['MLWeight'], background['MLWeight']), ignore_index=True).values
	#Finally, generate an array of truth labels y to distinguish among the different classes in the problem
	y = []
	for _df, ID in [(signal, 1), (background, 0)]:
		y.extend([ID] * _df.shape[0])
	y = numpy.array(y)
	#Extra Pre-Processing Steps: Shuffling, Splitting into Train-Test, Scaling Inputs
	ix = range(X.shape[0]) # array of indices, just to keep track of them for safety reasons and future checks
	X_train, X_test, y_train, y_test, w_train, w_test, ix_train, ix_test = train_test_split(X, y, w, ix, train_size=0.8,random_state=9999)
	#Scaling
	scaler = StandardScaler()
	X_train = scaler.fit_transform(X_train)
	X_test = scaler.transform(X_test) 
	X = scaler.transform(X) 
	dump(scaler, './mymodels/std_scaler_%s.bin'%tag, compress=True)
	return X,y,w,X_train, X_test, y_train, y_test, w_train, w_test


def trainNNmodel(X_train,y_train,w_train,X_test,y_test,w_test,model,design,tag):
	class_weights = class_weight.compute_class_weight('balanced',numpy.unique(y_train),y_train)
	try:
		history = model.fit(
			X_train, y_train, 
			class_weight=class_weights,
			callbacks = [
				EarlyStopping(verbose=True, patience=5, monitor='val_loss'),
				ModelCheckpoint('./mymodels/mymodel.h5', monitor='val_loss', verbose=True, save_best_only=True)
			],
			validation_split = 0.25,
#			validation_data =[X_test,y_test], 
			verbose=True,**design 
	) 
	except KeyboardInterrupt:
		print 'Training ended early.'
	print 'Saving model...'
	model.save('./mymodels/mymodel_%s.h5'%tag)
	print 'Done'
	os.system('rm ./mymodels/mymodel.h5')
	return model,history


def buildNNmodel(X_train,nodes,dropout,tag):
	n=numpy.array(nodes)
	layers = n.size
	print layers
	inputs = Input(shape=(X_train.shape[1], )) # placeholder
	i=0
	if layers > 0:
		hidden = Dense(n[0], activation='relu')(inputs)
		if dropout>0: hidden = Dropout(0.2)(hidden)
		i += 1
	if layers > 1:
		while i<layers:
			hidden = Dense(n[i], activation='relu')(hidden)
			if dropout>0: hidden = Dropout(0.2)(hidden)
			i += 1
	outputs = Dense(1, activation='sigmoid')(hidden)
	model = Model(inputs, outputs)
	#Summary of the architecture and compile it
	model.summary()
	model.compile('adam', 'binary_crossentropy',metrics=["accuracy"])
	#Now you need to declare what loss function and optimizer to use. We pass these as arguments to compile:
	plot_model(model, 'myplots/NN_%s.eps'%tag, show_shapes=True)
	return model

def GetConfusionMatrix(X_test,y_test,w_test,model,labels,point,tag):
	yhat = model.predict(X_test, verbose = True, batch_size = 512)
	yhat_cls = (yhat >= point).astype(numpy.int)
	# Compute confusion matrix
	cnf_matrix = confusion_matrix(y_test, yhat_cls, sample_weight=w_test)
	numpy.set_printoptions(precision=2)
	# Plot confusion matrix
	plot_confusion_matrix(tag,cnf_matrix, classes=labels,
						  normalize=True,
						  title='Normalized confusion matrix')


def plot_confusion_matrix(tag,cm, classes,
						  normalize=False,
						  title='Confusion matrix',
						  cmap=plt.cm.Blues):
	"""
	This function prints and plots the confusion matrix.
	Normalization can be applied by setting `normalize=True`.
	"""
	if normalize:
		cm = cm.astype('float') / cm.sum(axis=1)[:, numpy.newaxis]
		print("Normalized confusion matrix")
	else:
		print('Confusion matrix, without normalization')

	print(cm)

	plt.figure(figsize=(5,5))   
	plt.imshow(cm, interpolation='nearest', cmap=cmap)
	plt.title(title)
	plt.colorbar()
	tick_marks = numpy.arange(len(classes))
	plt.xticks(tick_marks, classes, rotation=45)
	plt.yticks(tick_marks, classes)

	fmt = '.2f' if normalize else 'd'
	thresh = cm.max() / 2.
	for i, j in itertools.product(range(cm.shape[0]), range(cm.shape[1])):
		plt.text(j, i, format(cm[i, j], fmt),
				 horizontalalignment="center",
				 color="white" if cm[i, j] > thresh else "black")

	plt.ylabel('True label')
	plt.xlabel('Predicted label')
	plt.savefig('myplots/ConfusionMatrix_%s.png'%tag)

def historyNNmodel(history,tag):
	# summarize history for accuracy
	plt.figure(figsize=(7,7))
	plt.plot(history.history['acc'])
	plt.plot(history.history['val_acc'])
	plt.title('model accuracy')
	plt.ylabel('accuracy')
	plt.xlabel('epoch')
	plt.legend(['train', 'validation'], loc='upper left')
	plt.savefig("myplots/Accuracy_%s.png"%tag) 
	# summarize history for loss
	plt.figure(figsize=(7,7))
	plt.plot(history.history['loss'])
	plt.plot(history.history['val_loss'])
	plt.title('model loss')
	plt.ylabel('loss')
	plt.xlabel('epoch')
	plt.legend(['train', 'validation'], loc='upper left')
	plt.savefig("myplots/Loss_%s.png"%tag) 

def EvaluateNN(X,model):
	yhat = model.predict(X, verbose = True, batch_size = 512)
	return yhat

def GetROCAUC(X_test,y_test,model,tag):
	# compute the ROC AUC score
	yhat = model.predict(X_test, verbose = True, batch_size = 512)
	auc = roc_auc_score(y_test, yhat)
	print("Sample %s"%tag)
	print("ROC AUC: {:.3f}".format(auc))

def GetNNDistributionComparison(X_train,y_train,X_test,y_test,model,norm,tag):
	yhat_train = model.predict(X_train, verbose = True, batch_size = 512)
	yhat_test = model.predict(X_test, verbose = True, batch_size = 512)
	plotter.DrawDNNScoreComparison(yhat_train,y_train,yhat_test,y_test,norm, tag)

def GetNNDistribution(X,y,model,norm,tag):
	yhat = model.predict(X, verbose = True, batch_size = 512)
	plotter.DrawDNNScore(yhat,y,norm,tag)