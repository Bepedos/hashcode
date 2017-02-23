import sys
import numpy as np
import matplotlib.pyplot as plt

import matplotlib.patches as patches
import sys
NB_ITER = 3

VERBOSE =  False
TRUE_VERBOSE= False

filename = "me_at_the_zoo"

with open("%s.in"%filename) as f:
		l1 = f.readline()[:-1].split(" ")
		print l1
		V = int(l1[0])  # number of videos
		E = int(l1[1])	# number of endpoints
		R = int(l1[2]) 	#number of request descriptions
		C = int(l1[3])	# number of cache servers
		X= int(l1[4]) 	#capacity of each server
			# self.servers_to_allocate = int(l1[4])

			# self.map = np.zeros((self.nb_rows, self.nb_slots_per_row))
		print "{} {} {} {} {}".format(V,E,R,C,X)

		sizes_of_videos = np.array(V)
		print l1[:100]
		l1 = f.readline()[:-1].split(" ")
		sizes_of_videos = np.array(l1).astype(int)

		endpoint_latency = np.ones(E)
		number_of_cache_to_endpoint = np.ones(E)
		end_cache_matrix = (10**6)* np.ones((E,C))
		for i in range(E):
			l1 = f.readline()[:-1].split(" ")
			endpoint_latency[i] = int(l1[0])
			K = int(l1[1])
			number_of_cache_to_endpoint[i] = K
			for j in range(K):
				l2 = f.readline()[:-1].split(" ")
				# print c, " ", i
				c =int(l2[0])
				end_cache_matrix[i][c] = int(l2[1])

		requests = np.ones((R,3))
		for i in range(R):
			l2 = f.readline()[:-1].split(" ")
			u = np.array(l2).astype(int)
			if i == 40:
				print "hahah", l2, u
			# assert len(l2)==3
			requests[i] = u



		print "NUMBER OF CACHE TO ENDPOINT" , number_of_cache_to_endpoint[:10]

		print "SIZeS OF VIDEOS", sizes_of_videos[:10]



def are_equivalent_requests(requests1, requests2):
	requests1 = requests1.astype(int)
	requests2 = requests2.astype(int)
	cnt1 = Counter()
	cnt2 = Counter()
	for i in range(requests1.shape[0]):
		cnt1[(requests1[i][0],requests1[i][1])] += requests1[i][2]
	
	for i in range(requests2.shape[0]):
		cnt2[(requests2[i][0],requests2[i][1])] += requests2[i][2]

	return cnt1, cnt2

def are_equivalent_counters(cnt1, cnt2):
	for u in cnt1.keys():
		if cnt1[u] != cnt2[u]:
			return False
	return True

requests1 = requests[:100].copy()
requests2 = requests[100:300].copy()

print are_equivalent_counters(cnt1,cnt1)

def simple_greedy():
	remaining_space = X * np.ones(C)
	#### AMELIORER
	
	solution = [{} for i in range(C)]
	
	# SORT ENDPOINTS
	for i in range(E):
		requests_of_endpoint = requests[requests[:,1] == i]
# a changer
		principal_request = requests_of_endpoint[0]
		id_video = principal_request[0]
		c = np.random.randint(C)

		if remaining_space[c] >= sizes_of_videos[id_video]:




def print_result(result, filename):
	with open("%s.out"%filename, 'w') as f:
		f.write(str(len(result))+"\n")
		for i in result:
			f.write("{} {} {} {}\n".format(i[0],i[1],i[2],i[3]))
		# f.write(str(len(result)))


def score(result):
	"""
	"""
	pass

%time result, eaten, covered,_,_ = solution(pizza, L ,H, nb_iter=-1, manual=False, cur_row=0, cur_col=0, rand=0.02)
result = np.array(result)
