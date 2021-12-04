from mpl_toolkits.mplot3d import Axes3D

import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
import matplotlib.patches as mpatches

import numpy as np
import pandas as pd

# Ghostscript command to convert a figure to grayscale
# gs -sOutputFile=Fattahi6grey.pdf -sDEVICE=pdfwrite -sColorConversionStrategy=Gray -dProcessColorModel=/DeviceGray -dNOPAUSE -dBATCH -dAutoRotatePages=/None Fattahi6.pdf 


problem_dict = {
#			'Fattahi1' : 'SFJS1',
#			'Fattahi2': 'SFJS2',
#			'Fattahi3': 'SFJS3',
#			'Fattahi4': 'SFJS4',
#			'Fattahi5': 'SFJS5',
#			'Fattahi6': 'SFJS6',
#			'Fattahi7': 'SFJS7',
#			'Fattahi8': 'SFJS8',
#			'Fattahi9': 'SFJS9',
#			'Fattahi10': 'SFJS10',
#			'Fattahi11': 'MFJS1',
#			'Fattahi12': 'MFJS2',
#			'Fattahi13': 'MFJS3',
#			'Fattahi14': 'MFJS4',
#			'Fattahi15': 'MFJS5',
#			'Fattahi16': 'MFJS6',
#			'Fattahi17': 'MFJS7',
#			'Fattahi18': 'MFJS8',
#			'Fattahi19': 'MFJS9',
#			'Fattahi20': 'MFJS10',
            'Kacem1': 'Kacem1'
			}
problem_dict_best = {
            'Fattahi1': 1,
            'Fattahi2': 1,
            'Fattahi3': 1,
            'Fattahi4': 1,
            'Fattahi5': 1,
            'Fattahi6': 1,
            'Fattahi7': 1,
            'Fattahi8': 1,
            'Fattahi9': 1,
            'Fattahi10': 1,
            'Fattahi11': 1,
            'Fattahi12': 1,
            'Fattahi13': 1,
            'Fattahi14': 1,
            'Fattahi15': 1,
            'Fattahi16': 1,
            'Fattahi17': 1,
            'Fattahi18': 1,
            'Fattahi19': 1,
            'Fattahi20': 1,
            'Kacem1': 1
            }

for key in problem_dict:
	problem = key

	n_run = 1
	total_points = 0
	for n_run in range(1, 31):
		pareto_front = pd.read_csv('/Users/carlossoto/workspace/Results/FJSSP/paretoFronts/' + problem + '.txt', sep=' ', header=None, names=['f1', 'f2','f3'])
		reference_front = pd.read_csv('/Users/carlossoto/workspace/Results/FJSSP/NSGA2/NSGA2' + problem + '-r' + str(n_run) + '.csv', sep=' ', header=None, names=['f1', 'f2','f3'])

		all_fronts = pareto_front.append(reference_front, ignore_index = True) 
		points_in_run = 0
		for index_r, row_r in reference_front.iterrows():
			for index_p, row_p in pareto_front.iterrows():
				if row_r['f1'] ==  row_p['f1'] and row_r['f2'] == row_p['f2'] and row_r['f3'] == row_p['f3']:
					points_in_run += 1
					break
		total_points += points_in_run
		print(problem, ' ', n_run, ' ', len(pareto_front), ' ', len(reference_front),' ', points_in_run, ' ', total_points)
