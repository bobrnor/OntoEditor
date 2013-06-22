# -*- coding: utf-8 -*-

import sys
import random
import getopt
import json
from xml.dom.minidom import *

reload(sys)
sys.setdefaultencoding('utf-8')
random.seed()

nextObjectId = 0
nodes = []
relations = []

def creatRelation(name, firstClassName, secondClassName, attributes = {}):
	global nextObjectId
	global relations

	firstClassNode = findNodeByName(firstClassName)
	secondClassNode = findNodeByName(secondClassName)

	relation = {'id': nextObjectId, 'name': name, 'source_node_id': firstClassNode['id'], 'destination_node_id': secondClassNode['id']}
	if attributes != {}:
		relation['attributes'] = attributes
	relations.append(relation)
	nextObjectId = nextObjectId + 1

	return relation

def createNodeIfNeeded(name, attributes = {}):
	global nextObjectId
	global nodes

	node = findNodeByName(name)
	if node == None:
		node = {'id': nextObjectId, 'name': name, 'position_x': random.uniform(0, 2000), 'position_y': random.uniform(0, 2000)}
		if attributes != {}:
			node['attributes'] = attributes
		nextObjectId = nextObjectId + 1
		nodes.append(node)
	elif attributes != {}:
		node['attributes'] = attributes

	return node

def findNodeByName(nodeName):
	global nodes
	return next((node for node in nodes if node['name'] == nodeName), None)

def processSubClassOf(owlClass, subClassOfNode):
	if subClassOfNode.hasAttribute('rdf:resource'):
		subClassNodeName = subClassOfNode.getAttribute('rdf:resource')
		if findNodeByName(subClassNodeName) == None:
			createNodeIfNeeded(subClassNodeName)
		creatRelation('rdfs:subClassOf', owlClass.getAttribute('rdf:ID'), subClassNodeName)
	else:
		restrictionNode = subClassOfNode.getElementsByTagName('owl:Restriction')[0]
		onPropertyNode = restrictionNode.getElementsByTagName('owl:onProperty')[0]
		hasValueNodes = restrictionNode.getElementsByTagName('owl:hasValue')
		someValuesFromNodes = restrictionNode.getElementsByTagName('owl:someValuesFrom')

		if hasValueNodes != []:
			hasValueNode = hasValueNodes[0]
			subClassNodeName = hasValueNode.getAttribute('rdf:resource').strip('_#')
			if findNodeByName(subClassNodeName) == None:
				createNodeIfNeeded(subClassNodeName)
			creatRelation(onPropertyNode.getAttribute('rdf:resource'), owlClass.getAttribute('rdf:ID'), subClassNodeName)
		elif someValuesFromNodes != []:
			someValuesFromNode = someValuesFromNodes[0]
			subClassNodeName = someValuesFromNode.getAttribute('rdf:resource').strip('_#')
			if findNodeByName(subClassNodeName) == None:
				createNodeIfNeeded(subClassNodeName)
			creatRelation(onPropertyNode.getAttribute('rdf:resource'), owlClass.getAttribute('rdf:ID'), subClassNodeName)

def processClassAttributes(owlClass):
	attrs = {'mypropid': owlClass.getAttribute('mypropid')}
	return attrs

def processClassContent(owlClass):
	unknownContent = []
	for childNode in owlClass.childNodes:
		if childNode.nodeType != Node.TEXT_NODE:
			if childNode.nodeName == 'rdfs:subClassOf':
				processSubClassOf(owlClass, childNode)
			else:
				unknownContent.append(childNode.toxml())

	return unknownContent

def processClass(owlClass):
	name = owlClass.getAttribute('rdf:ID')	
	node = createNodeIfNeeded(name)
	classAttributes = processClassAttributes(owlClass)
	unknownAttributes = processClassContent(owlClass)

	attributes = {}
	if classAttributes != {}:
		attributes['class-attribytes'] = classAttributes

	if unknownAttributes != {}:
		attributes['unknown-attributes'] = unknownAttributes

	if attributes != {}:
		node['attributes'] = attributes

def printSupportedExtensions():
	print ".owl"

def convertToInnerFormat(path):
	global nextObjectId
	global nodes
	global relations

	xml = parse(path)
	owlClasses = xml.getElementsByTagName('owl:Class')

	for owlClass in owlClasses:
		processClass(owlClass)

	print json.dumps({'last_id': nextObjectId - 1, 'nodes': nodes, 'relations': relations})

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