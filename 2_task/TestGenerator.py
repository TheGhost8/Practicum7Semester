import random as rd

def JobListGenerator(jobsNum, minJobTime, maxJobTime):
	jobs = []
	for i in range(jobsNum):
		# jobs.append({i : rd.randint(minJobTime, maxJobTime)})
		jobs.append((i, rd.randint(minJobTime, maxJobTime)))
	return jobs

def ForestGenerator(jobsNum=10, minJobTime=1, maxJobTime=10, minNodesNumber=1, maxNodesNumber=3, minLeafsNumber=1, maxLeafsNumber=3):
	allJobs = JobListGenerator(jobsNum, minJobTime, maxJobTime)
	outputTrees = {i : [] for i in allJobs}
	trees = []
	while allJobs:
		probableTreeSize = rd.randint(minNodesNumber, maxNodesNumber)
		newTreeSize = len(allJobs) if len(allJobs) < probableTreeSize else probableTreeSize
		newTree = []
		for i in range(newTreeSize):
			newTree.append(allJobs[0])
			allJobs.pop(0)
		trees.append(newTree)
	for i in range(len(trees)):
		sortedTree = []
		copyTree = trees[i].copy()
		copyTree.pop(0)
		for j in range(len(trees[i])):
			node = [trees[i][j]]
			randLeafsNum = rd.randint(minLeafsNumber, maxLeafsNumber)
			leafsNum = len(copyTree) if len(copyTree) < randLeafsNum else randLeafsNum
			for k in range(leafsNum):
				node.append(copyTree[0])
				copyTree.pop(0)
			sortedTree.append(node)
		trees[i] = sortedTree
	# print(trees)
	for tree in trees:
		for node in tree:
			for i in range(1, len(node)):
				if outputTrees[node[0]]:
					for j in outputTrees[node[0]]:
						outputTrees[node[i]].append(j)
				outputTrees[node[i]].append(node[0])
	PrintToFile(outputTrees)

def PrintToFile(jobs):
	# print(jobs)
	file = 'job_graph.txt'
	with open(file, 'w') as output:
		output.write("# File with jobs\n")
		output.write(str(len(jobs)) + "\n\n")
		for i in jobs:
			output.write(str(i[1]) + "  ")
			for j in jobs[i]:
				output.write(str(j[0] + 1) + " ")
			output.write("\n")


#ForestGenerator()
#ForestGenerator(100000, 50, 200, 10, 25, 200, 400)
ForestGenerator(5, 50, 200, 1, 2, 1, 2)
