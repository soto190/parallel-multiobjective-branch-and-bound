from mpl_toolkits.mplot3d import Axes3D 

import matplotlib.pyplot as plt
from matplotlib.lines import Line2D
import matplotlib.patches as mpatches

import numpy as np
import pandas as pd

# Ghostscript command to convert a figure to grayscale
# gs -sOutputFile=Fattahi6grey.pdf -sDEVICE=pdfwrite -sColorConversionStrategy=Gray -dProcessColorModel=/DeviceGray -dNOPAUSE -dBATCH -dAutoRotatePages=/None Fattahi6.pdf 


problem_dict = {
			#'Fattahi1' : 'SFJS1',
			#'Fattahi2': 'SFJS2',
			#'Fattahi3': 'SFJS3',
			#'Fattahi4': 'SFJS4',
			#'Fattahi5': 'SFJS5',
			#'Fattahi6': 'SFJS6',
			#'Fattahi7': 'SFJS7',
			#'Fattahi8': 'SFJS8',
			#'Fattahi9': 'SFJS9',
			#'Fattahi10': 'SFJS10',
			#'Fattahi11': 'MFJS1',
			#'Fattahi12': 'MFJS2',
			#'Fattahi13': 'MFJS3',
			'Fattahi14': 'MFJS4',
			#'Fattahi15': 'MFJS5',
			#'Fattahi16': 'MFJS6',
			#'Fattahi17': 'MFJS7',
			#'Fattahi18': 'MFJS8',
			#'Fattahi19': 'MFJS9',
			#'Fattahi20': 'MFJS10',
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
            'Fattahi10': 5,
            'Fattahi11': 8,
            'Fattahi12': 22,
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

path_to_pareto_fronts = '/Users/carlossoto/workspace/Results/FJSSP/paretoFronts/'
path_to_approximate_fronts = '/Users/carlossoto/workspace/Results/FJSSP/NSGA2/NSGA2'

# For plotting in three diferent 2D plots for each pair of objectives.
for key in problem_dict:
	problem = key
	pareto_front = pd.read_csv(path_to_pareto_fronts + problem + '.txt', sep=' ', header=None, names=['f1', 'f2','f3'])
	reference_front = pd.read_csv(path_to_approximate_fronts + problem + '-r1.csv', sep=' ', header=None, names=['f1', 'f2','f3'])

	all_fronts = pareto_front.append(reference_front, ignore_index = True) 

	marker_bb = 'o'
	marker_nsga = 'x'

	legend_elements = [Line2D([0], [0], lw=0, marker=marker_bb, color='k', markerfacecolor='none', label='B&B', markersize=7),
	                   Line2D([0], [0], lw=0, marker=marker_nsga, color='k', label='NSGA-II', markersize=7)]


	#fig_front = plt.add_subplot()
	#ax = fig_front.add_subplot(1,3, sharey=False, sharex=False)

	fig, ax = plt.subplots(1, 3, figsize=(15, 3), sharey=False, sharex=False)

	ax[0].scatter(pareto_front['f1'], pareto_front['f2'], marker=marker_bb, facecolors='none', edgecolors='k')
	ax[0].scatter(reference_front['f1'], reference_front['f2'], marker=marker_nsga, facecolor='0.0')

	ax[0].set_xlabel('f1(x)')
	ax[0].set_ylabel('f2(x)')

	ax[1].scatter(pareto_front['f1'], pareto_front['f3'], marker=marker_bb, facecolors='none', edgecolors='k')
	ax[1].scatter(reference_front['f1'], reference_front['f3'], marker=marker_nsga, facecolor='0.0')

	ax[1].set_xlabel('f1(x)')
	ax[1].set_ylabel('f3(x)')

	ax[2].scatter(pareto_front['f2'], pareto_front['f3'], marker=marker_bb, facecolors='none', edgecolors='k')
	ax[2].scatter(reference_front['f2'], reference_front['f3'], marker=marker_nsga, facecolor='0.0')

	ax[2].set_xlabel('f2(x)')
	ax[2].set_ylabel('f3(x)')

	fig.suptitle(problem_dict[problem])

	plt.legend(handles=legend_elements)

	mins = all_fronts.max(axis = 0)
	maxs = all_fronts.min(axis = 0)

	#ax.set_xlim(mins[0], maxs[0])
	#ax.set_ylim(mins[1], maxs[1])

	#plt.title(problem_dict[problem])

	plt.savefig('/Users/carlossoto/Desktop/' + problem +'.pdf', dpi=None, facecolor=None, edgecolor=None,
	        orientation='portrait', papertype=None, format=None,
	        transparent=False, bbox_inches='tight', pad_inches=0.1)

	# plt.show()

# For plotting scatter points in 3D.
'''
for key in problem_dict:
	problem = key
	pareto_front = pd.read_csv(path_to_pareto_fronts + problem + '.txt', sep=' ', header=None, names=['f1', 'f2','f3'])
	reference_front = pd.read_csv('/Users/carlossoto/workspace/Results/FJSSP/NSGA2/NSGA2' + problem + '-r10.csv', sep=' ', header=None, names=['f1', 'f2','f3'])

	all_fronts = pareto_front.append(reference_front, ignore_index = True) 

	fig_front = plt.figure()
	ax = fig_front.add_subplot(111, projection='3d')
	ax.scatter(pareto_front['f1'], pareto_front['f2'], pareto_front['f3'], marker='.')
	ax.scatter(reference_front['f1'], reference_front['f2'], reference_front['f3'], marker='x')

	ax.set_xlabel('f1(x)')
	ax.set_ylabel('f2(x)')
	ax.set_zlabel('f3(x)')

	legend_elements = [Line2D([0], [0], lw=0.1, marker='.', label='B&B', markersize=10),
	                   Line2D([0], [0], lw=0.1, marker='x', color='b', label='NSGA-II', markersize=10)]


	ax.legend(handles=legend_elements)

	mins = all_fronts.max(axis = 0)
	maxs = all_fronts.min(axis = 0)

	#ax.set_xlim(mins[0], maxs[0])
	#ax.set_ylim(mins[1], maxs[1])

	plt.title(problem_dict[problem])

	plt.savefig('/Users/carlossoto/Desktop/' + problem +'.pdf', dpi=None, facecolor=None, edgecolor=None,
	        orientation='portrait', papertype=None, format=None,
	        transparent=False, bbox_inches='tight', pad_inches=0.1,
	        frameon=None, metadata=None)

	# plt.show()
'''
