import csv

with open('Defines.csv', 'r') as inputDefines,\
		open('Python\.py', 'w') as pythonDefines:
	csv_reader = csv.reader(inputDefines)
	next(csv_reader, None)  # skip the headers
	for row in csv_reader:

		print(row)

		pythonDefines.write(row[0] + (' '* (20 - len(row[0]))) + "= " + row[1] + "\n")

with open('Items.csv', 'r') as inputDefines,\
		open('Python\Items.py', 'w') as pythonDefines:
	csv_reader = csv.reader(inputDefines)
	next(csv_reader, None)  # skip the headers
	for row in csv_reader:
		print(row)
		
