import json

from layout.section import Section
from layout.entrypoint import Entrypoint

class Layout:

	def __init__(self):
		print ("reading from json")
	
		JSONSections = self.readLayoutFromJSON()

		self.sections = self.createSections(JSONSections)


	def Print(self):
		print (self.sections)


	def readLayoutFromJSON(self):
		# Opening JSON file
		f = open("C:/Users/Tuomas/Projects/TrainController/src/layout/layout.json")
		
		# returns JSON object as 
		# a dictionary
		sections = json.load(f)
		
		# Iterating through the json
		# list
		#for i in data:
		#	print(i)
		
		# Closing file
		f.close()

		return sections

	def calculate_entrypoint_coordinates(self, sections):
		origin_is_set = False

		for section in sections:
			if origin_is_set == False:
				for entrypoint in section.entrypoints:
					print ("set origin")
					entrypoint.set_coordinates(0,0)
					origin_is_set = True

		return sections

	def createSections(self, JSONSections):
		# TODO fail if all data not present
		sections = []

		for JSONSection in JSONSections:

			id = JSONSection["id"]
			description = JSONSection["description"]
			length = JSONSection["length"]
			paths = JSONSection["paths"]

			entrypoints = self.createEntrypoints(JSONSection)

			print ("entrypoints :" , entrypoints)

			section = Section(id, description, length, entrypoints, paths)

			sections.append(section)

		sections = self.calculate_entrypoint_coordinates(sections)

		return sections


	# create endpoints for section, and calculate their coordinates in the UI
	def createEntrypoints(self, JSONSection):
		JSONEntrypoints = JSONSection["entrypoints"]
		entrypoints = []
		
		for JSONEntrypoint in JSONEntrypoints:

			entrypointID = JSONEntrypoint["id"]
			entrypointConnection = JSONEntrypoint["connection"]
			
			entrypoint = Entrypoint(entrypointID, entrypointConnection)
			
			entrypoints.append(entrypoint)

		return entrypoints

