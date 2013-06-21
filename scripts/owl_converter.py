# -*- coding: utf-8 -*-

import sys
import getopt
from xml.dom.minidom import *

reload(sys)
sys.setdefaultencoding('utf-8')

def printSupportedExtensions():
	print ".owl"

def convertToInnerFormat(path):
	xml = parse(path)
	owlClasses = xml.getElementsByTagName('owl:Class')

	for owlClass in owlClasses:
		print owlClass.toxml() + '\n\n\n'

def convertToOuterFormat(path):
	print "Not implemented yet"


if __name__ == '__main__':
	path = '/Users/bobrnor/Dropbox/PSU/Projects/OntoEditor/examples/model.owl'
	convertToInnerFormat(path)

	# opts, extraparams = getopt.getopt(sys.argv[1:], '', ['method=', 'source-path='])
	
	# print opts

	# methodName = (item[1] for item in opts if item[0] == '--method').next()
	# if methodName == 'supported_extensions':
	# 	printSupportedExtensions()
	# elif methodName == 'import':
	# 	path = (item[1] for item in opts if item[0] == '--source-path').next()
	# 	convertToInnerFormat(path)
	# elif  methodName == 'export':
	# 	path = (item[1] for item in opts if item[0] == '--source-path').next()
	# 	convertToOuterFormat(path)
	# else:
	# 	print "Unknown method name " + methodName